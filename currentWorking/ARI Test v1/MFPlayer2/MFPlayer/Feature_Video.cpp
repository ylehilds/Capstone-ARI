//------------------------------------------------------------------------------
//
// File: Feature_Video.cpp
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

#include "MFPlayer.h"
#include "Feature_Base.h"
#include "Feature_Video.h"

// CLSID of the sample EVR presenter. 
// (To use this CLSID, you must build the EVRPresenter sample and register the DLL.)
extern const GUID CLSID_CustomEVRPresenter = 
    { 0x9707fc9c, 0x807b, 0x41e3, { 0x98, 0xa8, 0x75, 0x17, 0x6f, 0x95, 0xa0, 0x62 } };


//------------------------------------------------------------------------------
// Clear
//
// Clears any resources for the current topology.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::Clear()
{
    SAFE_RELEASE(m_pDisplay);
    return S_OK;
}


//------------------------------------------------------------------------------
// SetTopology
//
// Called when the full playback topology is ready.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::SetTopology(IMFMediaSession *pSession, IMFTopology *pTopology)
{
    Clear();

    // Get the video display interface.
    // It's OK if this fails, that just means there is no video.
    MFGetService(pSession, MR_VIDEO_RENDER_SERVICE, 
        __uuidof(IMFVideoDisplayControl), (void**)&m_pDisplay);

    SetVideoWindow();

    return S_OK;
}

//------------------------------------------------------------------------------
// RenderStream
//
// Requests the feature object to render a stream.
//
// Note: The PlayerVideo class renders video streams.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::RenderStream(REFGUID majorType, IMFTopology *pTopology, IMFTopologyNode *pSourceNode)
{
    if (majorType != MFMediaType_Video)
    {
        return MF_E_INVALIDMEDIATYPE;
    }

    HRESULT hr = S_OK;

    IMFActivate *pActivate = NULL;

    // Creates the EVR activation object for the topology.
    CHECK_HR(hr = MFCreateVideoRendererActivate(m_hwnd, &pActivate));

    // If the application specified a custom presenter or mixer for the EVR,
    // set the CLSIDs as attributes on the activation object.

    if (m_clsidPresenter != GUID_NULL)
    {
        CHECK_HR(hr = pActivate->SetGUID(MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_CLSID, m_clsidPresenter));
    }

    if (m_clsidMixer != GUID_NULL)
    {
        CHECK_HR(hr = pActivate->SetGUID(MF_ACTIVATE_CUSTOM_VIDEO_MIXER_ACTIVATE, m_clsidMixer));
    }


    CHECK_HR(hr = ::RenderStream(pTopology, pSourceNode, pActivate));

done:
    SAFE_RELEASE(pActivate);
    return hr;
}


//------------------------------------------------------------------------------
// HasVideo
//
// Queries if there is a video stream. 
//
// Note: Specifically, this method queries if the PlayerVideo object got the
//       IMFVideoDisplayControl interface from the media session.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::HasVideo(BOOL *pfHasVideo)
{
    if (pfHasVideo == NULL)
    {
        return E_POINTER;
    }

    *pfHasVideo = (m_pDisplay != NULL);
    return S_OK;
}


//------------------------------------------------------------------------------
// SetVideoWindow
//
// Sets the video destination window.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::SetVideoWindow(HWND hwnd)
{
    m_hwnd = hwnd;
    return SetVideoWindow();
}


//------------------------------------------------------------------------------
// SetZoom
//
// Sets the zoom level.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::SetZoom(float fZoom)
{
    if (fZoom < 1.0f)
    {
        return E_INVALIDARG;
    }

    if (m_fZoom == fZoom)
    {
        return S_OK; // no-op
    }

    m_fZoom = fZoom;
    return SetZoom();
}


//------------------------------------------------------------------------------
// Repaint
//
// Repaints the window. Call this method when the application window receives
// a WM_PAINT message. (Strictly speaking, this is only necessary when the
// video is paused and DWM is disabled.)
//------------------------------------------------------------------------------

HRESULT PlayerVideo::Repaint()
{
    HRESULT hr = S_OK;

    if (m_pDisplay)
    {
        hr = m_pDisplay->RepaintVideo();
    }

    return hr;
}


//------------------------------------------------------------------------------
// SetPresenter
//
// Sets the CLSID for a custom EVR presenter. The new setting will take effect
// when the next video file is played. For the default presenter, use GUID_NULL.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::SetPresenter(REFCLSID clsid)
{
    m_clsidPresenter = clsid;
    return S_OK;
}


//------------------------------------------------------------------------------
// SetMixer
//
// Sets the CLSID for a custom EVR mixer. The new setting will take effect when
// the next video file is played. For the default mixer, use GUID_NULL.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::SetMixer(REFCLSID clsid)
{
    m_clsidMixer = clsid;
    return S_OK;
}



/// Private methods


//------------------------------------------------------------------------------
// SetVideoWindow
//
// Updates the video window.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::SetVideoWindow()
{
    HRESULT hr = S_OK;
    if (m_pDisplay && m_hwnd)
    {
        hr = m_pDisplay->SetVideoWindow(m_hwnd);

        if (SUCCEEDED(hr))
        {
            hr = SetZoom();
        }
    }
    return hr;
}


//------------------------------------------------------------------------------
// SetZoom
//
// Updates the zoom level.
//------------------------------------------------------------------------------

HRESULT PlayerVideo::SetZoom()
{
    HRESULT hr = S_OK;
    if (m_pDisplay && m_hwnd)
    {
        if (m_fZoom == 1.0f)
        {
            // For 100% zoom, the normalized rectangle is {0,0,1,1}.

            MFVideoNormalizedRect nrcSource = { 0.0f, 0.0f, 1.0f, 1.0f };

            hr = m_pDisplay->SetVideoPosition(&nrcSource, NULL);
        }
        else
        {
            // For higher zoom levels, calculate the normalized rectangle.

            float fMargin = (0.5f - (0.5f / m_fZoom));

            MFVideoNormalizedRect nrcSource = { 
                fMargin, fMargin, (1.0f - fMargin), (1.0f - fMargin)
            };

            hr = m_pDisplay->SetVideoPosition(&nrcSource, NULL);
        }
    }
    return hr;
}




