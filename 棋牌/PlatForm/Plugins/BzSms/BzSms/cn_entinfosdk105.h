//
// sproxy.exe generated file
// do not modify this file
//
// Created: 01/10/2013@11:51:23
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace WebService
{

struct RegistryInfo
{
	BSTR RESULT;
	BSTR REGISTRYCODE;
	BSTR SERVICECODE;
	BSTR BALANCE;
	BSTR CREATEDATE;
	BSTR REGISTRYSTATEID;
	BSTR PRIORITY;
	BSTR FIRSTREGISTRYDATE;
	BSTR REGISTRYDATE;
	BSTR AGENTID;
	BSTR PRODUCTNAME;
	BSTR DESTMOBILE;
	BSTR FLAG;
	BSTR REPLYCONTENT;
	BSTR ISRETURN;
	BSTR VERSION;
	BSTR GRADE;
	BSTR PARENT;
	BSTR SUBSIDIARY;
	BSTR ROLES;
	BSTR BALSTATUS;
	BSTR DISCOUNT;
};

struct MOBody
{
	BSTR total_num;
	BSTR this_num;
	BSTR recvtel;
	BSTR sender;
	BSTR content;
	BSTR recdate;
};

struct RegistryInfo2
{
	BSTR RESULT;
	BSTR REGISTRYCODE;
	BSTR SERVICECODE;
	BSTR BALANCE;
	BSTR CREATEDATE;
	BSTR REGISTRYSTATEID;
	BSTR PRIORITY;
	BSTR FIRSTREGISTRYDATE;
	BSTR REGISTRYDATE;
	BSTR AGENTID;
	BSTR PRODUCTNAME;
	BSTR DESTMOBILE;
	BSTR FLAG;
	BSTR REPLYCONTENT;
	BSTR ISRETURN;
	BSTR VERSION;
	BSTR GRADE;
	BSTR PARENT;
	BSTR SUBSIDIARY;
	BSTR ROLES;
	BSTR BALSTATUS;
	BSTR DISCOUNT;
	BSTR SLIST;
	BSTR POPM;
	BSTR UPDATE;
	BSTR MMS;
};

template <typename TClient = CSoapSocketClientT<> >
class CWebServiceT : 
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

	CWebServiceT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://sdk105.entinfo.cn/webservice.asmx"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CWebServiceT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT GetGaoDuan(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetGaoDuanResult
	);

	HRESULT GetBalance(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetBalanceResult
	);

	HRESULT mdSmsSend_DES(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mdSmsSend_DESResult
	);

	HRESULT balance(
		BSTR sn, 
		BSTR pwd, 
		BSTR* balanceResult
	);

	HRESULT mt(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mtResult
	);

	HRESULT UDPSIGNEX(
		BSTR sn, 
		BSTR pwd, 
		BSTR subcode, 
		BSTR sign, 
		BSTR comName, 
		BSTR* UDPSIGNEXResult
	);

	HRESULT SendSMS(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR* SendSMSResult
	);

	HRESULT mdMmsSend(
		BSTR sn, 
		BSTR pwd, 
		BSTR title, 
		BSTR mobile, 
		BSTR content, 
		BSTR stime, 
		BSTR* mdMmsSendResult
	);

	HRESULT SetGaoDuan(
		BSTR sn, 
		BSTR pwd, 
		BSTR gd, 
		double* SetGaoDuanResult
	);

	HRESULT mdAudioSend(
		BSTR sn, 
		BSTR pwd, 
		BSTR title, 
		BSTR mobile, 
		BSTR txt, 
		BSTR content, 
		BSTR srcnumber, 
		BSTR stime, 
		BSTR* mdAudioSendResult
	);

	HRESULT RECSMS(
		BSTR sn, 
		BSTR pwd, 
		MOBody** RECSMSResult, int* RECSMSResult_nSizeIs
	);

	HRESULT UDPSIGN(
		BSTR sn, 
		BSTR pwd, 
		BSTR sign, 
		BSTR* UDPSIGNResult
	);

	HRESULT report(
		BSTR sn, 
		BSTR pwd, 
		__int64 maxid, 
		BSTR* reportResult
	);

	HRESULT Register(
		BSTR sn, 
		BSTR pwd, 
		BSTR province, 
		BSTR city, 
		BSTR trade, 
		BSTR entname, 
		BSTR linkman, 
		BSTR phone, 
		BSTR mobile, 
		BSTR email, 
		BSTR fax, 
		BSTR address, 
		BSTR postcode, 
		BSTR sign, 
		BSTR* RegisterResult
	);

	HRESULT SendSMSEx(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR subcode, 
		BSTR* SendSMSExResult
	);

	HRESULT SendSMSEx_R(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR subcode, 
		BSTR rrid, 
		BSTR* SendSMSEx_RResult
	);

	HRESULT mdMmsReceive(
		BSTR sn, 
		BSTR pwd, 
		BSTR* mdMmsReceiveResult
	);

	HRESULT TestCode(
		BSTR content, 
		BSTR code, 
		BSTR type, 
		BSTR* TestCodeResult
	);

	HRESULT mdSmsSend(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mdSmsSendResult
	);

	HRESULT mdSmsSend_AES(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mdSmsSend_AESResult
	);

	HRESULT GetAllInfo2(
		BSTR sn, 
		BSTR pwd, 
		BSTR ver, 
		BSTR oem, 
		RegistryInfo2* GetAllInfo2Result
	);

	HRESULT report2(
		BSTR sn, 
		BSTR pwd, 
		__int64 maxid, 
		BSTR rrid, 
		BSTR* report2Result
	);

	HRESULT UnRegister(
		BSTR sn, 
		BSTR pwd, 
		BSTR* UnRegisterResult
	);

	HRESULT fileMT(
		BSTR sn, 
		BSTR pwd, 
		BSTR fname, 
		BSTR rrid, 
		int sort, 
		int total, 
		int ftype, 
		BSTR content, 
		BSTR* fileMTResult
	);

	HRESULT GetFlag(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetFlagResult
	);

	HRESULT ChargUp(
		BSTR sn, 
		BSTR pwd, 
		BSTR cardno, 
		BSTR cardpwd, 
		BSTR* ChargUpResult
	);

	HRESULT SMSTest(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR* SMSTestResult
	);

	HRESULT RECSMSEx(
		BSTR sn, 
		BSTR pwd, 
		BSTR subcode, 
		MOBody** RECSMSExResult, int* RECSMSExResult_nSizeIs
	);

	HRESULT gxmt(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* gxmtResult
	);

	HRESULT RECSMS_UTF8(
		BSTR sn, 
		BSTR pwd, 
		MOBody** RECSMS_UTF8Result, int* RECSMS_UTF8Result_nSizeIs
	);

	HRESULT RECSMSEx_UTF8(
		BSTR sn, 
		BSTR pwd, 
		BSTR subcode, 
		MOBody** RECSMSEx_UTF8Result, int* RECSMSEx_UTF8Result_nSizeIs
	);

	HRESULT report2DES(
		BSTR sn, 
		BSTR pwd, 
		__int64 maxid, 
		BSTR rrid, 
		BSTR* report2DESResult
	);

	HRESULT GetAllInfo(
		BSTR sn, 
		BSTR pwd, 
		RegistryInfo* GetAllInfoResult
	);

	HRESULT mdSmsSend_u(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mdSmsSend_uResult
	);

	HRESULT mmsFileMT(
		BSTR sn, 
		BSTR pwd, 
		BSTR rrid, 
		BSTR content, 
		BSTR* mmsFileMTResult
	);

	HRESULT mo2(
		BSTR sn, 
		BSTR pwd, 
		int maxID, 
		BSTR* mo2Result
	);

	HRESULT mdMmsSendF(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR stime, 
		BSTR* mdMmsSendFResult
	);

	HRESULT msgid(
		BSTR sn, 
		BSTR pwd, 
		BSTR* msgidResult
	);

	HRESULT GetCode(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetCodeResult
	);

	HRESULT SendSMS_R(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR rrid, 
		BSTR* SendSMS_RResult
	);

	HRESULT UDPPwd(
		BSTR sn, 
		BSTR pwd, 
		BSTR newpwd, 
		BSTR* UDPPwdResult
	);

	HRESULT GetStatus(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetStatusResult
	);

	HRESULT mo(
		BSTR sn, 
		BSTR pwd, 
		BSTR* moResult
	);

	HRESULT mdFaxSend(
		BSTR sn, 
		BSTR pwd, 
		BSTR title, 
		BSTR mobile, 
		BSTR content, 
		BSTR srcnumber, 
		BSTR stime, 
		BSTR* mdFaxSendResult
	);
};

typedef CWebServiceT<> CWebService;

