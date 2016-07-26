#include "stdafx.h"
#include "SoGameSettingImpl.h"
#include "SoIddeFine.h"
#include "SoDeskLogicImpl.h"
#include "ComConfig.h"

using namespace MyGame;

#pragma message("here!******************************************************************************")


//下面的代码还没有实现，在后面再实现吧
SoGameSettingImpl::SoGameSettingImpl(CDeskLogicImpl* pDeskLogicImpl)
{
    m_pDeskLogicImpl = pDeskLogicImpl;
    m_pDeskLogicImpl->m_pGameFactory->RegisterInterface(MODULE_GAMESETTING, this);

    memset(m_SectName, 0, sizeof(m_SectName));
    m_bSectBeable = FALSE;
}

SoGameSettingImpl::~SoGameSettingImpl(void)
{
    
}

BOOL SoGameSettingImpl::SetCfgPath(char *lpszCfgPath)
{
    BOOL bRet = FALSE;
    char  cuPath[256];
    ZeroMemory(cuPath, sizeof(cuPath));
    DWORD dwLength = GetModuleFileNameA(GetModuleHandle(NULL), cuPath, sizeof(cuPath));
    char *p = _tcsrchr(cuPath, TEXT('\\'));
    *p = '\0';
    strcat(cuPath,"/");
    strcat(cuPath, lpszCfgPath);

    ComConfig* pConfig = ComConfig::GetInstance();

    /*看看是否是同一个配置文件，不然的话就要重新加载文件，一般是在开机时用*/
    bRet = pConfig->PaserConfigPath(cuPath);
   
    /*这里调用一下开机配置文件回调*/
    if (bRet == TRUE)
    {
        FireEvent(&ISoGameSettingEvent ::OnLoadCfg);
    }

    return bRet;
}

BOOL SoGameSettingImpl::GetItemValue(char *lpszName, char &nValue, int nDefaultValue)
{
    int  nValue32= 0;

    BOOL ret =  GetItemValue(lpszName, nValue32, nDefaultValue);

    nValue = (char)nValue32;
    return ret;
}

BOOL SoGameSettingImpl::GetItemValue(char *lpszName, int &nValue, int nDefaultValue)
{
    char szSectName[32];
    char szValueName[32];

    memset(szSectName, 0,  sizeof(szSectName));
    memset(szValueName, 0,  sizeof(szValueName));
    
    //解析里面有没有"/"
    if (PaserSectName(lpszName, szSectName, szValueName) == FALSE)
    {
        return FALSE;
    }

    if (m_bSectBeable)
    {
        //如果内部已有段名字的设定，所以直接用他们
        return ComConfig::GetInstance()->GetItemValue(m_SectName, szValueName, nValue, nDefaultValue);
    }
    else
    {   
        //内部没有段名字，所以直接用normal字段的属性,内部会区别这两种
        return ComConfig::GetInstance()->GetItemValue(szSectName, szValueName, nValue, nDefaultValue);
    }

    return FALSE;
}

BOOL SoGameSettingImpl::GetItemValue(char *lpszName,  char *lpszReturnedString, int nSize, char* pszDefaultValue)
{
    char szSectName[32];
    char szValueName[32];

    memset(szSectName, 0,  sizeof(szSectName));
    memset(szValueName, 0,  sizeof(szValueName));

    //解析里面有没有"/"
    if (PaserSectName(lpszName, szSectName, szValueName) == FALSE)
    {
        return FALSE;
    }

    if (m_bSectBeable)
    {
        //如果内部已有段名字的设定，所以直接用他们
        return ComConfig::GetInstance()->GetItemValue(m_SectName, szValueName, lpszReturnedString, nSize, pszDefaultValue);
    }
    else
    {   
        //内部没有段名字，所以直接用normal字段的属性
        return ComConfig::GetInstance()->GetItemValue(szSectName, szValueName, lpszReturnedString, nSize, pszDefaultValue);
    }

    return FALSE;
}


BOOL SoGameSettingImpl::PaserSectName(char* lpszName, char* pSectName, char* pValueName)
{
    char* pSectNameEnd = strstr(lpszName, "/");

    if (pSectNameEnd)
    {
        if ((pSectNameEnd - lpszName) >= (32 - 1)) 
        {
            return FALSE;
        }

        memcpy(pSectName, lpszName, pSectNameEnd - lpszName);

        if ((*(pSectNameEnd+1)) != 0)
        {
            //如果段名字直接传入进来，就直接使用这个段名字
            strcpy(pValueName, pSectNameEnd+1);
            return TRUE;
        }   
        else
        {
            //"newSet/" 是这种情况，就直接把段名字记住，这样只读段名字就好的，
            memcpy(m_SectName, pSectName, sizeof(m_SectName));
            m_bSectBeable = TRUE;
            return TRUE;
        }
    }
    else
    {
        strcpy(pSectName, "Normal");
        strcpy(pValueName, lpszName);
        return TRUE;
    }

    return FALSE;
}


