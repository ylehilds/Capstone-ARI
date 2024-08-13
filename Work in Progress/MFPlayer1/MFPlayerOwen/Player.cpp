//------------------------------------------------------------------------------
//
// File: Player.cpp
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

#include "MFPlayer.h"
#include "Player.h"
#include "Feature_Seeking.h"

#pragma warning( push )
#pragma warning( disable : 4355 )  // 'this' used in base member initializer list


//------------------------------------------------------------------------------
//  CreateSamplePlayer function.
//  Function to create an instance of the MFPlayer object.
//
//  hwndEvent: 
//  Handle to an application window that receives player events, in the form of 
//  private window messages. The following messages are defined:
//  - WM_APP_NOTIFY
//  - WM_APP_ERROR
//  - WM_APP_EVENT
//------------------------------------------------------------------------------

STDAPI CreateSamplePlayer(HWND hwndEvent, ISamplePlayer **ppPlayer)
{
    return MFPlayer::CreateInstance(hwndEvent, ppPlayer);
}


/********************************** MFPlayer Class **********************************/


//------------------------------------------------------------------------------
//  CreateInstance.
//  Static method to create an instance of the MFPlayer object.
//
//  The application should call CreateSamplePlayer, which calls this method. 
//------------------------------------------------------------------------------

/*static */ 
HRESULT MFPlayer::CreateInstance(HWND hwndEvent, ISamplePlayer **ppPlayer)
{
    MFPlayer *pPlayer = new MFPlayer();
    if (pPlayer == NULL)
    {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = S_OK;

    hr = pPlayer->Initialize(hwndEvent);
    if (SUCCEEDED(hr))
    {
        *ppPlayer = pPlayer;
        (*ppPlayer)->AddRef();
    }

    SAFE_RELEASE(pPlayer);
    return hr;
}

//------------------------------------------------------------------------------
//  Constructor
//------------------------------------------------------------------------------

MFPlayer::MFPlayer() : 
    m_cRef(1), 
	m_state(Player_Closed),
    m_hwndEvent(NULL),
	m_hCloseEvent(NULL),
    m_pResolver(NULL), 
    m_pCancelToken(NULL),
    m_pSession(NULL), 
    m_pSource(NULL),
    m_OnGetEvent(this, &MFPlayer::OnGetEvent),
    m_OnOpenURL(this, &MFPlayer::OnOpenURL)
{
    ZeroMemory(m_Features, sizeof(m_Features));
}


//------------------------------------------------------------------------------
//  Destructor
//------------------------------------------------------------------------------

MFPlayer::~MFPlayer()
{
    assert(m_pSession == NULL); // If not, the app failed to call Shutdown.

    Shutdown();
}


//------------------------------------------------------------------------------
//  AddRef
//------------------------------------------------------------------------------

ULONG MFPlayer::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}


//------------------------------------------------------------------------------
//  Release
//------------------------------------------------------------------------------

ULONG MFPlayer::Release()
{
    ULONG uCount = InterlockedDecrement(&m_cRef);
    if (uCount == 0)
    {
        delete this;
    }
    return uCount;
}


//------------------------------------------------------------------------------
//  QueryInterface
//------------------------------------------------------------------------------

