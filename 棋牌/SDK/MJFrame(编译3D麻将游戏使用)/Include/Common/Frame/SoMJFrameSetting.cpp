#include "SoMJFrameSetting.h"
#include "SoHelperFun.h"
#include "Clog.h"

using namespace MyGame;

CSoMJFrameSetting ::CSoMJFrameSetting ()
{
	m_pGameSetting  = NULL;
	m_pSoGameLogger = NULL;
 	m_bTileConfig   = FALSE;
    m_nGameID = 0;
    m_nRoomID = 0;
}

CSoMJFrameSetting ::~CSoMJFrameSetting ()
{
	m_pGameSetting  = NULL;
	m_pSoGameLogger = NULL;
    
	MyGame_RemoveSoEventHandler(ISoGameSetting, ISoGameSettingEvent, this);
}

void CSoMJFrameSetting::Initialize(CSoBaseLogic* pSoGameLogicp, int nGameID)
{
    ISoGameFactory * pGameFactory = pSoGameLogicp->GetGameFactory();
    m_pSoGameLogger = pSoGameLogicp->GetSoGameLogger();
	m_pGameSetting = pSoGameLogicp->GetSoGameSetting();
    m_nGameID = nGameID;
    m_nRoomID = pSoGameLogicp->GetSoGameProcessEx()->GetRoomID();

    MyGame_AddSoEventHandler(ISoGameSetting, ISoGameSettingEvent, this); 

    BOOL bRes = m_pGameSetting->SetCfgPath("mj.cfg");

    if (bRes)
    {
        _DOUT("CSoMJFrameSetting 读取配置文件成功!\n");
    }
    else
    {
        _DOUT("CSoMJFrameSetting 读取配置文件失败!\n");
    }
}

