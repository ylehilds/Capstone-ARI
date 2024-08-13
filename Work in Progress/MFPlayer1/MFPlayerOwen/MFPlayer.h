//------------------------------------------------------------------------------
//
// File: MFPlayer.h
// Main header for the application
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once

#define NTDDI_VERSION NTDDI_LONGHORN 

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <d3d9.h>
#include <evr.h>
#include <evr9.h>
#include <strsafe.h>

// Win32 helpers
#include "wincontrol.h"
#include "button.h"
#include "trackbar.h"
#include "slider.h"
#include "dialog.h"

// MF helpers
#define USE_LOGGING
#include "common.h"
using namespace MediaFoundationSamples;

#define CHECK_HR(hr) IF_FAILED_GOTO(hr, done)

// Application headers
#include "resource.h"
#include "Player_h.h"
#include "ThemedButton.h"
#include "statusbar.h"
#include "MainDialog.h"

// Libraries
#pragma comment(lib, "mf")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "strmiids")
#pragma comment(lib, "comctl32")

#define LOG_IF_FAILED(x,hr) if (FAILED(hr)) { TRACE((L"%s hr=0x%X", x, hr)); }

// Include the v6 common controls in the manifest
#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")


// CLSID of the sample EVR presenter.
extern const GUID CLSID_CustomEVRPresenter;