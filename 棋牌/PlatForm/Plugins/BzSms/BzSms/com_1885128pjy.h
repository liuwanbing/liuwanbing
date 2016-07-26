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

namespace MessageUtil
{

template <typename TClient = CSoapSocketClientT<> >
class CMessageUtilT : 
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

	CMessageUtilT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://jc.1885128.com/MessageUtil.asmx"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CMessageUtilT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT SendMessage(
		BSTR _uMobile, 
		BSTR _Content, 
		BSTR _SendTime, 
		BSTR _UserId
	);
};

typedef CMessageUtilT<> CMessageUtil;

struct __CMessageUtil_SendMessage_struct
{
	BSTR _uMobile;
	BSTR _Content;
	BSTR _SendTime;
	BSTR _UserId;
};

extern __declspec(selectany) const _soapmapentry __CMessageUtil_SendMessage_entries[] =
{

	{
		0x9679262C, 
		"_uMobile", 
		L"_uMobile", 
		sizeof("_uMobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CMessageUtil_SendMessage_struct, _uMobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xDCE0637A, 
		"_Content", 
		L"_Content", 
		sizeof("_Content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CMessageUtil_SendMessage_struct, _Content),
		NULL,
		NULL,
		-1,
	},
	{
		0x3A18D0F8, 
		"_SendTime", 
		L"_SendTime", 
		sizeof("_SendTime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CMessageUtil_SendMessage_struct, _SendTime),
		NULL,
		NULL,
		-1,
	},
	{
		0x5F7D5C0B, 
		"_UserId", 
		L"_UserId", 
		sizeof("_UserId")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CMessageUtil_SendMessage_struct, _UserId),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CMessageUtil_SendMessage_map =
{
	0x837806FE,
	"SendMessage",
	L"SendMessageResponse",
	sizeof("SendMessage")-1,
	sizeof("SendMessageResponse")-1,
	SOAPMAP_FUNC,
	__CMessageUtil_SendMessage_entries,
	sizeof(__CMessageUtil_SendMessage_struct),
	0,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};

extern __declspec(selectany) const _soapmap * __CMessageUtil_funcs[] =
{
	&__CMessageUtil_SendMessage_map,
	NULL
};

template <typename TClient>
inline HRESULT CMessageUtilT<TClient>::SendMessage(
		BSTR _uMobile, 
		BSTR _Content, 
		BSTR _SendTime, 
		BSTR _UserId
	)
{

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CMessageUtil_SendMessage_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params._uMobile = _uMobile;
	__params._Content = _Content;
	__params._SendTime = _SendTime;
	__params._UserId = _UserId;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SendMessage\"\r\n"));
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

	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CMessageUtilT<TClient>::GetFunctionMap()
{
	return __CMessageUtil_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CMessageUtilT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CMessageUtil_SendMessage_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CMessageUtil_SendMessage_atlsoapheader_map = 
	{
		0x837806FE,
		"SendMessage",
		L"SendMessageResponse",
		sizeof("SendMessage")-1,
		sizeof("SendMessageResponse")-1,
		SOAPMAP_HEADER,
		__CMessageUtil_SendMessage_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};


	static const _soapmap * __CMessageUtil_headers[] =
	{
		&__CMessageUtil_SendMessage_atlsoapheader_map,
		NULL
	};
	
	return __CMessageUtil_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CMessageUtilT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CMessageUtilT<TClient>::GetNamespaceUri()
{
	return L"http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline const char * CMessageUtilT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CMessageUtilT<TClient>::GetNamespaceUriA()
{
	return "http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CMessageUtilT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CMessageUtilT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace MessageUtil
