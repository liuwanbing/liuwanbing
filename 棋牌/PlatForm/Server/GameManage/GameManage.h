// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GAMEMANAGE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GAMEMANAGE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GAMEMANAGE_EXPORTS
#define GAMEMANAGE_API __declspec(dllexport)
#else
#define GAMEMANAGE_API __declspec(dllimport)
#endif

// This class is exported from the GameManage.dll
class GAMEMANAGE_API CGameManage {
public:
	CGameManage(void);
	// TODO: add your methods here.
};

extern GAMEMANAGE_API int nGameManage;

GAMEMANAGE_API int fnGameManage(void);