void CSoMJFrameSetting::LoadCfg()
{	
	// 当开发人员向SDK的Setting模块设置文件路径的时候，会触发读取配置项的事件
	if (m_pGameSetting != NULL)
	{
		// TODO:读取所有的房间配置项
        memset(&m_tagMJSettingData, 0, sizeof(m_tagMJSettingData));
		m_pGameSetting->GetItemValue("Normal/MaxFan", m_tagMJSettingData.nMaxFan, 1000 );
		m_pGameSetting->GetItemValue("Normal/nBaseScore", m_tagMJSettingData.nBaseScore, 1 );
		m_pGameSetting->GetItemValue("Normal/nTicket", m_tagMJSettingData.nTicket, 0 );
		m_pGameSetting->GetItemValue("Normal/nMinFan", m_tagMJSettingData.nMinFan, 0 );
		m_pGameSetting->GetItemValue("Normal/nGangScore", m_tagMJSettingData.nGangScore, 1 );
		m_pGameSetting->GetItemValue("Normal/nScoreToMoney", m_tagMJSettingData.nScoreToMoney, 0 );
		m_pGameSetting->GetItemValue("Normal/nMoneyType", m_tagMJSettingData.nMoneyType, 0 );
		m_pGameSetting->GetItemValue("Normal/nMaxMoney", m_tagMJSettingData.nMaxMoney, 20000 );
		m_pGameSetting->GetItemValue("Normal/nBuyItemMoney", m_tagMJSettingData.nBuyItemMoney, 10000 );
		m_pGameSetting->GetItemValue("Normal/nGameType", m_stSettingData.nCoolTime, 0);
		m_pGameSetting->GetItemValue("Normal/nGiveOrder", m_tagMJSettingData.nGiveOrder, 0);

        // 加载房间专用配置[游戏ID_房间ID]
        char strTemp[32] = {0};
        sprintf( strTemp, "%d_%d/MaxFan", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nMaxFan, m_tagMJSettingData.nMaxFan );
        sprintf( strTemp, "%d_%d/nBaseScore", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nBaseScore, m_tagMJSettingData.nBaseScore );
        sprintf( strTemp, "%d_%d/nTicket", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nTicket, m_tagMJSettingData.nTicket );
        sprintf( strTemp, "%d_%d/nMinFan", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nMinFan, m_tagMJSettingData.nMinFan );
        sprintf( strTemp, "%d_%d/nGangScore", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nGangScore, m_tagMJSettingData.nGangScore );
        sprintf( strTemp, "%d_%d/nScoreToMoney", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nScoreToMoney, m_tagMJSettingData.nScoreToMoney );
        sprintf( strTemp, "%d_%d/nMoneyType", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nMoneyType, m_tagMJSettingData.nMoneyType );
        sprintf( strTemp, "%d_%d/nMaxMoney", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nMaxMoney, m_tagMJSettingData.nMaxMoney );
        sprintf( strTemp, "%d_%d/nBuyItemMoney", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nBuyItemMoney, m_tagMJSettingData.nBuyItemMoney );
        sprintf( strTemp, "%d_%d/nGameType", m_nGameID, m_nRoomID);
        m_pGameSetting->GetItemValue(strTemp, m_stSettingData.nCoolTime, m_stSettingData.nCoolTime);
		sprintf( strTemp, "%d_%d/nGiveOrder", m_nGameID, m_nRoomID);
		m_pGameSetting->GetItemValue(strTemp, m_tagMJSettingData.nGiveOrder, m_tagMJSettingData.nGiveOrder);

		// 出牌顺序(0: 代表顺时针, 1: 代表逆时针, 读取到内在中会转化为1与3, 方便游戏进行下一个玩家位置计算)
		if (m_tagMJSettingData.nGiveOrder > 0)
		{
			m_tagMJSettingData.nGiveOrder = PLAYER_NBR - 1;  // 逆时针
		}
		else
		{
			m_tagMJSettingData.nGiveOrder = 1;               // 顺时针
		}

        // 配牌开关
        m_pGameSetting->GetItemValue("CardsFrame/bTileConfig", m_bTileConfig, 0);

        // 测试相关开启
        m_pGameSetting->GetItemValue("MJFrame/nOpenGM", m_tagMJSettingData.nOpenGM, 1 );

		// 读麻将的配置
		m_pGameSetting->GetItemValue("MJFrame/nTilesNumber", m_tagMJSettingData.nTilesNumber, 136 );
		m_pGameSetting->GetItemValue("MJFrame/nTilesHandNbr", m_tagMJSettingData.nTilesHandNbr, 13 );
		m_pGameSetting->GetItemValue("MJFrame/nMoreWin", m_tagMJSettingData.nMoreWin, 0 );

		m_pGameSetting->GetItemValue("MJFrame/nQiangGang", m_tagMJSettingData.nQiangGang, 1 );
		m_pGameSetting->GetItemValue("MJFrame/nLouHu", m_tagMJSettingData.nLouHu, 0 );
        m_pGameSetting->GetItemValue("MJFrame/nWuFanHu", m_tagMJSettingData.nWuFanHu, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nCollateWind", m_tagMJSettingData.nCollateWind, 0 );
        m_pGameSetting->GetItemValue("MJFrame/nCollateArrow", m_tagMJSettingData.nCollateArrow, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nAllCanCollate", m_tagMJSettingData.nAllCanCollate, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nShowTing", m_tagMJSettingData.nShowTing, 1 );
		m_pGameSetting->GetItemValue("MJFrame/nTingCanGang", m_tagMJSettingData.nTingCanGang, 1 );
        m_pGameSetting->GetItemValue("MJFrame/nAnGangShow", m_tagMJSettingData.nAnGangShow, 0 );
        m_pGameSetting->GetItemValue("MJFrame/nDongNanXiBeiGang", m_tagMJSettingData.nDongNanXiBeiGang, 0 );
        m_pGameSetting->GetItemValue("MJFrame/nZhongFaBaiGang", m_tagMJSettingData.nZhongFaBaiGang, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nVoicePrice", m_tagMJSettingData.nVoicePrice, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nDrawGangBack", m_tagMJSettingData.nDrawGangBack, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nCanWinLaizi", m_tagMJSettingData.nCanWinLaizi, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nCanBlockLaizi", m_tagMJSettingData.nCanBlockLaizi, 0 );

		m_pGameSetting->GetItemValue("MJFrame/nTileChar", m_tagMJSettingData.nTilesChar, 1 );
		m_pGameSetting->GetItemValue("MJFrame/nTileBamboo", m_tagMJSettingData.nTilesBamboo, 1 );
		m_pGameSetting->GetItemValue("MJFrame/nTileBall", m_tagMJSettingData.nTilesBall, 1 );
		m_pGameSetting->GetItemValue("MJFrame/nTileWind", m_tagMJSettingData.nTilesWind, 1 );
		m_pGameSetting->GetItemValue("MJFrame/nTileArrow", m_tagMJSettingData.nTilesArrow, 1 );
		m_pGameSetting->GetItemValue("MJFrame/nTileFlower", m_tagMJSettingData.nTilesFlower, 1 );

		m_pGameSetting->GetItemValue("MJFrame/nBlockLevelCollect", m_tagMJSettingData.nBlockLevelCollect, 1 );
		m_pGameSetting->GetItemValue("MJFrame/nBlockLevelTriplet", m_tagMJSettingData.nBlockLevelTriplet, 3 );
		m_pGameSetting->GetItemValue("MJFrame/nBlockLevelQuadruplet", m_tagMJSettingData.nBlockLevelQuadruplet, 2 );
		m_pGameSetting->GetItemValue("MJFrame/nBlockLevelTing", m_tagMJSettingData.nBlockLevelTing, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nBlockLevelWin", m_tagMJSettingData.nBlockLevelWin, 4 );

		m_pGameSetting->GetItemValue("MJFrame/nTimerBlock", m_tagMJSettingData.nTimerBlock, 10 );
		m_pGameSetting->GetItemValue("MJFrame/nTimerGive", m_tagMJSettingData.nTimerGive, 15 );
		m_pGameSetting->GetItemValue("MJFrame/nTimerFlower", m_tagMJSettingData.nTimerFlower, 10 );
		m_pGameSetting->GetItemValue("MJFrame/nTimerServer", m_tagMJSettingData.nTimerServer, 20 );
		m_pGameSetting->GetItemValue("MJFrame/nTimerReady", m_tagMJSettingData.nTimerReady, 15 );

        m_pGameSetting->GetItemValue("MJFrame/nTimerCutCard", m_tagMJSettingData.nTimerCutCard, 10);   // 切牌操作时间
        m_pGameSetting->GetItemValue("MJFrame/nTimerDice", m_tagMJSettingData.nTimerDice, 10);         // 骰子操作时间
        m_pGameSetting->GetItemValue("MJFrame/nTimerDeal", m_tagMJSettingData.nTimerDeal, 10);         // 发牌操作时间
        m_pGameSetting->GetItemValue("MJFrame/nTimerJP", m_tagMJSettingData.nTimerJP, 10);             // 精牌操作时间

        m_pGameSetting->GetItemValue("MJFrame/nOpenLookTile", m_tagMJSettingData.nOpenLookTile, 0);    // 是否看牌(游戏结束后, 其他玩家的牌是否显示)
        m_pGameSetting->GetItemValue("MJFrame/nTimerLookTile", m_tagMJSettingData.nTimerLookTile, 10); // 看牌结束后, 间隔多长时间再洗牌

		m_pGameSetting->GetItemValue("MJFrame/nCheck7dui", m_tagMJSettingData.nCheck7dui, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nCheckQuanbukao", m_tagMJSettingData.nCheckQuanbukao, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nCheck131", m_tagMJSettingData.nCheck131, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nCheckZuhelong", m_tagMJSettingData.nCheckZuhelong, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nCheckLualv", m_tagMJSettingData.nCheckLualv, 0 );
		m_pGameSetting->GetItemValue("MJFrame/nCHeckLuafeng", m_tagMJSettingData.nCHeckLuafeng, 0 );

        m_pGameSetting->GetItemValue("MJFrame/nGiveBlockShow", m_tagMJSettingData.nGiveBlockShow, 1 ); // 是否显示出牌区域中被吃碰杠的牌


		int i, j, nCheckNbr, nFanNode = 0;
        char strFan[32] = {0};
        char strFanCheckNbr[32] = {0};

		// 获取番部分
		m_pGameSetting->GetItemValue("MJFan/nFanNodeNbr", nFanNode, 0 );
		memset( &m_tagMJSettingData.tagFanCount, 0, sizeof(m_tagMJSettingData.tagFanCount) );

		for ( i = 0; i < nFanNode && i < MAX_FAN_NUMBER; ++i )
		{
            // 番型序号
            m_tagMJSettingData.tagFanCount.m_FanNode[i].byCount = i;

			// 取到判断的数量
            sprintf( strFanCheckNbr, "MJFan/nFanNode%d_CheckNbr", i );
			m_pGameSetting->GetItemValue( strFanCheckNbr, nCheckNbr, 1 );
			m_tagMJSettingData.tagFanCount.m_FanNode[i].nCheckNbr = nCheckNbr;
			for ( j = 0; j < nCheckNbr && j < MAX_FAN_CHECK_NBR; ++j )
			{
				// 设置每个ID
				sprintf(strFan, "MJFan/nFanNode%d_CheckID%d", i, j );
				m_pGameSetting->GetItemValue( strFan, m_tagMJSettingData.tagFanCount.m_FanNode[i].nCheckFunID[j], 2 );
			}

			// 取到不判断的数量
            sprintf( strFanCheckNbr, "MJFan/nFanNode%d_NoCheckNbr", i );
			m_pGameSetting->GetItemValue( strFanCheckNbr, nCheckNbr, 0 );
			m_tagMJSettingData.tagFanCount.m_FanNode[i].nNoCheckNbr = nCheckNbr;
			for ( j = 0; j < nCheckNbr && j < MAX_FAN_NOCHECK_NBR; ++j )
			{
				// 设置每个ID
				sprintf(strFan, "MJFan/nFanNode%d_NoCheckFan%d", i, j );
				m_pGameSetting->GetItemValue( strFan, m_tagMJSettingData.tagFanCount.m_FanNode[i].nNoCheckFan[j], 0 );
			}

            // 番型点数
			sprintf( strFan, "MJFan/nFanNode%d_FanPoint", i );
			m_pGameSetting->GetItemValue( strFan, m_tagMJSettingData.tagFanCount.m_FanNode[i].byFanPoint, 1 );

            // 番型名称
			sprintf( strFan, "MJFan/nFanNode%d_FanName", i );
			m_pGameSetting->GetItemValue( strFan, m_tagMJSettingData.tagFanCount.m_FanNode[i].szFanName, MAX_FAN_NAME, "Error");
		}
	}
}

//////////////////////////////////////////////////////////////////////////

int CSoMJFrameSetting ::OnLoadCfg()
{
	// 当开发人员或运维人员在服务器上使用retrace命令刷新房间配置项时，会派发OnReloadCfg事件
	// 注意：游戏中使用retrace的时候，会延迟到游戏结束的时候调用 
	LoadCfg();
	return 0;
}
	
int CSoMJFrameSetting ::OnReloadCfg()
{
	// 当开发人员或运维人员在服务器上使用retrace命名刷新房间配置项时，会派发OnReloadCfg事件
	// 注意：游戏中使用retrace的时候，会延迟到游戏结束的时候调用 

	// TODO:重新读取所有配置项；向所有客户端发送新的配置项
	LoadCfg();
	
	return 0;
}

// 获取麻将配置
void CSoMJFrameSetting ::GetMJSettingData( MJ_SETTING_DATA &tagMjSettingData)
{
    tagMjSettingData = m_tagMJSettingData;
}