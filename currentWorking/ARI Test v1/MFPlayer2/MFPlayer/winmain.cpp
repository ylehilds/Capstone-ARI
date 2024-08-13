//------------------------------------------------------------------------------
//
// File: winmain.cpp
// Application entry-point.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//------------------------------------------------------------------------------

#include "MFPlayer.h"
#include "MainDialog.h"
#include <string>
#include <AtlConv.h>
#include <time.h>
#include <stdio.h>
using namespace std;

HINSTANCE g_hInstance = NULL;	// Handle to the application instance.
string fName = ""; //Makes video path global

int WINAPI wWinMain(HINSTANCE  hInstance,
                     HINSTANCE  /*hPrevInstance*/,
                     LPWSTR     lpstrCmdLine,
                     int        nCmdShow)
{

    g_hInstance = hInstance;
	LPWSTR *szArgList;
    int argCount;
	//parse the incoming parameter from the executable call by spaces
	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	USES_CONVERSION;
	int j = 1;

	//create a variable to store the video path coming in through the executable call
	fName= T2A(szArgList[1]);
	
	//loop through and concatenate all video path varaibles, this is only really needed if there are spaces in the video file path. Append variables with a space to remain consistant with real file path
	while(j < (argCount-1)){
		j++;
		fName.append(" ");
		fName.append( T2A(szArgList[j]));
	}
    // Initialize the COM library.
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("CoInitialize failed."), NULL, MB_ICONSTOP);
        return 1;
    }

    // Initialize the common control library.
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES;
    if (!InitCommonControlsEx(&icc))
    {
        MessageBox(NULL, TEXT("InitCommonControlsEx failed."), NULL, MB_ICONSTOP);
        CoUninitialize();
        return 1;
    }

    // Initialize our custom slider class.
    hr = Slider_Init(); 
    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Slider_Init failed."), NULL, MB_ICONSTOP);
        CoUninitialize();
        return 1;
    }

    TRACE_INIT(NULL);

    // Create and show the dialog.
	MainDialog *pDlg = new MainDialog();
	if (pDlg == NULL)
	{
        MessageBox(NULL, TEXT("Out of memory."), NULL, MB_ICONSTOP);
	}
	else
	{
		pDlg->ShowDialog(hInstance, NULL);

		delete pDlg;
	}


    TRACE_CLOSE();
    CoUninitialize();
    return 0;
}
