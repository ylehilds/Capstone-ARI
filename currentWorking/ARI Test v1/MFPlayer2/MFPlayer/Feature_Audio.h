//------------------------------------------------------------------------------
//
// File: Feature_Audio.h
// Implements the audio feature.
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
// PlayerAudio class
//
// Implements audio functionality and exposes the IPlayerAudio interface.
// 
//------------------------------------------------------------------------------

class PlayerAudio : 
    public PlayerFeature<PlayerAudio, IPlayerAudio>, 
    public IPlayerAudio
{
public:

    PlayerAudio() : m_pVolume(NULL), m_fLevel(1.0f), m_bMute(FALSE)
    {
    }

    // IUnknown methods
    STDMETHODIMP_(ULONG) AddRef() { return PlayerFeatureImpl::AddRef(); }
    STDMETHODIMP_(ULONG) Release() { return PlayerFeatureImpl::Release(); }
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv)
    {
        return PlayerFeatureImpl::QueryInterface(iid, ppv);
    }

    // IPlayerFeature methods
    STDMETHODIMP Clear();
    STDMETHODIMP SetTopology(IMFMediaSession *pSession, IMFTopology *pTopology);
    STDMETHODIMP SessionEvent(MediaEventType type, HRESULT hrStatus, IMFMediaEvent *pEvent);
    STDMETHODIMP RenderStream(REFGUID majorType, IMFTopology *pTopology, IMFTopologyNode *pSourceNode);

    // IPlayerAudio methods
    STDMETHODIMP SetVolume(float fLevel);
    STDMETHODIMP GetVolume(float *pfLevel);
    STDMETHODIMP SetMute(BOOL bMute);
    STDMETHODIMP GetMute(BOOL *pbMute);

private:
    float                   m_fLevel;   // Audio level.
    BOOL                    m_bMute;    // Is the audio muted?

    IMFSimpleAudioVolume    *m_pVolume;
};