//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <functiondiscoverykeys.h> // PKEY_Device_FriendlyName
#include <wil/result.h>
#include <wil/com.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.Headers.h>

#include "DeviceState.h"

using namespace Cpp_Uwp_ServerBase;
using namespace winrt::SDKTemplate;

using namespace Platform;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Headers;
using namespace winrt;

int TargetFrequency = 440;
int TargetLatency = 30;
int TargetDurationInSec = 10;
bool ShowHelp;
bool UseConsoleDevice;
bool UseCommunicationsDevice;
bool UseMultimediaDevice;
bool DisableMMCSS;
bool EnableAudioViewManagerService;

wchar_t* OutputEndpoint;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}


void Cpp_Uwp_ServerBase::MainPage::Page_Loaded(Platform::Object^ sender, RoutedEventArgs^ e)
{
    //
    //  A GUI application should use COINIT_APARTMENTTHREADED instead of COINIT_MULTITHREADED.
    //
    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
    {
        printf("Unable to initialize COM\n");
        return;
    }

    // Unit Test
    Translate();

    EnumerateDevices(eCapture);
    EnumerateDevices(eRender);
}


//
//  Retrieves the device id for a particular device in a device collection.
//
HRESULT MainPage::GetDeviceId(IMMDevice* device, LPWSTR* _deviceId)
{
    wil::unique_cotaskmem_string deviceId;

    RETURN_IF_FAILED(device->GetId(&deviceId));

    *_deviceId = deviceId.release();

    return S_OK;
}


//
//  Retrieves the device friendly name for a particular device in a device collection.
//
HRESULT MainPage::GetDeviceName(IMMDevice* device, LPWSTR* _deviceName)
{
    wil::com_ptr_nothrow<IPropertyStore> propertyStore;
    RETURN_IF_FAILED(device->OpenPropertyStore(STGM_READ, &propertyStore));

    wil::unique_prop_variant friendlyName;
    RETURN_IF_FAILED(propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName));

    wil::unique_cotaskmem_string deviceName;
    RETURN_IF_FAILED(wil::str_printf_nothrow(deviceName, L"%ls", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal));

    *_deviceName = deviceName.release();

    return S_OK;
}


//
//  Based on the input switches, pick the specified device to use.
//
HRESULT MainPage::EnumerateDevices(const EDataFlow dataFlow)
{
    wil::com_ptr_nothrow<IMMDeviceEnumerator> deviceEnumerator;
    RETURN_IF_FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator)));

    wil::com_ptr_nothrow<IMMDeviceCollection> deviceCollection;
    RETURN_IF_FAILED(deviceEnumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &deviceCollection));

    UINT deviceCount;
    RETURN_IF_FAILED(deviceCollection->GetCount(&deviceCount));

    for (UINT i = 0; i < deviceCount; ++i)
    {
        wil::com_ptr_nothrow<IMMDevice> device;
        RETURN_IF_FAILED(deviceCollection->Item(i, &device));

        wil::unique_cotaskmem_string deviceId;
        RETURN_IF_FAILED(GetDeviceId(device.get(), &deviceId));

        wil::unique_cotaskmem_string deviceName;
        RETURN_IF_FAILED(GetDeviceName(device.get(), &deviceName));

        DeviceInfo deviceInfo;
        deviceInfo.DataFlow = dataFlow;
        deviceInfo.DeviceId = ref new String(deviceId.get());
        deviceInfo.DeviceName = ref new String(deviceName.get());

        _mDevices.push_back(deviceInfo);
        cboAudioDevices->Items->Append(deviceInfo.DeviceName);
    }

    return S_OK;
}


void Cpp_Uwp_ServerBase::MainPage::cboAudioDevices_SelectionChanged(Platform::Object^ sender, RoutedEventArgs^ e)
{
    DeviceInfo& deviceInfo = _mDevices[cboAudioDevices->SelectedIndex];

    txtDevice->Text = deviceInfo.DeviceName;

    wil::com_ptr_nothrow<IMMDeviceEnumerator> deviceEnumerator;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator))))
    {
        return;
    }

    wil::com_ptr_nothrow<IMMDeviceCollection> deviceCollection;
    if (FAILED(deviceEnumerator->EnumAudioEndpoints(deviceInfo.DataFlow, DEVICE_STATE_ACTIVE, &deviceCollection)))
    {
        return;
    }

    UINT deviceCount;
    if (FAILED(deviceCollection->GetCount(&deviceCount)))
    {
        return;
    }

    wil::com_ptr_nothrow<IMMDevice> device = nullptr;
    for (UINT i = 0; i < deviceCount; ++i)
    {
        device = nullptr;
        if (FAILED(deviceCollection->Item(i, &device)))
        {
            return;
        }

        wil::unique_cotaskmem_string deviceId;
        if (FAILED(GetDeviceId(device.get(), &deviceId)))
        {
            return;
        }

        String^ compareDeviceId = ref new String(deviceId.get());

        if (String::CompareOrdinal(compareDeviceId, deviceInfo.DeviceId) == 0)
        {
            break;
        }
    }

    if (device == nullptr)
    {
        return;
    }

    StopCapture();
    ClearCapture();
    StartCapture();
}


