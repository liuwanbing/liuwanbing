#include "Stdafx.h"
#include <AfxDllX.h>
#include "AFCResource.h"

//名字空间
//using namespace AFC;
//using namespace AFC::AFCResource;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

static AFX_EXTENSION_MODULE AFCDLL = { NULL, NULL };
// 此为静态成员，须由使用者在进程结束时显式调用CBcfFile::ClearMap()函数来释放，否则MFC会报内存泄漏
EXT_CLASS std::map<CString, CBcfFileNameItem*> CBcfFile::mapFile;
EXT_CLASS std::map<CString,FileItem*> CUnZipRes::mapFile;
unsigned char CUnZipRes::m_bPassWord[MAX_PASSWORD_LENGTH];
int g_nGameID = -1;

int GetAvatarIndexByMoney(__int64 i64Money,UINT uGameNameID);
int GetAvatarIndexByPoint(__int64 dwPoint,UINT uGameNameID);

CBcfFile g_cfgModual(CBcfFile::GetAppPath() + "Module.bcf");	//模块配置文件
CBcfFile g_cfgOrder(CBcfFile::GetAppPath() + "Order.bcf");		//Order配置文件

int GetModuleStatus(char* module,char* compoment,int defval/* = 0*/);


//DLL 入口函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(AFCDLL, hInstance)) return 0;
		new CDynLinkLibrary(AFCDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(AFCDLL);
	}
	return 1;
}

///默认等级函数
/// @param dwPoint[in] 输入一个数值，可以是很大的数
/// @return 字符串指针，如果没有读取取BCF文件，则返回空字符串

extern "C" __declspec(dllexport) void SetGameID(int iGameID)
{
	g_nGameID = iGameID;
}

extern "C" __declspec(dllexport) TCHAR * GetMoneyOrderName(__int64 dwPoint,UINT uGameNameID)
{
	CString strSection;
	CString strKey;
	strSection.Format("%d",uGameNameID);

	static TCHAR orderName[18] = "";

	//游戏没有定义级别，就使用公共配置
	if (g_cfgOrder.GetKeyVal(strSection,"Coint_Lv01",-1) == -1)
	{
		strSection = "Public";
	}

	int idx = 1;
	CString strMoney = "";

	for (int i = 1; i <= 50; ++i)
	{
		strKey.Format("Coint_Lv%02d",i);

		strMoney = g_cfgOrder.GetKeyVal(strSection,strKey,"-1");

		if (strMoney == "-1")
		{
			idx =  i - 1;
			break;
		}
		else
		{
			if (dwPoint < _atoi64(strMoney.GetBuffer()))
			{
				idx = i - 1;
				break;
			}
		}
	}

	strKey.Format("Coint_Lv%02d",idx);

	::strcpy(orderName,g_cfgOrder.GetKeyMemo(strSection,strKey,""));

	return orderName;
}

extern "C" __declspec(dllexport) TCHAR * GetPointOrderName(__int64 dwPoint,UINT uGameNameID)
{

	CString strSection;
	CString strKey;
	strSection.Format("%d",uGameNameID);

	static TCHAR orderName[18] = "";

	//游戏没有定义级别，就使用公共配置
	if (g_cfgOrder.GetKeyVal(strSection,"Point_Lv01",-1) == -1)
	{
		strSection = "Public";
	}

	int idx = 1;
	CString strMoney = "";

	for (int i = 1; i <= 50; ++i)
	{
		strKey.Format("Point_Lv%02d",i);

		strMoney = g_cfgOrder.GetKeyVal(strSection,strKey,"-1");

		if (strMoney == "-1")
		{
			idx =  i - 1;
			break;
		}
		else
		{
			if (dwPoint < _atoi64(strMoney.GetBuffer()))
			{
				idx = i - 1;
				break;
			}
		}
	}

	strKey.Format("Point_Lv%02d",idx);

	::strcpy(orderName,g_cfgOrder.GetKeyMemo(strSection,strKey,""));

	return orderName;
}

