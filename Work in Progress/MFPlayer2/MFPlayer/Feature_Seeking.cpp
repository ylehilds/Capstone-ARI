//------------------------------------------------------------------------------
//
// File: Feature_Seeking.cpp
// Implements seeking and rate control features.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//------------------------------------------------------------------------------


/*
    Notes:

    The goal of this class is to hide (as much as possuble) two complications
    of the Media Foundation pipeline:

    1. Seeking is an asynchronous operation. That means an application can 
       queue multiple seek requests, causing the UI to run ahead of the actual
       state. For example, if you drag the seekbar forward and then backward, 
       there is a lag while the forward seeks are executed in FIFO order. 

       Therefore, while a seek is in progress, the PlayerSeeking class simply 
       caches new seek requests. Once the pending operation completes, the
       most recent seek request is submitted.

    2. Some rate transitions are not allowed in some transport states (eg,
       forward -> reverse is not allowed while playing). The PlayerSeeking
       class handles this by switching to an allowed state before submitting
       the rate-change request, and then switching back to the previous state.

    To coordinate rate-change requests with transport state, the Player class
    delegates the Stop, Pause, and Play methods to the PlayerSeeking class.
    
*/


#include "MFPlayer.h"
#include "Feature_Base.h"
#include "Feature_Seeking.h"

#define CMD_PENDING      0x01
#define CMD_PENDING_SEEK 0x02
#define CMD_PENDING_RATE 0x04



//------------------------------------------------------------------------------
// PlayerSeeking constructor.
//------------------------------------------------------------------------------

PlayerSeeking::PlayerSeeking() 
    : m_pClock(NULL), 
      m_pSession(NULL), 
      m_pRate(NULL), 
      m_pRateSupport(NULL),
      m_bPending(FALSE)
{
    Clear();
}


//------------------------------------------------------------------------------
// Clear
//
// Clears any resources for the current topology.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::Clear()
{
    m_caps = 0;
    m_bCanScrub = FALSE;

    m_hnsDuration = 0;
    m_fPrevRate = 1.0f;

    m_bPending = FALSE;

    SAFE_RELEASE(m_pClock);
    SAFE_RELEASE(m_pSession);
    SAFE_RELEASE(m_pRate);
    SAFE_RELEASE(m_pRateSupport);

    ZeroMemory(&m_state, sizeof(m_state));
    m_state.command = CmdStop;
    m_state.fRate = 1.0f;

    ZeroMemory(&m_request, sizeof(m_request));
    m_request.command = CmdNone;
    m_request.fRate = 1.0f;

    return S_OK;
}


//------------------------------------------------------------------------------
// SetTopology
//
// Called when the full playback topology is ready.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::SetTopology(IMFMediaSession *pSession, IMFTopology *pTopology)
{
    HRESULT hr = S_OK;
    HRESULT hrTmp = S_OK;   // For non-critical failures.

    IMFClock *pClock = NULL;
    IMFPresentationDescriptor *pPD = NULL;

    Clear();

    // Get the session capabilities. 
    CHECK_HR(hr = pSession->GetSessionCapabilities(&m_caps));

    // Get the presentation descriptor from the topology.
    CHECK_HR(hr = GetPresentationDescriptorFromTopology(pTopology, &pPD));

    // Get the duration from the presentation descriptor (optional)
    (void)pPD->GetUINT64(MF_PD_DURATION, (UINT64*)&m_hnsDuration);

    TRACE((L"Duration: %I64d", m_hnsDuration));

    // Get the presentation clock (optional)
    hrTmp = pSession->GetClock(&pClock);
    if (SUCCEEDED(hrTmp))
    {
        CHECK_HR(hr = pClock->QueryInterface(__uuidof(IMFPresentationClock), (void**)&m_pClock));
    }

    // Get the rate control interface (optional)
    hrTmp = MFGetService(pSession, MF_RATE_CONTROL_SERVICE, __uuidof(IMFRateControl), (void**)&m_pRate);

    // Get the rate support interface (optional)
    if (SUCCEEDED(hrTmp))
    {
        hrTmp = MFGetService(pSession, MF_RATE_CONTROL_SERVICE, __uuidof(IMFRateSupport),
            (void**)&m_pRateSupport);
    }
    if (SUCCEEDED(hrTmp))
    {
        // Check if rate 0 (scrubbing) is supported.
        hrTmp = m_pRateSupport->IsRateSupported(TRUE, 0, NULL);
    }
    if (SUCCEEDED(hrTmp))
    {
        m_bCanScrub = TRUE;
    }

    assert(m_pRate || !m_bCanScrub); // m_bCanScrub must be FALSE if m_pRate is NULL.

    // Cache a pointer to the session.
    m_pSession = pSession;
    m_pSession->AddRef();


done:
    SAFE_RELEASE(pPD);
    SAFE_RELEASE(pClock);
    return hr;
}


