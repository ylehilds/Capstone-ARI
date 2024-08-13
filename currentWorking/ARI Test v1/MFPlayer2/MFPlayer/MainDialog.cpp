//------------------------------------------------------------------------------
//
// File: MainDialog.h
// Implements the main dialog.
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
#include "windows.h"
#include <iostream>
#include <fstream>
#include "time.h"
#include "Feature_Base.h"
#include "Feature_Seeking.h"
using namespace std;

// Constants 

const UINT_PTR	IDT_TIMER1 = 1;		// Timer ID
const UINT		TICK_FREQ = 250;	// Timer frequency in msec		

const DWORD     GUID_STR_LEN = 39;  // Length of a string for a GUID in canonical form.

const LONG      MIN_VOL = 0;
const LONG      MAX_VOL = 100;


UINT_PTR m_nTimer =0;
ifstream myfile;
MainDialog *mainDialogPointer = NULL;

// Structs

//------------------------------------------------------------------------------
// EvrConfig struct
//
// Configuration information for the EVR.
//------------------------------------------------------------------------------

struct EvrConfig
{
    GUID    clsidPresenter;     // CLSID of a custom presenter, or GUID_NULL.
    GUID    clsidMixer;         // CLSID of a custom mixer, or GUID_NULL.
};


//------------------------------------------------------------------------------
// OpenUrlDialogInfo struct
// 
// Contains data passed to the "Open URL" dialog proc.
//------------------------------------------------------------------------------

struct OpenUrlDialogInfo
{
    WCHAR *pszURL;
    DWORD cch;
};


// Function declarations

INT_PTR CALLBACK OpenUrlDialogProc(HWND, UINT, WPARAM, LPARAM);

UINT_PTR CALLBACK OFNHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

void OFNHookProc_InitEditBox(HWND hDlg, int nIdEdit, const GUID& clsid);
void OFNHookProc_OnCommand(HWND hDlg, int nIdButton, int nIdEdit);
void OFNHookProc_OnOK(HWND hDlg, int nIdButton, int nIdEdit, GUID *pCLSID);

void NotifyError(HWND hwnd, const WCHAR *sErrorMessage, HRESULT hrErr);


// Inline functions

inline float VolumeFromSlider(LONG pos)
{
    return (float)pos / MAX_VOL;
}

inline LONG SliderPosFromVolume(float fLevel)
{
    return (LONG)(fLevel * MAX_VOL);
}



//-----------------------------------------------------------------------------
// MainDialog constructor
//-----------------------------------------------------------------------------

MainDialog::MainDialog() : 
    CBaseDialog(IDD_DIALOG1),
    m_pPlayer(NULL),
    m_pVideo(NULL),
    m_pAudio(NULL),
    m_pSeeking(NULL),
    m_timerID(0)
{
}


//-----------------------------------------------------------------------------
// MainDialog destructor
//-----------------------------------------------------------------------------

MainDialog::~MainDialog()
{
    SAFE_RELEASE(m_pVideo);
    SAFE_RELEASE(m_pAudio);
    SAFE_RELEASE(m_pSeeking);

    if (m_pPlayer)
    {
        m_pPlayer->Shutdown();
    }

	SAFE_RELEASE(m_pPlayer);
}


//-----------------------------------------------------------------------------
// OnInitDialog
// Handler for WM_INITDIALOG message.
//-----------------------------------------------------------------------------

HRESULT MainDialog::OnInitDialog()
{
    HRESULT hr = S_OK;

	// Create the player object.
    CHECK_HR(hr = CreateSamplePlayer(m_hDlg, &m_pPlayer));

    // Get features.
    CHECK_HR(hr = m_pPlayer->GetFeature(__uuidof(IPlayerVideo), (void**)&m_pVideo));
    CHECK_HR(hr = m_pPlayer->GetFeature(__uuidof(IPlayerAudio), (void**)&m_pAudio));
    CHECK_HR(hr = m_pPlayer->GetFeature(__uuidof(IPlayerSeeking), (void**)&m_pSeeking));

    CHECK_HR(hr = m_pVideo->SetVideoWindow(GetDlgItem(IDC_VIDEO)));

    InitializeControls();

	// Set default UI state.
    UpdateUI();
done:
    return hr;
}
//Implemented by Pancake
void CALLBACK syncState(HWND hWnd, UINT nMsg,UINT nIDEvent, DWORD dwTime) 
{	
	mainDialogPointer->stateLog();
}
//Implemented by Pancake	
 void MainDialog::stateLog()
{	
	std::string stateFile;
	//Use the following path for debugging in Visual Studio
	myfile.open("C:\\Users\\Public\\Videos\\Sample Videos\\state.txt");
	//Use the following path when creating a new executable for the ARI folder
	//myfile.open("state.txt");
	if (myfile.good()){

		getline(myfile, stateFile);
		if(stateFile == "play")
		{
			 OnPlay();
		}
		if(stateFile == "pause")
		{
			OnPause();
		}
	}

	myfile.close();
}

