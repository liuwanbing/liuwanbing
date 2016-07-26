//
// sproxy.exe generated file
// do not modify this file
//
// Created: 01/10/2013@11:51:24
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace MobileCodeWS
{

template <typename TClient = CSoapSocketClientT<> >
class CMobileCodeWST : 
	public TClient, 
	public CSoapRootHandler
{
protected:

	const _soapmap ** GetFunctionMap();
	const _soapmap ** GetHeaderMap();
	void * GetHeaderValue();
	const wchar_t * GetNamespaceUri();
	const char * GetServiceName();
	const char * GetNamespaceUriA();
	HRESULT CallFunction(
		void *pvParam, 
		const wchar_t *wszLocalName, int cchLocalName,
		size_t nItem);
	HRESULT GetClientReader(ISAXXMLReader **ppReader);

public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	CMobileCodeWST(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://webservice.webxml.com.cn/WebServices/MobileCodeWS.asmx"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CMobileCodeWST()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT getMobileCodeInfo(
		BSTR mobileCode, 
		BSTR userID, 
		BSTR* getMobileCodeInfoResult
	);

	HRESULT getDatabaseInfo(
		BSTR** getDatabaseInfoResult, int* getDatabaseInfoResult_nSizeIs
	);
};

typedef CMobileCodeWST<> CMobileCodeWS;

struct __CMobileCodeWS_getMobileCodeInfo_struct
{
	BSTR mobileCode;
	BSTR userID;
	BSTR getMobileCodeInfoResult;
};

extern __declspec(selectany) const _soapmapentry __CMobileCodeWS_getMobileCodeInfo_entries[] =
{

	{
		0x4182A573, 
		"mobileCode", 
		L"mobileCode", 
		sizeof("mobileCode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CMobileCodeWS_getMobileCodeInfo_struct, mobileCode),
		NULL,
		NULL,
		-1,
	},
	{
		0x1945E46C, 
		"userID", 
		L"userID", 
		sizeof("userID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CMobileCodeWS_getMobileCodeInfo_struct, userID),
		NULL,
		NULL,
		-1,
	},
	{
		0xB3E530FE, 
		"getMobileCodeInfoResult", 
		L"getMobileCodeInfoResult", 
		sizeof("getMobileCodeInfoResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CMobileCodeWS_getMobileCodeInfo_struct, getMobileCodeInfoResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CMobileCodeWS_getMobileCodeInfo_map =
{
	0x419C88CE,
	"getMobileCodeInfo",
	L"getMobileCodeInfoResponse",
	sizeof("getMobileCodeInfo")-1,
	sizeof("getMobileCodeInfoResponse")-1,
	SOAPMAP_FUNC,
	__CMobileCodeWS_getMobileCodeInfo_entries,
	sizeof(__CMobileCodeWS_getMobileCodeInfo_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x6197EE82,
	"http://WebXml.com.cn/",
	L"http://WebXml.com.cn/",
	sizeof("http://WebXml.com.cn/")-1
};


struct __CMobileCodeWS_getDatabaseInfo_struct
{
	BSTR *getDatabaseInfoResult;
	int __getDatabaseInfoResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CMobileCodeWS_getDatabaseInfo_entries[] =
{

	{
		0x6D9BD5E0, 
		"getDatabaseInfoResult", 
		L"getDatabaseInfoResult", 
		sizeof("getDatabaseInfoResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CMobileCodeWS_getDatabaseInfo_struct, getDatabaseInfoResult),
		NULL,
		NULL,
		0+1,
	},
	{
		0x6D9BD5E0,
		"__getDatabaseInfoResult_nSizeIs",
		L"__getDatabaseInfoResult_nSizeIs",
		sizeof("__getDatabaseInfoResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CMobileCodeWS_getDatabaseInfo_struct, __getDatabaseInfoResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CMobileCodeWS_getDatabaseInfo_map =
{
	0x438FEE30,
	"getDatabaseInfo",
	L"getDatabaseInfoResponse",
	sizeof("getDatabaseInfo")-1,
	sizeof("getDatabaseInfoResponse")-1,
	SOAPMAP_FUNC,
	__CMobileCodeWS_getDatabaseInfo_entries,
	sizeof(__CMobileCodeWS_getDatabaseInfo_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x6197EE82,
	"http://WebXml.com.cn/",
	L"http://WebXml.com.cn/",
	sizeof("http://WebXml.com.cn/")-1
};

extern __declspec(selectany) const _soapmap * __CMobileCodeWS_funcs[] =
{
	&__CMobileCodeWS_getMobileCodeInfo_map,
	&__CMobileCodeWS_getDatabaseInfo_map,
	NULL
};

template <typename TClient>
inline HRESULT CMobileCodeWST<TClient>::getMobileCodeInfo(
		BSTR mobileCode, 
		BSTR userID, 
		BSTR* getMobileCodeInfoResult
	)
{
    if ( getMobileCodeInfoResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CMobileCodeWS_getMobileCodeInfo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.mobileCode = mobileCode;
	__params.userID = userID;

	__atlsoap_hr = SetClientStruct(&__params, 0);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://WebXml.com.cn/getMobileCodeInfo\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*getMobileCodeInfoResult = __params.getMobileCodeInfoResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CMobileCodeWST<TClient>::getDatabaseInfo(
		BSTR** getDatabaseInfoResult, int* __getDatabaseInfoResult_nSizeIs
	)
{
    if ( getDatabaseInfoResult == NULL )
		return E_POINTER;
	if( __getDatabaseInfoResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CMobileCodeWS_getDatabaseInfo_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 1);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://WebXml.com.cn/getDatabaseInfo\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*getDatabaseInfoResult = __params.getDatabaseInfoResult;
	*__getDatabaseInfoResult_nSizeIs = __params.__getDatabaseInfoResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CMobileCodeWST<TClient>::GetFunctionMap()
{
	return __CMobileCodeWS_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CMobileCodeWST<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CMobileCodeWS_getMobileCodeInfo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CMobileCodeWS_getMobileCodeInfo_atlsoapheader_map = 
	{
		0x419C88CE,
		"getMobileCodeInfo",
		L"getMobileCodeInfoResponse",
		sizeof("getMobileCodeInfo")-1,
		sizeof("getMobileCodeInfoResponse")-1,
		SOAPMAP_HEADER,
		__CMobileCodeWS_getMobileCodeInfo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x6197EE82,
		"http://WebXml.com.cn/",
		L"http://WebXml.com.cn/",
		sizeof("http://WebXml.com.cn/")-1
	};

	static const _soapmapentry __CMobileCodeWS_getDatabaseInfo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CMobileCodeWS_getDatabaseInfo_atlsoapheader_map = 
	{
		0x438FEE30,
		"getDatabaseInfo",
		L"getDatabaseInfoResponse",
		sizeof("getDatabaseInfo")-1,
		sizeof("getDatabaseInfoResponse")-1,
		SOAPMAP_HEADER,
		__CMobileCodeWS_getDatabaseInfo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x6197EE82,
		"http://WebXml.com.cn/",
		L"http://WebXml.com.cn/",
		sizeof("http://WebXml.com.cn/")-1
	};


	static const _soapmap * __CMobileCodeWS_headers[] =
	{
		&__CMobileCodeWS_getMobileCodeInfo_atlsoapheader_map,
		&__CMobileCodeWS_getDatabaseInfo_atlsoapheader_map,
		NULL
	};
	
	return __CMobileCodeWS_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CMobileCodeWST<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CMobileCodeWST<TClient>::GetNamespaceUri()
{
	return L"http://WebXml.com.cn/";
}

template <typename TClient>
ATL_NOINLINE inline const char * CMobileCodeWST<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CMobileCodeWST<TClient>::GetNamespaceUriA()
{
	return "http://WebXml.com.cn/";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CMobileCodeWST<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CMobileCodeWST<TClient>::GetClientReader(ISAXXMLReader **ppReader)
{
	if (ppReader == NULL)
	{
		return E_INVALIDARG;
	}
	
	CComPtr<ISAXXMLReader> spReader = GetReader();
	if (spReader.p != NULL)
	{
		*ppReader = spReader.Detach();
		return S_OK;
	}
	return TClient::GetClientReader(ppReader);
}

} // namespace MobileCodeWS