void Cpp_Uwp_ServerBase::MainPage::Page_Unloaded(Platform::Object^ sender, RoutedEventArgs^ e)
{
    ClearCapture();

    wil::unique_couninitialize_call comUninitialize;
}

void MainPage::ClearCapture()
{
    if (m_deviceStateChangeToken)
    {
        m_capture->DeviceStateChanged(std::exchange(m_deviceStateChangeToken, {}));
    }

    if (m_plotDataReadyToken)
    {
        m_capture->PlotDataReady(std::exchange(m_plotDataReadyToken, {}));
    }

    StopCapture();
    m_capture = nullptr;
}

#pragma region UI Related Code
// Updates transport controls based on current playstate
void MainPage::UpdateMediaControlUI(winrt::SDKTemplate::DeviceState deviceState)
{
    switch (deviceState)
    {
    case DeviceState::Capturing:
        //btnStartCapture().IsEnabled(false);
        //btnStopCapture().IsEnabled(true);
        //toggleMinimumLatency().IsEnabled(false);
        break;

    case DeviceState::Stopped:
        //btnStartCapture().IsEnabled(true);
        //btnStopCapture().IsEnabled(false);
        //toggleMinimumLatency().IsEnabled(true);
        break;

    case DeviceState::Initialized:
    case DeviceState::Starting:
    case DeviceState::Stopping:
    case DeviceState::Flushing:
    case DeviceState::Error:
        //btnStartCapture().IsEnabled(false);
        //btnStopCapture().IsEnabled(false);
        break;
    }
}
#pragma endregion

void MainPage::StartCapture()
{
    InitializeCapture();
}

// Event callback from WASAPI capture for changes in device state
fire_and_forget MainPage::OnDeviceStateChange(IDeviceStateSource const&, SDKTemplate::DeviceStateChangedEventArgs e)
{
    //auto lifetime = get_strong();
    auto lifetime = this;

    // Get the current time for messages
    std::time_t now = clock::to_time_t(clock::now());
    char buffer[26];
    ctime_s(buffer, ARRAYSIZE(buffer), &now);

    // Continue on UI thread.
    //co_await resume_foreground(Dispatcher());

    // Update Control Buttons
    DeviceState state = e.DeviceState();
    UpdateMediaControlUI(state);

    // Handle state specific messages
    switch (state)
    {
    case DeviceState::Initialized:
        m_capture->AsyncStartCapture();
        break;

    case DeviceState::Capturing:
        /*
        if (m_isLowLatency)
        {
            rootPage.NotifyUser(L"Capture Started (minimum latency) at " + to_hstring(buffer), NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Capture Started (normal latency) at " + to_hstring(buffer), NotifyType::StatusMessage);
        }
        */
        break;

    case DeviceState::Discontinuity:
    {
        m_discontinuityCount++;

        /*
        if (m_discontinuityCount > 0)
        {
            rootPage.NotifyUser(L"DISCONTINUITY DETECTED: " + to_hstring(buffer) + L" (Count = " + to_hstring(m_discontinuityCount) + L")", NotifyType::StatusMessage);
        }
        */
    }
    break;

    case DeviceState::Flushing:
        if (m_plotDataReadyToken)
        {
            m_capture->PlotDataReady(std::exchange(m_plotDataReadyToken, {}));
        }

        //rootPage.NotifyUser(L"Finalizing WAV Header.  This may take a few minutes...", NotifyType::StatusMessage);

        //Oscilloscope().Points().ReplaceAll({ { OSC_START_X, OSC_START_Y }, { OSC_X_LENGTH, OSC_START_Y } });
        break;

    case DeviceState::Stopped:
        // For the stopped state, completely tear down the audio device
        ClearCapture();

        //rootPage.NotifyUser(L"Capture Stopped", NotifyType::StatusMessage);
        break;

    case DeviceState::Error:
        ClearCapture();

        // Specifically handle a couple of known errors
        switch (hresult error = e.ExtendedError(); error)
        {
        case __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
            //rootPage.NotifyUser(L"ERROR: Check the Sound control panel for an active recording device.", NotifyType::ErrorMessage);
            break;

        case AUDCLNT_E_RESOURCES_INVALIDATED:
            //rootPage.NotifyUser(L"ERROR: Endpoint Lost Access To Resources", NotifyType::ErrorMessage);
            break;

        case E_ACCESSDENIED:
            //rootPage.NotifyUser(L"ERROR: Access Denied.  Check 'Settings->Permissions' for access to Microphone.", NotifyType::ErrorMessage);
            break;

        default:
            //rootPage.NotifyUser(L"ERROR: " + hresult_error(error).message(), NotifyType::ErrorMessage);
            break;
        }
    }

    co_return;
}

