

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Wed Apr 11 14:09:46 2012
 */
/* Compiler settings for player.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __player_h_h__
#define __player_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISamplePlayer_FWD_DEFINED__
#define __ISamplePlayer_FWD_DEFINED__
typedef interface ISamplePlayer ISamplePlayer;
#endif 	/* __ISamplePlayer_FWD_DEFINED__ */


#ifndef __IPlayerFeature_FWD_DEFINED__
#define __IPlayerFeature_FWD_DEFINED__
typedef interface IPlayerFeature IPlayerFeature;
#endif 	/* __IPlayerFeature_FWD_DEFINED__ */


#ifndef __IPlayerAudio_FWD_DEFINED__
#define __IPlayerAudio_FWD_DEFINED__
typedef interface IPlayerAudio IPlayerAudio;
#endif 	/* __IPlayerAudio_FWD_DEFINED__ */


#ifndef __IPlayerVideo_FWD_DEFINED__
#define __IPlayerVideo_FWD_DEFINED__
typedef interface IPlayerVideo IPlayerVideo;
#endif 	/* __IPlayerVideo_FWD_DEFINED__ */


#ifndef __IPlayerSeeking_FWD_DEFINED__
#define __IPlayerSeeking_FWD_DEFINED__
typedef interface IPlayerSeeking IPlayerSeeking;
#endif 	/* __IPlayerSeeking_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "mfidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_player_0000_0000 */
/* [local] */ 


STDAPI CreateSamplePlayer(HWND hwndEvent, ISamplePlayer **ppPlayer);
typedef 
enum tagPlayerState
    {	Player_Closed	= 0,
	Player_OpenPending	= ( Player_Closed + 1 ) ,
	Player_Ready	= ( Player_OpenPending + 1 ) ,
	Player_Started	= ( Player_Ready + 1 ) ,
	Player_Paused	= ( Player_Started + 1 ) ,
	Player_Stopped	= ( Player_Paused + 1 ) ,
	Player_Buffering	= ( Player_Stopped + 1 ) ,
	Player_Shutdown	= ( Player_Buffering + 1 ) 
    } 	PlayerState;

typedef 
enum tagPlayerEvent
    {	Player_VolumeChanged	= 0,
	Player_Connect	= ( Player_VolumeChanged + 1 ) ,
	Player_OpenURLCancelled	= ( Player_Connect + 1 ) 
    } 	PlayerEvent;

static const UINT WM_APP_NOTIFY = WM_APP + 1;
static const UINT WM_APP_ERROR = WM_APP + 2;
static const UINT WM_APP_EVENT = WM_APP + 3;


extern RPC_IF_HANDLE __MIDL_itf_player_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_player_0000_0000_v0_0_s_ifspec;

#ifndef __ISamplePlayer_INTERFACE_DEFINED__
#define __ISamplePlayer_INTERFACE_DEFINED__