//默认银子等级函数
extern "C" __declspec(dllexport) int GetMoneyGrade(__int64 dwPoint)
{
	CString s = CBcfFile::GetAppPath();
	CBcfFile f("Order.bcf");
	CString m_key;
	m_key.Format("PlatMoneyOrderName%d", g_nGameID);
    static int szOrderNum[50];
	CString strKey,strTmp;
	int tmpNum = 0;
	int nCount = 0;
	for (int i = 1;i <=50;i++)
	{
		strKey.Format("platOrder_%d",i);
		tmpNum=f.GetKeyVal(m_key,strKey,0);
		szOrderNum[i-1] = tmpNum;
		strTmp = f.GetKeyMemo(m_key,strKey,"");
		if (strTmp == "")
		{
			nCount = i-1;
			break;
		}
	}
	for (int i=0;i<nCount;i++)	
		if (dwPoint<szOrderNum[i]) 
			return i;
	return nCount;
	//static int uBasePoint[6]=
	//{
	//	10000,		100000,		1000000,	10000000,
	//	50000000,	100000000
	//};
	//for (int i=0;i<6;i++)	
	//	if (dwPoint<uBasePoint[i]) 
	//		return i;
	//return 5;
}

//默认比赛等级函数
extern "C" __declspec(dllexport) TCHAR * GetMatchOrderName(__int64 dwPoint,UINT uGameNameID)
{
	static int uBasePoint[19]=
	{
		-500L,	-200L,	-100L,	50L,
		150,	300,	500,	700,
		900,	1200,	1500,	1800,
		2100,	2400,	2700,	3000,
		3500,	4000,	5000,
	};

	static TCHAR * szOrderName[19]=
	{
		TEXT("九级挖手"),TEXT("八级挖手"),TEXT("七级挖手"),TEXT("六级挖手"),
		TEXT("五级挖手"),TEXT("四级挖手"),TEXT("三级挖手"),TEXT("二级挖手"),
		TEXT("一级挖手"),TEXT("九级坑王"),TEXT("八级坑王"),TEXT("七级坑王"),
		TEXT("六级坑王"),TEXT("五级坑王"),TEXT("四级坑王"),TEXT("三级坑王"),
		TEXT("二级坑王"),TEXT("一级坑王"),TEXT("坑神")
	};

	for (int i=0;i<19;i++)	
		if (dwPoint<uBasePoint[i]) 
			return szOrderName[i];

	return TEXT("坑神");
}

//默认普通等级函数
extern "C" __declspec(dllexport) TCHAR * GetNormalOrderName(__int64 dwPoint,UINT uGameNameID)
{
	static int uBasePoint[19]=
	{
		400,		800,		1600,		3200,
		6400,		12800,		25600,		51200,
		102400,		204800,		409600,		819200,
		1638400,	3276800,	6553600,	13107200,
		26214400,	52428800,	104857600
	};
	static TCHAR * szOrderName[19]=
	{
		TEXT("民兵"),TEXT("列兵"),TEXT("下等兵"),	TEXT("上等兵"),
		TEXT("下士"),TEXT("中士"),TEXT("上士"),		TEXT("少尉"),
		TEXT("中尉"),TEXT("上尉"),TEXT("大尉"),		TEXT("少校"),
		TEXT("中校"),TEXT("上校"),TEXT("大校"),		TEXT("少将"),
		TEXT("中将"),TEXT("上将"),TEXT("大将")
	};
	for (int i=0;i<19;i++)	
	{
		if (dwPoint<uBasePoint[i]) 
			return szOrderName[i];
	}
	return TEXT("元帅");
}

