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
#include "windows.h"
#include "fstream"
#include "time.h"
#include "string"
using namespace std;
#pragma once

class MainDialog : public CBaseDialog
{
public:
    MainDialog( string log);
    ~MainDialog();

    // Message handlers

    // WM_INITDIALOG
	HRESULT OnInitDialog(); 
    // WM_COMMAND
    INT_PTR OnCommand(HWND hControl, WORD idControl, WORD msg);
    // WM_NOTIFY
    LRESULT OnNotify(NMHDR *pHdr);
    // Other messages
    INT_PTR OnReceiveMsg(UINT msg, WPARAM wParam, LPARAM lParam);


public:
	 void	log ();
	string	computerTime();

private:

//	HRESULT InitializeControls(int argc, char *argv[], char *envp[]);
    HRESULT InitializeControls();
	void    UpdateUI();
    void    UpdateSeekBar();
    void    StopTimer();
    HRESULT InitStatusBar();
    void    SetStatusText(const WCHAR *szStatus)
    {
        m_status.SetText(0, szStatus);
    }
    void    SetStatusTime(const MFTIME& time);

    // Commands
    BOOL    OnCancel();
	void	OnFileOpen();
	void	OnPlayOpen();
    void    OnOpenURL();
    void    OnScroll(WORD request, WORD position, HWND hControl);
    void    OnSeekbarNotify(const NMSLIDER_INFO *pInfo);
	void    OnVolumeNotify(const NMSLIDER_INFO *pInfo);
    void    OnTimer();
    void    OnMute();
    void    OnPlayOrPause();
	void	MoveLeft();
	void	MoveRight();
	void	MoveUp();
	void	MoveDown();
	void	writeState();

    // Player state notifications
    void    OnPlayerNotify(PlayerState state);
    void    OnPlayerReady();

    // Player events
    void    OnPlayerEvent(PlayerEvent playerEvent, LPARAM lParam);
    void    OnVolumeChanged();


public:
    IPlayerSeeking  *m_pSeeking;
    Trackbar        m_Zoom;
    Slider          m_volume;
	Trackbar		m_Brightness;
	Trackbar		m_Contrast;

private:
    ISamplePlayer   *m_pPlayer;
    IPlayerVideo    *m_pVideo;
    IPlayerAudio    *m_pAudio;
    //IPlayerSeeking  *m_pSeeking;

    // UI Controls
    Slider          m_seekbar;
//    Slider          m_volume;
//    Trackbar        m_Zoom;
    ThemedButton    m_mute;
    ThemedButton    m_play;
    Button          m_btnFastFwd;
    Button          m_btnRewind;
	ThemedButton	m_up;
	ThemedButton	m_down;
	ThemedButton	m_left;
	ThemedButton	m_right;
    StatusBar       m_status;

	UINT_PTR	    m_timerID; 
};


