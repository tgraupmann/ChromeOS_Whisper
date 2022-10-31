﻿//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
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

		void Translate();

		HRESULT GetDeviceId(IMMDevice* device, LPWSTR* _deviceId);
		HRESULT GetDeviceName(IMMDevice* device, LPWSTR* _deviceName);

		HRESULT EnumerateDevices(const EDataFlow dataFlow);

		std::vector<DeviceInfo> _mDevices; // device ids
	};
}