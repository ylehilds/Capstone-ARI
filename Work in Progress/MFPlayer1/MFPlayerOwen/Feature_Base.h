//------------------------------------------------------------------------------
//
// File: Feature_Base.h
// Base implementation of IPlayerFeature.
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

// FeatureID:
// Identifies the various feature objects.
enum FeatureID
{
    Audio = 0,
    Video,
    Seeking,
    FeatureID_LAST
};

const DWORD PLAYER_FEATURE_COUNT = FeatureID_LAST;  // Total number of features

// Function declarations,
HRESULT CreatePlayerFeature(FeatureID id, IPlayerFeature **ppFeature);

HRESULT GetPresentationDescriptorFromTopology(
    IMFTopology *pTopology, 
    IMFPresentationDescriptor **ppPD
    );

HRESULT RenderStream(
    IMFTopology *pTopology, 
    IMFTopologyNode *pSourceNode, 
    IMFActivate *pSinkActivate
    );

//------------------------------------------------------------------------------
// PlayerFeature class template
//
// Base implementation of IPlayerFeature.
//
// DERIVED: Name of the derived class.
// IFACE: Name of the IPlayerXXXX interface implemented by the derived class.
// 
//------------------------------------------------------------------------------

template <class DERIVED, class IFACE>
class PlayerFeature : public IPlayerFeature
{
protected:
    typedef PlayerFeature<DERIVED, IFACE> PlayerFeatureImpl;

public:

    PlayerFeature() : m_cRef(1)
    {
    }
    virtual ~PlayerFeature()
    {
        Clear();
    }
    
    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }
    STDMETHODIMP_(ULONG) Release()
    {
        ULONG uCount = InterlockedDecrement(&m_cRef);
        if (uCount == 0)
        {
            delete this;
        }
        return uCount;
    }
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv)
    {
        if (!ppv)
        {
            return E_POINTER;
        }
        if (iid == __uuidof(IUnknown))
        {
            *ppv = static_cast<IUnknown*>(static_cast<IPlayerFeature*>(this));
        }
        else if (iid == __uuidof(IPlayerFeature))
        {
            *ppv = static_cast<IPlayerFeature*>(this);
        }
        else if (iid == __uuidof(IFACE))
        {
            *ppv = static_cast<IFACE*>(reinterpret_cast<DERIVED*>(this));
        }
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }

    virtual STDMETHODIMP SetTopology(IMFMediaSession *pSession, IMFTopology *pTopology)
    {
        return S_OK;
    }
    virtual STDMETHODIMP Clear()
    {
        return S_OK;
    }

    virtual STDMETHODIMP SessionEvent(MediaEventType type, HRESULT hrStatus, IMFMediaEvent *pEvent)
    {
        return S_OK;
    }

    virtual STDMETHODIMP RenderStream(REFGUID majorType, IMFTopology *pTopology, IMFTopologyNode *pSourceNode)
    {
        return E_NOTIMPL;
    }


private:
    long m_cRef;    // Reference count.
};

