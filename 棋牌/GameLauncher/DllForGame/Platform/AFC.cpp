#include "../stdafx.h"
//#include <AfxDllX.h>
#include "AFCResource.h"

//通过路径获取游戏ID
UINT GetGameNameIDByPath();

int GetAvatarIndexByMoney(__int64 i64Money,UINT uGameNameID);
int GetAvatarIndexByPoint(__int64 dwPoint,UINT uGameNameID);

//是否VIP的有效时间
bool IsVipEffectTime(long lUserVipTime);

CBcfFile g_cfgModual(CBcfFile::GetAppPath() + "../Module.bcf");	//模块配置文件
CBcfFile g_cfgOrder(CBcfFile::GetAppPath() + "../Order.bcf");		//Order配置文件

UINT g_GameNameID = GetGameNameIDByPath();

int GetModuleStatus(char* module,char* compoment,int defval/* = 0*/);

//名字空间
//using namespace AFC;
//using namespace AFC::AFCResource;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

//static AFX_EXTENSION_MODULE AFCDLL = { NULL, NULL };

//DLL 入口函数
//extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
//{
//	UNREFERENCED_PARAMETER(lpReserved);
//
//	if (dwReason == DLL_PROCESS_ATTACH)
//	{
//		if (!AfxInitExtensionModule(AFCDLL, hInstance)) return 0;
//		new CDynLinkLibrary(AFCDLL);
//	}
//	else if (dwReason == DLL_PROCESS_DETACH)
//	{
//		AfxTermExtensionModule(AFCDLL);
//	}
//	return 1;
//}

//通过路径获取游戏ID
UINT GetGameNameIDByPath()
{
	CString str = CBcfFile::GetAppPath().Right(9).Left(8);

	return atol(str.GetBuffer());
}

//默认金币等级函数
TCHAR * GetMoneyOrderName(__int64 dwPoint)
{

	CString strSection;
	CString strKey;
	strSection.Format("%d",g_GameNameID);

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

//默认金币等级函数
TCHAR * GetPointOrderName(__int64 dwPoint)
{

	CString strSection;
	CString strKey;
	strSection.Format("%d",g_GameNameID);

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

///< 读取金币级别
///< @param dwPoint[in] 输入一个数值，可以是很大的数
///< @return int 级别

int GetMoneyOrderLevel(__int64 dwPoint)
{
	CString s = CBcfFile::GetAppPath();
	CBcfFile f("GameOption.bcf");
	CString m_key;
	m_key.Format("PlatMoneyOrderName");
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
}

//默认金币等级函数
int GetMoneyGrade(__int64 dwPoint)
{
	static int uBasePoint[6]=
	{
		10000,		100000,		1000000,	10000000,
		50000000,	100000000
	};
	for (int i=0;i<6;i++)	
		if (dwPoint<uBasePoint[i]) 
			return i;
	return 5;
}

//默认比赛等级函数
TCHAR * GetMatchOrderName(__int64 dwPoint)
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
TCHAR * GetNormalOrderName(__int64 dwPoint)
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
int GetNormalOrderBasePoint(__int64 dwPoint)
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
int GetNormalOrderLevel(__int64 dwPoint)
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
TCHAR * GetCharmGrade(__int64 dwFascination)
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


///获取用户的形象图片索引(游戏房间使用)
/// @param[in] uLogoID     用户的LogoID;
/// @param[in] i64Bank     用户存款;
/// @param[in] i64Money    用户身上金币;
/// @param[in] dwPoint     用户积分;
/// @param[in] uGameNameID 游戏ID;
/// @param[in] uComType    房间类型，1-为积分场，3-为金币场
/// @return 返回形象图片索引
UINT GetAvatarIndex(UINT uLogoID,__int64 i64Bank,__int64 i64Money,long dwPoint,UINT uGameNameID,UINT uComType)
{
	CString str;
	str.Format("DClient:GetAvatarIndexInRoom(uLogoID = %d,i64Bank = %I64d,i64Money = %I64d,dwPoint = %d,uGameNameID = %d,uComType = %d)",\
		uLogoID,i64Bank,i64Money,dwPoint,uGameNameID,uComType);
	OutputDebugString(str);

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


//是否VIP的有效时间
bool IsVipEffectTime(long lUserVipTime)
{
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	return (lUserVipTime > atol(stime));
}

int GetModuleStatus(char* module,char* compoment,int defval/* = 0*/)
{
	//本地路径
	DWORD dwCfgTalk = ::cfgOpenFile("..\\Module.bcf",_T("szbzw!!@@##"));

	if ( dwCfgTalk >= 0x10)
	{
		int retVal = ::cfgGetValue(dwCfgTalk,module,compoment,defval); 
		::cfgClose(dwCfgTalk);
		return retVal;

	}

	return 0;
}
