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

HINSTANCE g_hInstance = NULL;        // Handle to the application instance.
string lFileName = "";
string fName = "";

int WINAPI wWinMain(HINSTANCE  hInstance,
                     HINSTANCE  /*hPrevInstance*/,
                     LPWSTR     lpstrCmdLine,
                     int        nCmdShow)
{
	LPWSTR *szArgList;
    int argCount;
    
	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
    
	if (szArgList == NULL)
    {
        MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
        return 10;
    }
    USES_CONVERSION;

	int j = 1;
	fName= T2A(szArgList[1]);
	
	while(j < (argCount-3)){
		j++;
		fName.append(" ");
		fName.append( T2A(szArgList[j]));
	}
    if(szArgList[argCount-2]!=NULL){
          lFileName = T2A(szArgList[argCount-2]);
          lFileName.append("_");
    }
	if(szArgList[argCount-1]!=NULL){
          lFileName.append(T2A(szArgList[argCount-1]));
          lFileName.append("_");
    }
    char buffer[80];
        
    // current date/time based on current system
    time_t now = time(0); //Retrieve current time

    tm *ltm = localtime(&now); //Break apart Current Time for formatting
    strftime (buffer,80,"%m-%d-%y_%H-%M-%S.csv",ltm);
 
    lFileName.append( buffer);

    LocalFree(szArgList);

    g_hInstance = hInstance;

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
	MainDialog *pDlg = new MainDialog(lFileName);
	if (pDlg == NULL)
	{
        MessageBox(NULL, TEXT("Out of memory."), NULL, MB_ICONSTOP);
	}
	else
	{
		//pDlg->OnFileOpen();
		pDlg->ShowDialog(hInstance, NULL);

		delete pDlg;
	}
}