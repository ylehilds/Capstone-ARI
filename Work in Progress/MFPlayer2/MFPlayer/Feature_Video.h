//------------------------------------------------------------------------------
//
// File: Feature_Video.h
// Implements the video feature.
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
// PlayerVideo class
//
// Implements video functionality and exposes the IPlayerVideo interface.
// 
//------------------------------------------------------------------------------

class PlayerVideo : 
    public PlayerFeature<PlayerVideo, IPlayerVideo>, 
    public IPlayerVideo
{
public:

    PlayerVideo() : 
      m_pDisplay(NULL), 
      m_hwnd(NULL), 
      m_fZoom(1.0f), 
      m_clsidPresenter(GUID_NULL), 
      m_clsidMixer(GUID_NULL)
    {

    }

    STDMETHODIMP_(ULONG) AddRef() { return PlayerFeatureImpl::AddRef(); }
    STDMETHODIMP_(ULONG) Release() { return PlayerFeatureImpl::Release(); }
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv)
    {
        return PlayerFeatureImpl::QueryInterface(iid, ppv);
    }


    // IPlayerFeature methods
    STDMETHODIMP Clear();
    STDMETHODIMP RenderStream(REFGUID majorType, IMFTopology *pTopology, IMFTopologyNode *pSourceNode);
    STDMETHODIMP SetTopology(IMFMediaSession *pSession, IMFTopology *pTopology);

    // IPlayerVideo methods
    STDMETHODIMP HasVideo(BOOL *pfHasVideo);
    STDMETHODIMP SetVideoWindow(HWND hwnd);
    STDMETHODIMP SetZoom(float fZoom);
    STDMETHODIMP Repaint();
    STDMETHODIMP SetPresenter(REFCLSID clsid);
    STDMETHODIMP SetMixer(REFCLSID clsid);

private:

    HRESULT SetVideoWindow();
    HRESULT SetZoom();

private:

    IMFVideoDisplayControl  *m_pDisplay;

    HWND                    m_hwnd;             // Destination widow.
    float                   m_fZoom;            // Zoom level (1.0 = 100%)
    GUID                    m_clsidPresenter;   // CLSID of a custom presenter.
    GUID                    m_clsidMixer;       // CLSID of a custom mixer
};