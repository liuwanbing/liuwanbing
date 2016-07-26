#pragma once
#include "atlcrypt.h"

class FCCrypt
{
public:
    /// Calculate MD5, return empty string if failed.
    static CString Get_MD5 (const void* p, int nLength)
    {
        CCryptProv  prov ;
        LRESULT     l = prov.Initialize() ;
        if (l == 0x80090016L)
        {
            prov.Initialize (PROV_RSA_FULL, NULL, MS_DEF_PROV, CRYPT_NEWKEYSET) ;
        }

        CCryptMD5Hash   hash ;
        hash.Initialize(prov) ;

        BYTE    buf[32] ;
        DWORD   dw = 32 ;
        l = hash.AddData ((const BYTE*)p, nLength) ;
        if (SUCCEEDED(l))
        {
            l = hash.GetValue (buf, &dw) ;
            if (SUCCEEDED(l) && (dw == 16))
            {
                CString   s ;
                for (int i=0 ; i < (int)dw ; i++)
                {
                    s.AppendFormat (_T("%02x"), (int)buf[i]) ;
                }
                return s ;
            }
        }
        return _T("") ;
    }
};
