#include "MFPlayer.h"
#include "Feature_Base.h"
#include "Feature_Audio.h"
#include "Feature_Seeking.h"
#include "Feature_Video.h"


//------------------------------------------------------------------------------
// CreatePlayerFeature
//
// Creates an instance of a feature object.
//
// id: Identifies the feature object to create.
// ppFeature: Receives a pointer to the object's IPlayerFeature interface.
//------------------------------------------------------------------------------

HRESULT CreatePlayerFeature(FeatureID id, IPlayerFeature **ppFeature)
{
    if (ppFeature == NULL)
    {
        return E_NOTIMPL;
    }

    IPlayerFeature *pFeature = NULL;

    switch (id)
    {
    case Audio:
        pFeature = new PlayerAudio();
        break;

    case Seeking:
        pFeature = new PlayerSeeking();
        break;

    case Video:
        pFeature = new PlayerVideo();
        break;

    default:
        return E_INVALIDARG;
    }

    if (pFeature == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppFeature = pFeature;
    return S_OK;
}


//------------------------------------------------------------------------------
// GetPresentationDescriptorFromTopology
//
// Given a topology, returns a pointer to the presentation descriptor.
//------------------------------------------------------------------------------

HRESULT GetPresentationDescriptorFromTopology(
    IMFTopology *pTopology, 
    IMFPresentationDescriptor **ppPD
    )
{
    assert(pTopology != NULL);
    assert(ppPD != NULL);

    HRESULT hr = S_OK;

    IMFCollection *pCollection = NULL;
    IUnknown *pUnk = NULL;
    IMFTopologyNode *pNode = NULL;
    IMFPresentationDescriptor *pPD = NULL;

    // Get the collection of source nodes from the topology.
    CHECK_HR(hr = pTopology->GetSourceNodeCollection(&pCollection));

    // Any of the source nodes should have the PD, so take the first
    // object in the collection.

    CHECK_HR(hr = pCollection->GetElement(0, &pUnk));
    
    CHECK_HR(hr = pUnk->QueryInterface(__uuidof(IMFTopologyNode), (void**)&pNode));
    
    // Get the PD, which is stored as an attribute.
    CHECK_HR(hr = pNode->GetUnknown(
        MF_TOPONODE_PRESENTATION_DESCRIPTOR, 
        __uuidof(IMFPresentationDescriptor),
        (void**)&pPD));

    *ppPD = pPD;
    (*ppPD)->AddRef();

done:
    SAFE_RELEASE(pCollection);
    SAFE_RELEASE(pUnk);
    SAFE_RELEASE(pNode);
    SAFE_RELEASE(pPD);

    return hr;
}



//------------------------------------------------------------------------------
// RenderStream
//
// Creates a branch of a partial topology, given the source node and an 
// activation object for a sink.
//
// This function simply connects the source node to an output node that contains
// the activation object. However, this is a common case when building topology
// branches.
//
//------------------------------------------------------------------------------

HRESULT RenderStream(
    IMFTopology *pTopology, 
    IMFTopologyNode *pSourceNode, 
    IMFActivate *pSinkActivate
    )
{
    HRESULT hr = S_OK;
	IMFTopologyNode *pOutputNode = NULL;

    // Create the output node for the renderer.
	CHECK_HR(hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pOutputNode));

	// Set the IActivate object on the output node.
	CHECK_HR(hr = pOutputNode->SetObject(pSinkActivate));

    // Add the output node to the topology.
    CHECK_HR(hr = pTopology->AddNode(pOutputNode));

    // Connect the source node to the output node.
    CHECK_HR(hr = pSourceNode->ConnectOutput(0, pOutputNode, 0));

done:
    SAFE_RELEASE(pOutputNode);
    return hr;
}