//computer time method
/*string MainDialog::computerTime()
{
	string mytime="";
	time_t ltime; // calendar time 
    ltime=time(NULL); //get current cal time
    mytime="%s",asctime( localtime(&ltime));
	string time = ("%s",asctime( localtime(&ltime)));
	return time;
	
}*/


void OnStartTimer() 
{
	m_nTimer = SetTimer(NULL,2, 100, syncState);
}

/*void OnStopTimer() 
{
	KillTimer(NULL,m_nTimer);
}*/

//-----------------------------------------------------------------------------
// OnCommand
// Handler for WM_COMMAND messages.
//-----------------------------------------------------------------------------

INT_PTR MainDialog::OnCommand(HWND hControl, WORD idControl, WORD msg)
{
    switch (idControl)
    {
		case ID_FILE_OPENFILE:
			OnFileOpen();
			break;

        case ID_FILE_OPENURL:
            OnOpenURL();
            break;

        case ID_FILE_EXIT:
            PostQuitMessage(0);
            break;

        case IDC_MUTE:
            OnMute();
            break;

        case IDC_PLAY:
            OnPlayOrPause();
            break;

		case IDC_BRIGHTNESS:
            OnPlayOrPause();
            break;

        case IDC_REWIND:
            if (m_pSeeking)
            {
                m_pSeeking->Rewind();
            }
            break;

        case IDC_FASTFORWARD:
            if (m_pSeeking)
            {
                m_pSeeking->FastForward();
            }
            break;

    }
	return 1;
}


//-----------------------------------------------------------------------------
// OnCommand
// Handler for WM_NOTIFY messages.
//-----------------------------------------------------------------------------

LRESULT MainDialog::OnNotify(NMHDR *pHdr)
{
    LRESULT result = 0; // ignored unless documented otherwise.

	switch (pHdr->idFrom)
	{
	case IDC_SEEKBAR:
		OnSeekbarNotify((NMSLIDER_INFO*)pHdr);
		break;

    case IDC_VOLUME:
        OnVolumeNotify((NMSLIDER_INFO*)pHdr);
        break;

    case IDC_MUTE:
        if (pHdr->code == NM_CUSTOMDRAW)
        {
            result = m_mute.Draw((NMCUSTOMDRAW*)pHdr);
        }
        break;

    case IDC_PLAY:
        if (pHdr->code == NM_CUSTOMDRAW)
        {
            result = m_play.Draw((NMCUSTOMDRAW*)pHdr);
        }
        break;
	case IDC_BRIGHTNESS:
        if (pHdr->code == NM_CUSTOMDRAW)
        {
            result = m_brightness.Draw((NMCUSTOMDRAW*)pHdr);
        }
        break;
    }


    return result;   
}

//-----------------------------------------------------------------------------
// OnReceiveMsg
// Handler for any other window messages.
//-----------------------------------------------------------------------------

INT_PTR MainDialog::OnReceiveMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
	case WM_TIMER:          // Timer message
		OnTimer();
		break;

    case WM_HSCROLL:        // Trackbar scroll
        OnScroll(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
        break;

    case WM_THEMECHANGED:   // User changed the current theme.
        m_mute.ResetTheme();
        m_play.ResetTheme();
        break;

    case WM_PAINT:
        m_pVideo->Repaint();
        break;

    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    // Private window messages from the MFPlayer class.

    case WM_APP_NOTIFY:
        OnPlayerNotify((PlayerState)wParam);
        break;
   
    case WM_APP_ERROR:
        NotifyError(m_hDlg, L"Playback Error", (HRESULT)wParam);
        UpdateUI();
        break;

    case WM_APP_EVENT:
        OnPlayerEvent((PlayerEvent)wParam, lParam);
        break;

    }
    return FALSE;
}


//-----------------------------------------------------------------------------
// OnCancel
// Called when the user clicks IDCANCEL (not applicable to this dialog) or
// ESC, Alt-F4, etc
//-----------------------------------------------------------------------------

BOOL MainDialog::OnCancel()
{
    return FALSE;   // Don't exit.
}


//-----------------------------------------------------------------------------
// InitializeControls
// Initializes the UI controls.
//-----------------------------------------------------------------------------