//------------------------------------------------------------------------------
// SessionEvent
//
// Called when media session fires an event.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::SessionEvent(MediaEventType type, HRESULT hrStatus, IMFMediaEvent *pEvent)
{
    PROPVARIANT var;
    HRESULT hr = S_OK;

    switch (type)
    {
    case MESessionStarted:
        OnSessionStart(hrStatus);
        break;

    case MESessionStopped:
        OnSessionStop(hrStatus);
        break;

    case MESessionPaused:
        OnSessionPause(hrStatus);
        break;

    case MESessionRateChanged:
        // If the rate change succeeded, we've already got the rate
        // cached. If it failed, try to get the actual rate.
        if (FAILED(hrStatus))
        {
            PropVariantInit(&var);

            hr = pEvent->GetValue(&var);

            if (SUCCEEDED(hr) && (var.vt == VT_R4))
            {
                TRACE((L"OnRateChange: %f", var.fltVal));
                m_state.fRate = var.fltVal;
            }
        }
        break;

    case MESessionEnded:
        OnSessionEnded(hrStatus);
        break;

    case MESessionCapabilitiesChanged:
        // The media session's capabilities changed. Get the updated capabilities.
        m_caps = MFGetAttributeUINT32(pEvent, MF_EVENT_SESSIONCAPS, m_caps);
        break;
    }
    return S_OK;
}


//------------------------------------------------------------------------------
// Start
//
// Starts playback. 
// Note: The Player class delegates Start calls to the PlayerSeeking class.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::Start()
{
    HRESULT hr = S_OK;

    AutoLock lock(m_critsec);

    // If another operation is pending, cache the request.
    // Otherwise, start the media session.
    if (m_bPending)
    {
       m_request.command = CmdStart;
    }
    else
    {
        PROPVARIANT varStart;
        PropVariantInit(&varStart);

        hr =  m_pSession->Start(NULL, &varStart);

        m_state.command = CmdStart;
        m_bPending = CMD_PENDING;
    }
    return hr;
}


//------------------------------------------------------------------------------
// Pause
//
// Pauses playback.
// Note: The Player class delegates Start calls to the PlayerSeeking class.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::Pause()
{
    HRESULT hr = S_OK;

    AutoLock lock(m_critsec);

    // If another operation is pending, cache the request.
    // Otherwise, pause the media session.
    if (m_bPending)
    {
        m_request.command = CmdPause;
    }
    else
    {
        hr =  m_pSession->Pause();

        m_state.command = CmdPause;
        m_bPending = CMD_PENDING;
    }
    return hr;
}


//------------------------------------------------------------------------------
// Stop
//
// Stops playback.
// Note: The Player class delegates Stop calls to the PlayerSeeking class.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::Stop()
{
    HRESULT hr = S_OK;

    AutoLock lock(m_critsec);

    // If another operation is pending, cache the request.
    // Otherwise, stop the media session.
    if (m_bPending)
    {
        m_request.command = CmdStop;
    }
    else
    {
        hr =  m_pSession->Stop();

        m_state.command = CmdStop;
        m_bPending = CMD_PENDING;
    }
    return hr;
}



//------------------------------------------------------------------------------
// CanSeek
//
// Queries whether the current session supports seeking.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::CanSeek(BOOL *pbCanSeek)
{
    if (pbCanSeek == NULL)
    {
        return E_POINTER;
    }

    // Note: Really the MFSESSIONCAP_SEEK cap is sufficient. However,
    // to implement a proper seek bar, the application also needs a
    // duration (to know the seek range) and a presentation clock (to
    // get the current position).

    *pbCanSeek = ( 
        ((m_caps & MFSESSIONCAP_SEEK) == MFSESSIONCAP_SEEK) && 
        (m_hnsDuration > 0) &&
        (m_pClock != NULL)
        );

    return S_OK;
}


//------------------------------------------------------------------------------
// GetDuration
//
// Gets the duration of the current presentation.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::GetDuration(MFTIME *phnsDuration)
{
    if (phnsDuration == NULL)
    {
        return E_POINTER;
    }

    *phnsDuration = m_hnsDuration;

    if (m_hnsDuration == 0)
    {
        return MF_E_NO_DURATION;
    }
    else
    {
        return S_OK;
    }
}


