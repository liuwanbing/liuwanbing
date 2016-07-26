
//-------------------------------------------------------------------------------------
/**
    HTTP request header info.
*/
struct HTTP_REQUEST_HEADER
{
private:
    std::string   m_post_data ;

public:
    enum VERB_TYPE
    {
        VERB_TYPE_GET = 0,
        VERB_TYPE_POST = 1,
        VERB_TYPE_POST_MULTIPART = 2,
        VERB_TYPE_DELETE = 3,
    };

    /**
        verb of request, can be :\n\n
        VERB_TYPE_GET\n
        VERB_TYPE_POST\n
        VERB_TYPE_POST_MULTIPART\n
        VERB_TYPE_DELETE\n
    */
    VERB_TYPE m_verb ;
    CString   m_url ; ///< url to send request
    int       m_start ; ///< start position of get, in byte, default is 0, a range field will be added to HTTP request header if it is not 0
    CString   m_user_agent ; ///< default is same to IE
    /**
        header field, \\r\\n at end of each line, default is empty, don't include Range field if you have set m_start member\n\n
        m_header += _T(\"Accept-Encoding: gzip, deflate\\r\\n\") ;\n
        m_header += _T(\"Accept: text/html\\r\\n\") ;\n
    */
    CString   m_header ;
    /// param dwFlags of Win32API <b>HttpOpenRequest</b>, default is <b>INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE</b>
    DWORD     m_open_flag ;
    /**
        request lifetime limit, in milliseconds, FCHttpRequestManager::OnOverRequestLifetime will be called when request over its lifetime\n
        default is 0, meaning no lifetime limit.
    */
    int       m_lifetime_limit ;

    CString   m_proxy_ip ; ///< proxy server IP
    int       m_proxy_port ; ///< proxy server port, default is 80
    CString   m_proxy_username ; ///< user name to login the proxy if need
    CString   m_proxy_password ; ///< password to login the proxy if need

    /// Default to construct a HTTP get request.
    HTTP_REQUEST_HEADER (VERB_TYPE nVerb=VERB_TYPE_GET)
    {
        m_verb = nVerb ;
        m_start = 0 ;
        m_user_agent = GetSystemUserAgent() ;
        m_open_flag = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE ;
        m_lifetime_limit = 0 ;
        m_proxy_port = 80 ;
    }

    /// Get text of current verb.
    CString GetVerbText() const
    {
        switch (m_verb)
        {
            case VERB_TYPE_GET : return _T("GET") ;
            case VERB_TYPE_POST :
            case VERB_TYPE_POST_MULTIPART : return _T("POST") ;
            case VERB_TYPE_DELETE : return _T("DELETE") ;
        }
        return _T("GET") ;
    }

    /// Get host and object from current url.
    void GetUrlComponent (CString& sHost, CString& sObject, INTERNET_SCHEME& nScheme) const
    {
        URL_COMPONENTS   uc ;
        ZeroMemory (&uc, sizeof(uc)) ;

        DWORD   nBufferSize = m_url.GetLength() + 8 ;

        std::vector<TCHAR>   t1 (nBufferSize, (TCHAR)0) ;
        std::vector<TCHAR>   t2 (nBufferSize, (TCHAR)0) ;

        uc.dwStructSize = sizeof(uc) ;
        uc.lpszHostName = &t1[0] ;
        uc.dwHostNameLength = nBufferSize ;
        uc.lpszUrlPath = &t2[0] ;
        uc.dwUrlPathLength = nBufferSize ;

        InternetCrackUrl (m_url, m_url.GetLength(), 0, &uc) ;
        sHost = &t1[0] ;
        sObject = &t2[0] ;
        nScheme = uc.nScheme ;
    }

    /// @name Add post data.
    //@{
    /// Add string to post data, <b>not</b> including terminator.
    void AddPostData (const CString& s)
    {
        AddPostData ((LPCSTR)s, s.GetLength()) ;
    }
    /// Add buffer to post data.
    void AddPostData (const void* pBuffer, size_t nSize)
    {
        m_post_data.append ((const char*)pBuffer, nSize) ;
    }
    //@}

    /// @name Add multipart/form-data.
    //@{
    /// Add string param, <b>not</b> including terminator.
    void AddMultipartFormData (const CString& sName, const CString& sValue)
    {
        AddMultipartFormData (sName, (LPCSTR)sValue, sValue.GetLength(), NULL) ;
    }
    /// Add buffer param.
    void AddMultipartFormData (const CString& sName, const void* pValue, size_t nValueSize, LPCSTR sFileName)
    {
        CString s1;
		s1.Format(_T("--%s\r\nContent-Disposition: form-data; name=\"%s\""), DEFAULT_BOUNDARY(), sName);
        if (sFileName)
        {
			CString s2;
			s2.Format(_T("; filename=\"%s\""), sFileName);
			s1 += s2;
        }
        s1 += "\r\n" ;

        s1 += "\r\n" ; // blank line
        AddPostData (s1) ;
        AddPostData (pValue, nValueSize) ;
        AddPostData ("\r\n") ; // after data
    }

    /// End add param.
    void EndMultipartFormData()
    {
        // terminator
		CString s1;
		s1.Format(_T("--%s--\r\n"), DEFAULT_BOUNDARY());

        m_header += _T("Accept: */*\r\n") ;
		m_header.Format(_T("%sContent-Type: multipart/form-data; boundary=%s\r\nContent-Length: %d\r\n"), m_header, DEFAULT_BOUNDARY(), (int)m_post_data.size());
    }
    //@}

private:
    // Get user-agent used by IE.
    static CString GetSystemUserAgent()
    {
        DWORD  n = 1024 ;
        char   t[1024] = {0} ;
        ObtainUserAgentString (0, t, &n) ;
        return CString(t) ;
    }

    static CString DEFAULT_BOUNDARY()
    {
        // D514116929C24f35AE047F2B82C59B08 , avoid to find string in binary including this library
		CString s1;
		s1.Format(_T("---D514116929C24f35AE047F2B82C59B08"));
		return s1;
    }

friend class FCHttpRequest ; // to access m_post_data
};
