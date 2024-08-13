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
    MainDialog();
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
	 void	stateLog ();
	string	computerTime();

private:

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
	void    OnPlayOpen();
    void    OnOpenURL();
    void    OnScroll(WORD request, WORD position, HWND hControl);
    void    OnSeekbarNotify(const NMSLIDER_INFO *pInfo);
	void    OnVolumeNotify(const NMSLIDER_INFO *pInfo);
    void    OnTimer();
    void    OnMute();
    void    OnPlayOrPause();
	void	OnPlay();
	void	OnPause();

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

private:
    ISamplePlayer   *m_pPlayer;
    IPlayerVideo    *m_pVideo;
    IPlayerAudio    *m_pAudio;

    // UI Controls
    Slider          m_seekbar;
    ThemedButton    m_mute;
    ThemedButton    m_play;
	ThemedButton	m_brightness;
    Button          m_btnFastFwd;
    Button          m_btnRewind;
    StatusBar       m_status;

	UINT_PTR	    m_timerID; 
};