HRESULT MainDialog::InitializeControls()
{
    HRESULT hr = S_OK;

    m_seekbar.SetWindow(GetDlgItem(IDC_SEEKBAR));
    m_volume.SetWindow(GetDlgItem(IDC_VOLUME));
    m_mute.SetWindow(GetDlgItem(IDC_MUTE));
    m_play.SetWindow(GetDlgItem(IDC_PLAY));
	m_brightness.SetWindow(GetDlgItem(IDC_BRIGHTNESS));
    m_btnFastFwd.SetWindow(GetDlgItem(IDC_FASTFORWARD));
    m_btnRewind.SetWindow(GetDlgItem(IDC_REWIND));

    // Create a brush for the seek bar background. 
    // Note: Don't use GetSysColorBrush because the slider control destroys the brush.
    HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));

    if (hBrush == NULL)
    {
        CHECK_HR(hr = HRESULT_FROM_WIN32(GetLastError()));
    }
    
    CHECK_HR(hr = m_seekbar.SetBackground(hBrush));
    CHECK_HR(hr = m_seekbar.SetThumbBitmap(IDB_SLIDER));
    m_seekbar.Enable(FALSE);

    // Create a brush for the volume slider background. 
    hBrush = CreateSolidBrush(RGB(12,3,127));
    if (hBrush == NULL)
    {
        CHECK_HR(hr = HRESULT_FROM_WIN32(GetLastError()));
    }

    // Set the background, thumb bitmap, range, and position of the volume slide.
    CHECK_HR(hr = m_volume.SetBackground(hBrush));
    CHECK_HR(hr = m_volume.SetThumbBitmap(IDB_VOLUME));
    CHECK_HR(hr = m_volume.SetRange(MIN_VOL, MAX_VOL));
    CHECK_HR(hr = m_volume.SetPosition(MAX_VOL));
    m_volume.Enable(TRUE);

    // Set the image for the mute button.
    CHECK_HR(hr = m_mute.LoadBitmap(IDB_MUTE, 2));
    CHECK_HR(hr = m_mute.SetButtonImage((UINT)-1, 0));

    // Set the image for the play/pause button.
    CHECK_HR(hr = m_play.LoadBitmap(IDB_PLAY, 2));
    CHECK_HR(hr = m_play.SetButtonImage((UINT)-1, 0));

    m_Zoom.SetWindow(GetDlgItem(IDC_VIDEO_ZOOM));
    m_Zoom.SetRangeAndPosition(100, 500, 100);

    CHECK_HR(hr = InitStatusBar());

	OnPlayOpen();

done:
    return hr;
}


//-----------------------------------------------------------------------------
// OnPlayerNotify
// 
// Called when the player object changes state.
//-----------------------------------------------------------------------------

void MainDialog::OnPlayerNotify(PlayerState state)
{
    switch (state)
    {
        // No actions
    case Player_Paused:
    case Player_Closed:
    case Player_OpenPending:
    case Player_Started:
        break;

    case Player_Ready:
        OnPlayerReady();
        break;

    case Player_Stopped:
        UpdateSeekBar();
        break;
    }

    UpdateUI();
}


//-----------------------------------------------------------------------------
// OnPlayerEvent
// 
// Called when the player object sends an event.
//-----------------------------------------------------------------------------

void MainDialog::OnPlayerEvent(PlayerEvent playerEvent, LPARAM lParam)
{
    switch (playerEvent)
    {
    case Player_VolumeChanged:
        OnVolumeChanged();
        break;

    case Player_Connect:
        if (lParam)
        {
            SetStatusText(L"Connecting...");
        }
        else
        {
            SetStatusText(L"Connected");
        }
        break;
    }
}



//-----------------------------------------------------------------------------
// OnFileOpen
// Open a new file for playback.
//-----------------------------------------------------------------------------

void MainDialog::OnFileOpen()
{
	HRESULT hr = S_OK;

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	WCHAR szFileName[MAX_PATH];
	szFileName[0] = L'\0';

    // EVR configuration information. The user can change these.
    EvrConfig config = { CLSID_CustomEVRPresenter, GUID_NULL }; 


    // Note: We customize the Open File dialog to provide an option to select
    // a custom presenter or mixer for the EVR. For details, see OFNHookProc().

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hDlg;
	ofn.hInstance = GetInstance();
	ofn.lpstrFilter = L"Windows Media\0*.wmv;*.wma;*.asf\0MP3\0*.mp3\0All files\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;

    // Customization information:
    ofn.lpTemplateName = MAKEINTRESOURCE(IDD_OPENFILE_EXT);
    ofn.lpfnHook = OFNHookProc;
    ofn.lCustData = (LPARAM)&config;

	if (GetOpenFileName(&ofn))
	{
        // Set the presenter and mixer CLSIDs. If the user did not select these
        // options, the relevant CLSID is set to GUID_NULL. 

        m_pVideo->SetPresenter(config.clsidPresenter);

        m_pVideo->SetMixer(config.clsidMixer);
        
        // Now try to open the file.
		hr = m_pPlayer->OpenURL(szFileName);

		// Update the state of the UI. (Regardless of success/failure code)
		UpdateUI();

		// Invalidate the video window, in case there is an old video 
		// frame from the previous file and there is no video now. (eg, the
		// new file is audio only, or we failed to open this file.)
		InvalidateRect( GetDlgItem(IDC_VIDEO) , NULL, FALSE);

		if (FAILED(hr))
		{
			NotifyError(m_hDlg, TEXT("Cannot open this file."), hr);
		}
	}
    else
    {
        // GetOpenFileName can return FALSE because the user cancelled,
        // or because it failed. Check for errors.

        DWORD err = CommDlgExtendedError();
        if (err != 0)
        {
            NotifyError(m_hDlg, TEXT("GetOpenFileName failed."), E_FAIL);
        }
    }
}


