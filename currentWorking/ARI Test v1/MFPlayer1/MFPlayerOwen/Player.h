//------------------------------------------------------------------------------
//
// File: Player.h
// Description: Implements the sample player object.
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

#include "Player_h.h"
#include "Feature_Base.h"

typedef ComPtrList<IPlayerFeature> FeatureList;

// CreateSamplePlayer: Creates the player object.
STDAPI CreateSamplePlayer(HWND hwndEvent, ISamplePlayer **ppPlayer);


//------------------------------------------------------------------------------
// MFPlayer class
//
// Implements basic playback functionality, and provides access to the
// feature-level objects. 
//
// This class exposes the ISamplePlayer interface, defined in player.idl.
//
// All UI aspects are handled by the MainDialog class.
//
// One purpose of the MFPlayer class is to isolate the UI code from the code
// that calls Media Foundation APIs. Hence, some Media Foundation APIs are
// translated into "Sample Player" APIs as defined by ISamplePlayer.
//------------------------------------------------------------------------------

class MFPlayer : public ISamplePlayer, public IMFSourceOpenMonitor
{
public:
	static HRESULT CreateInstance(HWND hwndEvent, ISamplePlayer **ppPlayer);

    // IUnknown methods
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);

	// ISamplePlayer methods
	STDMETHODIMP    OpenURL(const WCHAR *sURL);
    STDMETHODIMP    CancelOpenURL();
    STDMETHODIMP    Play();
	STDMETHODIMP    Pause();
	STDMETHODIMP    Shutdown();
    STDMETHODIMP    GetFeature(REFIID iid, void **ppv);
	STDMETHODIMP    GetState(PlayerState *pState);

    // IMFSourceOpenMonitor
    STDMETHODIMP    OnSourceEvent(IMFMediaEvent* pEvent);

    // Callbacks for asynchronous operations.
	HRESULT         OnGetEvent(IMFAsyncResult* pAsyncResult);
    HRESULT         OnOpenURL(IMFAsyncResult* pAsyncResult);

protected:
	// Constructor is private. Call CreateSamplePlayer to instantiate.
    MFPlayer();

    // Destructor is private. Application must call Release().
    virtual ~MFPlayer();

    // ChechShutdown: Fails if the state is "shutdown".
    HRESULT CheckShutdown()
    {
        return (m_state == Player_Shutdown ? MF_E_SHUTDOWN : S_OK);
    }

    HRESULT Initialize(HWND hwndEvent);

    HRESULT BeginOpenURL(const WCHAR *sURL);

	HRESULT CreateSession();
	HRESULT CloseSession();

	// NotifyState: Notifies the application when the state changes.
	void NotifyState()
	{
		PostMessage(m_hwndEvent, WM_APP_NOTIFY, (WPARAM)m_state, (LPARAM)0);
	}

	// NotifyError: Notifies the application when an error occurs.
	void NotifyError(HRESULT hr)
	{
        m_state = Player_Closed;
		PostMessage(m_hwndEvent, WM_APP_ERROR, (WPARAM)hr, 0);
	}
	
    // NotifyEvent: Notifies the application some event occurs.
	void NotifyEvent(PlayerEvent ev, LPARAM lParam = 0)
	{
		PostMessage(m_hwndEvent, WM_APP_EVENT, (WPARAM)ev, (LPARAM)lParam);
	}


	// Media event handlers
	HRESULT OnTopologyReady(IMFMediaEvent *pEvent);
	HRESULT OnSessionClosed(IMFMediaEvent *pEvent);
	HRESULT OnPresentationEnded(IMFMediaEvent *pEvent);
    HRESULT OnBuffering(BOOL bBuffering);

    // Topology building.
    HRESULT CreateTopology(IMFMediaSource *pSource, IMFTopology **ppTopology);

    HRESULT AddBranchToPartialTopology(
        IMFMediaSource *pSource, 
        IMFTopology *pTopology, 
        IMFPresentationDescriptor *pSourcePD, 
        DWORD iStream
        );

    HRESULT CreateSourceStreamNode(
        IMFMediaSource *pSource,
	    IMFPresentationDescriptor *pSourcePD, 
	    IMFStreamDescriptor *pSourceSD,
	    IMFTopologyNode **ppNode
	    );

private:
    volatile long           m_cRef;        // Reference count.

    IPlayerFeature          *m_Features[PLAYER_FEATURE_COUNT];  // List of feature objects.

    IMFSourceResolver       *m_pResolver;
    IUnknown                *m_pCancelToken;

	IMFMediaSession			*m_pSession;
	IMFMediaSource			*m_pSource;


    CritSec                 m_critsec;  

	HWND				    m_hwndEvent;		// App window to receive events.
	PlayerState			    m_state;			// Current state of the media session.
	HANDLE				    m_hCloseEvent;		// Event to wait on while closing

    // Callbacks
    AsyncCallback<MFPlayer> m_OnGetEvent;
    AsyncCallback<MFPlayer> m_OnOpenURL;

};

