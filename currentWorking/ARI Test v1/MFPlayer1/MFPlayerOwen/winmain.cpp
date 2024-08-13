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
string lFileName = ""; //Makes log name path global
string fName = ""; //Makes video path global

int WINAPI wWinMain(HINSTANCE  hInstance,
                     HINSTANCE  /*hPrevInstance*/,
                     LPWSTR     lpstrCmdLine,
                     int        nCmdShow)
{
	//szArgList = variable that will hold the command line arguments
	LPWSTR *szArgList;
    int argCount; //This will hold how many arguments there are, which were parsed by spaces
    //parse the incoming parameter from the executable call by spaces
	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
    
	//if no parameter is given in the call, this error will appear.
	if (szArgList == NULL)
    {
        MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
        return 10;
    }
    USES_CONVERSION;

	//create a variable to store the video path, the first parameter entry
	int j = 1;
	fName= T2A(szArgList[1]);
	
	//loop through and concatenate all video path varaibles, this is only really needed if there are spaces in the video file path. Append variables with a space to remain consistant with real file path
	while(j < (argCount-3)){
		j++;
		fName.append(" ");
		fName.append( T2A(szArgList[j]));
	}
	//reserve the last two variables to be stored as the user's name in lFileName, split names by a _
    if(szArgList[argCount-2]!=NULL){
          lFileName = T2A(szArgList[argCount-2]);
          lFileName.append("_");
    }
	//The last appending '_' sperates the user's name from the appending date
	if(szArgList[argCount-1]!=NULL){
          lFileName.append(T2A(szArgList[argCount-1]));
          lFileName.append("_");
    }
	//var which will hold the current date and time
    char buffer[80];
        
    // current date/time based on current system
    time_t now = time(0); //Retrieve current time

    tm *ltm = localtime(&now); //Break apart Current Time for formatting
    strftime (buffer,80,"%m-%d-%y_%H-%M-%S.csv",ltm); //create the correct formatting for the log name
 
    lFileName.append( buffer);//append the string just made to the log name

    LocalFree(szArgList); //release szArgList to do some clean up.

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
	MainDialog *pDlg = new MainDialog(lFileName); //the log file name is passed over to MainDialog in the parameters
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