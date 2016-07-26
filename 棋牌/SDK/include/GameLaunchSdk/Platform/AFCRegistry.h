#ifndef CAFCREGISTRY_HEAD_FILE
#define CAFCREGISTRY_HEAD_FILE

#include "PublicDefine.h"

//注册表读写类
class EXT_CLASS CGameGoMemory//CGameGoMemory
{
	//变量定义
protected:
	HKEY					m_hRegKey;					//注册表键值

	//函数定义
public:
	//构造函数
	CGameGoMemory(HKEY hRegKey=HKEY_LOCAL_MACHINE);
	//析构函数
	virtual ~CGameGoMemory(void);

	//功能函数
public:
	//打开
	bool Open(TCHAR * szSubKey);
	//关闭
	void Close();
	//建立子键
	bool CreateKey(TCHAR * szSubKey);
	//删除
	bool DeleteKey(HKEY hRegKey, TCHAR * szSubKey);
	//删除
	bool DeleteValue(TCHAR * szValueName);
	//保存
	bool SaveKey(TCHAR * szFileName);
	//恢复
	bool RestoreKey(TCHAR * szFileName);
	//读取
	bool Read(TCHAR * szValueName, CString & strOutVal);
	//读取
	bool Read(TCHAR * szValueName, TCHAR * szOutVal, UINT uBufferSize);
	//读取
	bool Read(TCHAR * szValueName, DWORD & dwOutVal);
	//读取
	bool Read(TCHAR * szValueName, int & iOutVal);
	//写入
	bool Write(TCHAR * szSubKey, TCHAR * szWriteVal);
	//写入
	bool Write(TCHAR * szSubKey, DWORD dwWriteVal);
	//写入
	bool Write(TCHAR * szSubKey, int nWriteVal);
};

#endif
