//
// pch.h
//

#pragma once

#define WINAPI_PARTITION_DESKTOP   0x00000001

#include <unknwn.h> //before winrt/base.h
#include <winrt/base.h>
#include <collection.h>
#include <ppltasks.h>

#pragma region Audio
#include <mmdeviceapi.h>
#pragma endregion Audio

#include "App.xaml.h"