HRESULT MFPlayer::QueryInterface(REFIID iid, void** ppv)
{
    if (!ppv)
    {
        return E_POINTER;
    }
    if (iid == IID_IUnknown)
    {
        *ppv = static_cast<IUnknown*>(static_cast<ISamplePlayer*>(this));
    }
    else if (iid == __uuidof(ISamplePlayer))
    {
        *ppv = static_cast<ISamplePlayer*>(this);
    }
    else if (iid == __uuidof(IMFSourceOpenMonitor))
    {
        *ppv = static_cast<IMFSourceOpenMonitor*>(this);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}


//------------------------------------------------------------------------------
//  OnGetEvent
//  Callback for asynchronous BeginGetEvent method.
//  This method is called when the media session has a new event. 
//  
//  pResult: Pointer to the async result object.
//
//------------------------------------------------------------------------------

HRESULT MFPlayer::OnGetEvent(IMFAsyncResult *pResult)
{
	HRESULT hr = S_OK;
	IMFMediaEvent* pEvent = NULL;
	MediaEventType meType = MEUnknown;  // Event type
	HRESULT hrStatus = S_OK;	        // Event status

    MF_TOPOSTATUS TopoStatus = MF_TOPOSTATUS_INVALID; // Used with MESessionTopologyStatus event.    

    AutoLock lock(m_critsec);

	// Get the event from the event queue.
	CHECK_HR(hr = m_pSession->EndGetEvent(pResult, &pEvent));

	// Get the event type.
    CHECK_HR(hr = pEvent->GetType(&meType));

	// Get the event status. If the operation that triggered the event did
	// not succeed, the status is a failure code.
    CHECK_HR(hr = pEvent->GetStatus(&hrStatus));

    TRACE((L"%s, hr = 0x%X", EventName(meType), hrStatus));

	// Request another event, unless this is the "session closed" event, which
    // is the last event we'll get for this instance of the media session.
	if (meType != MESessionClosed)
	{
		CHECK_HR(hr = m_pSession->BeginGetEvent(&m_OnGetEvent, NULL));
	}

    // Notify the feature objects about the event.
    for (DWORD i = 0; i < PLAYER_FEATURE_COUNT; i++)
    {
        m_Features[i]->SessionEvent(meType, hrStatus, pEvent);
    }

    // Check if the async operation succeeded.
    if (SUCCEEDED(hrStatus))
    {
        // Switch on the event type. Update the internal state as needed.
        switch(meType)
        {
        case MESessionTopologyStatus:
            // This event includes a status code as an attribute.
            CHECK_HR(hr = pEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, (UINT32*)&TopoStatus));
            switch (TopoStatus)
            {
            case MF_TOPOSTATUS_READY:
                hr = OnTopologyReady(pEvent);
                break;

            default: 
                // Nothing to do.
                break;  
            }
            break;

        case MESessionClosed:
            hr = OnSessionClosed(pEvent);
            break;

        case MESessionEnded:
            hr = OnPresentationEnded(pEvent);
            break;

        case MEAudioSessionVolumeChanged:
            NotifyEvent(Player_VolumeChanged);
            break;

        case MEBufferingStarted:
            OnBuffering(TRUE);
            break;

        case MEBufferingStopped:
            OnBuffering(FALSE);
            break;
        }
    }

done:
    if (FAILED(hr))
    {
        // An error occurred inside this method. Notify the application.
        NotifyError(hr);
    }
    else if (FAILED(hrStatus))
    {
        // The async operation failed. Notify the application.
        NotifyError(hrStatus);
    }

	SAFE_RELEASE(pEvent);
	return hr;
}


//
//
// IMFSourceOpenMonitor methods.
//
//


//------------------------------------------------------------------------------
//  OnSourceEvent
//  Called by the network source when it is opening a URL.
//
//  The network source uses this callback to send events about the connection
//  status while the source resolver is opening a network URL. For more
//  information, see the topic "How to Get Events from the Network Source" in
//  the SDK documentation.
//  
//  pEvent: Pointer to the event.
//
//------------------------------------------------------------------------------

HRESULT MFPlayer::OnSourceEvent(IMFMediaEvent* pEvent)
{
    HRESULT hr = S_OK;
    HRESULT hrStatus = S_OK;
    MediaEventType meType = MEUnknown;

    CHECK_HR(hr = pEvent->GetType(&meType));

    CHECK_HR(hr = pEvent->GetStatus(&hrStatus));

    if (FAILED(hrStatus))
    {
        NotifyError(hrStatus);
    }
    else switch (meType)
    {
        // Notify the application.
        case MEConnectStart:
            NotifyEvent(Player_Connect, TRUE);
            break;

        case MEConnectEnd:
            NotifyEvent(Player_Connect, FALSE);
            break;
    }

done:
    return hr;
}