//------------------------------------------------------------------------------
// GetCurrentPosition
//
// Gets the current playback position.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::GetCurrentPosition(MFTIME *phnsPosition)
{
    if (phnsPosition == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    AutoLock lock(m_critsec);

    if (m_pClock == NULL)
    {
        return MF_E_NO_CLOCK;
    }

    // Return, in order:
    // 1. Cached seek request (nominal position).
    // 2. Pending seek operation (nominal position).
    // 3. Presentation time (actual position).

    if (m_request.command == CmdSeek)
    {
        *phnsPosition = m_request.hnsStart;
    }
    else if (m_bPending & CMD_PENDING_SEEK)
    {
        *phnsPosition = m_state.hnsStart;
    }
    else
    {
        hr = m_pClock->GetTime(phnsPosition);
    }

    return hr;
}


//------------------------------------------------------------------------------
// SetPosition
//
// Sets the current playback position.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::SetPosition(MFTIME hnsPosition)
{
    TRACE((L"SetPosition: %I64d", hnsPosition));

    AutoLock lock(m_critsec);

    HRESULT hr = S_OK;

    if (m_bPending)
    {
        // We are currently seeking or changing rates, so just cache this request.
        TRACE((L"Cache request."));
        m_request.command = CmdSeek;
        m_request.hnsStart = hnsPosition;
    }
    else
    {
        hr = SetPositionInternal(hnsPosition);
    }
           
    return hr;
}


//------------------------------------------------------------------------------
// CanScrub
//
// Queries whether the current session supports scrubbing.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::CanScrub(BOOL *pbCanScrub)
{
    if (pbCanScrub == NULL)
    {
        return E_POINTER;
    }

    *pbCanScrub = m_bCanScrub;
    return S_OK;
}


//------------------------------------------------------------------------------
// Scrub
//
// Enables or disables scrubbing.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::Scrub(BOOL bScrub)
{
    // Scrubbing is implemented as rate = 0.

    AutoLock lock(m_critsec);

    if (!m_pRate)
    {
        return MF_E_INVALIDREQUEST;
    }
    if (!m_bCanScrub)
    {
        return MF_E_INVALIDREQUEST;
    }


    HRESULT hr = S_OK;
    
    if (bScrub)
    {
        // Enter scrubbing mode. Cache the current rate.

        if (GetNominalRate() != 0)
        {
            m_fPrevRate = m_state.fRate;
        }

        hr = SetRate(0.0f);
    }
    else
    {
        // Leaving scrubbing mode. Restore the old rate.

        if (GetNominalRate() == 0)
        {
            hr = SetRate(m_fPrevRate);
        }
    }

    return hr;
}


//------------------------------------------------------------------------------
// CanFastForward
//
// Queries whether the current session supports fast-forward.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::CanFastForward(BOOL *pbCanFF)
{
    if (pbCanFF == NULL)
    {
        return E_POINTER;
    }

    *pbCanFF = ((m_caps & MFSESSIONCAP_RATE_FORWARD) == MFSESSIONCAP_RATE_FORWARD);
    return S_OK;
}


//------------------------------------------------------------------------------
// CanFastForward
//
// Queries whether the current session supports rewind (reverse play).
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::CanRewind(BOOL *pbCanRewind)
{
    if (pbCanRewind == NULL)
    {
        return E_POINTER;
    }

    *pbCanRewind = ((m_caps & MFSESSIONCAP_RATE_REVERSE) == MFSESSIONCAP_RATE_REVERSE);
    return S_OK;
}


//------------------------------------------------------------------------------
// FastForward
//
// Switches to fast-forward playback, as follows:
// - If the current rate is < 0 (reverse play), switch to 1x speed.
// - Otherwise, double the current playback rate.
//
// Note: This method is just for convenience; the app could call SetRate().
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::FastForward()
{
    if (!m_pRate)
    {
        return MF_E_INVALIDREQUEST;
    }

    HRESULT hr = S_OK;
    float   fTarget = GetNominalRate() * 2;

    if (fTarget <= 0.0f)
    {
        fTarget = 1.0f;
    }

    hr = SetRate(fTarget);

    return hr;
}


//------------------------------------------------------------------------------
// FastForward
//
// Switches to reverse playback, as follows:
// - If the current rate is > 0 (forward playback), switch to -1x speed.
// - Otherwise, double the current (reverse) playback rate.
//
// Note: This method is just for convenience; the app could call SetRate().
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::Rewind()
{
    if (!m_pRate)
    {
        return MF_E_INVALIDREQUEST;
    }

    HRESULT hr = S_OK;
    float   fTarget = GetNominalRate() * 2;

    if (fTarget >= 0.0f)
    {   
        fTarget = -1.0f;
    }

    hr = SetRate(fTarget);

    return hr;
}


//------------------------------------------------------------------------------
// SetRate
// 
// Sets the playback rate.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::SetRate(float fRate)
{
    TRACE((L"SetRate = %f", fRate));

    HRESULT hr = S_OK;
    BOOL bThin = FALSE;

    AutoLock lock(m_critsec);

    if (fRate == GetNominalRate())
    {
        TRACE((L"No-op"));
        return S_OK; // no-op
    }

    if (m_pRateSupport == NULL)
    {
        return MF_E_INVALIDREQUEST;
    }

    // Check if this rate is supported. Try non-thinned playback first, 
    // then fall back to thinned playback.

    hr = m_pRateSupport->IsRateSupported(FALSE, fRate, NULL);

    if (FAILED(hr))
    {
        bThin = TRUE;
        hr = m_pRateSupport->IsRateSupported(TRUE, fRate, NULL);
    }

    if (FAILED(hr))
    {
        // Unsupported rate.
        return hr;
    }

    // If there is an operation pending, cache the request.
    if (m_bPending)
    {
        m_request.fRate = fRate;
        m_request.bThin = bThin;

        // Remember the current transport state (play, paused, etc), so that we can 
        // restore it after the rate change, if necessary. However, if another 
        // command is already pending, that one takes precedent.

        if (m_request.command == CmdNone)
        {
            m_request.command = m_state.command;
        }

    }
    else
    {
        // No pending operation. Commit the new rate.
        hr  = CommitRateChange(fRate, bThin);
    }

    return hr;

}


/// Protected methods


//------------------------------------------------------------------------------
// SetPositionInternal
// 
// Sets the playback position.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::SetPositionInternal(const MFTIME &hnsPosition)
{

    assert (!m_bPending);

    if (m_pSession == NULL)
    {
        return MF_E_INVALIDREQUEST;
    }

    TRACE((L"SetPositionInternal"));

    HRESULT hr = S_OK;

    PROPVARIANT varStart;
    varStart.vt = VT_I8;
    varStart.hVal.QuadPart = hnsPosition;

    hr =  m_pSession->Start(NULL, &varStart);
    LOG_IF_FAILED(L"IMFMediaSession::Start", hr);

    if (SUCCEEDED(hr))
    {
        // Store the pending state.
        m_state.command = CmdStart;
        m_state.hnsStart = hnsPosition;
        m_bPending = CMD_PENDING_SEEK;
    }

    return hr;
}




//------------------------------------------------------------------------------
// CommitRateChange
// 
// Sets the playback rate.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::CommitRateChange(float fRate, BOOL bThin)
{
    TRACE((L"CommitRateChange(%f, %d)", fRate, bThin));

    TRACE((L"m_state.fRate = %f, m_state.cmd = %d", m_state.fRate, m_state.command));

    assert (!m_bPending);

    // Caller holds the lock.

    HRESULT hr = S_OK;
    MFTIME  hnsSystemTime = 0;
    MFTIME  hnsClockTime = 0;

    Command cmdNow = m_state.command;

    IMFClock *pClock = NULL;

    // Allowed rate transitions:

    // Positive <-> negative:   Stopped
    // Negative <-> zero:       Stopped
    // Postive <-> zero:        Paused or stopped

    if ((fRate > 0 && m_state.fRate <= 0) || (fRate < 0 && m_state.fRate >= 0))
    {
        TRACE((L"Transition to stopped"));

        if (cmdNow == CmdStart)
        {
            // Get the current clock position. This will be the restart time.
            CHECK_HR(hr = m_pSession->GetClock(&pClock));

            hr = pClock->GetCorrelatedTime(0, &hnsClockTime, &hnsSystemTime);

            assert(hnsSystemTime != 0);

            // Stop and set the rate
            CHECK_HR(hr = Stop());

            // Cache Request: Restart from stop.
            m_request.command = CmdSeek;
            m_request.hnsStart = hnsClockTime;
        }
        else if (cmdNow == CmdPause)
        {
            // The current state is paused.

            // For this rate change, the session session must be stopped.
            // However, the session cannot transition back from stopped to paused.
            // Therefore, this rate transition is not supported while paused.

            CHECK_HR(hr = MF_E_UNSUPPORTED_STATE_TRANSITION);
        }
    }
    else if (fRate == 0 && m_state.fRate != 0)
    {
        if (cmdNow != CmdPause)
        {
            TRACE((L"Transition to paused"));

            // This transisition requires the paused state.

            // Pause and set the rate.
            CHECK_HR(hr = Pause());

            // Request: Switch back to current state.
            m_request.command = cmdNow;
        }
    }

    // Set the rate.
    CHECK_HR(hr = m_pRate->SetRate(bThin, fRate));

    // Adjust our current rate and requested rate. 
    m_request.fRate = m_state.fRate = fRate;

done:

    if (FAILED(hr))
    {
        LOG_IF_FAILED(L"CommitRateChange failed", hr);
    }

    SAFE_RELEASE(pClock);
    return hr;
}


//------------------------------------------------------------------------------
// GetNominalRate
// 
// Returns the nominal playback rate.
//------------------------------------------------------------------------------

float PlayerSeeking::GetNominalRate()
{
    return m_request.fRate; 
}



//------------------------------------------------------------------------------
// OnSessionStart
// 
// Called when playback starts or restarts.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::OnSessionStart(HRESULT hrStatus)
{
    HRESULT hr = S_OK;

    TRACE((L"OnSessionStart (hr = 0x%x)", hrStatus));

    if (FAILED(hrStatus))
    {
        return hrStatus;
    }

    // The Media Session completed a start/seek operation. Check if there
    // is another seek request pending.
    UpdatePendingCommands(CmdStart);

    return hr;
}


//------------------------------------------------------------------------------
// OnSessionStop
// 
// Called when playback stops.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::OnSessionStop(HRESULT hrStatus)
{
    HRESULT hr = S_OK;

    TRACE((L"OnSessionStop (hr = 0x%x)", hrStatus));

    if (FAILED(hrStatus))
    {
        return hrStatus;
    }


    // The Media Session completed a transition to stopped. This might occur
    // because we are changing playback direction (forward/rewind). Check if
    // there is a pending rate-change request.

    UpdatePendingCommands(CmdStop);

    return hr;
}



//------------------------------------------------------------------------------
// OnSessionPause
// 
// Called when playback pauses.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::OnSessionPause(HRESULT hrStatus)
{
    HRESULT hr = S_OK;

    if (FAILED(hrStatus))
    {
        return hrStatus;
    }

    hr = UpdatePendingCommands(CmdPause);

    return hr;
}



//------------------------------------------------------------------------------
// OnSessionEnded
// 
// Called when the session ends.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::OnSessionEnded(HRESULT hr)
{
    // After the session ends, playback starts from position zero. 
    // But if the current playback rate is reversed, then playback would end
    // immediately (reversing from position 0). Therefore, in this case reset
    // the rate to 1x.

    if (GetNominalRate() < 0.0f)
    {
        m_state.command = CmdStop;

        hr = CommitRateChange(1.0f, FALSE);
    }

    return hr;
}


//------------------------------------------------------------------------------
// UpdatePendingCommands
// 
// Called after an operation completes. 
// This method executes any cached requests.
//------------------------------------------------------------------------------

HRESULT PlayerSeeking::UpdatePendingCommands(Command cmd)
{
    HRESULT hr = S_OK;

    PROPVARIANT varStart;
    PropVariantInit(&varStart);

    AutoLock lock(m_critsec);

    TRACE((L"UpdatePendingCommands (cmd = %d / state.cmd = %d / request.cmd = %d)", 
        cmd, m_state.command, m_request.command));

    if (m_bPending && m_state.command == cmd)
    {
        m_bPending = FALSE;

        // The current pending command has completed.

        // First look for rate changes.
        if (m_request.fRate != m_state.fRate)
        {
            TRACE((L"m_request.fRate = %f; m_state.fRate = %f", m_request.fRate, m_state.fRate));
            CHECK_HR(hr = CommitRateChange(m_request.fRate, m_request.bThin));
        }
        
        // Now look for seek requests.
        if (!m_bPending)
        {
            switch (m_request.command)
            {
            case CmdNone:
                // Nothing to do.
                break;

            case CmdStart:
                Start();
                break;

            case CmdPause:
                Pause();
                break;

            case CmdStop:
                Stop();
                break;

            case CmdSeek:
                SetPositionInternal(m_request.hnsStart);
                break;
            }

            m_request.command = CmdNone;
        }
    }

done:
    return hr;
}



