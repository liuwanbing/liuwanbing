
//-------------------------------------------------------------------------------------
/**
    HTTP response info.
*/
struct HTTP_RESPONSE_INFO
{
    /// status code, such as: HTTP_STATUS_OK / HTTP_STATUS_NOT_FOUND, is 0 if can't connect server.
    int   m_status_code ;

    /// Content-Length field, is 0 if this field not exist.
    int   m_content_length ;

    /// Last-Modified field, empty if this field not exist.
    CString   m_last_modified ;

    /// software used by the server, such as: Apache, Microsoft-IIS/6.0.
    CString   m_server ;

    /// only valid in FCHttpRequestManager::OnAfterRequestFinish callback, the last read result before download finish.
    BOOL   m_final_read_result ;

    HTTP_RESPONSE_INFO()
    {
        m_status_code = 0 ;
        m_content_length = 0 ;
        m_request = NULL ;
    }

    void SetResponse (HINTERNET hRequest)
    {
        m_request = hRequest ;
        if (hRequest)
        {
            CString   s = QueryInfo(HTTP_QUERY_STATUS_CODE) ;
            if (s.GetLength())
                m_status_code = _ttoi(s) ;
            s = QueryInfo(HTTP_QUERY_CONTENT_LENGTH) ;
            if (s.GetLength())
                m_content_length = _ttoi(s) ;
            m_last_modified = QueryInfo(HTTP_QUERY_LAST_MODIFIED) ;
            m_server = QueryInfo(HTTP_QUERY_SERVER) ;
        }
    }

    /// Refer to param dwInfoLevel of Win32 API <B>HttpQueryInfo</B>.
    CString QueryInfo (DWORD dwInfoLevel) const
    {
        CString  s ;
        DWORD    dwLen = 0 ;
        if (!::HttpQueryInfo (m_request, dwInfoLevel, NULL, &dwLen, 0) && dwLen)
        {
            std::vector<BYTE>   buf (dwLen + 2, (BYTE)0) ;
            ::HttpQueryInfo (m_request, dwInfoLevel, &buf[0], &dwLen, 0) ;
            s = (LPCTSTR)&buf[0] ;
        }
        return s ;
    }

private:
    HINTERNET   m_request ;
};