//
//
// IMFSamplePlayer methods.
//
//

//------------------------------------------------------------------------------
//  GetFeature
//  Returns a player feature object.
//
//  iid: IID of the feature object. (See player.idl for definitions.)
//  ppv: Receives the interface pointer.
//------------------------------------------------------------------------------

HRESULT MFPlayer::GetFeature(REFIID iid, void **ppv)
{
    if (ppv == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    CHECK_HR(hr = CheckShutdown());

    // Return the first feature object that supports the requested
    // interface.

    // NOTE: This is not very efficient, but the number of features
    // is small, and the application gets them once at the beginning.

    for (DWORD id = 0; id < PLAYER_FEATURE_COUNT; id++)
    {
        hr = m_Features[id]->QueryInterface(iid, ppv);
		if (SUCCEEDED(hr))
        {
            break;
        }
    }

done:
    return hr;
}


//------------------------------------------------------------------------------
// GetState
// Returns the current state of the player.
//------------------------------------------------------------------------------

HRESULT MFPlayer::GetState(PlayerState *pState)
{
    if (pState == NULL)
    {
        return E_POINTER;
    }

    AutoLock lock(m_critsec);

    *pState = m_state;

    return S_OK;
}




//------------------------------------------------------------------------------
//  OpenURL
//  Opens a URL for playback.
//
//  Note: This method is asynchronous. When the operation completes, 
//  the OnOpenURL method is called.
//------------------------------------------------------------------------------

HRESULT	MFPlayer::OpenURL(const WCHAR *sURL)
{
	HRESULT hr = S_OK;

    if (sURL == NULL)
    {
        return E_POINTER;
    }

    AutoLock lock(m_critsec);

    CHECK_HR(hr = CheckShutdown());

    if (m_state == Player_OpenPending)
    {
        CancelOpenURL();
        m_state = Player_Closed;
    }

	// Create the media session.
	CHECK_HR(hr = CreateSession());

	// Create the media source.
	CHECK_HR(hr = BeginOpenURL(sURL));

	m_state = Player_OpenPending;

done:
    return hr;
}

//------------------------------------------------------------------------------
//  CancelOpenURL
//  Cancels a pending "OpenURL" operation.
//------------------------------------------------------------------------------

HRESULT MFPlayer::CancelOpenURL()
{
    AutoLock lock(m_critsec);

    HRESULT hr = S_OK;

    if (m_pResolver && m_pCancelToken)
    {
        hr = m_pResolver->CancelObjectCreation(m_pCancelToken);
        SAFE_RELEASE(m_pCancelToken);
        SAFE_RELEASE(m_pResolver);
    }

    m_state = Player_Closed;

    return S_OK;
}


//------------------------------------------------------------------------------
//  Play
//  Starts playback from the current position.
//------------------------------------------------------------------------------

HRESULT	MFPlayer::Play()
{
    HRESULT hr = S_OK;
    PlayerSeeking *pSeek = NULL; // No add-ref

    AutoLock lock(m_critsec);

    // Fail if shut down.
    CHECK_HR(hr = CheckShutdown());

    // Delegate this operation to the seeking feature.
    pSeek = static_cast<PlayerSeeking*>(m_Features[Seeking]);

    CHECK_HR(hr = pSeek->Start());

    m_state = Player_Started;

done:
    return hr;
}


//------------------------------------------------------------------------------
//  Pause
//  Pauses playback.
//------------------------------------------------------------------------------

HRESULT	MFPlayer::Pause()
{
    HRESULT hr = S_OK;
    PlayerSeeking *pSeek = NULL; // No add-ref

    AutoLock lock(m_critsec);

    // Fail if shut down.
    CHECK_HR(hr = CheckShutdown());


    // Delegate this operation to the seeking feature.
    pSeek = static_cast<PlayerSeeking*>(m_Features[Seeking]);

    CHECK_HR(hr = pSeek->Pause());

    m_state = Player_Paused;

done:
    return hr;
}


//------------------------------------------------------------------------------
//  Shutdown
//  Releases resources and shuts down the media session.
//------------------------------------------------------------------------------

HRESULT	MFPlayer::Shutdown()
{
	HRESULT hr = S_OK;

    AutoLock lock(m_critsec);

    if (m_state == Player_Shutdown)
    {
        return S_OK;
    }

    SAFE_RELEASE(m_pResolver);
    SAFE_RELEASE(m_pCancelToken);

	// Close the session. This also closes the media source.
	hr = CloseSession();

    // Release feature objects.
    for (DWORD i = 0; i < PLAYER_FEATURE_COUNT; i++)
    {
        SAFE_RELEASE(m_Features[i]);
    }

	// Shutdown the Media Foundation platform.
	MFShutdown();

	CloseHandle(m_hCloseEvent);

    m_state = Player_Shutdown;

	return hr;
}


//
//
// Private/protected MFPlayer methods.
//
//


//------------------------------------------------------------------------------
//  Initialize
//  Initializes the player object.
//
//  hwndEvent: 
//  Handle to an application window that receives player events. 
//------------------------------------------------------------------------------

HRESULT MFPlayer::Initialize(HWND hwndEvent)
{
    m_hwndEvent = hwndEvent;

    HRESULT hr = S_OK;

    m_hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hCloseEvent == NULL)
	{
		CHECK_HR(hr = HRESULT_FROM_WIN32(GetLastError()));
	}

    // Start the Media Foundation platform.
	CHECK_HR(hr = MFStartup(MF_VERSION));

    // Add all of the feature objects. 
    // Each feature object implements IPlayerFeature. (See player.idl.)
    for (DWORD id = 0; id < PLAYER_FEATURE_COUNT; id++)
    {
        CHECK_HR(hr = CreatePlayerFeature((FeatureID)id, &m_Features[id]));
    }

done:
    return hr;
}