// Event callback when visualization data is ready to be plotted
fire_and_forget MainPage::OnPlotDataReady(IPlotDataSource const&, SDKTemplate::PlotDataReadyEventArgs e)
{
    //auto lifetime = get_strong();
    auto lifetime = this;

    //co_await resume_foreground(Dispatcher());

    winrt::Windows::Storage::Streams::IBuffer buffer = e.Points();
    size_t pointCount = buffer.Length() / sizeof(int16_t);
    int16_t* points = reinterpret_cast<int16_t*>(buffer.data());

    // Scale the incoming point list so that it fills our scope.
    //float y_range = static_cast<float>((std::numeric_limits<int16_t>::max)()) - std::numeric_limits<int16_t>::lowest();
    //float x_inc = pointCount > 0 ? OSC_X_LENGTH / static_cast<float>(pointCount - 1) : 0.0f;

    //UINT32 existingPoints = Oscilloscope().Points().Size();

    /*
    for (UINT32 i = 0; i < pointCount; i++)
    {
        Point p;

        // Fixup the x and y coordinates and set it back into the collection
        p.X = OSC_START_X + i * x_inc;
        p.Y = OSC_START_Y - (points[i] * OSC_TOTAL_HEIGHT) / y_range;

        if (i < existingPoints)
        {
            Oscilloscope().Points().SetAt(i, p);
        }
        else
        {
            Oscilloscope().Points().Append(p);
        }
    }
    */

    co_return;
}

//
//  InitializeCapture()
//
void MainPage::InitializeCapture()
{
    m_capture = nullptr;

    // Create a new WASAPI capture instance
    m_capture = make_self<WASAPICapture>();

    // Register for events
    //m_deviceStateChangeToken = m_capture->DeviceStateChanged({ get_weak(), &MainPage::OnDeviceStateChange });
    //m_deviceStateChangeToken = m_capture->DeviceStateChanged({ Platform::WeakReference(), &MainPage::OnDeviceStateChange});
    //m_plotDataReadyToken = m_capture->PlotDataReady({ get_weak(), &MainPage::OnPlotDataReady });
    //m_plotDataReadyToken = m_capture->PlotDataReady({ Platform::WeakReference(), &MainPage::OnPlotDataReady });

    // There is an initial discontinuity when we start, so reset discontinuity counter
    // to -1 so that we ignore that initial discontinuity.
    m_discontinuityCount = -1;

    // Configure whether we are using low-latency capture
    //m_isLowLatency = toggleMinimumLatency().IsOn();
    m_capture->SetLowLatencyCapture(m_isLowLatency);

    // Perform the initialization
    m_capture->AsyncInitializeAudioDevice();
}

//
//  StopCapture()
//
void MainPage::StopCapture()
{
    if (m_capture)
    {
        // Set the event to stop playback
        m_capture->AsyncStopCapture();
    }
}

void MainPage::Translate()
{
    //ref: https://learn.microsoft.com/en-us/windows/uwp/networking/httpclient
    //translated from WinRT C++ Sample to UWP

    // Create an HttpClient object.
    HttpClient httpClient;

    // Add a user-agent header to the GET request.
    HttpRequestHeaderCollection^ headers = httpClient.DefaultRequestHeaders;

    // The safe way to add a header value is to use the TryParseAdd method, and verify the return value is true.
    // This is especially important if the header value is coming from user input.
    String^ header = ref new String(L"ie");
    if (!headers->UserAgent->TryParseAdd(header))
    {
        //Invalid header value
        return;
    }

    String^ url = ref new String(L"http://localhost:8000/translate");
    Uri^ requestUri = ref new Uri(url);

    // Send the GET request asynchronously, and retrieve the response as a string.
    try
    {
        // Send the POST request.
        HttpRequestMessage^ httpRequestMessage = ref new HttpRequestMessage(HttpMethod::Post, requestUri);

        // Construct the JSON to post.
        JsonObject^ jobject = ref new JsonObject();
        jobject->SetNamedValue(ref new String(L"sampleRate"), JsonValue::CreateNumberValue(16000));
        JsonArray^ jarray = ref new JsonArray();
        jobject->SetNamedValue(ref new String(L"data"), jarray);

        String^ jsonBody = jobject->ToString();

        HttpStringContent^ jsonContent = ref new HttpStringContent(
            jsonBody,
            UnicodeEncoding::Utf8,
            L"application/json");

        IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ opPostAsync =
            httpClient.PostAsync(requestUri, jsonContent);
        opPostAsync->Completed = ref new AsyncOperationWithProgressCompletedHandler<HttpResponseMessage^, HttpProgress>(
            [=](IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ asyncInfo,
                AsyncStatus)
            {
                HttpResponseMessage^ httpResponseMessage = asyncInfo->GetResults();
                httpResponseMessage->EnsureSuccessStatusCode();
                String^ httpResponseBody = httpResponseMessage->Content->ReadAsStringAsync()->GetResults();

                // run on UI thread
                txtTranslations->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
                    ref new DispatchedHandler([this, httpResponseBody]
                        {
                            JsonObject^ jobject = JsonObject::Parse(httpResponseBody);
                            String^ text = jobject->GetNamedValue(L"text")->GetString();
                            txtTranslations->Text += L" " + text;
                        }));
            });
    }
    catch (winrt::hresult_error const&)
    {
        // failed
        return;
    }
}