//默认普通等级基础分
extern "C" __declspec(dllexport) int GetNormalOrderBasePoint(__int64 dwPoint)
{
	return 0;
	static int uBasePoint[19]=
	{
		400,		800,		1600,		3200,
		6400,		12800,		25600,		51200,
		102400,		204800,		409600,		819200,
		1638400,	3276800,	6553600,	13107200,
		26214400,	52428800,	104857600
	};
	for (int i = 0; i < 19; i++)	
	{		
		if (dwPoint < uBasePoint[i]) 
		{
			if(i == 0) 
				return 0;

			return uBasePoint[i - 1];
		}
	}
		
	return 104857600;
}
//默认普通等级函数
// 返回0 ~ 20
extern "C" __declspec(dllexport) int GetNormalOrderLevel(__int64 dwPoint)
{
	static int uBasePoint[19]=
	{
		400,			800,		1600,		3200,
		6400,			12800,		25600,		51200,
		102400,			204800,		409600,		819200,
		1638400,		3276800,	6553600,	13107200,
		26214400,		52428800,	104857600
	};
	for (int i=0;i<19;i++)	
	{
		if (dwPoint<uBasePoint[i]) 
			return i+1;
	}
	return 20;
}


//魅力级别
extern "C" __declspec(dllexport) TCHAR * GetCharmGrade(__int64 dwFascination)
{
	static int uBasePoint[19]=
	{
		400,		800,		1600,		3200,
		6400,		12800,		25600,		51200,
		102400,		204800,		409600,		819200,
		1638400,	3276800,	6553600,	13107200,
		26214400,	52428800,	104857600
	};
	static TCHAR * szOrderName[19]=
	{
		TEXT("迷人"),TEXT("迷人"),TEXT("迷人"),		TEXT("迷人"),
		TEXT("迷人"),TEXT("迷人"),TEXT("迷人"),		TEXT("迷人"),
		TEXT("迷人"),TEXT("迷人"),TEXT("迷人"),		TEXT("少校"),
		TEXT("迷人"),TEXT("迷人"),TEXT("迷人"),		TEXT("迷人"),
		TEXT("迷人"),TEXT("迷人"),TEXT("迷人")
	};
	for (int i=0;i<19;i++)	
	{
		if (dwFascination<uBasePoint[i]) 
			return szOrderName[i];
	}
	return TEXT("迷人");
}

