//------------------------------------------------------------------------------
//
// File: Feature_Seeking.h
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

#pragma once

//------------------------------------------------------------------------------
// PlayerSeeking class
//
// Implements seeking and rate control functionality and exposes the 
// IPlayerSeeking interface.
// 
//------------------------------------------------------------------------------

class PlayerSeeking : 
    public PlayerFeature<PlayerSeeking, IPlayerSeeking>, 
    public IPlayerSeeking
{
public:

    PlayerSeeking();

    STDMETHODIMP_(ULONG) AddRef() { return PlayerFeatureImpl::AddRef(); }
    STDMETHODIMP_(ULONG) Release() { return PlayerFeatureImpl::Release(); }
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv)
    {
        return PlayerFeatureImpl::QueryInterface(iid, ppv);
    }

    // IPlayerFeature
    STDMETHODIMP SetTopology(IMFMediaSession *pSession, IMFTopology *pTopology);
    STDMETHODIMP Clear();
    STDMETHODIMP SessionEvent(MediaEventType type, HRESULT hrStatus, IMFMediaEvent *pEvent);

    // IPlayerSeeking
    STDMETHODIMP CanSeek(BOOL *pbCanSeek);
    STDMETHODIMP GetDuration(MFTIME *phnsDuration);
    STDMETHODIMP GetCurrentPosition(MFTIME *phnsPosition);
    STDMETHODIMP SetPosition(MFTIME hnsPosition);

    STDMETHODIMP CanScrub(BOOL *pbCanScrub);
    STDMETHODIMP Scrub(BOOL bScrub);

    STDMETHODIMP CanFastForward(BOOL *pbCanFF);
    STDMETHODIMP CanRewind(BOOL *pbCanRewind);
    STDMETHODIMP SetRate(float fRate);
    STDMETHODIMP FastForward();
    STDMETHODIMP Rewind();

    // Methods called by the Player object.
    HRESULT Start();
    HRESULT Pause();
    HRESULT Stop();

private:

    enum Command 
    { 
        CmdNone = 0,
        CmdStop, 
        CmdStart, 
        CmdPause,
        CmdSeek,
    };


    HRESULT SetPositionInternal(const MFTIME &hnsPosition);
    HRESULT CommitRateChange(float fRate, BOOL bThin);
    float   GetNominalRate();

    HRESULT OnSessionStart(HRESULT hr);
    HRESULT OnSessionStop(HRESULT hr);
    HRESULT OnSessionPause(HRESULT hr);
    HRESULT OnSessionEnded(HRESULT hr);

    HRESULT UpdatePendingCommands(Command cmd);

private:

    // SeekState: Describes the current or requested
    // state, with respect to seeking and playback rate.
    struct SeekState
    {
        Command command;
        float   fRate;      // Playback rate
        BOOL    bThin;      // Thinned playback?
        MFTIME  hnsStart;   // Start position
    };

    BOOL        m_bPending;             // Is a request pending?

    SeekState   m_state;                // Current nominal state.
    SeekState   m_request;              // Pending request.
    
    CritSec     m_critsec;              // Protects the seeking and rate-change states.

    DWORD       m_caps;                 // Session caps.
    BOOL        m_bCanScrub;            // Does the current session support rate = 0.

    MFTIME      m_hnsDuration;          // Duration of the current presentation.
    float       m_fPrevRate;
  
    IMFMediaSession         *m_pSession;
    IMFRateControl          *m_pRate;
    IMFRateSupport          *m_pRateSupport;
    IMFPresentationClock    *m_pClock;
};