//-----------------------------------------------------------------------------
//  OnOpenURL
//  Opens a media file from a URL.
//-----------------------------------------------------------------------------

void MainDialog::OnOpenURL()
{
    HRESULT hr = S_OK;
    INT_PTR result = 0;

    // Pass in an OpenUrlDialogInfo structure to the dialog. The dialog 
    // fills in this structure with the URL. The dialog proc allocates
    // the memory for the string. 

    OpenUrlDialogInfo url;
    ZeroMemory(&url, sizeof(&url));

	// Show the Open URL dialog.
    result = DialogBoxParam(GetInstance(), MAKEINTRESOURCE(IDD_OPENURL), m_hDlg, 
        OpenUrlDialogProc, (LPARAM)&url);

    if (result == IDOK)
    {
		// Open the file with the playback object.
        hr = m_pPlayer->OpenURL(url.pszURL);

		// Update the state of the UI. (Regardless of success/failure code)
		UpdateUI();

		// Invalidate the video window, in case there is an old video 
		// frame from the previous file and there is no video now. (eg, the
		// new file is audio only, or we failed to open this file.)
		InvalidateRect(GetDlgItem(IDC_VIDEO), NULL, FALSE);

		if (FAILED(hr))
		{
			NotifyError(m_hDlg, TEXT("Cannot open this URL."), hr);
		}
    }

    // The app must release the string for the URL.
    CoTaskMemFree(url.pszURL);
}


void MainDialog::OnPlayOpen()
{
	HRESULT hr = S_OK;

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	WCHAR szFileName[MAX_PATH];
	szFileName[0] = L'\0';

    // EVR configuration information. The user can change these.
    ////EvrConfig config = { CLSID_CustomEVRPresenter, GUID_NULL }; 


    // Note: We customize the Open File dialog to provide an option to select
    // a custom presenter or mixer for the EVR. For details, see OFNHookProc().

	////ofn.lStructSize = sizeof(ofn);
	////ofn.hwndOwner = m_hDlg;
	////ofn.hInstance = GetInstance();
	////ofn.lpstrFilter = L"Windows Media\0*.wmv;*.wma;*.asf\0MP3\0*.mp3\0All files\0*.*\0";
	//extern WCHAR* filenameMF;
	////ofn.lpstrFile = szFileName;
	////ofn.nMaxFile = MAX_PATH;
	////ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;

    // Customization information:
    ////ofn.lpTemplateName = MAKEINTRESOURCE(IDD_OPENFILE_EXT);
    ////ofn.lpfnHook = OFNHookProc;
    ////ofn.lCustData = (LPARAM)&config;

	////if (GetOpenFileName(&ofn))
	////{
        // Set the presenter and mixer CLSIDs. If the user did not select these
        // options, the relevant CLSID is set to GUID_NULL. 

        ////m_pVideo->SetPresenter(config.clsidPresenter);

        ////m_pVideo->SetMixer(config.clsidMixer);
		// Define filename as string to URL we want
		extern std::string fName;
		const char *filename = fName.c_str();
		//char *filename = "C:\\Users\\Public\\Videos\\Sample Videos\\TAKE EIGHT.wmv";
		// Do some funky array calculation stuff that I copied from MSDN (ms235631)
		size_t origsize = strlen(filename) + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		//Init wcfilename
		wchar_t wcfilename[newsize];
		//This line converts the CHAR to WCHAR -- no idea how it does that
		mbstowcs_s(&convertedChars, wcfilename, origsize, filename, _TRUNCATE);

		//Open up MPlayer with the file specified
		hr = m_pPlayer->OpenURL(wcfilename);
		
        // Now try to open the file.
		//hr = m_pPlayer->OpenURL(szFileName);

		// Update the state of the UI. (Regardless of success/failure code)
		UpdateUI();

		// Invalidate the video window, in case there is an old video 
		// frame from the previous file and there is no video now. (eg, the
		// new file is audio only, or we failed to open this file.)
		InvalidateRect( GetDlgItem(IDC_VIDEO) , NULL, FALSE);

		if (FAILED(hr))
		{
			NotifyError(m_hDlg, TEXT("Cannot open this file."), hr);
		}
	////}
    ////else
    ////{
        // GetOpenFileName can return FALSE because the user cancelled,
        // or because it failed. Check for errors.

        ////DWORD err = CommDlgExtendedError();
        ////if (err != 0)
        ////{
            ////NotifyError(m_hDlg, TEXT("GetOpenFileName failed."), E_FAIL);
        ////}
    ////}
}

//-----------------------------------------------------------------------------
//  OnPlayerReady
//  Called when the player successfully opens a file or URL.
//-----------------------------------------------------------------------------

