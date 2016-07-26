#ifndef _BZ_COMMON_H_INCLUDE
#define _BZ_COMMON_H_INCLUDE

//∂®“Â ‰≥ˆ
#ifndef EXT_CLASS
	#ifdef  BZW_CRASH_RPT_DLL
		#define EXT_CLASS _declspec(dllexport)
	#else
		#define EXT_CLASS _declspec(dllimport)
	#endif
#endif

#define IDD_PROJECT_NAME "MyProject"

#endif