//------------------------------------------------------------------------------
//  BeginOpenURL
//  Starts an asynchronous OpenURL operation.
//
//  When the operation completes, the MFPlayer::OnOpenURL method is called.
//------------------------------------------------------------------------------

HRESULT MFPlayer::BeginOpenURL(const WCHAR *sURL)
{
    HRESULT hr = S_OK;
    PROPVARIANT var;
    PropVariantInit(&var);

    IPropertyStore *pProps = NULL;  
    IUnknown *pUnk = NULL;

    // If we have an old cancel token, discard it.
    SAFE_RELEASE(m_pCancelToken);

    // Create the source resolver.
    if (m_pResolver == NULL)
    {
        CHECK_HR(hr = MFCreateSourceResolver(&m_pResolver));
    }

    // Create the property store, to set the IMFSourceOpenMonitor callback.
    // (See notes for OnSourceEvent)
    CHECK_HR(hr = CreatePropertyStore(&pProps));

    // Set this object as the IUnknown pointer for the property key.
    CHECK_HR(hr = QueryInterface(IID_IUnknown, (void**)&var.punkVal));

    var.vt = VT_UNKNOWN;

    CHECK_HR(hr = pProps->SetValue(MFPKEY_SourceOpenMonitor, var));

    // Ask the source resolver to create the media source.
    CHECK_HR(hr = m_pResolver->BeginCreateObjectFromURL(
        sURL,
        MF_RESOLUTION_MEDIASOURCE,
        pProps,
        &m_pCancelToken,
        &m_OnOpenURL,
        NULL
        ));

done:
    PropVariantClear(&var);
    SAFE_RELEASE(pProps);
    return hr;
}


//------------------------------------------------------------------------------
//  OnOpenURL
//  Completes an asynchronous OpenURL operation.
//  
//  pResult: Pointer to the async result object.
//
//------------------------------------------------------------------------------

