
namespace inner
{
    class FCAutoCSec
    {
        CRITICAL_SECTION   m_cs ;
    public:
        FCAutoCSec() {InitializeCriticalSection (&m_cs) ;}
        virtual ~FCAutoCSec() {DeleteCriticalSection (&m_cs) ;}
        void Lock() {EnterCriticalSection(&m_cs);}
        void UnLock() {LeaveCriticalSection(&m_cs);}
    };

    // buffer to auto grow for read response data
    class CReceiveBuffer : public FCAutoCSec
    {
    public:
        std::string   m_buf ; // must be protected by Lock/UnLock
        LONG   m_total_receive ;

        CReceiveBuffer()
        {
            m_total_receive = 0 ;
        }
    public:
        void Write (const void* p, size_t nLen)
        {
            InterlockedExchangeAdd (&m_total_receive, (LONG)nLen) ;

            Lock() ;
            m_buf.append ((const char*)p, nLen) ;
            UnLock() ;
        }
    };

    class FCInternetHandle
    {
        HINTERNET   m_handle ;
    public:
        FCInternetHandle() : m_handle(NULL) {}

        ~FCInternetHandle()
        {
            Close() ;
        }

        void Attach (HINTERNET h)
        {
            m_handle = h ;
        }

        operator HINTERNET() const {return m_handle;}

        void Close()
        {
            if (m_handle)
            {
                InternetCloseHandle(m_handle) ;
            }
            m_handle = NULL ;
        }
    };

    class FCThreadHandle
    {
        HANDLE   m_handle ;
    public:
        FCThreadHandle() : m_handle(NULL) {}

        ~FCThreadHandle()
        {
            Close() ;
        }

        void Attach (HANDLE h)
        {
            m_handle = h ;
        }

        operator HANDLE() const {return m_handle;}

        BOOL IsRunning() const
        {
            if (m_handle && (WaitForSingleObject(m_handle,0) == WAIT_TIMEOUT))
                return TRUE ;
            return FALSE ;
        }

        void Close()
        {
            if (m_handle)
            {
                CloseHandle(m_handle) ;
            }
            m_handle = NULL ;
        }
    };
}
