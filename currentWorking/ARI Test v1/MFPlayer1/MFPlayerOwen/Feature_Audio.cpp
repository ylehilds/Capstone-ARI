#include "MFPlayer.h"
#include "Feature_Base.h"
#include "Feature_Audio.h"


//------------------------------------------------------------------------------
// Clear
//
// Clears any resources for the current topology.
//------------------------------------------------------------------------------

HRESULT PlayerAudio::Clear()
{
    SAFE_RELEASE(m_pVolume);
    return S_OK;
}


//------------------------------------------------------------------------------
// SetTopology
//
// Called when the full playback topology is ready.
//------------------------------------------------------------------------------

HRESULT PlayerAudio::SetTopology(IMFMediaSession *pSession, IMFTopology *pTopology)
{
    Clear();

    // Get the audio volume interface.
    // It's OK if this fails, that just means there is no audio.
    MFGetService(pSession, MR_POLICY_VOLUME_SERVICE, 
        __uuidof(IMFSimpleAudioVolume), (void**)&m_pVolume);

    // Apply the current volume and mute settings.
    SetVolume(m_fLevel);
    SetMute(m_bMute);

    return S_OK;
}


//------------------------------------------------------------------------------
// SessionEvent
//
// Called when media session fires an event.
//------------------------------------------------------------------------------

HRESULT PlayerAudio::SessionEvent(
    MediaEventType type, 
    HRESULT hrStatus, 
    IMFMediaEvent *pEvent
    )
{
    switch (type)
    {
    case MEAudioSessionVolumeChanged:
        // The audio volume changed. (e.g., The user adjusted the audio level
        // in the volume mixer control.) Get the new settings.
        if (m_pVolume)
        {
            m_pVolume->GetMasterVolume(&m_fLevel);
            m_pVolume->GetMute(&m_bMute);
        }
        break;
    }

    return S_OK;
}


//------------------------------------------------------------------------------
// RenderStream
//
// Requests the feature object to render a stream.
//
// Note: The PlayerAudio class renders audio streams.
//------------------------------------------------------------------------------

HRESULT PlayerAudio::RenderStream(
    REFGUID majorType, 
    IMFTopology *pTopology, 
    IMFTopologyNode *pSourceNode
    )
{
    if (majorType != MFMediaType_Audio)
    {
        // Not an audio stream. Tell the player that we don't support this
        // media type. The player object will try to render the stream with
        // a different feature object.
        return MF_E_INVALIDMEDIATYPE;   
    }

    HRESULT hr = S_OK;

    IMFActivate *pActivate = NULL;

    // For audio, create the SAR activation object.
    CHECK_HR(hr = MFCreateAudioRendererActivate(&pActivate));

    CHECK_HR(hr = ::RenderStream(pTopology, pSourceNode, pActivate));

done:
    SAFE_RELEASE(pActivate);
    return hr;
}


//------------------------------------------------------------------------------
// SetVolume
//
// Sets the audio volume.
//------------------------------------------------------------------------------

HRESULT PlayerAudio::SetVolume(float fLevel)
{
    HRESULT hr = S_OK;
    if (m_pVolume)
    {
        hr = m_pVolume->SetMasterVolume(fLevel);
        if (SUCCEEDED(hr))
        {
            m_fLevel = fLevel;
        }
    }
    else
    {
        // No volume control, just cache the value.
        m_fLevel = fLevel;
    }
    return hr;
}

//------------------------------------------------------------------------------
// GetVolume
//
// Gets the current audio volume.
//------------------------------------------------------------------------------

HRESULT PlayerAudio::GetVolume(float *pfLevel)
{
    if (pfLevel == NULL)
    {
        return E_POINTER;
    }
    *pfLevel = m_fLevel;
    return S_OK;
}


//------------------------------------------------------------------------------
// SetMute
//
// Mutes or un-mutes the audio.
//------------------------------------------------------------------------------

HRESULT PlayerAudio::SetMute(BOOL bMute) 
{ 
    HRESULT hr = S_OK;
    if (m_pVolume)
    {
        hr = m_pVolume->SetMute(bMute);
        if (SUCCEEDED(hr))
        {
            m_bMute = bMute;
        }
    }
    else
    {
        // No volume control, just cache the value.
        m_bMute = bMute;
    }
    return hr;
}



//------------------------------------------------------------------------------
// GetMute
//
// Gets the current mute state.
//------------------------------------------------------------------------------

HRESULT PlayerAudio::GetMute(BOOL *pbMute)  
{ 
    if (pbMute == NULL)
    {
        return E_POINTER;
    }
    *pbMute = m_bMute;
    return S_OK;
}

