#pragma once
/*
	This function need ZLib : http://www.zlib.org/
*/

//-------------------------------------------------------------------------------------
inline BOOL IsReceiveGZipStream (FCHttpRequest& rTask)
{
    CString   s = rTask.GetResponseInfo().QueryInfo(HTTP_QUERY_CONTENT_ENCODING) ;
    return (lstrcmpi(s, _T("gzip")) == 0) ;
}

//-------------------------------------------------------------------------------------
inline BOOL gzip_decompress (const void* pInDate, size_t nSize, std::string& raw_data)
{
    z_stream   zs ;
    ZeroMemory (&zs, sizeof(zs)) ;

    std::vector<BYTE>   temp_buf (8 * 1024) ;

    BOOL   bRet = FALSE ;
    int    nErr = inflateInit2 (&zs, 47) ;
    if (nErr == Z_OK)
    {
        zs.next_in = (BYTE*)const_cast<void*>(pInDate) ;
        zs.avail_in = (UINT)nSize ;

        for (;;)
        {
            zs.avail_out = (UINT)temp_buf.size() ;
            zs.next_out = &temp_buf[0] ;

            nErr = inflate (&zs, Z_NO_FLUSH) ;

            size_t   nWrite = zs.total_out - raw_data.size() ;
            if (nWrite)
            {
                raw_data.append ((char*)&temp_buf[0], nWrite) ;
            }

            if (nErr == Z_STREAM_END)
            {
                bRet = TRUE ;
                break;
            }
            else if (nErr == Z_OK) // continue
            {
            }
            else
            {
                break;
            }
        }
        inflateEnd (&zs) ;
    }
    assert(bRet) ;
    return bRet ;
}
