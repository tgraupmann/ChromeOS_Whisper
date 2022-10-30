//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <functiondiscoverykeys.h> // PKEY_Device_FriendlyName
#include <wil/result.h>
#include <wil/com.h>

using namespace Cpp_Uwp_ServerBase;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

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


//
//  Retrieves the device friendly name for a particular device in a device collection.
//
HRESULT GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex, LPWSTR* _deviceName)
{
    wil::com_ptr_nothrow<IMMDevice> device;
    wil::unique_cotaskmem_string deviceId;

    RETURN_IF_FAILED(DeviceCollection->Item(DeviceIndex, &device));

    RETURN_IF_FAILED(device->GetId(&deviceId));

    wil::com_ptr_nothrow<IPropertyStore> propertyStore;
    RETURN_IF_FAILED(device->OpenPropertyStore(STGM_READ, &propertyStore));

    wil::unique_prop_variant friendlyName;
    RETURN_IF_FAILED(propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName));

    wil::unique_cotaskmem_string deviceName;
    RETURN_IF_FAILED(wil::str_printf_nothrow(deviceName, L"%ls (%ls)", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal, deviceId.get()));

    *_deviceName = deviceName.release();

    return S_OK;
}


//
//  Based on the input switches, pick the specified device to use.
//
HRESULT PickDevice(IMMDevice** DeviceToUse, bool* IsDefaultDevice, ERole* DefaultDeviceRole)
{
    wil::com_ptr_nothrow<IMMDeviceEnumerator> deviceEnumerator;
    wil::com_ptr_nothrow<IMMDeviceCollection> deviceCollection;
    wil::com_ptr_nothrow<IMMDevice> device;

    *IsDefaultDevice = false;   // Assume we're not using the default device.

    RETURN_IF_FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator)));

    //
    //  First off, if none of the console switches was specified, use the console device.
    //
    if (!UseConsoleDevice && !UseCommunicationsDevice && !UseMultimediaDevice && OutputEndpoint == nullptr)
    {
        //
        //  The user didn't specify an output device, prompt the user for a device and use that.
        //
        RETURN_IF_FAILED(deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection));

        printf("Select an output device:\n");
        printf("    0:  Default Console Device\n");
        printf("    1:  Default Communications Device\n");
        printf("    2:  Default Multimedia Device\n");
        UINT deviceCount;
        RETURN_IF_FAILED(deviceCollection->GetCount(&deviceCount));

        for (UINT i = 0; i < deviceCount; i += 1)
        {
            wil::unique_cotaskmem_string deviceName;

            RETURN_IF_FAILED(GetDeviceName(deviceCollection.get(), i, &deviceName));
            printf("    %d:  %ls\n", i + 3, deviceName.get());
        }
        wchar_t choice[10];
        _getws_s(choice);   // Note: Using the safe CRT version of _getws.

        long deviceIndex;
        wchar_t* endPointer;

        deviceIndex = wcstoul(choice, &endPointer, 0);
        if (deviceIndex == 0 && endPointer == choice)
        {
            printf("unrecognized device index: %ls\n", choice);
            return E_UNEXPECTED;
        }
        switch (deviceIndex)
        {
        case 0:
            UseConsoleDevice = 1;
            break;
        case 1:
            UseCommunicationsDevice = 1;
            break;
        case 2:
            UseMultimediaDevice = 1;
            break;
        default:
            RETURN_IF_FAILED(deviceCollection->Item(deviceIndex - 3, &device));
            break;
        }
    }
    else if (OutputEndpoint != nullptr)
    {
        RETURN_IF_FAILED(deviceEnumerator->GetDevice(OutputEndpoint, &device));
    }

    if (device == nullptr)
    {
        ERole deviceRole = eConsole;    // Assume we're using the console role.
        if (UseConsoleDevice)
        {
            deviceRole = eConsole;
        }
        else if (UseCommunicationsDevice)
        {
            deviceRole = eCommunications;
        }
        else if (UseMultimediaDevice)
        {
            deviceRole = eMultimedia;
        }
        RETURN_IF_FAILED(deviceEnumerator->GetDefaultAudioEndpoint(eRender, deviceRole, &device));
        *IsDefaultDevice = true;
        *DefaultDeviceRole = deviceRole;
    }

    *DeviceToUse = device.detach();

    return S_OK;
}


void Cpp_Uwp_ServerBase::MainPage::Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    //
    //  A GUI application should use COINIT_APARTMENTTHREADED instead of COINIT_MULTITHREADED.
    //
    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
    {
        printf("Unable to initialize COM\n");
        return;
    }

    wil::com_ptr_nothrow<IMMDevice> device;
    bool isDefaultDevice;
    ERole role;
    if (PickDevice(&device, &isDefaultDevice, &role) != S_OK)
    {
        return;
    }
}


void Cpp_Uwp_ServerBase::MainPage::Page_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    wil::unique_couninitialize_call comUninitialize;
}