__if_not_exists(__RegistryInfo_entries)
{
extern __declspec(selectany) const _soapmapentry __RegistryInfo_entries[] =
{
	{ 
		0xC456957F, 
		"RESULT", 
		L"RESULT", 
		sizeof("RESULT")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, RESULT),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9786D034, 
		"REGISTRYCODE", 
		L"REGISTRYCODE", 
		sizeof("REGISTRYCODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, REGISTRYCODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x968D3E4C, 
		"SERVICECODE", 
		L"SERVICECODE", 
		sizeof("SERVICECODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, SERVICECODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x75A73A46, 
		"BALANCE", 
		L"BALANCE", 
		sizeof("BALANCE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, BALANCE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x60E05492, 
		"CREATEDATE", 
		L"CREATEDATE", 
		sizeof("CREATEDATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, CREATEDATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFF120A47, 
		"REGISTRYSTATEID", 
		L"REGISTRYSTATEID", 
		sizeof("REGISTRYSTATEID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, REGISTRYSTATEID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2CC65AA2, 
		"PRIORITY", 
		L"PRIORITY", 
		sizeof("PRIORITY")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, PRIORITY),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC825B1DF, 
		"FIRSTREGISTRYDATE", 
		L"FIRSTREGISTRYDATE", 
		sizeof("FIRSTREGISTRYDATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, FIRSTREGISTRYDATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x97872317, 
		"REGISTRYDATE", 
		L"REGISTRYDATE", 
		sizeof("REGISTRYDATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, REGISTRYDATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x363480FC, 
		"AGENTID", 
		L"AGENTID", 
		sizeof("AGENTID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, AGENTID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xDDBA48C2, 
		"PRODUCTNAME", 
		L"PRODUCTNAME", 
		sizeof("PRODUCTNAME")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, PRODUCTNAME),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0D253028, 
		"DESTMOBILE", 
		L"DESTMOBILE", 
		sizeof("DESTMOBILE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, DESTMOBILE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0027AE7A, 
		"FLAG", 
		L"FLAG", 
		sizeof("FLAG")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, FLAG),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7E8AD3A7, 
		"REPLYCONTENT", 
		L"REPLYCONTENT", 
		sizeof("REPLYCONTENT")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, REPLYCONTENT),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x183E895C, 
		"ISRETURN", 
		L"ISRETURN", 
		sizeof("ISRETURN")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, ISRETURN),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x82FEFA06, 
		"VERSION", 
		L"VERSION", 
		sizeof("VERSION")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, VERSION),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0532E063, 
		"GRADE", 
		L"GRADE", 
		sizeof("GRADE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, GRADE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBF63120A, 
		"PARENT", 
		L"PARENT", 
		sizeof("PARENT")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, PARENT),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8A1966DF, 
		"SUBSIDIARY", 
		L"SUBSIDIARY", 
		sizeof("SUBSIDIARY")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, SUBSIDIARY),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05F877C5, 
		"ROLES", 
		L"ROLES", 
		sizeof("ROLES")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, ROLES),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA6C78473, 
		"BALSTATUS", 
		L"BALSTATUS", 
		sizeof("BALSTATUS")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, BALSTATUS),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7B5D3AA9, 
		"DISCOUNT", 
		L"DISCOUNT", 
		sizeof("DISCOUNT")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo, DISCOUNT),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __RegistryInfo_map =
{
	0xC86BE7C5,
	"RegistryInfo",
	L"RegistryInfo",
	sizeof("RegistryInfo")-1,
	sizeof("RegistryInfo")-1,
	SOAPMAP_STRUCT,
	__RegistryInfo_entries,
	sizeof(RegistryInfo),
	22,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__MOBody_entries)
{
extern __declspec(selectany) const _soapmapentry __MOBody_entries[] =
{
	{ 
		0x046EC053, 
		"total_num", 
		L"total_num", 
		sizeof("total_num")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(MOBody, total_num),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x50C1CBC7, 
		"this_num", 
		L"this_num", 
		sizeof("this_num")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(MOBody, this_num),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3A4A3EF5, 
		"recvtel", 
		L"recvtel", 
		sizeof("recvtel")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(MOBody, recvtel),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x13A2F3C1, 
		"sender", 
		L"sender", 
		sizeof("sender")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(MOBody, sender),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(MOBody, content),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3A401138, 
		"recdate", 
		L"recdate", 
		sizeof("recdate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(MOBody, recdate),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __MOBody_map =
{
	0xB958DECA,
	"MOBody",
	L"MOBody",
	sizeof("MOBody")-1,
	sizeof("MOBody")-1,
	SOAPMAP_STRUCT,
	__MOBody_entries,
	sizeof(MOBody),
	6,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__RegistryInfo2_entries)
{
extern __declspec(selectany) const _soapmapentry __RegistryInfo2_entries[] =
{
	{ 
		0xC456957F, 
		"RESULT", 
		L"RESULT", 
		sizeof("RESULT")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, RESULT),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9786D034, 
		"REGISTRYCODE", 
		L"REGISTRYCODE", 
		sizeof("REGISTRYCODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, REGISTRYCODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x968D3E4C, 
		"SERVICECODE", 
		L"SERVICECODE", 
		sizeof("SERVICECODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, SERVICECODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x75A73A46, 
		"BALANCE", 
		L"BALANCE", 
		sizeof("BALANCE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, BALANCE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x60E05492, 
		"CREATEDATE", 
		L"CREATEDATE", 
		sizeof("CREATEDATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, CREATEDATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFF120A47, 
		"REGISTRYSTATEID", 
		L"REGISTRYSTATEID", 
		sizeof("REGISTRYSTATEID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, REGISTRYSTATEID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2CC65AA2, 
		"PRIORITY", 
		L"PRIORITY", 
		sizeof("PRIORITY")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, PRIORITY),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC825B1DF, 
		"FIRSTREGISTRYDATE", 
		L"FIRSTREGISTRYDATE", 
		sizeof("FIRSTREGISTRYDATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, FIRSTREGISTRYDATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x97872317, 
		"REGISTRYDATE", 
		L"REGISTRYDATE", 
		sizeof("REGISTRYDATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, REGISTRYDATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x363480FC, 
		"AGENTID", 
		L"AGENTID", 
		sizeof("AGENTID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, AGENTID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xDDBA48C2, 
		"PRODUCTNAME", 
		L"PRODUCTNAME", 
		sizeof("PRODUCTNAME")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, PRODUCTNAME),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0D253028, 
		"DESTMOBILE", 
		L"DESTMOBILE", 
		sizeof("DESTMOBILE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, DESTMOBILE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0027AE7A, 
		"FLAG", 
		L"FLAG", 
		sizeof("FLAG")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, FLAG),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7E8AD3A7, 
		"REPLYCONTENT", 
		L"REPLYCONTENT", 
		sizeof("REPLYCONTENT")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, REPLYCONTENT),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x183E895C, 
		"ISRETURN", 
		L"ISRETURN", 
		sizeof("ISRETURN")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, ISRETURN),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x82FEFA06, 
		"VERSION", 
		L"VERSION", 
		sizeof("VERSION")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, VERSION),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0532E063, 
		"GRADE", 
		L"GRADE", 
		sizeof("GRADE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, GRADE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBF63120A, 
		"PARENT", 
		L"PARENT", 
		sizeof("PARENT")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, PARENT),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8A1966DF, 
		"SUBSIDIARY", 
		L"SUBSIDIARY", 
		sizeof("SUBSIDIARY")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, SUBSIDIARY),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05F877C5, 
		"ROLES", 
		L"ROLES", 
		sizeof("ROLES")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, ROLES),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA6C78473, 
		"BALSTATUS", 
		L"BALSTATUS", 
		sizeof("BALSTATUS")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, BALSTATUS),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7B5D3AA9, 
		"DISCOUNT", 
		L"DISCOUNT", 
		sizeof("DISCOUNT")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, DISCOUNT),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0608E02F, 
		"SLIST", 
		L"SLIST", 
		sizeof("SLIST")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, SLIST),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D38FC, 
		"POPM", 
		L"POPM", 
		sizeof("POPM")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, POPM),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCC148F23, 
		"UPDATE", 
		L"UPDATE", 
		sizeof("UPDATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, UPDATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x000151CD, 
		"MMS", 
		L"MMS", 
		sizeof("MMS")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RegistryInfo2, MMS),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __RegistryInfo2_map =
{
	0xD5E8E097,
	"RegistryInfo2",
	L"RegistryInfo2",
	sizeof("RegistryInfo2")-1,
	sizeof("RegistryInfo2")-1,
	SOAPMAP_STRUCT,
	__RegistryInfo2_entries,
	sizeof(RegistryInfo2),
	26,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

struct __CWebService_GetGaoDuan_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR GetGaoDuanResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_GetGaoDuan_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetGaoDuan_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetGaoDuan_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xB89C3BBE, 
		"GetGaoDuanResult", 
		L"GetGaoDuanResult", 
		sizeof("GetGaoDuanResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetGaoDuan_struct, GetGaoDuanResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_GetGaoDuan_map =
{
	0x5041438E,
	"GetGaoDuan",
	L"GetGaoDuanResponse",
	sizeof("GetGaoDuan")-1,
	sizeof("GetGaoDuanResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_GetGaoDuan_entries,
	sizeof(__CWebService_GetGaoDuan_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_GetBalance_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR GetBalanceResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_GetBalance_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetBalance_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetBalance_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x052DCD25, 
		"GetBalanceResult", 
		L"GetBalanceResult", 
		sizeof("GetBalanceResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetBalance_struct, GetBalanceResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_GetBalance_map =
{
	0x077CCAB5,
	"GetBalance",
	L"GetBalanceResponse",
	sizeof("GetBalance")-1,
	sizeof("GetBalanceResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_GetBalance_entries,
	sizeof(__CWebService_GetBalance_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mdSmsSend_DES_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR ext;
	BSTR stime;
	BSTR rrid;
	BSTR mdSmsSend_DESResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mdSmsSend_DES_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_DES_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_DES_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_DES_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_DES_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001BD91, 
		"ext", 
		L"ext", 
		sizeof("ext")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_DES_struct, ext),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_DES_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_DES_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0xF79EA068, 
		"mdSmsSend_DESResult", 
		L"mdSmsSend_DESResult", 
		sizeof("mdSmsSend_DESResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_DES_struct, mdSmsSend_DESResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mdSmsSend_DES_map =
{
	0x596F7AB8,
	"mdSmsSend_DES",
	L"mdSmsSend_DESResponse",
	sizeof("mdSmsSend_DES")-1,
	sizeof("mdSmsSend_DESResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mdSmsSend_DES_entries,
	sizeof(__CWebService_mdSmsSend_DES_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_balance_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR balanceResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_balance_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_balance_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_balance_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x4A0911C5, 
		"balanceResult", 
		L"balanceResult", 
		sizeof("balanceResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_balance_struct, balanceResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_balance_map =
{
	0xF03BB755,
	"balance",
	L"balanceResponse",
	sizeof("balance")-1,
	sizeof("balanceResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_balance_entries,
	sizeof(__CWebService_balance_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mt_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR ext;
	BSTR stime;
	BSTR rrid;
	BSTR mtResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mt_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mt_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mt_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mt_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mt_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001BD91, 
		"ext", 
		L"ext", 
		sizeof("ext")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mt_struct, ext),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mt_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mt_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0x4016E960, 
		"mtResult", 
		L"mtResult", 
		sizeof("mtResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mt_struct, mtResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mt_map =
{
	0xA11DE1B0,
	"mt",
	L"mtResponse",
	sizeof("mt")-1,
	sizeof("mtResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mt_entries,
	sizeof(__CWebService_mt_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_UDPSIGNEX_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR subcode;
	BSTR sign;
	BSTR comName;
	BSTR UDPSIGNEXResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_UDPSIGNEX_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGNEX_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGNEX_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xAC7A6CA5, 
		"subcode", 
		L"subcode", 
		sizeof("subcode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGNEX_struct, subcode),
		NULL,
		NULL,
		-1,
	},
	{
		0x0040DBF1, 
		"sign", 
		L"sign", 
		sizeof("sign")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGNEX_struct, sign),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF92FC00, 
		"comName", 
		L"comName", 
		sizeof("comName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGNEX_struct, comName),
		NULL,
		NULL,
		-1,
	},
	{
		0x2F185DB6, 
		"UDPSIGNEXResult", 
		L"UDPSIGNEXResult", 
		sizeof("UDPSIGNEXResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGNEX_struct, UDPSIGNEXResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_UDPSIGNEX_map =
{
	0x564DC386,
	"UDPSIGNEX",
	L"UDPSIGNEXResponse",
	sizeof("UDPSIGNEX")-1,
	sizeof("UDPSIGNEXResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_UDPSIGNEX_entries,
	sizeof(__CWebService_UDPSIGNEX_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_SendSMS_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR SendSMSResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_SendSMS_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x7DBD9A9C, 
		"SendSMSResult", 
		L"SendSMSResult", 
		sizeof("SendSMSResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_struct, SendSMSResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_SendSMS_map =
{
	0xE335D1EC,
	"SendSMS",
	L"SendSMSResponse",
	sizeof("SendSMS")-1,
	sizeof("SendSMSResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_SendSMS_entries,
	sizeof(__CWebService_SendSMS_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mdMmsSend_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR title;
	BSTR mobile;
	BSTR content;
	BSTR stime;
	BSTR mdMmsSendResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mdMmsSend_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSend_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSend_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSend_struct, title),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSend_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSend_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSend_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0xEB94DDA7, 
		"mdMmsSendResult", 
		L"mdMmsSendResult", 
		sizeof("mdMmsSendResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSend_struct, mdMmsSendResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mdMmsSend_map =
{
	0x23EA03B7,
	"mdMmsSend",
	L"mdMmsSendResponse",
	sizeof("mdMmsSend")-1,
	sizeof("mdMmsSendResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mdMmsSend_entries,
	sizeof(__CWebService_mdMmsSend_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_SetGaoDuan_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR gd;
	double SetGaoDuanResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_SetGaoDuan_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SetGaoDuan_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SetGaoDuan_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000DAB, 
		"gd", 
		L"gd", 
		sizeof("gd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SetGaoDuan_struct, gd),
		NULL,
		NULL,
		-1,
	},
	{
		0x4B1A024A, 
		"SetGaoDuanResult", 
		L"SetGaoDuanResult", 
		sizeof("SetGaoDuanResult")-1, 
		SOAPTYPE_DOUBLE, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_SetGaoDuan_struct, SetGaoDuanResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_SetGaoDuan_map =
{
	0x794ADD1A,
	"SetGaoDuan",
	L"SetGaoDuanResponse",
	sizeof("SetGaoDuan")-1,
	sizeof("SetGaoDuanResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_SetGaoDuan_entries,
	sizeof(__CWebService_SetGaoDuan_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mdAudioSend_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR title;
	BSTR mobile;
	BSTR txt;
	BSTR content;
	BSTR srcnumber;
	BSTR stime;
	BSTR mdAudioSendResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mdAudioSend_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdAudioSend_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdAudioSend_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdAudioSend_struct, title),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdAudioSend_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001FD60, 
		"txt", 
		L"txt", 
		sizeof("txt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdAudioSend_struct, txt),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdAudioSend_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x57172A11, 
		"srcnumber", 
		L"srcnumber", 
		sizeof("srcnumber")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdAudioSend_struct, srcnumber),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdAudioSend_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0x5E636A8C, 
		"mdAudioSendResult", 
		L"mdAudioSendResult", 
		sizeof("mdAudioSendResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdAudioSend_struct, mdAudioSendResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mdAudioSend_map =
{
	0x848F5DDC,
	"mdAudioSend",
	L"mdAudioSendResponse",
	sizeof("mdAudioSend")-1,
	sizeof("mdAudioSendResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mdAudioSend_entries,
	sizeof(__CWebService_mdAudioSend_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_RECSMS_struct
{
	BSTR sn;
	BSTR pwd;
	MOBody *RECSMSResult;
	int __RECSMSResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWebService_RECSMS_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMS_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMS_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x3EA7DAEC, 
		"RECSMSResult", 
		L"RECSMSResult", 
		sizeof("RECSMSResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMS_struct, RECSMSResult),
		NULL,
		&__MOBody_map,
		2+1,
	},
	{
		0x3EA7DAEC,
		"__RECSMSResult_nSizeIs",
		L"__RECSMSResult_nSizeIs",
		sizeof("__RECSMSResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWebService_RECSMS_struct, __RECSMSResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_RECSMS_map =
{
	0x87B1663C,
	"RECSMS",
	L"RECSMSResponse",
	sizeof("RECSMS")-1,
	sizeof("RECSMSResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_RECSMS_entries,
	sizeof(__CWebService_RECSMS_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_UDPSIGN_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR sign;
	BSTR UDPSIGNResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_UDPSIGN_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGN_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGN_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x0040DBF1, 
		"sign", 
		L"sign", 
		sizeof("sign")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGN_struct, sign),
		NULL,
		NULL,
		-1,
	},
	{
		0x50412C39, 
		"UDPSIGNResult", 
		L"UDPSIGNResult", 
		sizeof("UDPSIGNResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPSIGN_struct, UDPSIGNResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_UDPSIGN_map =
{
	0x64E43EC9,
	"UDPSIGN",
	L"UDPSIGNResponse",
	sizeof("UDPSIGN")-1,
	sizeof("UDPSIGNResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_UDPSIGN_entries,
	sizeof(__CWebService_UDPSIGN_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_report_struct
{
	BSTR sn;
	BSTR pwd;
	__int64 maxid;
	BSTR reportResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_report_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x07EBAC13, 
		"maxid", 
		L"maxid", 
		sizeof("maxid")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_report_struct, maxid),
		NULL,
		NULL,
		-1,
	},
	{
		0x8DE7A77B, 
		"reportResult", 
		L"reportResult", 
		sizeof("reportResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report_struct, reportResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_report_map =
{
	0xA616928B,
	"report",
	L"reportResponse",
	sizeof("report")-1,
	sizeof("reportResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_report_entries,
	sizeof(__CWebService_report_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_Register_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR province;
	BSTR city;
	BSTR trade;
	BSTR entname;
	BSTR linkman;
	BSTR phone;
	BSTR mobile;
	BSTR email;
	BSTR fax;
	BSTR address;
	BSTR postcode;
	BSTR sign;
	BSTR RegisterResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_Register_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xAFBB8E86, 
		"province", 
		L"province", 
		sizeof("province")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, province),
		NULL,
		NULL,
		-1,
	},
	{
		0x00381799, 
		"city", 
		L"city", 
		sizeof("city")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, city),
		NULL,
		NULL,
		-1,
	},
	{
		0x08734790, 
		"trade", 
		L"trade", 
		sizeof("trade")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, trade),
		NULL,
		NULL,
		-1,
	},
	{
		0x67C28488, 
		"entname", 
		L"entname", 
		sizeof("entname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, entname),
		NULL,
		NULL,
		-1,
	},
	{
		0x76825C0A, 
		"linkman", 
		L"linkman", 
		sizeof("linkman")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, linkman),
		NULL,
		NULL,
		-1,
	},
	{
		0x0825A69A, 
		"phone", 
		L"phone", 
		sizeof("phone")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, phone),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0x07611AC8, 
		"email", 
		L"email", 
		sizeof("email")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, email),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001BEDF, 
		"fax", 
		L"fax", 
		sizeof("fax")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, fax),
		NULL,
		NULL,
		-1,
	},
	{
		0x1B66B686, 
		"address", 
		L"address", 
		sizeof("address")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, address),
		NULL,
		NULL,
		-1,
	},
	{
		0xD1FA03E1, 
		"postcode", 
		L"postcode", 
		sizeof("postcode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, postcode),
		NULL,
		NULL,
		-1,
	},
	{
		0x0040DBF1, 
		"sign", 
		L"sign", 
		sizeof("sign")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, sign),
		NULL,
		NULL,
		-1,
	},
	{
		0x8D384E44, 
		"RegisterResult", 
		L"RegisterResult", 
		sizeof("RegisterResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_Register_struct, RegisterResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_Register_map =
{
	0xBC2C0F94,
	"Register",
	L"RegisterResponse",
	sizeof("Register")-1,
	sizeof("RegisterResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_Register_entries,
	sizeof(__CWebService_Register_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_SendSMSEx_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR subcode;
	BSTR SendSMSExResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_SendSMSEx_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0xAC7A6CA5, 
		"subcode", 
		L"subcode", 
		sizeof("subcode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_struct, subcode),
		NULL,
		NULL,
		-1,
	},
	{
		0x4CB188F9, 
		"SendSMSExResult", 
		L"SendSMSExResult", 
		sizeof("SendSMSExResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_struct, SendSMSExResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_SendSMSEx_map =
{
	0x3EDECB89,
	"SendSMSEx",
	L"SendSMSExResponse",
	sizeof("SendSMSEx")-1,
	sizeof("SendSMSExResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_SendSMSEx_entries,
	sizeof(__CWebService_SendSMSEx_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_SendSMSEx_R_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR subcode;
	BSTR rrid;
	BSTR SendSMSEx_RResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_SendSMSEx_R_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_R_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_R_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_R_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_R_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0xAC7A6CA5, 
		"subcode", 
		L"subcode", 
		sizeof("subcode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_R_struct, subcode),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_R_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0x39E51CCA, 
		"SendSMSEx_RResult", 
		L"SendSMSEx_RResult", 
		sizeof("SendSMSEx_RResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMSEx_R_struct, SendSMSEx_RResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_SendSMSEx_R_map =
{
	0x4746979A,
	"SendSMSEx_R",
	L"SendSMSEx_RResponse",
	sizeof("SendSMSEx_R")-1,
	sizeof("SendSMSEx_RResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_SendSMSEx_R_entries,
	sizeof(__CWebService_SendSMSEx_R_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mdMmsReceive_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mdMmsReceiveResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mdMmsReceive_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsReceive_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsReceive_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x460B3FC0, 
		"mdMmsReceiveResult", 
		L"mdMmsReceiveResult", 
		sizeof("mdMmsReceiveResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsReceive_struct, mdMmsReceiveResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mdMmsReceive_map =
{
	0xF5815010,
	"mdMmsReceive",
	L"mdMmsReceiveResponse",
	sizeof("mdMmsReceive")-1,
	sizeof("mdMmsReceiveResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mdMmsReceive_entries,
	sizeof(__CWebService_mdMmsReceive_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_TestCode_struct
{
	BSTR content;
	BSTR code;
	BSTR type;
	BSTR TestCodeResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_TestCode_entries[] =
{

	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_TestCode_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x00382EFB, 
		"code", 
		L"code", 
		sizeof("code")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_TestCode_struct, code),
		NULL,
		NULL,
		-1,
	},
	{
		0x0041AD82, 
		"type", 
		L"type", 
		sizeof("type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_TestCode_struct, type),
		NULL,
		NULL,
		-1,
	},
	{
		0x2CBC95FA, 
		"TestCodeResult", 
		L"TestCodeResult", 
		sizeof("TestCodeResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_TestCode_struct, TestCodeResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_TestCode_map =
{
	0x4DE11CCA,
	"TestCode",
	L"TestCodeResponse",
	sizeof("TestCode")-1,
	sizeof("TestCodeResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_TestCode_entries,
	sizeof(__CWebService_TestCode_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mdSmsSend_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR ext;
	BSTR stime;
	BSTR rrid;
	BSTR mdSmsSendResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mdSmsSend_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001BD91, 
		"ext", 
		L"ext", 
		sizeof("ext")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_struct, ext),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7CF16AD, 
		"mdSmsSendResult", 
		L"mdSmsSendResult", 
		sizeof("mdSmsSendResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_struct, mdSmsSendResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mdSmsSend_map =
{
	0xF796963D,
	"mdSmsSend",
	L"mdSmsSendResponse",
	sizeof("mdSmsSend")-1,
	sizeof("mdSmsSendResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mdSmsSend_entries,
	sizeof(__CWebService_mdSmsSend_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mdSmsSend_AES_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR ext;
	BSTR stime;
	BSTR rrid;
	BSTR mdSmsSend_AESResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mdSmsSend_AES_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_AES_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_AES_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_AES_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_AES_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001BD91, 
		"ext", 
		L"ext", 
		sizeof("ext")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_AES_struct, ext),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_AES_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_AES_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0x9A294D65, 
		"mdSmsSend_AESResult", 
		L"mdSmsSend_AESResult", 
		sizeof("mdSmsSend_AESResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_AES_struct, mdSmsSend_AESResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mdSmsSend_AES_map =
{
	0xC9595AF5,
	"mdSmsSend_AES",
	L"mdSmsSend_AESResponse",
	sizeof("mdSmsSend_AES")-1,
	sizeof("mdSmsSend_AESResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mdSmsSend_AES_entries,
	sizeof(__CWebService_mdSmsSend_AES_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_GetAllInfo2_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR ver;
	BSTR oem;
	RegistryInfo2 GetAllInfo2Result;
};

extern __declspec(selectany) const _soapmapentry __CWebService_GetAllInfo2_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetAllInfo2_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetAllInfo2_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x0002036D, 
		"ver", 
		L"ver", 
		sizeof("ver")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetAllInfo2_struct, ver),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001E5A1, 
		"oem", 
		L"oem", 
		sizeof("oem")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetAllInfo2_struct, oem),
		NULL,
		NULL,
		-1,
	},
	{
		0x237B6036, 
		"GetAllInfo2Result", 
		L"GetAllInfo2Result", 
		sizeof("GetAllInfo2Result")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_GetAllInfo2_struct, GetAllInfo2Result),
		NULL,
		&__RegistryInfo2_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_GetAllInfo2_map =
{
	0xEF7B6606,
	"GetAllInfo2",
	L"GetAllInfo2Response",
	sizeof("GetAllInfo2")-1,
	sizeof("GetAllInfo2Response")-1,
	SOAPMAP_FUNC,
	__CWebService_GetAllInfo2_entries,
	sizeof(__CWebService_GetAllInfo2_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_report2_struct
{
	BSTR sn;
	BSTR pwd;
	__int64 maxid;
	BSTR rrid;
	BSTR report2Result;
};

extern __declspec(selectany) const _soapmapentry __CWebService_report2_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report2_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report2_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x07EBAC13, 
		"maxid", 
		L"maxid", 
		sizeof("maxid")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_report2_struct, maxid),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report2_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0x7E44B0AD, 
		"report2Result", 
		L"report2Result", 
		sizeof("report2Result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report2_struct, report2Result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_report2_map =
{
	0x21DAB03D,
	"report2",
	L"report2Response",
	sizeof("report2")-1,
	sizeof("report2Response")-1,
	SOAPMAP_FUNC,
	__CWebService_report2_entries,
	sizeof(__CWebService_report2_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_UnRegister_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR UnRegisterResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_UnRegister_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UnRegister_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UnRegister_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xC38F0AE7, 
		"UnRegisterResult", 
		L"UnRegisterResult", 
		sizeof("UnRegisterResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UnRegister_struct, UnRegisterResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_UnRegister_map =
{
	0xE32480F7,
	"UnRegister",
	L"UnRegisterResponse",
	sizeof("UnRegister")-1,
	sizeof("UnRegisterResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_UnRegister_entries,
	sizeof(__CWebService_UnRegister_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_fileMT_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR fname;
	BSTR rrid;
	int sort;
	int total;
	int ftype;
	BSTR content;
	BSTR fileMTResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_fileMT_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_fileMT_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_fileMT_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x0773C027, 
		"fname", 
		L"fname", 
		sizeof("fname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_fileMT_struct, fname),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_fileMT_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0x0040F6E8, 
		"sort", 
		L"sort", 
		sizeof("sort")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_fileMT_struct, sort),
		NULL,
		NULL,
		-1,
	},
	{
		0x0871F2E4, 
		"total", 
		L"total", 
		sizeof("total")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_fileMT_struct, total),
		NULL,
		NULL,
		-1,
	},
	{
		0x077770E8, 
		"ftype", 
		L"ftype", 
		sizeof("ftype")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_fileMT_struct, ftype),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_fileMT_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0xF62010C0, 
		"fileMTResult", 
		L"fileMTResult", 
		sizeof("fileMTResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_fileMT_struct, fileMTResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_fileMT_map =
{
	0xFE0E6110,
	"fileMT",
	L"fileMTResponse",
	sizeof("fileMT")-1,
	sizeof("fileMTResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_fileMT_entries,
	sizeof(__CWebService_fileMT_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_GetFlag_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR GetFlagResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_GetFlag_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetFlag_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetFlag_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x4510CFF9, 
		"GetFlagResult", 
		L"GetFlagResult", 
		sizeof("GetFlagResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetFlag_struct, GetFlagResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_GetFlag_map =
{
	0xCC2BD289,
	"GetFlag",
	L"GetFlagResponse",
	sizeof("GetFlag")-1,
	sizeof("GetFlagResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_GetFlag_entries,
	sizeof(__CWebService_GetFlag_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_ChargUp_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR cardno;
	BSTR cardpwd;
	BSTR ChargUpResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_ChargUp_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_ChargUp_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_ChargUp_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xEE0A3A57, 
		"cardno", 
		L"cardno", 
		sizeof("cardno")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_ChargUp_struct, cardno),
		NULL,
		NULL,
		-1,
	},
	{
		0xAF518F25, 
		"cardpwd", 
		L"cardpwd", 
		sizeof("cardpwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_ChargUp_struct, cardpwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xC1A63329, 
		"ChargUpResult", 
		L"ChargUpResult", 
		sizeof("ChargUpResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_ChargUp_struct, ChargUpResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_ChargUp_map =
{
	0xC3A6C1B9,
	"ChargUp",
	L"ChargUpResponse",
	sizeof("ChargUp")-1,
	sizeof("ChargUpResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_ChargUp_entries,
	sizeof(__CWebService_ChargUp_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_SMSTest_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR SMSTestResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_SMSTest_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SMSTest_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SMSTest_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SMSTest_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xC44409F2, 
		"SMSTestResult", 
		L"SMSTestResult", 
		sizeof("SMSTestResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SMSTest_struct, SMSTestResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_SMSTest_map =
{
	0xE5156EC2,
	"SMSTest",
	L"SMSTestResponse",
	sizeof("SMSTest")-1,
	sizeof("SMSTestResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_SMSTest_entries,
	sizeof(__CWebService_SMSTest_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_RECSMSEx_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR subcode;
	MOBody *RECSMSExResult;
	int __RECSMSExResult_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWebService_RECSMSEx_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMSEx_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMSEx_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xAC7A6CA5, 
		"subcode", 
		L"subcode", 
		sizeof("subcode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMSEx_struct, subcode),
		NULL,
		NULL,
		-1,
	},
	{
		0xF12D1D49, 
		"RECSMSExResult", 
		L"RECSMSExResult", 
		sizeof("RECSMSExResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMSEx_struct, RECSMSExResult),
		NULL,
		&__MOBody_map,
		3+1,
	},
	{
		0xF12D1D49,
		"__RECSMSExResult_nSizeIs",
		L"__RECSMSExResult_nSizeIs",
		sizeof("__RECSMSExResult_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWebService_RECSMSEx_struct, __RECSMSExResult_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_RECSMSEx_map =
{
	0xF090B3D9,
	"RECSMSEx",
	L"RECSMSExResponse",
	sizeof("RECSMSEx")-1,
	sizeof("RECSMSExResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_RECSMSEx_entries,
	sizeof(__CWebService_RECSMSEx_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_gxmt_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR ext;
	BSTR stime;
	BSTR rrid;
	BSTR gxmtResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_gxmt_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_gxmt_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_gxmt_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_gxmt_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_gxmt_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001BD91, 
		"ext", 
		L"ext", 
		sizeof("ext")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_gxmt_struct, ext),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_gxmt_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_gxmt_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0x7AAC461F, 
		"gxmtResult", 
		L"gxmtResult", 
		sizeof("gxmtResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_gxmt_struct, gxmtResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_gxmt_map =
{
	0xD67D6A2F,
	"gxmt",
	L"gxmtResponse",
	sizeof("gxmt")-1,
	sizeof("gxmtResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_gxmt_entries,
	sizeof(__CWebService_gxmt_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_RECSMS_UTF8_struct
{
	BSTR sn;
	BSTR pwd;
	MOBody *RECSMS_UTF8Result;
	int __RECSMS_UTF8Result_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWebService_RECSMS_UTF8_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMS_UTF8_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMS_UTF8_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x4946EA32, 
		"RECSMS_UTF8Result", 
		L"RECSMS_UTF8Result", 
		sizeof("RECSMS_UTF8Result")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMS_UTF8_struct, RECSMS_UTF8Result),
		NULL,
		&__MOBody_map,
		2+1,
	},
	{
		0x4946EA32,
		"__RECSMS_UTF8Result_nSizeIs",
		L"__RECSMS_UTF8Result_nSizeIs",
		sizeof("__RECSMS_UTF8Result_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWebService_RECSMS_UTF8_struct, __RECSMS_UTF8Result_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_RECSMS_UTF8_map =
{
	0xB6515F02,
	"RECSMS_UTF8",
	L"RECSMS_UTF8Response",
	sizeof("RECSMS_UTF8")-1,
	sizeof("RECSMS_UTF8Response")-1,
	SOAPMAP_FUNC,
	__CWebService_RECSMS_UTF8_entries,
	sizeof(__CWebService_RECSMS_UTF8_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_RECSMSEx_UTF8_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR subcode;
	MOBody *RECSMSEx_UTF8Result;
	int __RECSMSEx_UTF8Result_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWebService_RECSMSEx_UTF8_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMSEx_UTF8_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMSEx_UTF8_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xAC7A6CA5, 
		"subcode", 
		L"subcode", 
		sizeof("subcode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMSEx_UTF8_struct, subcode),
		NULL,
		NULL,
		-1,
	},
	{
		0x24552EAF, 
		"RECSMSEx_UTF8Result", 
		L"RECSMSEx_UTF8Result", 
		sizeof("RECSMSEx_UTF8Result")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_RECSMSEx_UTF8_struct, RECSMSEx_UTF8Result),
		NULL,
		&__MOBody_map,
		3+1,
	},
	{
		0x24552EAF,
		"__RECSMSEx_UTF8Result_nSizeIs",
		L"__RECSMSEx_UTF8Result_nSizeIs",
		sizeof("__RECSMSEx_UTF8Result_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWebService_RECSMSEx_UTF8_struct, __RECSMSEx_UTF8Result_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_RECSMSEx_UTF8_map =
{
	0x8E02B6BF,
	"RECSMSEx_UTF8",
	L"RECSMSEx_UTF8Response",
	sizeof("RECSMSEx_UTF8")-1,
	sizeof("RECSMSEx_UTF8Response")-1,
	SOAPMAP_FUNC,
	__CWebService_RECSMSEx_UTF8_entries,
	sizeof(__CWebService_RECSMSEx_UTF8_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_report2DES_struct
{
	BSTR sn;
	BSTR pwd;
	__int64 maxid;
	BSTR rrid;
	BSTR report2DESResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_report2DES_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report2DES_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report2DES_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x07EBAC13, 
		"maxid", 
		L"maxid", 
		sizeof("maxid")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_report2DES_struct, maxid),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report2DES_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0x90336569, 
		"report2DESResult", 
		L"report2DESResult", 
		sizeof("report2DESResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_report2DES_struct, report2DESResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_report2DES_map =
{
	0x6A4983F9,
	"report2DES",
	L"report2DESResponse",
	sizeof("report2DES")-1,
	sizeof("report2DESResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_report2DES_entries,
	sizeof(__CWebService_report2DES_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_GetAllInfo_struct
{
	BSTR sn;
	BSTR pwd;
	RegistryInfo GetAllInfoResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_GetAllInfo_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetAllInfo_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetAllInfo_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x35D209E4, 
		"GetAllInfoResult", 
		L"GetAllInfoResult", 
		sizeof("GetAllInfoResult")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_GetAllInfo_struct, GetAllInfoResult),
		NULL,
		&__RegistryInfo_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_GetAllInfo_map =
{
	0xF2233334,
	"GetAllInfo",
	L"GetAllInfoResponse",
	sizeof("GetAllInfo")-1,
	sizeof("GetAllInfoResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_GetAllInfo_entries,
	sizeof(__CWebService_GetAllInfo_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mdSmsSend_u_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR ext;
	BSTR stime;
	BSTR rrid;
	BSTR mdSmsSend_uResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mdSmsSend_u_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_u_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_u_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_u_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_u_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001BD91, 
		"ext", 
		L"ext", 
		sizeof("ext")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_u_struct, ext),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_u_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_u_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0x78D335E1, 
		"mdSmsSend_uResult", 
		L"mdSmsSend_uResult", 
		sizeof("mdSmsSend_uResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdSmsSend_u_struct, mdSmsSend_uResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mdSmsSend_u_map =
{
	0xFA1F5271,
	"mdSmsSend_u",
	L"mdSmsSend_uResponse",
	sizeof("mdSmsSend_u")-1,
	sizeof("mdSmsSend_uResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mdSmsSend_u_entries,
	sizeof(__CWebService_mdSmsSend_u_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mmsFileMT_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR rrid;
	BSTR content;
	BSTR mmsFileMTResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mmsFileMT_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mmsFileMT_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mmsFileMT_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mmsFileMT_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mmsFileMT_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x3F27EE4D, 
		"mmsFileMTResult", 
		L"mmsFileMTResult", 
		sizeof("mmsFileMTResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mmsFileMT_struct, mmsFileMTResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mmsFileMT_map =
{
	0xA883D5DD,
	"mmsFileMT",
	L"mmsFileMTResponse",
	sizeof("mmsFileMT")-1,
	sizeof("mmsFileMTResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mmsFileMT_entries,
	sizeof(__CWebService_mmsFileMT_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mo2_struct
{
	BSTR sn;
	BSTR pwd;
	int maxID;
	BSTR mo2Result;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mo2_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mo2_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mo2_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x07EBA7D3, 
		"maxID", 
		L"maxID", 
		sizeof("maxID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CWebService_mo2_struct, maxID),
		NULL,
		NULL,
		-1,
	},
	{
		0xD91306CD, 
		"mo2Result", 
		L"mo2Result", 
		sizeof("mo2Result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mo2_struct, mo2Result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mo2_map =
{
	0x69970E5D,
	"mo2",
	L"mo2Response",
	sizeof("mo2")-1,
	sizeof("mo2Response")-1,
	SOAPMAP_FUNC,
	__CWebService_mo2_entries,
	sizeof(__CWebService_mo2_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mdMmsSendF_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR stime;
	BSTR mdMmsSendFResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mdMmsSendF_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSendF_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSendF_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSendF_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSendF_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSendF_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0x95256B6D, 
		"mdMmsSendFResult", 
		L"mdMmsSendFResult", 
		sizeof("mdMmsSendFResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdMmsSendF_struct, mdMmsSendFResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mdMmsSendF_map =
{
	0x73D51AFD,
	"mdMmsSendF",
	L"mdMmsSendFResponse",
	sizeof("mdMmsSendF")-1,
	sizeof("mdMmsSendFResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mdMmsSendF_entries,
	sizeof(__CWebService_mdMmsSendF_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_msgid_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR msgidResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_msgid_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_msgid_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_msgid_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x36B59FB3, 
		"msgidResult", 
		L"msgidResult", 
		sizeof("msgidResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_msgid_struct, msgidResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_msgid_map =
{
	0xBA4378C3,
	"msgid",
	L"msgidResponse",
	sizeof("msgid")-1,
	sizeof("msgidResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_msgid_entries,
	sizeof(__CWebService_msgid_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_GetCode_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR GetCodeResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_GetCode_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetCode_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetCode_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xC13774BA, 
		"GetCodeResult", 
		L"GetCodeResult", 
		sizeof("GetCodeResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetCode_struct, GetCodeResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_GetCode_map =
{
	0xEC8EAB8A,
	"GetCode",
	L"GetCodeResponse",
	sizeof("GetCode")-1,
	sizeof("GetCodeResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_GetCode_entries,
	sizeof(__CWebService_GetCode_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_SendSMS_R_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR mobile;
	BSTR content;
	BSTR rrid;
	BSTR SendSMS_RResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_SendSMS_R_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_R_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_R_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_R_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_R_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x00407611, 
		"rrid", 
		L"rrid", 
		sizeof("rrid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_R_struct, rrid),
		NULL,
		NULL,
		-1,
	},
	{
		0xDE3C232D, 
		"SendSMS_RResult", 
		L"SendSMS_RResult", 
		sizeof("SendSMS_RResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_SendSMS_R_struct, SendSMS_RResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_SendSMS_R_map =
{
	0x5D78C2BD,
	"SendSMS_R",
	L"SendSMS_RResponse",
	sizeof("SendSMS_R")-1,
	sizeof("SendSMS_RResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_SendSMS_R_entries,
	sizeof(__CWebService_SendSMS_R_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_UDPPwd_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR newpwd;
	BSTR UDPPwdResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_UDPPwd_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPPwd_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPPwd_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x07FE4D55, 
		"newpwd", 
		L"newpwd", 
		sizeof("newpwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPPwd_struct, newpwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x02A43F33, 
		"UDPPwdResult", 
		L"UDPPwdResult", 
		sizeof("UDPPwdResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_UDPPwd_struct, UDPPwdResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_UDPPwd_map =
{
	0x3C57F843,
	"UDPPwd",
	L"UDPPwdResponse",
	sizeof("UDPPwd")-1,
	sizeof("UDPPwdResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_UDPPwd_entries,
	sizeof(__CWebService_UDPPwd_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_GetStatus_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR GetStatusResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_GetStatus_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetStatus_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetStatus_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x92AF6903, 
		"GetStatusResult", 
		L"GetStatusResult", 
		sizeof("GetStatusResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_GetStatus_struct, GetStatusResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_GetStatus_map =
{
	0xFBD4D613,
	"GetStatus",
	L"GetStatusResponse",
	sizeof("GetStatus")-1,
	sizeof("GetStatusResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_GetStatus_entries,
	sizeof(__CWebService_GetStatus_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mo_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR moResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mo_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mo_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mo_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0xBF33B99B, 
		"moResult", 
		L"moResult", 
		sizeof("moResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mo_struct, moResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mo_map =
{
	0x5AAFACAB,
	"mo",
	L"moResponse",
	sizeof("mo")-1,
	sizeof("moResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mo_entries,
	sizeof(__CWebService_mo_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CWebService_mdFaxSend_struct
{
	BSTR sn;
	BSTR pwd;
	BSTR title;
	BSTR mobile;
	BSTR content;
	BSTR srcnumber;
	BSTR stime;
	BSTR mdFaxSendResult;
};

extern __declspec(selectany) const _soapmapentry __CWebService_mdFaxSend_entries[] =
{

	{
		0x00000F41, 
		"sn", 
		L"sn", 
		sizeof("sn")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdFaxSend_struct, sn),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001EC2B, 
		"pwd", 
		L"pwd", 
		sizeof("pwd")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdFaxSend_struct, pwd),
		NULL,
		NULL,
		-1,
	},
	{
		0x086EAA02, 
		"title", 
		L"title", 
		sizeof("title")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdFaxSend_struct, title),
		NULL,
		NULL,
		-1,
	},
	{
		0x06527698, 
		"mobile", 
		L"mobile", 
		sizeof("mobile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdFaxSend_struct, mobile),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFB9FC1B, 
		"content", 
		L"content", 
		sizeof("content")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdFaxSend_struct, content),
		NULL,
		NULL,
		-1,
	},
	{
		0x57172A11, 
		"srcnumber", 
		L"srcnumber", 
		sizeof("srcnumber")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdFaxSend_struct, srcnumber),
		NULL,
		NULL,
		-1,
	},
	{
		0x08626B02, 
		"stime", 
		L"stime", 
		sizeof("stime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdFaxSend_struct, stime),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F62C4D9, 
		"mdFaxSendResult", 
		L"mdFaxSendResult", 
		sizeof("mdFaxSendResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWebService_mdFaxSend_struct, mdFaxSendResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWebService_mdFaxSend_map =
{
	0x72CE7F69,
	"mdFaxSend",
	L"mdFaxSendResponse",
	sizeof("mdFaxSend")-1,
	sizeof("mdFaxSendResponse")-1,
	SOAPMAP_FUNC,
	__CWebService_mdFaxSend_entries,
	sizeof(__CWebService_mdFaxSend_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};

extern __declspec(selectany) const _soapmap * __CWebService_funcs[] =
{
	&__CWebService_GetGaoDuan_map,
	&__CWebService_GetBalance_map,
	&__CWebService_mdSmsSend_DES_map,
	&__CWebService_balance_map,
	&__CWebService_mt_map,
	&__CWebService_UDPSIGNEX_map,
	&__CWebService_SendSMS_map,
	&__CWebService_mdMmsSend_map,
	&__CWebService_SetGaoDuan_map,
	&__CWebService_mdAudioSend_map,
	&__CWebService_RECSMS_map,
	&__CWebService_UDPSIGN_map,
	&__CWebService_report_map,
	&__CWebService_Register_map,
	&__CWebService_SendSMSEx_map,
	&__CWebService_SendSMSEx_R_map,
	&__CWebService_mdMmsReceive_map,
	&__CWebService_TestCode_map,
	&__CWebService_mdSmsSend_map,
	&__CWebService_mdSmsSend_AES_map,
	&__CWebService_GetAllInfo2_map,
	&__CWebService_report2_map,
	&__CWebService_UnRegister_map,
	&__CWebService_fileMT_map,
	&__CWebService_GetFlag_map,
	&__CWebService_ChargUp_map,
	&__CWebService_SMSTest_map,
	&__CWebService_RECSMSEx_map,
	&__CWebService_gxmt_map,
	&__CWebService_RECSMS_UTF8_map,
	&__CWebService_RECSMSEx_UTF8_map,
	&__CWebService_report2DES_map,
	&__CWebService_GetAllInfo_map,
	&__CWebService_mdSmsSend_u_map,
	&__CWebService_mmsFileMT_map,
	&__CWebService_mo2_map,
	&__CWebService_mdMmsSendF_map,
	&__CWebService_msgid_map,
	&__CWebService_GetCode_map,
	&__CWebService_SendSMS_R_map,
	&__CWebService_UDPPwd_map,
	&__CWebService_GetStatus_map,
	&__CWebService_mo_map,
	&__CWebService_mdFaxSend_map,
	NULL
};

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::GetGaoDuan(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetGaoDuanResult
	)
{
    if ( GetGaoDuanResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_GetGaoDuan_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetGaoDuan\"\r\n"));
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

	*GetGaoDuanResult = __params.GetGaoDuanResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::GetBalance(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetBalanceResult
	)
{
    if ( GetBalanceResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_GetBalance_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetBalance\"\r\n"));
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

	*GetBalanceResult = __params.GetBalanceResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mdSmsSend_DES(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mdSmsSend_DESResult
	)
{
    if ( mdSmsSend_DESResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mdSmsSend_DES_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.ext = ext;
	__params.stime = stime;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 2);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mdSmsSend_DES\"\r\n"));
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

	*mdSmsSend_DESResult = __params.mdSmsSend_DESResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::balance(
		BSTR sn, 
		BSTR pwd, 
		BSTR* balanceResult
	)
{
    if ( balanceResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_balance_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 3);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/balance\"\r\n"));
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

	*balanceResult = __params.balanceResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mt(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mtResult
	)
{
    if ( mtResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mt_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.ext = ext;
	__params.stime = stime;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 4);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mt\"\r\n"));
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

	*mtResult = __params.mtResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::UDPSIGNEX(
		BSTR sn, 
		BSTR pwd, 
		BSTR subcode, 
		BSTR sign, 
		BSTR comName, 
		BSTR* UDPSIGNEXResult
	)
{
    if ( UDPSIGNEXResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_UDPSIGNEX_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.subcode = subcode;
	__params.sign = sign;
	__params.comName = comName;

	__atlsoap_hr = SetClientStruct(&__params, 5);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UDPSIGNEX\"\r\n"));
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

	*UDPSIGNEXResult = __params.UDPSIGNEXResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::SendSMS(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR* SendSMSResult
	)
{
    if ( SendSMSResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_SendSMS_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;

	__atlsoap_hr = SetClientStruct(&__params, 6);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SendSMS\"\r\n"));
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

	*SendSMSResult = __params.SendSMSResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mdMmsSend(
		BSTR sn, 
		BSTR pwd, 
		BSTR title, 
		BSTR mobile, 
		BSTR content, 
		BSTR stime, 
		BSTR* mdMmsSendResult
	)
{
    if ( mdMmsSendResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mdMmsSend_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.title = title;
	__params.mobile = mobile;
	__params.content = content;
	__params.stime = stime;

	__atlsoap_hr = SetClientStruct(&__params, 7);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mdMmsSend\"\r\n"));
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

	*mdMmsSendResult = __params.mdMmsSendResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::SetGaoDuan(
		BSTR sn, 
		BSTR pwd, 
		BSTR gd, 
		double* SetGaoDuanResult
	)
{
    if ( SetGaoDuanResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_SetGaoDuan_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.gd = gd;

	__atlsoap_hr = SetClientStruct(&__params, 8);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SetGaoDuan\"\r\n"));
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

	*SetGaoDuanResult = __params.SetGaoDuanResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mdAudioSend(
		BSTR sn, 
		BSTR pwd, 
		BSTR title, 
		BSTR mobile, 
		BSTR txt, 
		BSTR content, 
		BSTR srcnumber, 
		BSTR stime, 
		BSTR* mdAudioSendResult
	)
{
    if ( mdAudioSendResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mdAudioSend_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.title = title;
	__params.mobile = mobile;
	__params.txt = txt;
	__params.content = content;
	__params.srcnumber = srcnumber;
	__params.stime = stime;

	__atlsoap_hr = SetClientStruct(&__params, 9);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mdAudioSend\"\r\n"));
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

	*mdAudioSendResult = __params.mdAudioSendResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::RECSMS(
		BSTR sn, 
		BSTR pwd, 
		MOBody** RECSMSResult, int* __RECSMSResult_nSizeIs
	)
{
    if ( RECSMSResult == NULL )
		return E_POINTER;
	if( __RECSMSResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_RECSMS_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 10);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/RECSMS\"\r\n"));
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

	*RECSMSResult = __params.RECSMSResult;
	*__RECSMSResult_nSizeIs = __params.__RECSMSResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::UDPSIGN(
		BSTR sn, 
		BSTR pwd, 
		BSTR sign, 
		BSTR* UDPSIGNResult
	)
{
    if ( UDPSIGNResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_UDPSIGN_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.sign = sign;

	__atlsoap_hr = SetClientStruct(&__params, 11);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UDPSIGN\"\r\n"));
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

	*UDPSIGNResult = __params.UDPSIGNResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::report(
		BSTR sn, 
		BSTR pwd, 
		__int64 maxid, 
		BSTR* reportResult
	)
{
    if ( reportResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_report_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.maxid = maxid;

	__atlsoap_hr = SetClientStruct(&__params, 12);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/report\"\r\n"));
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

	*reportResult = __params.reportResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::Register(
		BSTR sn, 
		BSTR pwd, 
		BSTR province, 
		BSTR city, 
		BSTR trade, 
		BSTR entname, 
		BSTR linkman, 
		BSTR phone, 
		BSTR mobile, 
		BSTR email, 
		BSTR fax, 
		BSTR address, 
		BSTR postcode, 
		BSTR sign, 
		BSTR* RegisterResult
	)
{
    if ( RegisterResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_Register_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.province = province;
	__params.city = city;
	__params.trade = trade;
	__params.entname = entname;
	__params.linkman = linkman;
	__params.phone = phone;
	__params.mobile = mobile;
	__params.email = email;
	__params.fax = fax;
	__params.address = address;
	__params.postcode = postcode;
	__params.sign = sign;

	__atlsoap_hr = SetClientStruct(&__params, 13);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Register\"\r\n"));
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

	*RegisterResult = __params.RegisterResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::SendSMSEx(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR subcode, 
		BSTR* SendSMSExResult
	)
{
    if ( SendSMSExResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_SendSMSEx_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.subcode = subcode;

	__atlsoap_hr = SetClientStruct(&__params, 14);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SendSMSEx\"\r\n"));
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

	*SendSMSExResult = __params.SendSMSExResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::SendSMSEx_R(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR subcode, 
		BSTR rrid, 
		BSTR* SendSMSEx_RResult
	)
{
    if ( SendSMSEx_RResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_SendSMSEx_R_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.subcode = subcode;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 15);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SendSMSEx_R\"\r\n"));
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

	*SendSMSEx_RResult = __params.SendSMSEx_RResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mdMmsReceive(
		BSTR sn, 
		BSTR pwd, 
		BSTR* mdMmsReceiveResult
	)
{
    if ( mdMmsReceiveResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mdMmsReceive_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 16);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mdMmsReceive\"\r\n"));
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

	*mdMmsReceiveResult = __params.mdMmsReceiveResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::TestCode(
		BSTR content, 
		BSTR code, 
		BSTR type, 
		BSTR* TestCodeResult
	)
{
    if ( TestCodeResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_TestCode_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.content = content;
	__params.code = code;
	__params.type = type;

	__atlsoap_hr = SetClientStruct(&__params, 17);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/TestCode\"\r\n"));
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

	*TestCodeResult = __params.TestCodeResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mdSmsSend(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mdSmsSendResult
	)
{
    if ( mdSmsSendResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mdSmsSend_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.ext = ext;
	__params.stime = stime;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 18);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mdSmsSend\"\r\n"));
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

	*mdSmsSendResult = __params.mdSmsSendResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mdSmsSend_AES(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mdSmsSend_AESResult
	)
{
    if ( mdSmsSend_AESResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mdSmsSend_AES_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.ext = ext;
	__params.stime = stime;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 19);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mdSmsSend_AES\"\r\n"));
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

	*mdSmsSend_AESResult = __params.mdSmsSend_AESResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::GetAllInfo2(
		BSTR sn, 
		BSTR pwd, 
		BSTR ver, 
		BSTR oem, 
		RegistryInfo2* GetAllInfo2Result
	)
{
    if ( GetAllInfo2Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_GetAllInfo2_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.ver = ver;
	__params.oem = oem;

	__atlsoap_hr = SetClientStruct(&__params, 20);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetAllInfo2\"\r\n"));
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

	*GetAllInfo2Result = __params.GetAllInfo2Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::report2(
		BSTR sn, 
		BSTR pwd, 
		__int64 maxid, 
		BSTR rrid, 
		BSTR* report2Result
	)
{
    if ( report2Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_report2_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.maxid = maxid;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 21);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/report2\"\r\n"));
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

	*report2Result = __params.report2Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::UnRegister(
		BSTR sn, 
		BSTR pwd, 
		BSTR* UnRegisterResult
	)
{
    if ( UnRegisterResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_UnRegister_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 22);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UnRegister\"\r\n"));
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

	*UnRegisterResult = __params.UnRegisterResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::fileMT(
		BSTR sn, 
		BSTR pwd, 
		BSTR fname, 
		BSTR rrid, 
		int sort, 
		int total, 
		int ftype, 
		BSTR content, 
		BSTR* fileMTResult
	)
{
    if ( fileMTResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_fileMT_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.fname = fname;
	__params.rrid = rrid;
	__params.sort = sort;
	__params.total = total;
	__params.ftype = ftype;
	__params.content = content;

	__atlsoap_hr = SetClientStruct(&__params, 23);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/fileMT\"\r\n"));
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

	*fileMTResult = __params.fileMTResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::GetFlag(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetFlagResult
	)
{
    if ( GetFlagResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_GetFlag_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 24);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetFlag\"\r\n"));
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

	*GetFlagResult = __params.GetFlagResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::ChargUp(
		BSTR sn, 
		BSTR pwd, 
		BSTR cardno, 
		BSTR cardpwd, 
		BSTR* ChargUpResult
	)
{
    if ( ChargUpResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_ChargUp_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.cardno = cardno;
	__params.cardpwd = cardpwd;

	__atlsoap_hr = SetClientStruct(&__params, 25);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/ChargUp\"\r\n"));
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

	*ChargUpResult = __params.ChargUpResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::SMSTest(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR* SMSTestResult
	)
{
    if ( SMSTestResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_SMSTest_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;

	__atlsoap_hr = SetClientStruct(&__params, 26);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SMSTest\"\r\n"));
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

	*SMSTestResult = __params.SMSTestResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::RECSMSEx(
		BSTR sn, 
		BSTR pwd, 
		BSTR subcode, 
		MOBody** RECSMSExResult, int* __RECSMSExResult_nSizeIs
	)
{
    if ( RECSMSExResult == NULL )
		return E_POINTER;
	if( __RECSMSExResult_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_RECSMSEx_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.subcode = subcode;

	__atlsoap_hr = SetClientStruct(&__params, 27);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/RECSMSEx\"\r\n"));
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

	*RECSMSExResult = __params.RECSMSExResult;
	*__RECSMSExResult_nSizeIs = __params.__RECSMSExResult_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::gxmt(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* gxmtResult
	)
{
    if ( gxmtResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_gxmt_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.ext = ext;
	__params.stime = stime;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 28);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/gxmt\"\r\n"));
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

	*gxmtResult = __params.gxmtResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::RECSMS_UTF8(
		BSTR sn, 
		BSTR pwd, 
		MOBody** RECSMS_UTF8Result, int* __RECSMS_UTF8Result_nSizeIs
	)
{
    if ( RECSMS_UTF8Result == NULL )
		return E_POINTER;
	if( __RECSMS_UTF8Result_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_RECSMS_UTF8_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 29);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/RECSMS_UTF8\"\r\n"));
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

	*RECSMS_UTF8Result = __params.RECSMS_UTF8Result;
	*__RECSMS_UTF8Result_nSizeIs = __params.__RECSMS_UTF8Result_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::RECSMSEx_UTF8(
		BSTR sn, 
		BSTR pwd, 
		BSTR subcode, 
		MOBody** RECSMSEx_UTF8Result, int* __RECSMSEx_UTF8Result_nSizeIs
	)
{
    if ( RECSMSEx_UTF8Result == NULL )
		return E_POINTER;
	if( __RECSMSEx_UTF8Result_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_RECSMSEx_UTF8_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.subcode = subcode;

	__atlsoap_hr = SetClientStruct(&__params, 30);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/RECSMSEx_UTF8\"\r\n"));
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

	*RECSMSEx_UTF8Result = __params.RECSMSEx_UTF8Result;
	*__RECSMSEx_UTF8Result_nSizeIs = __params.__RECSMSEx_UTF8Result_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::report2DES(
		BSTR sn, 
		BSTR pwd, 
		__int64 maxid, 
		BSTR rrid, 
		BSTR* report2DESResult
	)
{
    if ( report2DESResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_report2DES_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.maxid = maxid;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 31);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/report2DES\"\r\n"));
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

	*report2DESResult = __params.report2DESResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::GetAllInfo(
		BSTR sn, 
		BSTR pwd, 
		RegistryInfo* GetAllInfoResult
	)
{
    if ( GetAllInfoResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_GetAllInfo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 32);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetAllInfo\"\r\n"));
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

	*GetAllInfoResult = __params.GetAllInfoResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mdSmsSend_u(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR ext, 
		BSTR stime, 
		BSTR rrid, 
		BSTR* mdSmsSend_uResult
	)
{
    if ( mdSmsSend_uResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mdSmsSend_u_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.ext = ext;
	__params.stime = stime;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 33);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mdSmsSend_u\"\r\n"));
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

	*mdSmsSend_uResult = __params.mdSmsSend_uResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mmsFileMT(
		BSTR sn, 
		BSTR pwd, 
		BSTR rrid, 
		BSTR content, 
		BSTR* mmsFileMTResult
	)
{
    if ( mmsFileMTResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mmsFileMT_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.rrid = rrid;
	__params.content = content;

	__atlsoap_hr = SetClientStruct(&__params, 34);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mmsFileMT\"\r\n"));
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

	*mmsFileMTResult = __params.mmsFileMTResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mo2(
		BSTR sn, 
		BSTR pwd, 
		int maxID, 
		BSTR* mo2Result
	)
{
    if ( mo2Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mo2_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.maxID = maxID;

	__atlsoap_hr = SetClientStruct(&__params, 35);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mo2\"\r\n"));
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

	*mo2Result = __params.mo2Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mdMmsSendF(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR stime, 
		BSTR* mdMmsSendFResult
	)
{
    if ( mdMmsSendFResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mdMmsSendF_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.stime = stime;

	__atlsoap_hr = SetClientStruct(&__params, 36);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mdMmsSendF\"\r\n"));
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

	*mdMmsSendFResult = __params.mdMmsSendFResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::msgid(
		BSTR sn, 
		BSTR pwd, 
		BSTR* msgidResult
	)
{
    if ( msgidResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_msgid_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 37);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/msgid\"\r\n"));
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

	*msgidResult = __params.msgidResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::GetCode(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetCodeResult
	)
{
    if ( GetCodeResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_GetCode_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 38);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetCode\"\r\n"));
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

	*GetCodeResult = __params.GetCodeResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::SendSMS_R(
		BSTR sn, 
		BSTR pwd, 
		BSTR mobile, 
		BSTR content, 
		BSTR rrid, 
		BSTR* SendSMS_RResult
	)
{
    if ( SendSMS_RResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_SendSMS_R_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.mobile = mobile;
	__params.content = content;
	__params.rrid = rrid;

	__atlsoap_hr = SetClientStruct(&__params, 39);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/SendSMS_R\"\r\n"));
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

	*SendSMS_RResult = __params.SendSMS_RResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::UDPPwd(
		BSTR sn, 
		BSTR pwd, 
		BSTR newpwd, 
		BSTR* UDPPwdResult
	)
{
    if ( UDPPwdResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_UDPPwd_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.newpwd = newpwd;

	__atlsoap_hr = SetClientStruct(&__params, 40);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/UDPPwd\"\r\n"));
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

	*UDPPwdResult = __params.UDPPwdResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::GetStatus(
		BSTR sn, 
		BSTR pwd, 
		BSTR* GetStatusResult
	)
{
    if ( GetStatusResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_GetStatus_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 41);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/GetStatus\"\r\n"));
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

	*GetStatusResult = __params.GetStatusResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mo(
		BSTR sn, 
		BSTR pwd, 
		BSTR* moResult
	)
{
    if ( moResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;

	__atlsoap_hr = SetClientStruct(&__params, 42);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mo\"\r\n"));
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

	*moResult = __params.moResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWebServiceT<TClient>::mdFaxSend(
		BSTR sn, 
		BSTR pwd, 
		BSTR title, 
		BSTR mobile, 
		BSTR content, 
		BSTR srcnumber, 
		BSTR stime, 
		BSTR* mdFaxSendResult
	)
{
    if ( mdFaxSendResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWebService_mdFaxSend_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sn = sn;
	__params.pwd = pwd;
	__params.title = title;
	__params.mobile = mobile;
	__params.content = content;
	__params.srcnumber = srcnumber;
	__params.stime = stime;

	__atlsoap_hr = SetClientStruct(&__params, 43);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/mdFaxSend\"\r\n"));
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

	*mdFaxSendResult = __params.mdFaxSendResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWebServiceT<TClient>::GetFunctionMap()
{
	return __CWebService_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWebServiceT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CWebService_GetGaoDuan_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_GetGaoDuan_atlsoapheader_map = 
	{
		0x5041438E,
		"GetGaoDuan",
		L"GetGaoDuanResponse",
		sizeof("GetGaoDuan")-1,
		sizeof("GetGaoDuanResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_GetGaoDuan_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_GetBalance_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_GetBalance_atlsoapheader_map = 
	{
		0x077CCAB5,
		"GetBalance",
		L"GetBalanceResponse",
		sizeof("GetBalance")-1,
		sizeof("GetBalanceResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_GetBalance_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mdSmsSend_DES_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mdSmsSend_DES_atlsoapheader_map = 
	{
		0x596F7AB8,
		"mdSmsSend_DES",
		L"mdSmsSend_DESResponse",
		sizeof("mdSmsSend_DES")-1,
		sizeof("mdSmsSend_DESResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mdSmsSend_DES_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_balance_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_balance_atlsoapheader_map = 
	{
		0xF03BB755,
		"balance",
		L"balanceResponse",
		sizeof("balance")-1,
		sizeof("balanceResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_balance_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mt_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mt_atlsoapheader_map = 
	{
		0xA11DE1B0,
		"mt",
		L"mtResponse",
		sizeof("mt")-1,
		sizeof("mtResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mt_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_UDPSIGNEX_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_UDPSIGNEX_atlsoapheader_map = 
	{
		0x564DC386,
		"UDPSIGNEX",
		L"UDPSIGNEXResponse",
		sizeof("UDPSIGNEX")-1,
		sizeof("UDPSIGNEXResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_UDPSIGNEX_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_SendSMS_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_SendSMS_atlsoapheader_map = 
	{
		0xE335D1EC,
		"SendSMS",
		L"SendSMSResponse",
		sizeof("SendSMS")-1,
		sizeof("SendSMSResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_SendSMS_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mdMmsSend_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mdMmsSend_atlsoapheader_map = 
	{
		0x23EA03B7,
		"mdMmsSend",
		L"mdMmsSendResponse",
		sizeof("mdMmsSend")-1,
		sizeof("mdMmsSendResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mdMmsSend_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_SetGaoDuan_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_SetGaoDuan_atlsoapheader_map = 
	{
		0x794ADD1A,
		"SetGaoDuan",
		L"SetGaoDuanResponse",
		sizeof("SetGaoDuan")-1,
		sizeof("SetGaoDuanResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_SetGaoDuan_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mdAudioSend_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mdAudioSend_atlsoapheader_map = 
	{
		0x848F5DDC,
		"mdAudioSend",
		L"mdAudioSendResponse",
		sizeof("mdAudioSend")-1,
		sizeof("mdAudioSendResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mdAudioSend_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_RECSMS_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_RECSMS_atlsoapheader_map = 
	{
		0x87B1663C,
		"RECSMS",
		L"RECSMSResponse",
		sizeof("RECSMS")-1,
		sizeof("RECSMSResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_RECSMS_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_UDPSIGN_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_UDPSIGN_atlsoapheader_map = 
	{
		0x64E43EC9,
		"UDPSIGN",
		L"UDPSIGNResponse",
		sizeof("UDPSIGN")-1,
		sizeof("UDPSIGNResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_UDPSIGN_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_report_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_report_atlsoapheader_map = 
	{
		0xA616928B,
		"report",
		L"reportResponse",
		sizeof("report")-1,
		sizeof("reportResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_report_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_Register_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_Register_atlsoapheader_map = 
	{
		0xBC2C0F94,
		"Register",
		L"RegisterResponse",
		sizeof("Register")-1,
		sizeof("RegisterResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_Register_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_SendSMSEx_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_SendSMSEx_atlsoapheader_map = 
	{
		0x3EDECB89,
		"SendSMSEx",
		L"SendSMSExResponse",
		sizeof("SendSMSEx")-1,
		sizeof("SendSMSExResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_SendSMSEx_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_SendSMSEx_R_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_SendSMSEx_R_atlsoapheader_map = 
	{
		0x4746979A,
		"SendSMSEx_R",
		L"SendSMSEx_RResponse",
		sizeof("SendSMSEx_R")-1,
		sizeof("SendSMSEx_RResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_SendSMSEx_R_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mdMmsReceive_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mdMmsReceive_atlsoapheader_map = 
	{
		0xF5815010,
		"mdMmsReceive",
		L"mdMmsReceiveResponse",
		sizeof("mdMmsReceive")-1,
		sizeof("mdMmsReceiveResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mdMmsReceive_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_TestCode_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_TestCode_atlsoapheader_map = 
	{
		0x4DE11CCA,
		"TestCode",
		L"TestCodeResponse",
		sizeof("TestCode")-1,
		sizeof("TestCodeResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_TestCode_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mdSmsSend_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mdSmsSend_atlsoapheader_map = 
	{
		0xF796963D,
		"mdSmsSend",
		L"mdSmsSendResponse",
		sizeof("mdSmsSend")-1,
		sizeof("mdSmsSendResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mdSmsSend_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mdSmsSend_AES_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mdSmsSend_AES_atlsoapheader_map = 
	{
		0xC9595AF5,
		"mdSmsSend_AES",
		L"mdSmsSend_AESResponse",
		sizeof("mdSmsSend_AES")-1,
		sizeof("mdSmsSend_AESResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mdSmsSend_AES_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_GetAllInfo2_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_GetAllInfo2_atlsoapheader_map = 
	{
		0xEF7B6606,
		"GetAllInfo2",
		L"GetAllInfo2Response",
		sizeof("GetAllInfo2")-1,
		sizeof("GetAllInfo2Response")-1,
		SOAPMAP_HEADER,
		__CWebService_GetAllInfo2_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_report2_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_report2_atlsoapheader_map = 
	{
		0x21DAB03D,
		"report2",
		L"report2Response",
		sizeof("report2")-1,
		sizeof("report2Response")-1,
		SOAPMAP_HEADER,
		__CWebService_report2_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_UnRegister_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_UnRegister_atlsoapheader_map = 
	{
		0xE32480F7,
		"UnRegister",
		L"UnRegisterResponse",
		sizeof("UnRegister")-1,
		sizeof("UnRegisterResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_UnRegister_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_fileMT_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_fileMT_atlsoapheader_map = 
	{
		0xFE0E6110,
		"fileMT",
		L"fileMTResponse",
		sizeof("fileMT")-1,
		sizeof("fileMTResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_fileMT_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_GetFlag_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_GetFlag_atlsoapheader_map = 
	{
		0xCC2BD289,
		"GetFlag",
		L"GetFlagResponse",
		sizeof("GetFlag")-1,
		sizeof("GetFlagResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_GetFlag_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_ChargUp_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_ChargUp_atlsoapheader_map = 
	{
		0xC3A6C1B9,
		"ChargUp",
		L"ChargUpResponse",
		sizeof("ChargUp")-1,
		sizeof("ChargUpResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_ChargUp_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_SMSTest_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_SMSTest_atlsoapheader_map = 
	{
		0xE5156EC2,
		"SMSTest",
		L"SMSTestResponse",
		sizeof("SMSTest")-1,
		sizeof("SMSTestResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_SMSTest_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_RECSMSEx_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_RECSMSEx_atlsoapheader_map = 
	{
		0xF090B3D9,
		"RECSMSEx",
		L"RECSMSExResponse",
		sizeof("RECSMSEx")-1,
		sizeof("RECSMSExResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_RECSMSEx_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_gxmt_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_gxmt_atlsoapheader_map = 
	{
		0xD67D6A2F,
		"gxmt",
		L"gxmtResponse",
		sizeof("gxmt")-1,
		sizeof("gxmtResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_gxmt_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_RECSMS_UTF8_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_RECSMS_UTF8_atlsoapheader_map = 
	{
		0xB6515F02,
		"RECSMS_UTF8",
		L"RECSMS_UTF8Response",
		sizeof("RECSMS_UTF8")-1,
		sizeof("RECSMS_UTF8Response")-1,
		SOAPMAP_HEADER,
		__CWebService_RECSMS_UTF8_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_RECSMSEx_UTF8_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_RECSMSEx_UTF8_atlsoapheader_map = 
	{
		0x8E02B6BF,
		"RECSMSEx_UTF8",
		L"RECSMSEx_UTF8Response",
		sizeof("RECSMSEx_UTF8")-1,
		sizeof("RECSMSEx_UTF8Response")-1,
		SOAPMAP_HEADER,
		__CWebService_RECSMSEx_UTF8_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_report2DES_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_report2DES_atlsoapheader_map = 
	{
		0x6A4983F9,
		"report2DES",
		L"report2DESResponse",
		sizeof("report2DES")-1,
		sizeof("report2DESResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_report2DES_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_GetAllInfo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_GetAllInfo_atlsoapheader_map = 
	{
		0xF2233334,
		"GetAllInfo",
		L"GetAllInfoResponse",
		sizeof("GetAllInfo")-1,
		sizeof("GetAllInfoResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_GetAllInfo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mdSmsSend_u_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mdSmsSend_u_atlsoapheader_map = 
	{
		0xFA1F5271,
		"mdSmsSend_u",
		L"mdSmsSend_uResponse",
		sizeof("mdSmsSend_u")-1,
		sizeof("mdSmsSend_uResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mdSmsSend_u_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mmsFileMT_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mmsFileMT_atlsoapheader_map = 
	{
		0xA883D5DD,
		"mmsFileMT",
		L"mmsFileMTResponse",
		sizeof("mmsFileMT")-1,
		sizeof("mmsFileMTResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mmsFileMT_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mo2_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mo2_atlsoapheader_map = 
	{
		0x69970E5D,
		"mo2",
		L"mo2Response",
		sizeof("mo2")-1,
		sizeof("mo2Response")-1,
		SOAPMAP_HEADER,
		__CWebService_mo2_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mdMmsSendF_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mdMmsSendF_atlsoapheader_map = 
	{
		0x73D51AFD,
		"mdMmsSendF",
		L"mdMmsSendFResponse",
		sizeof("mdMmsSendF")-1,
		sizeof("mdMmsSendFResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mdMmsSendF_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_msgid_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_msgid_atlsoapheader_map = 
	{
		0xBA4378C3,
		"msgid",
		L"msgidResponse",
		sizeof("msgid")-1,
		sizeof("msgidResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_msgid_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_GetCode_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_GetCode_atlsoapheader_map = 
	{
		0xEC8EAB8A,
		"GetCode",
		L"GetCodeResponse",
		sizeof("GetCode")-1,
		sizeof("GetCodeResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_GetCode_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_SendSMS_R_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_SendSMS_R_atlsoapheader_map = 
	{
		0x5D78C2BD,
		"SendSMS_R",
		L"SendSMS_RResponse",
		sizeof("SendSMS_R")-1,
		sizeof("SendSMS_RResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_SendSMS_R_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_UDPPwd_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_UDPPwd_atlsoapheader_map = 
	{
		0x3C57F843,
		"UDPPwd",
		L"UDPPwdResponse",
		sizeof("UDPPwd")-1,
		sizeof("UDPPwdResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_UDPPwd_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_GetStatus_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_GetStatus_atlsoapheader_map = 
	{
		0xFBD4D613,
		"GetStatus",
		L"GetStatusResponse",
		sizeof("GetStatus")-1,
		sizeof("GetStatusResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_GetStatus_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mo_atlsoapheader_map = 
	{
		0x5AAFACAB,
		"mo",
		L"moResponse",
		sizeof("mo")-1,
		sizeof("moResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CWebService_mdFaxSend_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWebService_mdFaxSend_atlsoapheader_map = 
	{
		0x72CE7F69,
		"mdFaxSend",
		L"mdFaxSendResponse",
		sizeof("mdFaxSend")-1,
		sizeof("mdFaxSendResponse")-1,
		SOAPMAP_HEADER,
		__CWebService_mdFaxSend_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};


	static const _soapmap * __CWebService_headers[] =
	{
		&__CWebService_GetGaoDuan_atlsoapheader_map,
		&__CWebService_GetBalance_atlsoapheader_map,
		&__CWebService_mdSmsSend_DES_atlsoapheader_map,
		&__CWebService_balance_atlsoapheader_map,
		&__CWebService_mt_atlsoapheader_map,
		&__CWebService_UDPSIGNEX_atlsoapheader_map,
		&__CWebService_SendSMS_atlsoapheader_map,
		&__CWebService_mdMmsSend_atlsoapheader_map,
		&__CWebService_SetGaoDuan_atlsoapheader_map,
		&__CWebService_mdAudioSend_atlsoapheader_map,
		&__CWebService_RECSMS_atlsoapheader_map,
		&__CWebService_UDPSIGN_atlsoapheader_map,
		&__CWebService_report_atlsoapheader_map,
		&__CWebService_Register_atlsoapheader_map,
		&__CWebService_SendSMSEx_atlsoapheader_map,
		&__CWebService_SendSMSEx_R_atlsoapheader_map,
		&__CWebService_mdMmsReceive_atlsoapheader_map,
		&__CWebService_TestCode_atlsoapheader_map,
		&__CWebService_mdSmsSend_atlsoapheader_map,
		&__CWebService_mdSmsSend_AES_atlsoapheader_map,
		&__CWebService_GetAllInfo2_atlsoapheader_map,
		&__CWebService_report2_atlsoapheader_map,
		&__CWebService_UnRegister_atlsoapheader_map,
		&__CWebService_fileMT_atlsoapheader_map,
		&__CWebService_GetFlag_atlsoapheader_map,
		&__CWebService_ChargUp_atlsoapheader_map,
		&__CWebService_SMSTest_atlsoapheader_map,
		&__CWebService_RECSMSEx_atlsoapheader_map,
		&__CWebService_gxmt_atlsoapheader_map,
		&__CWebService_RECSMS_UTF8_atlsoapheader_map,
		&__CWebService_RECSMSEx_UTF8_atlsoapheader_map,
		&__CWebService_report2DES_atlsoapheader_map,
		&__CWebService_GetAllInfo_atlsoapheader_map,
		&__CWebService_mdSmsSend_u_atlsoapheader_map,
		&__CWebService_mmsFileMT_atlsoapheader_map,
		&__CWebService_mo2_atlsoapheader_map,
		&__CWebService_mdMmsSendF_atlsoapheader_map,
		&__CWebService_msgid_atlsoapheader_map,
		&__CWebService_GetCode_atlsoapheader_map,
		&__CWebService_SendSMS_R_atlsoapheader_map,
		&__CWebService_UDPPwd_atlsoapheader_map,
		&__CWebService_GetStatus_atlsoapheader_map,
		&__CWebService_mo_atlsoapheader_map,
		&__CWebService_mdFaxSend_atlsoapheader_map,
		NULL
	};
	
	return __CWebService_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CWebServiceT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CWebServiceT<TClient>::GetNamespaceUri()
{
	return L"http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline const char * CWebServiceT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CWebServiceT<TClient>::GetNamespaceUriA()
{
	return "http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWebServiceT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWebServiceT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace WebService

template<>
inline HRESULT AtlCleanupValue<WebService::RegistryInfo>(WebService::RegistryInfo *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->RESULT);
	AtlCleanupValue(&pVal->REGISTRYCODE);
	AtlCleanupValue(&pVal->SERVICECODE);
	AtlCleanupValue(&pVal->BALANCE);
	AtlCleanupValue(&pVal->CREATEDATE);
	AtlCleanupValue(&pVal->REGISTRYSTATEID);
	AtlCleanupValue(&pVal->PRIORITY);
	AtlCleanupValue(&pVal->FIRSTREGISTRYDATE);
	AtlCleanupValue(&pVal->REGISTRYDATE);
	AtlCleanupValue(&pVal->AGENTID);
	AtlCleanupValue(&pVal->PRODUCTNAME);
	AtlCleanupValue(&pVal->DESTMOBILE);
	AtlCleanupValue(&pVal->FLAG);
	AtlCleanupValue(&pVal->REPLYCONTENT);
	AtlCleanupValue(&pVal->ISRETURN);
	AtlCleanupValue(&pVal->VERSION);
	AtlCleanupValue(&pVal->GRADE);
	AtlCleanupValue(&pVal->PARENT);
	AtlCleanupValue(&pVal->SUBSIDIARY);
	AtlCleanupValue(&pVal->ROLES);
	AtlCleanupValue(&pVal->BALSTATUS);
	AtlCleanupValue(&pVal->DISCOUNT);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WebService::RegistryInfo>(WebService::RegistryInfo *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->RESULT, pMemMgr);
	AtlCleanupValueEx(&pVal->REGISTRYCODE, pMemMgr);
	AtlCleanupValueEx(&pVal->SERVICECODE, pMemMgr);
	AtlCleanupValueEx(&pVal->BALANCE, pMemMgr);
	AtlCleanupValueEx(&pVal->CREATEDATE, pMemMgr);
	AtlCleanupValueEx(&pVal->REGISTRYSTATEID, pMemMgr);
	AtlCleanupValueEx(&pVal->PRIORITY, pMemMgr);
	AtlCleanupValueEx(&pVal->FIRSTREGISTRYDATE, pMemMgr);
	AtlCleanupValueEx(&pVal->REGISTRYDATE, pMemMgr);
	AtlCleanupValueEx(&pVal->AGENTID, pMemMgr);
	AtlCleanupValueEx(&pVal->PRODUCTNAME, pMemMgr);
	AtlCleanupValueEx(&pVal->DESTMOBILE, pMemMgr);
	AtlCleanupValueEx(&pVal->FLAG, pMemMgr);
	AtlCleanupValueEx(&pVal->REPLYCONTENT, pMemMgr);
	AtlCleanupValueEx(&pVal->ISRETURN, pMemMgr);
	AtlCleanupValueEx(&pVal->VERSION, pMemMgr);
	AtlCleanupValueEx(&pVal->GRADE, pMemMgr);
	AtlCleanupValueEx(&pVal->PARENT, pMemMgr);
	AtlCleanupValueEx(&pVal->SUBSIDIARY, pMemMgr);
	AtlCleanupValueEx(&pVal->ROLES, pMemMgr);
	AtlCleanupValueEx(&pVal->BALSTATUS, pMemMgr);
	AtlCleanupValueEx(&pVal->DISCOUNT, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WebService::MOBody>(WebService::MOBody *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->total_num);
	AtlCleanupValue(&pVal->this_num);
	AtlCleanupValue(&pVal->recvtel);
	AtlCleanupValue(&pVal->sender);
	AtlCleanupValue(&pVal->content);
	AtlCleanupValue(&pVal->recdate);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WebService::MOBody>(WebService::MOBody *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->total_num, pMemMgr);
	AtlCleanupValueEx(&pVal->this_num, pMemMgr);
	AtlCleanupValueEx(&pVal->recvtel, pMemMgr);
	AtlCleanupValueEx(&pVal->sender, pMemMgr);
	AtlCleanupValueEx(&pVal->content, pMemMgr);
	AtlCleanupValueEx(&pVal->recdate, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WebService::RegistryInfo2>(WebService::RegistryInfo2 *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->RESULT);
	AtlCleanupValue(&pVal->REGISTRYCODE);
	AtlCleanupValue(&pVal->SERVICECODE);
	AtlCleanupValue(&pVal->BALANCE);
	AtlCleanupValue(&pVal->CREATEDATE);
	AtlCleanupValue(&pVal->REGISTRYSTATEID);
	AtlCleanupValue(&pVal->PRIORITY);
	AtlCleanupValue(&pVal->FIRSTREGISTRYDATE);
	AtlCleanupValue(&pVal->REGISTRYDATE);
	AtlCleanupValue(&pVal->AGENTID);
	AtlCleanupValue(&pVal->PRODUCTNAME);
	AtlCleanupValue(&pVal->DESTMOBILE);
	AtlCleanupValue(&pVal->FLAG);
	AtlCleanupValue(&pVal->REPLYCONTENT);
	AtlCleanupValue(&pVal->ISRETURN);
	AtlCleanupValue(&pVal->VERSION);
	AtlCleanupValue(&pVal->GRADE);
	AtlCleanupValue(&pVal->PARENT);
	AtlCleanupValue(&pVal->SUBSIDIARY);
	AtlCleanupValue(&pVal->ROLES);
	AtlCleanupValue(&pVal->BALSTATUS);
	AtlCleanupValue(&pVal->DISCOUNT);
	AtlCleanupValue(&pVal->SLIST);
	AtlCleanupValue(&pVal->POPM);
	AtlCleanupValue(&pVal->UPDATE);
	AtlCleanupValue(&pVal->MMS);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WebService::RegistryInfo2>(WebService::RegistryInfo2 *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->RESULT, pMemMgr);
	AtlCleanupValueEx(&pVal->REGISTRYCODE, pMemMgr);
	AtlCleanupValueEx(&pVal->SERVICECODE, pMemMgr);
	AtlCleanupValueEx(&pVal->BALANCE, pMemMgr);
	AtlCleanupValueEx(&pVal->CREATEDATE, pMemMgr);
	AtlCleanupValueEx(&pVal->REGISTRYSTATEID, pMemMgr);
	AtlCleanupValueEx(&pVal->PRIORITY, pMemMgr);
	AtlCleanupValueEx(&pVal->FIRSTREGISTRYDATE, pMemMgr);
	AtlCleanupValueEx(&pVal->REGISTRYDATE, pMemMgr);
	AtlCleanupValueEx(&pVal->AGENTID, pMemMgr);
	AtlCleanupValueEx(&pVal->PRODUCTNAME, pMemMgr);
	AtlCleanupValueEx(&pVal->DESTMOBILE, pMemMgr);
	AtlCleanupValueEx(&pVal->FLAG, pMemMgr);
	AtlCleanupValueEx(&pVal->REPLYCONTENT, pMemMgr);
	AtlCleanupValueEx(&pVal->ISRETURN, pMemMgr);
	AtlCleanupValueEx(&pVal->VERSION, pMemMgr);
	AtlCleanupValueEx(&pVal->GRADE, pMemMgr);
	AtlCleanupValueEx(&pVal->PARENT, pMemMgr);
	AtlCleanupValueEx(&pVal->SUBSIDIARY, pMemMgr);
	AtlCleanupValueEx(&pVal->ROLES, pMemMgr);
	AtlCleanupValueEx(&pVal->BALSTATUS, pMemMgr);
	AtlCleanupValueEx(&pVal->DISCOUNT, pMemMgr);
	AtlCleanupValueEx(&pVal->SLIST, pMemMgr);
	AtlCleanupValueEx(&pVal->POPM, pMemMgr);
	AtlCleanupValueEx(&pVal->UPDATE, pMemMgr);
	AtlCleanupValueEx(&pVal->MMS, pMemMgr);
	return S_OK;
}