/// 获取指定数值的字符串
/// 本函数同时支持UNICODE和ANSI
/// @param[out] str 字符串数组，须由调用者分配空间
/// @param[in] nNum 须显示的整数数值
/// @param[in] nPower 以10的几次幂显示，如123显示成123000，则此参数为3
/// @param[in] bHasSpace 字符串是否分节显示，默认值false表示数字不分节，true表示用空格每三位分一节
/// @return 返回str所在的位置
///
// PengJiLin, 2010-8-3, 增加可以输入分隔符
// PengJiLin, 2011-7-8, 64位金币修改
extern "C" __declspec(dllexport) TCHAR *GetNumString(TCHAR str[], __int64 nNum, int nPower, bool bHasSpace, TCHAR* chSpace)
{
	/// 暂存结果
	TCHAR strTmp[32];
	/// 如果是0，就直接返回一个0 
	if (0==nNum)
	{
		_tcscpy(str, TEXT("0"));
		return str;
	}

    wsprintf( strTmp, TEXT("%I64d"), nNum);

	//Add By YangJie 2011-02-21
	if (nPower >= 0) {
		for (int i=0; i<nPower; ++i)
		{
			_tcscat( strTmp, TEXT("0"));
		}
	} else {
		bool bFlag = false;
		if (nNum < 0) {
			::memset(strTmp, 0, sizeof(strTmp));
			wsprintf(strTmp, TEXT("%I64d"), -nNum);
			bFlag = true;
		}

		int iLen = _tcslen(strTmp);
		int tmpPower = -nPower;
		if (iLen > tmpPower) {
			for (int i = iLen; i > (iLen - tmpPower); i --) {
				strTmp[i] = strTmp[i-1];
			}
			strTmp[iLen-tmpPower] = TEXT('.');
			strTmp[iLen+1] = TEXT('\0');
		} else {
			TCHAR strTmp1[32];
			int i = 0;
			int j = 0;
			int k = 0;
			::memset(strTmp1, 0, sizeof(strTmp1));
			for (i = 0; i < (tmpPower - iLen + 1); i ++) {
				strTmp1[i] = TEXT('0');
			}
			for (j = i, k = 0; k < iLen;j ++, k ++) {
				strTmp1[j] = strTmp[k];
			}
			strTmp1[j++] = TEXT('\0');
			::memset(strTmp, 0, sizeof(strTmp));
			_tcscpy(strTmp, strTmp1);
			for (i = j; i > 1; i --) {
				strTmp[i] = strTmp[i-1];
			}
			strTmp[1] = TEXT('.');
			strTmp[j+1] = TEXT('\0');
		}

		if (bFlag) {
			iLen = _tcslen(strTmp);
			for (int i = iLen; i > 0; i --) {
				strTmp[i] = strTmp[i-1];
			}
			strTmp[0] = TEXT('-');
			strTmp[iLen+1] = TEXT('\0');
		}
	}

	// 如果没有空格则直接返回整个字符串
	if (!bHasSpace)
	{
		_tcscpy(str, strTmp);
		return str;
	}

	if (nPower >= 0) {
		
		// PengJiLin, 2010-9-1, 可以配置多少位分为一节, 最大不超过8位   
		CString s = CBcfFile::GetAppPath();
		CBcfFile f( s + "Order.bcf");
		BYTE byBits = f.GetKeyVal("VirtualMoney", "SpaceBits", 3);

		if(byBits < 1) byBits = 1;
		if(byBits > 8) byBits = 8;

		/// 按位数每三位空一格显示
		int nLen = _tcslen(strTmp);	// 字符串长度
		int nTimes = nLen % byBits/*3*/;		// 三位一节时，多余的位数

		int nSrc = 0;
		int nDes = 0;

		// PengJiLin, 2010-8-3, 增加可以输入分隔符
		TCHAR chUseSpace = 0;
		if(NULL == chSpace)
		{
			chUseSpace = ' ';
		}
		else
		{
			chUseSpace = chSpace[0];
		}

		for (int i=0; i<nLen; ++i)
		{
			/// 负数时，负号与数值间不加空格
			if ((i>0) && (i%byBits==nTimes) && (!(i==1 && nNum<0)))
			{
				str[nDes++] = chUseSpace;
			}
			str[nDes++] = strTmp[nSrc++];
		}
		str[nDes] = TEXT('\0');
	} else {

		_tcscpy(str, strTmp);
	}

	return str;
}

///获取用户的形象图片索引(大厅使用)
/// @param[in] uLogoID 用户的LogoID;
/// @param[in] i64Bank 用户存款;
/// @param[in] i64Money 用户身上金币
/// @return 返回形象图片索引
extern "C" __declspec(dllexport) UINT GetAvatarIndexInMain(UINT uLogoID,__int64 i64Bank,__int64 i64Money)
{
// 	CString str;
// 	str.Format("DClient:GetAvatarIndexInMain(uLogoID = %d,i64Bank = %I64d,i64Money = %I64d)",uLogoID,i64Bank,i64Money);
// 	OutputDebugString(str);

    if (uLogoID > 0)
        return uLogoID + 100;

	/*用户形象更换方式 
		0-用户使用金币购买方式 
		1-用户拥有金币决定 
		2-用户拥有金币或积分决定*/
	int nChangeMethod = GetModuleStatus("Avatar","ChangeMethod",0);

	if (nChangeMethod == 0) //用户使用金币购买方式 
	{
		return uLogoID;
	}
	else if (nChangeMethod == 1) //用户拥有金币决定 
	{
		return GetAvatarIndexByMoney(i64Bank + i64Money,0);
	}
	else if (nChangeMethod == 2) //用户拥有金币或积分决定
	{
		return GetAvatarIndexByMoney(i64Bank + i64Money,0);
	}

	return uLogoID;
}