HRESULT MFPlayer::OnOpenURL(IMFAsyncResult* pResult)
{
    HRESULT hr = S_OK;
    MF_OBJECT_TYPE type = MF_OBJECT_INVALID;
    
    IMFTopology *pTopology = NULL;
    IUnknown *pUnk = NULL;
    IMFMediaSource *pSource = NULL;

    AutoLock lock(m_critsec);

    if (m_pResolver)
    {
        hr = m_pResolver->EndCreateObjectFromURL(pResult, &type, &pUnk);
    }

    if (hr == E_ABORT)
    {
        // The OpenURL operation was cancelled. Not an error.

        m_state = Player_Closed;
        hr = S_OK;

        NotifyEvent(Player_OpenURLCancelled);

        NotifyState();
    }
    else
    {
        // Check the status of the operation.
        CHECK_HR(hr);

        CHECK_HR(hr = pUnk->QueryInterface(__uuidof(IMFMediaSource), (void**)&pSource));

        m_pSource = pSource;
        m_pSource->AddRef();

        // Create a topology for this media source.
        CHECK_HR(hr = CreateTopology(m_pSource, &pTopology));

        // Queue the topology on the media session. (Asynchronous operation.)
        CHECK_HR(hr = m_pSession->SetTopology(0, pTopology));
    }

done:
    if (FAILED(hr))
    {
        NotifyError(hr);
    }
    SAFE_RELEASE(m_pCancelToken);
    SAFE_RELEASE(pTopology);
    SAFE_RELEASE(pUnk);
    SAFE_RELEASE(pSource);
    return hr;
}


//------------------------------------------------------------------------------
//  CreateSession
//  Creates a new instance of the media session.
//------------------------------------------------------------------------------

HRESULT MFPlayer::CreateSession()
{
    HRESULT hr = S_OK;

	// Close the old session, if any.
	CHECK_HR(hr = CloseSession());

	// Create the media session.
	CHECK_HR(hr = MFCreateMediaSession(NULL, &m_pSession));

	// Start pulling events from the media session
	CHECK_HR(hr = m_pSession->BeginGetEvent(&m_OnGetEvent, NULL));

done:
	return hr;
}

//------------------------------------------------------------------------------
//  CloseSession
//  Closes the media session. Also shuts down and releases the media source.
//------------------------------------------------------------------------------

HRESULT MFPlayer::CloseSession()
{
	HRESULT hr = S_OK;

    // Notify the feature objects.
    for (DWORD i = 0; i < PLAYER_FEATURE_COUNT; i++)
    {
        CHECK_HR(hr = m_Features[i]->Clear());
    }

    // Closes the media session.
    if (m_pSession)
    {
		CHECK_HR(hr = m_pSession->Close());

		// Wait for the close operation to complete.

        // Note: The session will send an MESessionClosed event. This
        // method blocks until the MFPlayer object receives the event
        // (with a timeout so we don't block forever if something goes
        // wrong).

        // After we get the MESessionClosed event, it is safe to
        // shut down the media source.

        // Release the critical section, or it will deadlock.
        m_critsec.Unlock();

        DWORD res = WaitForSingleObject(m_hCloseEvent, 5000);
		if (res == WAIT_TIMEOUT)
		{
			TRACE((L"CloseSession timed out!"));
		}
		
        m_critsec.Lock();
    }

	// Complete shutdown operations.
	if (m_pSource)
	{
		m_pSource->Shutdown();
	}

	if (m_pSession)
	{
		m_pSession->Shutdown();
	}

	SAFE_RELEASE(m_pSource);
	SAFE_RELEASE(m_pSession);

done:
	return hr;
}


//------------------------------------------------------------------------------
//  OnTopologyReady
//  Called when the media session sends the "topology ready" event.
//------------------------------------------------------------------------------