void MainDialog::OnPlayerReady()
{
    UpdateSeekBar();

    //m_pPlayer->Play();
	OnStartTimer();

    UpdateUI();
}


//-----------------------------------------------------------------------------
//  OnScroll
//  Called when user selects, drags, or releases a trackbar control.
//-----------------------------------------------------------------------------

void MainDialog::OnScroll(WORD request, WORD position, HWND hControl)
{
    // We ignore the following trackback requests:
    switch (request)
    {
    case SB_ENDSCROLL: 
    case SB_LEFT: 
    case SB_RIGHT:
    case SB_THUMBPOSITION: 
        return;
    }

    if (hControl == m_Zoom.Window())
    {
        // Scale the video.
        float fZoom = (float)m_Zoom.GetPosition() / 100;

        m_pVideo->SetZoom(fZoom);
    }
}




//-----------------------------------------------------------------------------
// OnSeekbarNotify
// Called when user selects, drags, or releases a seek bar.
//-----------------------------------------------------------------------------

void MainDialog::OnSeekbarNotify(const NMSLIDER_INFO *pInfo)
{
    // When dragging, switch to scrubbing mode.
    if (pInfo->hdr.code == SLIDER_NOTIFY_DRAG)
    {
        m_pSeeking->Scrub(TRUE);
    }

    // When dragging or selecting, seek to the position.
    if ((pInfo->hdr.code == SLIDER_NOTIFY_SELECT) ||
        (pInfo->hdr.code == SLIDER_NOTIFY_DRAG))
    {
        LONGLONG pos = ONE_MSEC * (LONGLONG)pInfo->position;

    	(void)m_pSeeking->SetPosition(pos);
    }

    // When releasing, disable scrubbing.
    if (pInfo->hdr.code == SLIDER_NOTIFY_RELEASE)
    {
        m_pSeeking->Scrub(FALSE);
    }

}

//-----------------------------------------------------------------------------
// OnPlayOrPause
// Called when the user clicks the play/pause button.
//-----------------------------------------------------------------------------

void MainDialog::OnPlayOrPause()
{
    // This button toggles between play and pause.

    PlayerState state;
	//OnStartTimer() starts the timer that checks the state file every 100 ms
	OnStartTimer();
    m_pPlayer->GetState(&state);

    if (state == Player_Started)
    {
        m_pPlayer->Pause();
		
    }
    else
    {
        m_pPlayer->Play();
    }

    UpdateUI();
}

//Implemented by Pancake
void MainDialog::OnPlay()
{
	//Check the player state and Play the video
	PlayerState state;
	m_pPlayer->GetState(&state);
	if (state != Player_Started)
	{
		m_pPlayer->Play();
	} 
	UpdateUI();
}
//Implemented by Pancake
void MainDialog::OnPause()
{
	//Check the player state and Pause the video
	PlayerState state;
	m_pPlayer->GetState(&state);
	if (state == Player_Started)
	{
		m_pPlayer->Pause();

	}
	UpdateUI();
}


//-----------------------------------------------------------------------------
// OnVolumeNotify
// Called when the user drags the volume slider.
//-----------------------------------------------------------------------------

void MainDialog::OnVolumeNotify(const NMSLIDER_INFO* pInfo)
{
    m_pAudio->SetVolume( VolumeFromSlider(pInfo->position) );
}



//-----------------------------------------------------------------------------
// OnVolumeChanged
//
// Called when the volume changes because of actions outside of this
// application. (For example, when the user changes the volume in the 
// Volume control panel.
//-----------------------------------------------------------------------------

void MainDialog::OnVolumeChanged()
{
    HRESULT hr = S_OK;

    float fLevel = 0;
    BOOL bMute = FALSE;

    // Get the new volume level and update the slider position.
    hr = m_pAudio->GetVolume(&fLevel);

    if (SUCCEEDED(hr))
    {
        m_volume.SetPosition( SliderPosFromVolume(fLevel) );
    }

    // Get the new mute state and update the image on the mute button.
    hr = m_pAudio->GetMute(&bMute);
    if (SUCCEEDED(hr))
    {
        m_mute.SetButtonImage((UINT)-1, (bMute ? 1 : 0));
        InvalidateRect(m_mute.Window(), NULL, FALSE);
    }

}


//-----------------------------------------------------------------------------
// OnMute
// Called when the user clicks the mute/unmute button.
//-----------------------------------------------------------------------------

void MainDialog::OnMute()
{
    BOOL bMute = FALSE;
    HRESULT hr = m_pAudio->GetMute(&bMute);

    if (SUCCEEDED(hr))
    {
        bMute = !bMute; // Flip the mute state.
        hr = m_pAudio->SetMute(bMute);
    }

    if (SUCCEEDED(hr))
    {
        m_mute.SetButtonImage((UINT)-1, (bMute ? 1 : 0));
    }
}




//-----------------------------------------------------------------------------
// OnTimer
// Called when the timer elapses.
//-----------------------------------------------------------------------------

