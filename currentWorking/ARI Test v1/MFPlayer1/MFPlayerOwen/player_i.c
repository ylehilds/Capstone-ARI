

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Jun 01 14:15:33 2012
 */
/* Compiler settings for player.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ISamplePlayer,0x982B799F,0x123F,0x47cb,0xA4,0x8B,0x0C,0xE3,0x6C,0x01,0x0E,0xCC);


MIDL_DEFINE_GUID(IID, IID_IPlayerFeature,0x4855D850,0x7BAC,0x44fc,0xAD,0xAD,0x84,0x03,0xB9,0x05,0xA8,0x5F);


MIDL_DEFINE_GUID(IID, IID_IPlayerAudio,0xE9BE9295,0x8F03,0x4418,0x95,0x91,0xAF,0xDA,0x3A,0x3B,0xBC,0x9D);


MIDL_DEFINE_GUID(IID, IID_IPlayerVideo,0x4D8B53FD,0xC3E2,0x4829,0xB8,0x88,0x0F,0x8D,0xFB,0xD9,0x98,0x55);


MIDL_DEFINE_GUID(IID, IID_IPlayerSeeking,0x1A003F0A,0xA1D7,0x42c8,0xBA,0x23,0xDF,0xA1,0x13,0xD6,0x5D,0xD6);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