///获取用户的形象图片索引(游戏房间使用)
/// @param[in] uLogoID     用户的LogoID;
/// @param[in] i64Bank     用户存款;
/// @param[in] i64Money    用户身上金币;
/// @param[in] dwPoint     用户积分;
/// @param[in] uGameNameID 游戏ID;
/// @param[in] uComType    房间类型，1-为积分场，3-为金币场
/// @return 返回形象图片索引
extern "C" __declspec(dllexport) UINT GetAvatarIndexInRoom(UINT uLogoID,__int64 i64Bank,__int64 i64Money,long dwPoint,UINT uGameNameID,UINT uComType)
{
	return uLogoID%12;

    if (uLogoID > 0)
        return uLogoID + 100;

	/*用户形象更换方式 
		0-用户使用金币购买方式 
		1-用户拥有金币决定 
		2-用户拥有金币或积分决定*/
	int nChangeMethod = GetModuleStatus("Avatar","ChangeMethod",0);

	if (nChangeMethod == 0) //用户使用金币购买方式 
	{
		return uLogoID;
	}
	else if (nChangeMethod == 1) //用户拥有金币决定 
	{
		return GetAvatarIndexByMoney(i64Money,uGameNameID);
	}
	else if (nChangeMethod == 2) //用户拥有金币或积分决定
	{
		if (uComType == 1)
		{
			return GetAvatarIndexByPoint(dwPoint,uGameNameID);
		}
		else if (uComType == 3)
		{
			return GetAvatarIndexByMoney(i64Money,uGameNameID);
		}
	}

	return 0;
}


///根据游戏的金币获取形象
/// @param[in] i64Money    用户身上金币;
/// @param[in] uGameNameID 游戏ID;
/// @return 返回形象图片索引
int GetAvatarIndexByMoney(__int64 i64Money,UINT uGameNameID)
{
	CString strSection;
	CString strKey;
	strSection.Format("%d",uGameNameID);

	//游戏没有定义级别，就使用公共配置
	if (g_cfgOrder.GetKeyVal(strSection,"Coint_Lv01",-1) == -1)
	{
		strSection = "Public";
	}

	CString strMoney = "";

	for (int i = 1; i <= 50; ++i)
	{
		strKey.Format("Coint_Lv%02d",i);

		strMoney = g_cfgOrder.GetKeyVal(strSection,strKey,"-1");

		if (strMoney == "-1")
		{
			return i - 2;
		}
		else
		{
			if (i64Money < _atoi64(strMoney.GetBuffer()))
			{
				return i - 2;
			}
		}
	}

	return 0;
}

///根据游戏的积分获取形象
/// @param[in] dwPoint     用户积分;
/// @param[in] uGameNameID 游戏ID;
/// @return 返回形象图片索引
int GetAvatarIndexByPoint(__int64 dwPoint,UINT uGameNameID)
{
	CString strSection;
	CString strKey;
	strSection.Format("%d",uGameNameID);

	//游戏没有定义级别，就使用公共配置
	if (g_cfgOrder.GetKeyVal(strSection,"Point_Lv01",-1) == -1)
	{
		strSection = "Public";
	}

	CString strMoney = "";

	for (int i = 1; i <= 50; ++i)
	{
		strKey.Format("Point_Lv%02d",i);

		strMoney = g_cfgOrder.GetKeyVal(strSection,strKey,"-1");
		
		if (strMoney == "-1")
		{
			return i - 2;
		}
		else
		{
			if (dwPoint < _atoi64(strMoney.GetBuffer()))
			{
				return i - 2;
			}
		}
		
	}

	return 0;
}


int GetModuleStatus(char* module,char* compoment,int defval/* = 0*/)
{
	//本地路径
	DWORD dwCfgTalk = ::cfgOpenFile("Module.bcf",_T("szbzw!!@@##"));

	if ( dwCfgTalk >= 0x10)
	{
		int retVal = ::cfgGetValue(dwCfgTalk,module,compoment,defval); 
		::cfgClose(dwCfgTalk);
		return retVal;

	}

	return 0;
}