void MainDialog::OnTimer()
{
    MFTIME timeNow;

	if(mainDialogPointer != this)
	{
		mainDialogPointer = this;
	}

    if (SUCCEEDED(m_pSeeking->GetCurrentPosition(&timeNow)))
    {
        SetStatusTime(timeNow);

		m_seekbar.SetPosition((LONG)(timeNow / ONE_MSEC));
    }
}


//-----------------------------------------------------------------------------
//  UpdateUI
//  Updates the dialog controls, based on the current state.
//-----------------------------------------------------------------------------

void MainDialog::UpdateUI()
{
    PlayerState state;
    
    m_pPlayer->GetState(&state);

    BOOL bFileOpen = TRUE;
    BOOL bHasVideo = FALSE;
    BOOL bEnablePlay = FALSE;
    BOOL bPlay = TRUE;  
    
    // If bPlay is TRUE, the Play/Pause button shows the "play" image.
    // If bEnablePlay is TRUE, the Play/Pause button is enabled.
    
    BOOL bCanSeek = FALSE;
    BOOL bCanScrub = FALSE;
    BOOL bEnableSeek = FALSE;

    BOOL bCanFF = FALSE;
    BOOL bCanRewind = FALSE;
    BOOL bTrickModeOK = FALSE;

    m_pVideo->HasVideo(&bHasVideo);
    m_pSeeking->CanSeek(&bCanSeek);
    m_pSeeking->CanScrub(&bCanScrub);
    m_pSeeking->CanFastForward(&bCanFF);
    m_pSeeking->CanRewind(&bCanRewind);

    switch (state)
    {
    case Player_Closed:
        bFileOpen = FALSE;
        SetStatusText(L"Closed");
        break;

    case Player_OpenPending:
        SetStatusText(L"Opening...");
        break;

    case Player_Ready:
        bEnablePlay = TRUE;
        break;

    case Player_Started:
        bEnablePlay = TRUE;
        bPlay = FALSE;
        bEnableSeek = bCanSeek;
        bTrickModeOK = TRUE;
        SetStatusText(L"Playing");
        break;

    case Player_Paused:
        bEnablePlay = TRUE;
        bEnableSeek = bCanSeek && bCanScrub;
        SetStatusText(L"Paused");
        break;

    case Player_Stopped:
        bEnablePlay = TRUE; 
        SetStatusText(L"Stopped");
        break;

    case Player_Buffering:
        SetStatusText(L"Buffering...");
    }

    m_Zoom.Enable(bHasVideo);

    m_play.Enable(bEnablePlay);
    m_seekbar.Enable(bEnablePlay);

    m_btnFastFwd.Enable(bTrickModeOK && bCanFF);
    m_btnRewind.Enable(bTrickModeOK && bCanRewind);

    m_play.SetButtonImage((UINT)-1, (bPlay ? 0 : 1));
    InvalidateRect(m_play.Window(), NULL, FALSE);
}


//-----------------------------------------------------------------------------
// UpdateSeekBar
// Update the state of the seek bar.
//-----------------------------------------------------------------------------

void MainDialog::UpdateSeekBar()
{
    HRESULT hr = S_OK;
    BOOL bCanSeek = FALSE;
    MFTIME duration = 0;

	// Stop the old timer, if any.
	StopTimer();

    CHECK_HR(hr = m_pSeeking->CanSeek(&bCanSeek));

    // If the player can seek, set the seekbar range and start the timer.
    // Otherwise, disable the seekbar.
	if (bCanSeek)
	{
        CHECK_HR(hr = m_pSeeking->GetDuration(&duration));

	    CHECK_HR(hr = m_seekbar.SetRange(0, (LONG)(duration / ONE_MSEC)));

		m_seekbar.Enable(TRUE);

		// Start the timer
		m_timerID = SetTimer(m_hDlg, IDT_TIMER1, TICK_FREQ, NULL);
	}
	else
	{
		m_seekbar.Enable(FALSE);
	}	


done:
	if (FAILED(hr))
	{
		m_seekbar.Enable(FALSE);
	}
}


//-----------------------------------------------------------------------------
// StopTimer
// Stop the timer.
//-----------------------------------------------------------------------------

void MainDialog::StopTimer()
{
	if (m_timerID != 0)
	{
		KillTimer(m_hDlg, m_timerID);
		m_timerID = 0;
	}
}


//-----------------------------------------------------------------------------
// InitStatusBar
// Initializes the status bar. Called during WM_INITDIALOG.
//-----------------------------------------------------------------------------

