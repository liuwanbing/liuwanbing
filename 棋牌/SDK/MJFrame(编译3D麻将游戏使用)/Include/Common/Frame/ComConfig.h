#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

#include "afxwin.h"
#include "typeDef.h"

#define MAX_CU_SECT_NUM     100

class SectItem
{
public:
    BOOL   m_bBeAble;
    char   m_SectName[32];
    char*  m_pSectStart;
    char*  m_pSectEnd;
public:
    SectItem();
    ~SectItem();
};

/*现在还没有分段的功能，在后期再加上*/

/*此对象是公用的全局唯一的类型*/
class ComConfig
{
public:
    char   m_cuPathName[256];          //路径名
    char   *m_pMemFile;                //内存文件
    UINT m_cuMemSizeLen;             //现在内存的长度
    BOOL   m_bRePaser;                 //是否要重新解析一下
    UINT m_cuSectNum;

    static  ComConfig*  m_pStance;
    
    SectItem    m_SectArry[MAX_CU_SECT_NUM];       //段数组

public:
    ComConfig(void);
    ~ComConfig(void);

    static ComConfig* GetInstance(); 

    /*如果这次不是第一次的话，就会重新回载新的文件内容到MEM*/
    BOOL PaserConfigPath(char* pConfigPath);

    //再次重新的解析配置文件，内部的文件还是在以前的文件名
    BOOL RePaserConfig();

    BOOL GetItemValue(char* lpszSectName, char *lpszName, int &nValue, int nDefaultValue);

	BOOL GetItemValue(char* lpszSectName, char *lpszName,  char *lpszReturnedString, int nSize, char* pszDefaultValue);

private:    
    //取得下一个Sectbuf 指针,只有在启动时使用一下
    char* GetNextSect(char* pMemStr, char* pLimit); 

    STATUS FindSectName(char* lpszSectName, char** ppStart, UINT *pbufLen);

    /*解析段的字样*/
    BOOL PaserSect();
};

#endif
