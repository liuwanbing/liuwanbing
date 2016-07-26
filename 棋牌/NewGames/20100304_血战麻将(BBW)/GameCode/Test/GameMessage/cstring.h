#pragma once

#define SafeDeleteArray( p )\
    if (NULL!=p)	\
    {				\
    delete []p;	\
    p = NULL;	\
    }

/// 自定义字符串类
class CStringEx
{
public:
    /// 默认构造函数
    CStringEx() : m_nLen(1)
    {
        m_pBuf = new char[1];
        m_pBuf[0] = TEXT('\0');
    }

    /// 传入字符串指针的构造函数
    CStringEx(const char *p)
    {
        if (p==NULL)
        {
            m_pBuf = new char[1];
            m_pBuf[0] = TEXT('\0');
            m_nLen = 1;
            return;
        }
        m_nLen = (int)strlen(p) + 1;
        m_pBuf = new char[m_nLen];
        strcpy(m_pBuf, p);
    }
    /// 析构函数
    ~CStringEx(){SafeDeleteArray(m_pBuf);m_nLen = 0;}

    /// 拷贝构造函数
    CStringEx(const CStringEx &rhs)
    {
        m_nLen = (int)strlen(rhs.m_pBuf) + 1;
        m_pBuf = new char[m_nLen];
        strcpy(m_pBuf, rhs.m_pBuf);
    }
    /// 赋值操作符
    CStringEx &operator=(const CStringEx &rhs)
    {
        int nLen = (int)strlen(rhs.m_pBuf)+1;
        if (m_nLen<nLen)
        {
            SafeDeleteArray(m_pBuf);
            m_pBuf = new char[nLen];
            strcpy(m_pBuf, rhs.m_pBuf);
            m_nLen = nLen;
        }
        else
        {
            strcpy(m_pBuf, rhs.m_pBuf);
        }
        return *this;
    }

    bool operator<(const CStringEx &rhs) const
    {
        return strcmp(m_pBuf, rhs.m_pBuf)<0;
    }
    bool operator==(const CStringEx &rhs) const
    {
        return strcmp(m_pBuf, rhs.m_pBuf)==0;
    }
    bool operator>(const CStringEx &rhs) const
    {
        return strcmp(m_pBuf, rhs.m_pBuf)>0;
    }
    /// 在字符串末尾添加另一字符串, +操作符
    CStringEx operator+(const char *szParam) const
    {
        CStringEx ret(*this);
        if (szParam == NULL)
        {
            return ret;
        }
        int nLen = (int)strlen(m_pBuf) + (int)strlen(szParam) + 1;
        if (ret.m_nLen<nLen)
        {
            char *p = ret.m_pBuf;
            ret.m_pBuf = new char[nLen];
            strcpy(ret.m_pBuf, p);
            SafeDeleteArray(p);
            strcat(ret.m_pBuf, szParam);
            ret.m_nLen = nLen;
        }
        else
        {
            strcat(ret.m_pBuf, szParam);
        }
        return ret;
    }
    /// 在字符串末尾添加另一字符串, +=操作符
    CStringEx &operator+=(const char *szParam)
    {
        if (szParam == NULL)
        {
            return *this;
        }
        int nLen = (int)strlen(m_pBuf) +(int) strlen(szParam) + 1;
        if (m_nLen<nLen)
        {
            char *p = m_pBuf;
            m_pBuf = new char[nLen];
            strcpy(m_pBuf, p);
            SafeDeleteArray(p);
            strcat(m_pBuf, szParam);
            m_nLen = nLen;
        }
        else
        {
            strcat(m_pBuf, szParam);
        }
        return *this;
    }
    /// 从后往前找一个字符
    int ReverseFind(char ch) const
    {
        int nLen = (int)strlen(m_pBuf);
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
    int GetLength(){return (int)strlen(m_pBuf);}
    /// 获取左边指定长度的字符串
    CStringEx Left(int nPos) const
    {
        CStringEx ret(*this);
        ret.m_pBuf[nPos] = TEXT('\0');
        return ret;
    }
    /// 返回const char指针
    operator const char *() const
    {
        return m_pBuf;
    }
    /// 格式化字符串
    CStringEx &Format(const char *szFormat, ...)
    {
        va_list arg;
        va_start( arg, szFormat );

        char szBuf[512];
        sprintf( szBuf, szFormat, arg );
        int nLen = (int)strlen(szBuf)+1;
        if (m_nLen<nLen)
        {
            SafeDeleteArray(m_pBuf);
            m_pBuf = new char[nLen];
            strcpy(m_pBuf, szBuf);
            m_nLen = nLen;
        }
        else
        {
            strcpy(m_pBuf, szBuf);
        }
        return *this;
    }
    const char *GetBuffer()
    {
        return m_pBuf;
    }

private:
    char *m_pBuf;	///< 缓冲区
    int m_nLen;		///< 缓冲区长度
};