HRESULT MainDialog::InitStatusBar()
{
    HRESULT hr = S_OK;
    BOOL result = 0;

    SIZE szTimeCode;
    RECT rcStatusBar;
    HDC hdc = NULL;

    // Create a new status bar.
    CHECK_HR(hr = m_status.Create(m_hDlg, IDC_STATUS_BAR));

    // Find the extent of a string that contains the largest time stamp we're
    // likely to see. (Note this is not the largest time stamp possible.)

    const WCHAR tmp[] = L"0000:00:00";

    hdc = GetDC(m_status.Window());
    if (hdc == NULL)
    {
        CHECK_HR(hr = HRESULT_FROM_WIN32(GetLastError()));
    }

    result = GetTextExtentPoint32(hdc, tmp, ARRAY_SIZE(tmp), &szTimeCode);
    if (!result)
    {
        CHECK_HR(hr = HRESULT_FROM_WIN32(GetLastError()));
    }

    // Get the client rectangle of the status bar.
    GetClientRect(m_status.Window(), &rcStatusBar);

    // Calculate the part sizes. Time stamp goes in the 2d part.
    int iParts[] = { rcStatusBar.right - szTimeCode.cx, -1};

    // Set the parts.
    result = m_status.SetParts(2, iParts);
    if (!result)
    {
        CHECK_HR(hr = HRESULT_FROM_WIN32(GetLastError()));
    }

done:
    if (hdc)
    {
        ReleaseDC(m_status.Window(), hdc);
    }
    return hr;
}


//-----------------------------------------------------------------------------
// SetStatusTime
// Displays a time stamp in the status bar.
//-----------------------------------------------------------------------------

void MainDialog::SetStatusTime(const MFTIME& time)
{
    HRESULT hr = S_OK;
    WCHAR szTimeStamp[32];
    MFTIME hour = 0, min = 0, sec = 0;

    sec = (time / ONE_SECOND);
    min = (time / (ONE_SECOND * 60));
    hour = (time / (ONE_SECOND * 360));

    hr = StringCchPrintf(
        szTimeStamp, 
        ARRAY_SIZE(szTimeStamp), 
        L"%d:%02d:%02d", 
        (DWORD)hour, (DWORD)(min % 60), (DWORD)(sec % 60)
        );

    if (SUCCEEDED(hr))
    {    
        m_status.SetText(1, szTimeStamp);
    }
}



//-----------------------------------------------------------------------------
//  NotifyError
//  Shows a message box with an error message.
//
//  sErrorMessage: NULL-terminated string containing the error message.
//  hrErr: HRESULT from the error.
//-----------------------------------------------------------------------------

void NotifyError(HWND hwnd, const WCHAR *sErrorMessage, HRESULT hrErr)
{
	const size_t MESSAGE_LEN = 512;
	WCHAR message[MESSAGE_LEN];

	HRESULT hr = StringCchPrintf (message, MESSAGE_LEN, L"%s (HRESULT = 0x%X)", sErrorMessage, hrErr);
	if (SUCCEEDED(hr))
	{
		MessageBox(hwnd, message, NULL, MB_OK | MB_ICONERROR);
	}
}




//-----------------------------------------------------------------------------
//  OpenUrlDialogProc
//  Message handler for the "Open URL" window.
//-----------------------------------------------------------------------------

INT_PTR CALLBACK OpenUrlDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static OpenUrlDialogInfo *pUrl = NULL;

    BOOL result = FALSE;

	switch (message)
	{
	case WM_INITDIALOG:
        // The app sends a pointer to an OpenUrlDialogInfo structure as the lParam. 
        // We use this structure to store the URL.
        pUrl = (OpenUrlDialogInfo*)lParam;
		return (INT_PTR)TRUE;

	case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            if (pUrl)
            {
                // Get the URL from the edit box in the dialog.
                // This function allocates memory. The app must call CoTaskMemAlloc.
                HRESULT hr = AllocGetWindowText(GetDlgItem(hDlg, IDC_EDIT_URL), &pUrl->pszURL, &pUrl->cch);
                if (SUCCEEDED(hr))
                {
                    result = TRUE;
                }
            }
            EndDialog(hDlg, result ? IDOK : IDABORT);
            break;

        case IDCANCEL:
            EndDialog(hDlg, LOWORD(IDCANCEL));
            break;
        }
		return (INT_PTR)FALSE;
	}
	return (INT_PTR)FALSE;
}



