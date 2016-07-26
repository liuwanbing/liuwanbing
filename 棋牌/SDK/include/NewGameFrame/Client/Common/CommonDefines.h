#pragma once

#include <windows.h>
#include <tchar.h>

#ifdef GRAPH_EXPORTS
#define GRAPH_API __declspec(dllexport)
#else
#define GRAPH_API __declspec(dllimport)
#endif GRAPH_EXPORTS

#pragma warning ( disable: 4996)

#define WM_IPC_MESSAGE				(WM_USER+999)						//IPC 消息

#ifdef _UNICODE
#	define tcsscanf swscanf
#else
#	define tcsscanf sscanf
#endif


/// 安全删除和释放指针

#define SafeDelete( p )\
	if (NULL!=p)	\
	{				\
		delete p;	\
		p = NULL;	\
	}

#define SafeDeleteArray( p )\
	if (NULL!=p)	\
	{				\
		delete []p;	\
		p = NULL;	\
	}
#define SafeRelease( p ) \
	if (NULL != p)		\
	{					\
		p->Release();	\
		p = NULL;		\
	}

#define SafeClose( p ) \
	if (NULL != p)		\
	{					\
		CloseHandle(p);	\
		p = NULL;		\
	}

/// 自定义字符串类
class Tstring
{
public:
	/// 默认构造函数
	Tstring() : m_nLen(1)
	{
		m_pBuf = new TCHAR[1];
		m_pBuf[0] = TEXT('\0');
	}

	/// 传入字符串指针的构造函数
	Tstring(const TCHAR *p)
	{
		if (p==NULL)
		{
			m_pBuf = new TCHAR[1];
			m_pBuf[0] = TEXT('\0');
			m_nLen = 1;
			return;
		}
		m_nLen = (int)_tcslen(p) + 1;
		m_pBuf = new TCHAR[m_nLen];
		_tcscpy(m_pBuf, p);
	}
	/// 析构函数
	~Tstring(){SafeDeleteArray(m_pBuf);m_nLen = 0;}

	/// 拷贝构造函数
	Tstring(const Tstring &rhs)
	{
		m_nLen = (int)_tcslen(rhs.m_pBuf) + 1;
		m_pBuf = new TCHAR[m_nLen];
		_tcscpy(m_pBuf, rhs.m_pBuf);
	}
	/// 赋值操作符
	Tstring &operator=(const Tstring &rhs)
	{
		int nLen = (int)_tcslen(rhs.m_pBuf)+1;
		if (m_nLen<nLen)
		{
			SafeDeleteArray(m_pBuf);
			m_pBuf = new TCHAR[nLen];
			_tcscpy(m_pBuf, rhs.m_pBuf);
			m_nLen = nLen;
		}
		else
		{
			_tcscpy(m_pBuf, rhs.m_pBuf);
		}
		return *this;
	}

	bool operator<(const Tstring &rhs) const
	{
		return _tcscmp(m_pBuf, rhs.m_pBuf)<0;
	}
	bool operator==(const Tstring &rhs) const
	{
		return _tcscmp(m_pBuf, rhs.m_pBuf)==0;
	}
	bool operator>(const Tstring &rhs) const
	{
		return _tcscmp(m_pBuf, rhs.m_pBuf)>0;
	}
	/// 在字符串末尾添加另一字符串, +操作符
	Tstring operator+(const TCHAR *szParam) const
	{
		Tstring ret(*this);
		if (szParam == NULL)
		{
			return ret;
		}
		int nLen = (int)_tcslen(m_pBuf) + (int)_tcslen(szParam) + 1;
		if (ret.m_nLen<nLen)
		{
			TCHAR *p = ret.m_pBuf;
			ret.m_pBuf = new TCHAR[nLen];
			_tcscpy(ret.m_pBuf, p);
			SafeDeleteArray(p);
			_tcscat(ret.m_pBuf, szParam);
			ret.m_nLen = nLen;
		}
		else
		{
			_tcscat(ret.m_pBuf, szParam);
		}
		return ret;
	}
	/// 在字符串末尾添加另一字符串, +=操作符
	Tstring &operator+=(const TCHAR *szParam)
	{
		if (szParam == NULL)
		{
			return *this;
		}
		int nLen = (int)_tcslen(m_pBuf) +(int) _tcslen(szParam) + 1;
		if (m_nLen<nLen)
		{
			TCHAR *p = m_pBuf;
			m_pBuf = new TCHAR[nLen];
			_tcscpy(m_pBuf, p);
			SafeDeleteArray(p);
			_tcscat(m_pBuf, szParam);
			m_nLen = nLen;
		}
		else
		{
			_tcscat(m_pBuf, szParam);
		}
		return *this;
	}
	/// 从后往前找一个字符
	int ReverseFind(TCHAR ch) const
	{
		int nLen = (int)_tcslen(m_pBuf);
		for (int i=nLen-1; i>=0; --i)
		{
			if (m_pBuf[i] == ch)
			{
				return i;
			}
		}
		return -1;
	}
	/// 返回长度
	int GetLength(){return (int)_tcslen(m_pBuf);}
	/// 获取左边指定长度的字符串
	Tstring Left(int nPos) const
	{
		Tstring ret(*this);
		ret.m_pBuf[nPos] = TEXT('\0');
		return ret;
	}
	/// 返回const TCHAR指针
	operator const TCHAR *() const
	{
		return m_pBuf;
	}
	/// 格式化字符串
	Tstring &Format(const TCHAR *szFormat, ...)
	{
		va_list arg;
		va_start( arg, szFormat );

		TCHAR szBuf[512];
		wsprintf( szBuf, szFormat, arg );
		int nLen = (int)_tcslen(szBuf)+1;
		if (m_nLen<nLen)
		{
			SafeDeleteArray(m_pBuf);
			m_pBuf = new TCHAR[nLen];
			_tcscpy(m_pBuf, szBuf);
			m_nLen = nLen;
		}
		else
		{
			_tcscpy(m_pBuf, szBuf);
		}
		return *this;
	}
    const TCHAR *GetBuffer()
    {
        return m_pBuf;
    }

private:
	TCHAR *m_pBuf;	///< 缓冲区
	int m_nLen;		///< 缓冲区长度
};

/// 用来在结构体里初始化成员的宏定义，把所有成员都赋初值0
/// ★★★使用注意事项，使用本宏的结构体内不能有虚函数★★★
///
#define INIT_STRUCT( T ) \
	T()\
	{\
		ZeroMemory(this, sizeof(T));\
	}
/// 示例代码
struct Test_t
{
	int _nLen;
	char _pStr[256];
	INIT_STRUCT( Test_t );	///< 结构体里加上这个宏，即相当于添加了一个初始化成员的构造函数
};



