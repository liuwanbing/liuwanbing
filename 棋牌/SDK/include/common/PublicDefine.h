#ifndef _PUBLIC_DEFINE_H_
#define _PUBLIC_DEFINE_H_

#ifndef FACE_DLL_EXT_CLASS	
	#ifdef AFC_FACE_DLL
		#define FACE_DLL_EXT_CLASS	_declspec(dllexport)
	#else
		#define FACE_DLL_EXT_CLASS	_declspec(dllimport)
	#endif
#endif

#define EXT_CLASS	FACE_DLL_EXT_CLASS

#ifndef FACE_DLL_NAME
#define FACE_DLL_NAME	TEXT("BZW.DLL")
#endif



#endif