HRESULT MFPlayer::OnTopologyReady(IMFMediaEvent *pEvent)
{
    HRESULT hr = S_OK;
    HRESULT hrStatus = S_OK;

    IMFTopology *pTopology = NULL;
    PROPVARIANT var;
    PropVariantInit(&var);

    // Check the status of the event.
    CHECK_HR(hr = pEvent->GetStatus(&hrStatus));
    CHECK_HR(hr = hrStatus);

    // The event value is a pointer to the resolved topology.
    CHECK_HR(hr = pEvent->GetValue(&var));

    if (var.vt != VT_UNKNOWN)
    {
        CHECK_HR(hr = E_UNEXPECTED);
    }

    CHECK_HR(hr = var.punkVal->QueryInterface(__uuidof(IMFTopology), (void**)&pTopology));
    PropVariantClear(&var);

    // Notify the feature objects that the topology is ready.
    for (DWORD i = 0; i < PLAYER_FEATURE_COUNT; i++)
    {
        CHECK_HR(hr = m_Features[i]->SetTopology(m_pSession, pTopology));
    }

    m_state = Player_Ready;

    // Notify the application
    NotifyState();

done:
    if (FAILED(hr))
    {
        NotifyError(hr);
    }

    SAFE_RELEASE(pTopology);
    PropVariantClear(&var);
    return hr;
}


//------------------------------------------------------------------------------
//  OnSessionClosed
//  Called when the media session sends the "session closed" event.
//------------------------------------------------------------------------------

HRESULT MFPlayer::OnSessionClosed(IMFMediaEvent *pEvent)
{
	// The application thread is waiting on this event, 
    // inside the CloseSession method. 
	SetEvent(m_hCloseEvent);
	return S_OK;
}


//------------------------------------------------------------------------------
//  OnPresentationEnded
//  Called when the media session sends the "session ended" event.
//------------------------------------------------------------------------------

HRESULT MFPlayer::OnPresentationEnded(IMFMediaEvent *pEvent)
{
    // The media session switches to stopped at the end of the 
    // session, so we must update our internal state accordingly.
    m_state = Player_Stopped;
    NotifyState();
    return S_OK;
}


//------------------------------------------------------------------------------
//  OnBuffering
//  Called when the media session sends the "buffering started" or 
//  "buffering stopped" event.
//
//  bBuffering: TRUE if buffering has started, FALSE if buffering has stopped.
//------------------------------------------------------------------------------

HRESULT MFPlayer::OnBuffering(BOOL bBuffering)
{
    if (bBuffering)
    {
        m_state = Player_Buffering;
    }
    else
    {
        m_state = Player_Started;
    }
    NotifyState();
    return S_OK;
}


//------------------------------------------------------------------------------
//  CreateTopology
//  Creates a playback topology from a media source.
//------------------------------------------------------------------------------

HRESULT MFPlayer::CreateTopology(IMFMediaSource *pSource, IMFTopology **ppTopology)
{
	assert(pSource != NULL);

	HRESULT hr = S_OK;

    IMFTopology *pTopology = NULL;
	IMFPresentationDescriptor* pSourcePD = NULL;
	DWORD cSourceStreams = 0;

	// Create a new topology.
	CHECK_HR(hr = MFCreateTopology(&pTopology));

	// Create the presentation descriptor for the media source.
	CHECK_HR(hr = pSource->CreatePresentationDescriptor(&pSourcePD));

	// Get the number of streams in the media source.
	CHECK_HR(hr = pSourcePD->GetStreamDescriptorCount(&cSourceStreams));

	// For each stream, create the topology nodes and add them to the topology.
	for (DWORD i = 0; i < cSourceStreams; i++)
	{
		CHECK_HR(hr = AddBranchToPartialTopology(pSource, pTopology, pSourcePD, i));
	}

	// Return the IMFTopology pointer to the caller.
	*ppTopology = pTopology;
	(*ppTopology)->AddRef();

done:
    SAFE_RELEASE(pTopology);
	SAFE_RELEASE(pSourcePD);
	return hr;
}


