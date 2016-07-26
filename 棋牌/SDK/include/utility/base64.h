#pragma once
#include <WinCrypt.h>
#pragma comment(lib, "Crypt32.lib")

//-------------------------------------------------------------------------------------
class FCBase64
{
public:
    static CStringA Encode (const void* pBuffer, DWORD nSize)
    {
        DWORD   nNeed = 0 ;
        CryptBinaryToStringA ((const BYTE*)pBuffer, nSize, CRYPT_STRING_BASE64, NULL, &nNeed) ;

        CStringA   s ;
        if (nNeed)
        {
            std::vector<char>   buf (nNeed) ;
            CryptBinaryToStringA ((const BYTE*)pBuffer, nSize, CRYPT_STRING_BASE64, &buf[0], &nNeed) ;
            s = &buf[0] ;
        }
        s.Replace ("\r\n", "") ; // default will add CR/LF
        return s ;
    }

    static void Decode (const char* sBase64, std::vector<BYTE>& decoded_data)
    {
        decoded_data.clear() ;

        if (sBase64)
        {
            DWORD   nNeed = 0 ;
            CryptStringToBinaryA (sBase64, 0, CRYPT_STRING_BASE64, NULL, &nNeed, NULL, NULL) ;
            if (nNeed)
            {
                decoded_data.resize (nNeed) ;
                CryptStringToBinaryA (sBase64, 0, CRYPT_STRING_BASE64, &decoded_data[0], &nNeed, NULL, NULL) ;
            }
        }
    }
};
