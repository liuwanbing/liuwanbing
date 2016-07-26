#include "stdafx.h"
#include "LoadMoudelHelp.h"

#ifdef SER_WIN
#include "Afxwin.h"
#include "tchar.h"
#include <windows.h> 

typedef IMainGameFactory* (*LPFNDLLFUNC)(void);   
 IMainGameFactory* LoadDllMoudel(char* pMoudelName, BOOL bFindProc)
{
    HINSTANCE hDLL;  
    LPFNDLLFUNC  lpfnDllFunc;
    IMainGameFactory* pGameFactory = NULL;
    hDLL = LoadLibrary(pMoudelName);
    if (hDLL != NULL)
    {
        if (bFindProc)
        {
            lpfnDllFunc = (LPFNDLLFUNC)GetProcAddress(hDLL,
                "GetMainGameFactory");
            
            if (lpfnDllFunc)
            {
                pGameFactory = lpfnDllFunc();
            }
        }
    }
    
    return pGameFactory;
}


#else

#include "dlfcn.h"
#include <stdio.h>
#include <stdlib.h>

#define SOFILE  ("libdoudizu.so")

typedef IMainGameFactory* (*LPFNDLLFUNC)(void);   
IMainGameFactory* LoadDllMoudel(char* pMoudelName,  BOOL bFindProc)
{
   void *dp; 
    char *error;
    IMainGameFactory* pGameFactory = NULL;

    dp = dlopen(pMoudelName,  RTLD_LAZY);

    if (dp == 0)
    {
        printf("cant open so file");
        exit(1);
    }

    if (bFindProc)
    {
        void* p  = dlsym(dp, "GetMainGameFactory");
         
        error = dlerror();
        if((error) || (!p))
        {
            printf("cant find function");
            exit(1);
        }
        pGameFactory =  ((LPFNDLLFUNC)p)();

    }
     //dlclose(dp);
    
    return pGameFactory;
}

#endif

 