/* interface ISamplePlayer */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_ISamplePlayer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("982B799F-123F-47cb-A48B-0CE36C010ECC")
    ISamplePlayer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OpenURL( 
            const WCHAR *sURL) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CancelOpenURL( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Play( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Shutdown( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFeature( 
            REFIID iid,
            void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            PlayerState *pState) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISamplePlayerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISamplePlayer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISamplePlayer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISamplePlayer * This);
        
        HRESULT ( STDMETHODCALLTYPE *OpenURL )( 
            ISamplePlayer * This,
            const WCHAR *sURL);
        
        HRESULT ( STDMETHODCALLTYPE *CancelOpenURL )( 
            ISamplePlayer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Play )( 
            ISamplePlayer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Pause )( 
            ISamplePlayer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Shutdown )( 
            ISamplePlayer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFeature )( 
            ISamplePlayer * This,
            REFIID iid,
            void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            ISamplePlayer * This,
            PlayerState *pState);
        
        END_INTERFACE
    } ISamplePlayerVtbl;

    interface ISamplePlayer
    {
        CONST_VTBL struct ISamplePlayerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISamplePlayer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISamplePlayer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISamplePlayer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISamplePlayer_OpenURL(This,sURL)	\
    ( (This)->lpVtbl -> OpenURL(This,sURL) ) 

#define ISamplePlayer_CancelOpenURL(This)	\
    ( (This)->lpVtbl -> CancelOpenURL(This) ) 

#define ISamplePlayer_Play(This)	\
    ( (This)->lpVtbl -> Play(This) ) 

#define ISamplePlayer_Pause(This)	\
    ( (This)->lpVtbl -> Pause(This) ) 

#define ISamplePlayer_Shutdown(This)	\
    ( (This)->lpVtbl -> Shutdown(This) ) 

#define ISamplePlayer_GetFeature(This,iid,ppv)	\
    ( (This)->lpVtbl -> GetFeature(This,iid,ppv) ) 

#define ISamplePlayer_GetState(This,pState)	\
    ( (This)->lpVtbl -> GetState(This,pState) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISamplePlayer_INTERFACE_DEFINED__ */


#ifndef __IPlayerFeature_INTERFACE_DEFINED__
#define __IPlayerFeature_INTERFACE_DEFINED__

/* interface IPlayerFeature */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IPlayerFeature;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4855D850-7BAC-44fc-ADAD-8403B905A85F")
    IPlayerFeature : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetTopology( 
            /* [in] */ IMFMediaSession *pSession,
            /* [in] */ IMFTopology *pTopology) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SessionEvent( 
            MediaEventType type,
            HRESULT hrStatus,
            /* [in] */ IMFMediaEvent *pEvent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenderStream( 
            /* [in] */ REFGUID majorType,
            /* [in] */ IMFTopology *pTopology,
            /* [in] */ IMFTopologyNode *pSourceNode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPlayerFeatureVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPlayerFeature * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPlayerFeature * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPlayerFeature * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetTopology )( 
            IPlayerFeature * This,
            /* [in] */ IMFMediaSession *pSession,
            /* [in] */ IMFTopology *pTopology);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IPlayerFeature * This);
        
        HRESULT ( STDMETHODCALLTYPE *SessionEvent )( 
            IPlayerFeature * This,
            MediaEventType type,
            HRESULT hrStatus,
            /* [in] */ IMFMediaEvent *pEvent);
        
        HRESULT ( STDMETHODCALLTYPE *RenderStream )( 
            IPlayerFeature * This,
            /* [in] */ REFGUID majorType,
            /* [in] */ IMFTopology *pTopology,
            /* [in] */ IMFTopologyNode *pSourceNode);
        
        END_INTERFACE
    } IPlayerFeatureVtbl;

    interface IPlayerFeature
    {
        CONST_VTBL struct IPlayerFeatureVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPlayerFeature_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPlayerFeature_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPlayerFeature_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPlayerFeature_SetTopology(This,pSession,pTopology)	\
    ( (This)->lpVtbl -> SetTopology(This,pSession,pTopology) ) 

#define IPlayerFeature_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IPlayerFeature_SessionEvent(This,type,hrStatus,pEvent)	\
    ( (This)->lpVtbl -> SessionEvent(This,type,hrStatus,pEvent) ) 

#define IPlayerFeature_RenderStream(This,majorType,pTopology,pSourceNode)	\
    ( (This)->lpVtbl -> RenderStream(This,majorType,pTopology,pSourceNode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPlayerFeature_INTERFACE_DEFINED__ */


#ifndef __IPlayerAudio_INTERFACE_DEFINED__
#define __IPlayerAudio_INTERFACE_DEFINED__

/* interface IPlayerAudio */
/* [uuid][object] */ 


EXTERN_C const IID IID_IPlayerAudio;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E9BE9295-8F03-4418-9591-AFDA3A3BBC9D")
    IPlayerAudio : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetVolume( 
            float fLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVolume( 
            /* [retval][out] */ float *pfLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMute( 
            BOOL bMute) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMute( 
            /* [retval][out] */ BOOL *pbMute) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPlayerAudioVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPlayerAudio * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPlayerAudio * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPlayerAudio * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetVolume )( 
            IPlayerAudio * This,
            float fLevel);
        
        HRESULT ( STDMETHODCALLTYPE *GetVolume )( 
            IPlayerAudio * This,
            /* [retval][out] */ float *pfLevel);
        
        HRESULT ( STDMETHODCALLTYPE *SetMute )( 
            IPlayerAudio * This,
            BOOL bMute);
        
        HRESULT ( STDMETHODCALLTYPE *GetMute )( 
            IPlayerAudio * This,
            /* [retval][out] */ BOOL *pbMute);
        
        END_INTERFACE
    } IPlayerAudioVtbl;

    interface IPlayerAudio
    {
        CONST_VTBL struct IPlayerAudioVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPlayerAudio_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPlayerAudio_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPlayerAudio_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPlayerAudio_SetVolume(This,fLevel)	\
    ( (This)->lpVtbl -> SetVolume(This,fLevel) ) 

#define IPlayerAudio_GetVolume(This,pfLevel)	\
    ( (This)->lpVtbl -> GetVolume(This,pfLevel) ) 

#define IPlayerAudio_SetMute(This,bMute)	\
    ( (This)->lpVtbl -> SetMute(This,bMute) ) 

#define IPlayerAudio_GetMute(This,pbMute)	\
    ( (This)->lpVtbl -> GetMute(This,pbMute) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPlayerAudio_INTERFACE_DEFINED__ */


#ifndef __IPlayerVideo_INTERFACE_DEFINED__
#define __IPlayerVideo_INTERFACE_DEFINED__

/* interface IPlayerVideo */
/* [uuid][object] */ 


EXTERN_C const IID IID_IPlayerVideo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4D8B53FD-C3E2-4829-B888-0F8DFBD99855")
    IPlayerVideo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE HasVideo( 
            /* [retval][out] */ BOOL *pfHasVideo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoWindow( 
            HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetZoom( 
            float fZoom) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Repaint( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPresenter( 
            REFCLSID clsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMixer( 
            REFCLSID clsid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPlayerVideoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPlayerVideo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPlayerVideo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPlayerVideo * This);
        
        HRESULT ( STDMETHODCALLTYPE *HasVideo )( 
            IPlayerVideo * This,
            /* [retval][out] */ BOOL *pfHasVideo);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoWindow )( 
            IPlayerVideo * This,
            HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoom )( 
            IPlayerVideo * This,
            float fZoom);
        
        HRESULT ( STDMETHODCALLTYPE *Repaint )( 
            IPlayerVideo * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetPresenter )( 
            IPlayerVideo * This,
            REFCLSID clsid);
        
        HRESULT ( STDMETHODCALLTYPE *SetMixer )( 
            IPlayerVideo * This,
            REFCLSID clsid);
        
        END_INTERFACE
    } IPlayerVideoVtbl;

    interface IPlayerVideo
    {
        CONST_VTBL struct IPlayerVideoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPlayerVideo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPlayerVideo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPlayerVideo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPlayerVideo_HasVideo(This,pfHasVideo)	\
    ( (This)->lpVtbl -> HasVideo(This,pfHasVideo) ) 

#define IPlayerVideo_SetVideoWindow(This,hwnd)	\
    ( (This)->lpVtbl -> SetVideoWindow(This,hwnd) ) 

#define IPlayerVideo_SetZoom(This,fZoom)	\
    ( (This)->lpVtbl -> SetZoom(This,fZoom) ) 

#define IPlayerVideo_Repaint(This)	\
    ( (This)->lpVtbl -> Repaint(This) ) 

#define IPlayerVideo_SetPresenter(This,clsid)	\
    ( (This)->lpVtbl -> SetPresenter(This,clsid) ) 

#define IPlayerVideo_SetMixer(This,clsid)	\
    ( (This)->lpVtbl -> SetMixer(This,clsid) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPlayerVideo_INTERFACE_DEFINED__ */


#ifndef __IPlayerSeeking_INTERFACE_DEFINED__
#define __IPlayerSeeking_INTERFACE_DEFINED__

/* interface IPlayerSeeking */
/* [uuid][object] */ 


EXTERN_C const IID IID_IPlayerSeeking;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1A003F0A-A1D7-42c8-BA23-DFA113D65DD6")
    IPlayerSeeking : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CanSeek( 
            /* [retval][out] */ BOOL *pbCanSeek) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDuration( 
            /* [out] */ MFTIME *phnsDuration) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentPosition( 
            /* [out] */ MFTIME *phnsPosition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPosition( 
            MFTIME hnsPosition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanScrub( 
            /* [retval][out] */ BOOL *pbCanScrub) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Scrub( 
            BOOL bScrub) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanFastForward( 
            /* [retval][out] */ BOOL *pbCanFF) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanRewind( 
            /* [retval][out] */ BOOL *pbCanRewind) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRate( 
            float fRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FastForward( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Rewind( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPlayerSeekingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPlayerSeeking * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPlayerSeeking * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPlayerSeeking * This);
        
        HRESULT ( STDMETHODCALLTYPE *CanSeek )( 
            IPlayerSeeking * This,
            /* [retval][out] */ BOOL *pbCanSeek);
        
        HRESULT ( STDMETHODCALLTYPE *GetDuration )( 
            IPlayerSeeking * This,
            /* [out] */ MFTIME *phnsDuration);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentPosition )( 
            IPlayerSeeking * This,
            /* [out] */ MFTIME *phnsPosition);
        
        HRESULT ( STDMETHODCALLTYPE *SetPosition )( 
            IPlayerSeeking * This,
            MFTIME hnsPosition);
        
        HRESULT ( STDMETHODCALLTYPE *CanScrub )( 
            IPlayerSeeking * This,
            /* [retval][out] */ BOOL *pbCanScrub);
        
        HRESULT ( STDMETHODCALLTYPE *Scrub )( 
            IPlayerSeeking * This,
            BOOL bScrub);
        
        HRESULT ( STDMETHODCALLTYPE *CanFastForward )( 
            IPlayerSeeking * This,
            /* [retval][out] */ BOOL *pbCanFF);
        
        HRESULT ( STDMETHODCALLTYPE *CanRewind )( 
            IPlayerSeeking * This,
            /* [retval][out] */ BOOL *pbCanRewind);
        
        HRESULT ( STDMETHODCALLTYPE *SetRate )( 
            IPlayerSeeking * This,
            float fRate);
        
        HRESULT ( STDMETHODCALLTYPE *FastForward )( 
            IPlayerSeeking * This);
        
        HRESULT ( STDMETHODCALLTYPE *Rewind )( 
            IPlayerSeeking * This);
        
        END_INTERFACE
    } IPlayerSeekingVtbl;

    interface IPlayerSeeking
    {
        CONST_VTBL struct IPlayerSeekingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPlayerSeeking_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPlayerSeeking_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPlayerSeeking_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPlayerSeeking_CanSeek(This,pbCanSeek)	\
    ( (This)->lpVtbl -> CanSeek(This,pbCanSeek) ) 

#define IPlayerSeeking_GetDuration(This,phnsDuration)	\
    ( (This)->lpVtbl -> GetDuration(This,phnsDuration) ) 

#define IPlayerSeeking_GetCurrentPosition(This,phnsPosition)	\
    ( (This)->lpVtbl -> GetCurrentPosition(This,phnsPosition) ) 

#define IPlayerSeeking_SetPosition(This,hnsPosition)	\
    ( (This)->lpVtbl -> SetPosition(This,hnsPosition) ) 

#define IPlayerSeeking_CanScrub(This,pbCanScrub)	\
    ( (This)->lpVtbl -> CanScrub(This,pbCanScrub) ) 

#define IPlayerSeeking_Scrub(This,bScrub)	\
    ( (This)->lpVtbl -> Scrub(This,bScrub) ) 

#define IPlayerSeeking_CanFastForward(This,pbCanFF)	\
    ( (This)->lpVtbl -> CanFastForward(This,pbCanFF) ) 

#define IPlayerSeeking_CanRewind(This,pbCanRewind)	\
    ( (This)->lpVtbl -> CanRewind(This,pbCanRewind) ) 

#define IPlayerSeeking_SetRate(This,fRate)	\
    ( (This)->lpVtbl -> SetRate(This,fRate) ) 

#define IPlayerSeeking_FastForward(This)	\
    ( (This)->lpVtbl -> FastForward(This) ) 

#define IPlayerSeeking_Rewind(This)	\
    ( (This)->lpVtbl -> Rewind(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPlayerSeeking_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