//------------------------------------------------------------------------------
//  AddBranchToPartialTopology 
//  Adds a topology branch for one stream.
//------------------------------------------------------------------------------

HRESULT MFPlayer::AddBranchToPartialTopology(
    IMFMediaSource *pSource, 
    IMFTopology *pTopology, 
    IMFPresentationDescriptor *pSourcePD, 
    DWORD iStream
    )
{
	assert(pSource != NULL);
	assert(pTopology != NULL);
	assert(pSourcePD != NULL);

	IMFStreamDescriptor* pSourceSD = NULL;
	IMFTopologyNode* pSourceNode = NULL;
	IMFMediaTypeHandler *pHandler = NULL;

	HRESULT hr = S_OK;
	BOOL fSelected = FALSE;
	GUID guidMajorType = GUID_NULL;

	// Get the stream descriptor for this stream.
	CHECK_HR(hr = pSourcePD->GetStreamDescriptorByIndex(iStream, &fSelected, &pSourceSD));

    // Create the topology branch only if the stream is selected.
    // Otherwise, do nothing.
    if (fSelected)
    {
        // Create a source node for this stream.
        CHECK_HR(hr = CreateSourceStreamNode(pSource, pSourcePD, pSourceSD, &pSourceNode));

        // Add the source node to the topology.
        CHECK_HR(hr = pTopology->AddNode(pSourceNode));

	    // Get the media type handler for the stream.
	    CHECK_HR(hr = pSourceSD->GetMediaTypeHandler(&pHandler));
    	
	    // Get the major media type.
	    CHECK_HR(hr = pHandler->GetMajorType(&guidMajorType));

        // Render the stream. 

        // Note:
        //
        // We delegate this operation to the feature objects. For a given stream, we try 
        // each feature object until one of them successfully builds a branch for that 
        // stream. If none succeed, the operation fails. 
        //
        // The feature objects look at the major type when deciding whether to render
        // a stream. e.g., the video feature renders video streams.
        //
        // A look-up table mapping major type GUIDs to feature objects would be more
        // efficient. However, there is a small number of feature objects, and typically
        // no more than 1-2 active streams.

        for (DWORD i = 0; i < PLAYER_FEATURE_COUNT; i++)
        {
            hr = m_Features[i]->RenderStream(guidMajorType, pTopology, pSourceNode);
            if (SUCCEEDED(hr))
            {
                break;
            }
        }

        if (FAILED(hr))
        {
            (void)pTopology->RemoveNode(pSourceNode);
        }
    }

done:
	SAFE_RELEASE(pSourceSD);
	SAFE_RELEASE(pSourceNode);
    SAFE_RELEASE(pHandler);
	return hr;
}



//------------------------------------------------------------------------------
//  CreateSourceStreamNode
//  Creates a source-stream node for a stream.
//------------------------------------------------------------------------------

HRESULT MFPlayer::CreateSourceStreamNode(
    IMFMediaSource *pSource,
	IMFPresentationDescriptor *pSourcePD, 
	IMFStreamDescriptor *pSourceSD,
	IMFTopologyNode **ppNode
	)
{
	assert(pSource != NULL);
	assert(pSourcePD != NULL);
	assert(pSourceSD != NULL);
	assert(ppNode != NULL);

	HRESULT hr = S_OK;

	IMFTopologyNode *pNode = NULL;

	// Create the source-stream node. 
	CHECK_HR(hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode));

	// Set attribute: Pointer to the media source.
	CHECK_HR(hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pSource));

	// Set attribute: Pointer to the presentation descriptor.
	CHECK_HR(hr = pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pSourcePD));

	// Set attribute: Pointer to the stream descriptor.
	CHECK_HR(hr = pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSourceSD));

	// Return the IMFTopologyNode pointer to the caller.
	*ppNode = pNode;
	(*ppNode)->AddRef();

done:
    SAFE_RELEASE(pNode);
	return hr;
}


#pragma warning( pop )
