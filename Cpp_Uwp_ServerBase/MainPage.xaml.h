//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "pch.h"
#include "MainPage.g.h"
#include "WASAPICapture.h"
#include "Common.h"
#include "SDKTemplate.0.h"
#include "SDKTemplate.1.h"
#include "SDKTemplate.2.h"
#include <vector>

namespace Cpp_Uwp_ServerBase
{
	struct DeviceInfo
	{
	public:
		EDataFlow DataFlow;
		Platform::String^ DeviceId;
		Platform::String^ DeviceName;
	};

	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Page_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void cboAudioDevices_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void StartCapture();

		void Translate();

		HRESULT GetDeviceId(IMMDevice* device, LPWSTR* _deviceId);
		HRESULT GetDeviceName(IMMDevice* device, LPWSTR* _deviceName);

		HRESULT EnumerateDevices(const EDataFlow dataFlow);

		std::vector<DeviceInfo> _mDevices; // device ids

		winrt::event_token m_deviceStateChangeToken;
		winrt::event_token m_plotDataReadyToken;

		int m_discontinuityCount;
		bool m_isLowLatency;
		winrt::com_ptr<winrt::SDKTemplate::WASAPICapture> m_capture;

		// UI Helpers
		void UpdateMediaControlUI(winrt::SDKTemplate::DeviceState deviceState);

		// Handlers
		winrt::fire_and_forget OnDeviceStateChange(winrt::SDKTemplate::IDeviceStateSource const& sender, winrt::SDKTemplate::DeviceStateChangedEventArgs e);
		winrt::fire_and_forget OnPlotDataReady(winrt::SDKTemplate::IPlotDataSource const& sender, winrt::SDKTemplate::PlotDataReadyEventArgs e);

		void InitializeCapture();
		void StopCapture();
		void ClearCapture();
	};
}
