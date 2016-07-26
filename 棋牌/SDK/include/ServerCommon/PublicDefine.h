#ifndef _PUBLIC_DEFINE_H_
#define _PUBLIC_DEFINE_H_

//#ifndef FACE_DLL_NAME
//#define FACE_DLL_NAME	TEXT("BZW.DLL")
//#endif

//∂®“Â ‰≥ˆ
#ifndef EXT_CLASS
	#ifdef  _AFC_COM_DLL_EX
		#define EXT_CLASS _declspec(dllexport)
	#else
		#define EXT_CLASS _declspec(dllimport)
	#endif
#endif

#endif