//-----------------------------------------------------------------------------
//  OFNHookProc
//  Message hook for the Open File dialog.
//
//  Note: 
//  In the OnFileOpen method, we customize the Open File dialog with the 
//  following additional controls:
//  - A check box for setting a custom EVR presenter.
//  - An edit box with a CLSID for a custom presenter. 
//  - A check box for setting a custom EVR mixer.
//  - An edit box with a CLSID for a custom mixer. 
//-----------------------------------------------------------------------------
UINT_PTR CALLBACK OFNHookProc(
    HWND hDlg,
    UINT uiMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    OPENFILENAME    *pOfn = NULL;
    NMHDR           *pHdr = NULL;
    OFNOTIFY        *pOFNotify = NULL;
    EvrConfig       *pEvrConfig = NULL;

    UINT_PTR result = 0;

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        // The OPENFILENAME structure is sent as the lParam.
        // The lCustData member of this structure is an EvrConfig pointer.
        // The EvrConfig pointer contains the initial CLSIDs for the edit boxes.

        pOfn = (OPENFILENAME*)lParam;

        pEvrConfig = (EvrConfig*)pOfn->lCustData;

        // Fill in the edit boxes.
        OFNHookProc_InitEditBox(hDlg, IDC_CLSID_PRESENTER, pEvrConfig->clsidPresenter);
        OFNHookProc_InitEditBox(hDlg, IDC_CLSID_MIXER, pEvrConfig->clsidMixer);
        
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_EVR_PRESENTER:
            // The user clicked the "EVR Presenter" check box.
            OFNHookProc_OnCommand(hDlg, IDC_EVR_PRESENTER, IDC_CLSID_PRESENTER);
            break;

        case IDC_EVR_MIXER:
            // The user clicked the "EVR mixer" check box.
            OFNHookProc_OnCommand(hDlg, IDC_EVR_MIXER, IDC_CLSID_MIXER);
            break;
        }
        break;

    case WM_NOTIFY:
        pHdr = (NMHDR*)lParam;
        if (pHdr->code == CDN_FILEOK)
        {
            // The user clicked OK.

            // The OFNOTIFY structure is sent as the lParam.
            // The lpOFN->lCustData member is an EvrConfig pointer.
            // We need to return the CLSIDs in the EvrConfig structure.

            pOFNotify = (OFNOTIFY*)lParam;

            pEvrConfig = (EvrConfig*)pOFNotify->lpOFN->lCustData;

            OFNHookProc_OnOK(hDlg, IDC_EVR_PRESENTER, IDC_CLSID_PRESENTER, &pEvrConfig->clsidPresenter);
            OFNHookProc_OnOK(hDlg, IDC_EVR_MIXER, IDC_CLSID_MIXER, &pEvrConfig->clsidMixer);
        }
        break;
    }
    return result;
}

//-----------------------------------------------------------------------------
// OFNHookProc_InitEditBox
//
// Initializes one of the edit boxes in the customized Open File dialog.
//
// hDlg: Handle to the Open File dialog.
// nIdEdit: Resource ID of the edit box.
// clsid: CLSID to store in the edit box.
//-----------------------------------------------------------------------------

void OFNHookProc_InitEditBox(HWND hDlg, int nIdEdit, const GUID& clsid)
{
    WCHAR szCLSID[GUID_STR_LEN];

    // Get the window to the edit box.
    HWND hEdit = GetDlgItem(hDlg, nIdEdit);

    if (hEdit != NULL)
    {
        // Convert the CLSID to a string and set the window text.

        StringFromGUID2(clsid, szCLSID, GUID_STR_LEN);
        SetWindowText(hEdit, szCLSID);

        // Disable the edit box. It will be re-enabled if the user checks the 
        // corresponding check box.
        EnableWindow(hEdit, FALSE);
    }
}


//-----------------------------------------------------------------------------
// OFNHookProc_OnCommand
//
// Called when the user checks one of the check boxes in the customized Open 
// File dialog.
//-----------------------------------------------------------------------------

void OFNHookProc_OnCommand(HWND hDlg, int nIdButton, int nIdEdit)
{
    HWND hEdit = GetDlgItem(hDlg, nIdEdit);

    // Enable or disable the corresponding edit box.

    if (IsDlgButtonChecked(hDlg, nIdButton) == BST_CHECKED)
    {
        EnableWindow(hEdit, TRUE);
    }
    else
    {
        EnableWindow(hEdit, FALSE);
    }
}


//-----------------------------------------------------------------------------
// OFNHookProc_OnOK
//
// Called when the user clicks OK on the customized Open File dialog.
//
// hDlg: Handle to the Open File Dialog.
// nIdButton: Resource ID of one of the check boxes.
// nIdEdit: Resource ID of the associated edit box.
// pCLSID: Receives the CLSID for the EVR presenter or mixer.
//
// Note: 
// The CLSID is returned to the application via the OPENFILENAME structure.
//-----------------------------------------------------------------------------

void OFNHookProc_OnOK(HWND hDlg, int nIdButton, int nIdEdit, GUID *pCLSID)
{
    HRESULT hr = S_OK;
    WCHAR szCLSID[GUID_STR_LEN];

    HWND hEdit = GetDlgItem(hDlg, nIdEdit);

    // If the check box is checked, get the CLSID from the edit box.
    // Otherwise, the CLSID is GUID_NULL.

    if (IsDlgButtonChecked(hDlg, nIdButton) == BST_CHECKED)
    {
        if (GetWindowText(hEdit, szCLSID, GUID_STR_LEN) != 0)
        {
            hr = CLSIDFromString(szCLSID, pCLSID);
        }
        if (FAILED(hr))
        {
            // Not a valid CLSID.
            *pCLSID = GUID_NULL;
        }
    }
    else
    {
        *pCLSID = GUID_NULL;
    }
}
