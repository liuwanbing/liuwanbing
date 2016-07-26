#include "stdafx.h"
#include "MJFrameLogic.h"

/** 构造函数 */
CMJFrameLogic::CMJFrameLogic(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;
	m_pSoundEngine = createIrrKlangDevice();

	memset(&m_tagSetting, 0, sizeof(m_tagSetting));
	memset(&m_bReady, 0, sizeof(m_bReady));
	memset(&m_bTuoGuan, 0, sizeof(m_bTuoGuan));
	memset(&m_byDice, 0, sizeof(m_byDice));
	memset(&m_nTempMoney, 0, sizeof(m_nTempMoney));

	m_nTimeOutCount = 0;
	m_byGangCount = 0;
	m_byDiceFinishCount = 0;
	m_byDealer = -1;
	m_byMyStation = -1;
	m_byTurnStation = -1;
	m_byGameState = MJ_STATE_NOSTART;
	m_byStateSelect = MJ_STATE_SELECT_NONE;
	m_enDiceState = En_Dice_None;
	m_TilesGod.ReleaseAll();
	m_TilesWall.ReleaseAll();
	
	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		m_TilesHand[i].ReleaseAll();
		m_TilesGive[i].ReleaseAll();
		m_TilesSet[i].ReleaseAll();
		m_TilesFlower[i].ReleaseAll();
	}
}

/** 析构函数 */
CMJFrameLogic::~CMJFrameLogic()
{
}

/** 
 * @brief 初始化
 */
int CMJFrameLogic::Initial()
{
	// 载入UI
	m_pUI->LoadData(L"UIGameComm.dat"); 

	// 读取声音配置
	CString s = CBcfFile::GetAppPath (); 
	CBcfFile f(s + SKIN_FOLDER +".bcf");
	CString key = TEXT("Operate");
	m_bSound = f.GetKeyVal(key, "Sound", TRUE);

	SAFE_CTRL_OPERATE(IButton, CTN_BTN_OpenSound, SetControlVisible(m_bSound));   // 打开音乐按钮
	SAFE_CTRL_OPERATE(IButton, CTN_BTN_CloseSound, SetControlVisible(!m_bSound)); // 关闭音乐按钮

	// 播放背景音效
	PlayBackSound();

	// 加载游戏音效
	LoadSound();

	return 0;
}

/** 
 * @brief 初始化UI
 */
int CMJFrameLogic::InitUI()
{
	memset(&m_bReady, 0, sizeof(m_bReady));

	// 显示玩家信息容器
	SAFE_CTRL_OPERATE(IContain, CTN_GameInfo, SetControlVisible(true));
	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		DrawUserInfo(i, m_tmpUserInfo, false);
		DrawReady(i, false);
	}

	// 隐藏准备按钮
	SAFE_CTRL_OPERATE(IButton, CTN_BTN_Start, SetControlVisible(false));

	// 隐藏GM命令控件
	SAFE_CTRL_OPERATE(IEditorText, CTN_EDITORTEXT_GM, SetControlVisible(false));

	InitData();

	return 0;
}

/** 
 * @brief 自定义初使化数据
 */
void CMJFrameLogic::InitData()
{
	memset(&m_bTuoGuan, 0, sizeof(m_bTuoGuan));
	memset(&m_byDice, 0, sizeof(m_byDice));
	memset(&m_nTempMoney, 0, sizeof(m_nTempMoney));
	m_nTimeOutCount = 0;
	m_byGangCount = 0;
	m_byDiceFinishCount = 0;
	m_byDealer = -1;
	m_byTurnStation = -1;
	m_byGameState = MJ_STATE_NOSTART;
	m_byStateSelect = MJ_STATE_SELECT_NONE;
	m_enDiceState = En_Dice_None;
	m_TilesGod.ReleaseAll();
	m_TilesWall.ReleaseAll();
	m_TilesWall.CurrentLength(m_tagSetting.nTilesNumber);

	SAFE_CTRL_OPERATE(IButton, CTN_BTN_TuoGuan, SetControlVisible(true));
	SAFE_CTRL_OPERATE(IButton, CTN_BTN_Cancel_TuoGuan, SetControlVisible(false));
	SAFE_CTRL_OPERATE(IImage, CTN_IMG_LANZHUANG, SetControlVisible(false))
	
	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		m_TilesHand[i].ReleaseAll();
		m_TilesGive[i].ReleaseAll();
		m_TilesSet[i].ReleaseAll();
		m_TilesFlower[i].ReleaseAll();

		// 取消禁用的牌控件
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_MJCtrl_HandCtrl_0 + i, SetInvalidTile(NULL, 0));
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_MJCtrl_HandCtrl_0 + i, SetGodTile(NULL, 0));

		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_MJCtrl_GiveCtrl_0 + i, SetGodTile(NULL, 0));

		SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_MJCtrl_WallCtrl_0 + i, FillWallBack(MJ_WALL_NUMBER));
		SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_MJCtrl_WallCtrl_0 + i, SetMaxLength(MJ_WALL_NUMBER));  // SetMaxLength这个函数必须设置, 不然设置麻将位置会从左边开始
	}


	// 显示麻将容器
	SAFE_CTRL_OPERATE(IContain, CTN_MJCtrl, SetControlVisible(true));
	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		DrawHand(i, m_TilesHand[i]);
		DrawGive(i, m_TilesGive[i]);
		DrawSet(i, m_TilesSet[i]);

		DrawTuoGuan(i, false);
		DrawDealer(i, false);
	}

	// 显示牌墙
	DrawWall(true);

	// 隐藏拦牌框
	DrawBlock(0);

	// 隐藏结算框
	DrawFinish(NULL, false);

	// 隐藏小手
	DrawSmallHand(0, false);

	// 隐藏出牌标记
	DrawGiveAnimate(0, false);

	// 隐藏拦牌信息
	OnBlockMoveSelect(TILE_BEGIN);
}

/** 
 * @brief 加载音效资源
 */
void CMJFrameLogic::LoadSound()
{
	CString s = CINIFile::GetAppPath (); // 本地路径   
	s = s + TEXT("/music/sound.ini");

	char m_pchar[MAX_PATH] = {0};
	wchar_t wszWho[MAX_PATH] = L"";

	int i, j, num = 0;
	CINIFile file(s);
	CString strSec = TEXT("Normal");
	CString value = "";

	
	// 万(1~9)
	for (i = 1; i < 10; i++)
	{
		SOUND_INFO info;
		int &nCount = info.nCount;
		nCount = 0;

		for (j = 0; j < MAX_SOUND_RAND; j++)
		{
			value.Format("%dwan%d", i, j);
			CString strVal = file.GetKeyVal(strSec, value, TEXT("NULL"));
			if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL"))) == 0)
			{
				break;
			}
			_tcscpy(info.szFileName[nCount++], strVal.GetBuffer());
		}        
		m_SoundMap[i] = info;
	}

	// 条(11~19)
	for (i = 1; i < 10; i++)
	{
		SOUND_INFO info;
		int &nCount = info.nCount;
		nCount = 0;

		for (j = 0; j < MAX_SOUND_RAND; j++)
		{
			value.Format("%dtiao%d", i, j);
			CString strVal = file.GetKeyVal(strSec, value, TEXT("NULL"));
			if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL"))) == 0)
			{
				break;
			}
			_tcscpy(info.szFileName[nCount++], strVal.GetBuffer());  
		}        
		m_SoundMap[10 + i] = info;
	}

	// 筒(21~29)
	for (i = 1; i < 10; i++)
	{
		SOUND_INFO info;
		int &nCount = info.nCount;
		nCount = 0;
		for (j = 0; j < MAX_SOUND_RAND; j++)
		{
			value.Format("%dtong%d", i, j);
			CString strVal = file.GetKeyVal(strSec, value, TEXT("NULL"));
			if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL")))==0)
			{
				break;
			}
			_tcscpy(info.szFileName[nCount++], strVal.GetBuffer());
		}        
		m_SoundMap[20 + i] = info;
	}

	// 东南西北中发白(31~37)
	CString strKey[7] = {TEXT("dong"), TEXT("nan"), TEXT("xi"), TEXT("bei"), TEXT("zhong"), TEXT("fa"),TEXT("bai")};
	for (i = 0; i < 7; i++)
	{
		SOUND_INFO info;
		int &nCount = info.nCount;
		nCount = 0;
		for (j = 0; j < MAX_SOUND_RAND; j++)
		{
			CString st="";
			st.Format("%d", j);
			value = strKey[i] + st;

			CString strVal = file.GetKeyVal(strSec, value, TEXT("NULL"));
			if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL"))) == 0)
			{
				break;
			}
			_tcscpy(info.szFileName[nCount++], strVal.GetBuffer());
		}        
		m_SoundMap[31 + i] = info;
	}

	// 吃、碰、杠、听、自摸、点炮(100~105)
	CString strKeyBlock[6] = {TEXT("chi"), TEXT("peng"), TEXT("gang"), TEXT("ting"), TEXT("Zimo"), TEXT("DianPao")};
	for (i = 0; i < 6; i++)
	{
		SOUND_INFO info;
		int &nCount = info.nCount;
		nCount = 0;
		for (j = 0; j < MAX_SOUND_RAND; j++)
		{
			CString st="";
			st.Format("%d", j);
			value = strKeyBlock[i] + st;
			CString strVal = file.GetKeyVal(strSec, value, TEXT("NULL"));
			if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL"))) == 0)
			{
				break;
			}
			_tcscpy(info.szFileName[nCount++], strVal.GetBuffer());
		}        
		m_SoundMap[SOUND_CHI + i] = info;
	}
}

/** 
 * @brief 游戏消息
 * @param [in] nMainID 游戏主ID
 * @param [in] nSubId  游戏次ID
 * @param [in] buffer  游戏数据包
 * @param [in] nLen    游戏数据包的长度
 * @return 
 */
int CMJFrameLogic::HandleGameMessage(WORD nMainID, WORD nSubId, BYTE * buffer, int nLen)
{
	if (nSubId == ASS_GM_AGREE_GAME/*nSubId == ASS_SUB_USERAGREE*/)
	{
		if (nLen != sizeof(MSG_GR_S_Agree))
		{
			return 0;
		}

		// 有玩家同意游戏
		MSG_GR_S_Agree * pCmd = (MSG_GR_S_Agree *)buffer;
		if (pCmd == NULL)
		{
			return 0;
		}

		if (m_byGameState == MJ_STATE_NONE || m_byGameState == MJ_STATE_NOSTART || m_byGameState == MJ_STATE_END)
		{
			// 显示准备信息
			DrawReady(pCmd->bDeskStation, true);

			// 播放准备音效
			if (pCmd->bDeskStation == m_byMyStation)
			{
				PlaySound(SOUND_BEGIN);
			}
			else
			{
				PlaySound(SOUND_READY);
			}
		}

		return 0;
	}
	else if (nSubId == ASS_SUB_TRUSTEE)
	{
		MSG_GR_S_UserTruste * pCmd = (MSG_GR_S_UserTruste *)buffer;
		if (pCmd == NULL)
		{
			return 0;
		}

		m_bTuoGuan[pCmd->byChair] = pCmd->byFlag;

		// 绘制拖管
		DrawTuoGuan(pCmd->byChair, pCmd->byFlag);

		if (pCmd->byChair == m_byMyStation)
		{
			if (m_bTuoGuan[m_byMyStation])
			{
				SAFE_CTRL_OPERATE(IButton, CTN_BTN_TuoGuan, SetControlVisible(false));
				SAFE_CTRL_OPERATE(IButton, CTN_BTN_Cancel_TuoGuan, SetControlVisible(true));

				// 自动处理
				if (m_byGameState == MJ_STATE_GIVE || m_byGameState == MJ_STATE_BLOCK)
				{
					OnRequestGive(0, true);
				}
			}
			else
			{
				m_nTimeOutCount = 0;
				SAFE_CTRL_OPERATE(IButton, CTN_BTN_TuoGuan, SetControlVisible(true));
				SAFE_CTRL_OPERATE(IButton, CTN_BTN_Cancel_TuoGuan, SetControlVisible(false));
			}
		}

		return 0;
	}

	// 游戏内部消息ID定义
	if (buffer != NULL && nLen >= sizeof(TCMD_HEADER))
	{
		TCMD_HEADER *pData = (TCMD_HEADER *)buffer;
		if (MJFRAME_MOUDLE_MSG == pData->cCmdID)
		{
			switch(pData->cCmdPara)
			{
			case SC_MJ_NOTIFY_BASE_INFO:     // 游戏基础信息(例如服务端的配置信息)
				{
					OnNotifyBaseInfo(buffer, nLen);
				}
				break; 
			case SC_MJ_NOTIFY_VIEW_INFO:     // 游戏数据信息(例如玩家进入、断线重回、旁观进入)
				{
					OnNotifyGameInfo(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_RUN_NBR:       // 接收到通知连庄信息(游戏开始发送的的第一条游戏消息)
				{
					OnNotifyRunNbr(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_DEALER:        // 接收到通知庄家位置消息
				{
					OnNotifyDealer(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_DEAL:          // 接收到通知发牌消息
				{
					OnNotifyDeal(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_FLOWER_OVER:   // 接收到补花结束消息(第一个玩家出牌)
				{
					OnNotifyFlowerOver(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_GIVE:          // 接收到出牌结果消息
				{
					OnNotifyGive(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_DRAW:          // 接收到摸牌消息
				{
					OnNotifyDraw(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_BLOCK:         // 接收到拦牌事件消息(如吃、碰、杠、听、胡)
				{
					OnNotifyBlock(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_BLOCK_RESULT:  // 接收到拦牌结果消息
				{
					OnNotifyBlockResult(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_FINISH:        // 接收到游戏结算信息
				{
					OnNotifyFinish(buffer, nLen);
				}
				break;
			case SC_MJ_UPDATEHANDTILES:      // 接收到更新手牌数据消息
				{
					OnNotifyUpdateHandTiles(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_CHANGE_LOOKON: // 接收到修改旁观后发给旁观者消息(目前没有什么作用, 留给旁观使用)
				{
					OnNotifyChangeLookOn(buffer, nLen);
				}
				break;
			case SC_MJ_NOTIFY_GMCMD:         // 接收到GM命令执行结果消息
				{
					OnNotifyGMCmd(buffer, nLen);
				}
				break;
			default:
				break;
			}
		}
	}

	return 0;
}

/** 
 * @brief 界面消息
 * @param [in] pMessage 响应界面操作的控件信息
 */
int CMJFrameLogic::OnUIMessage(TUIMessage* pMessage)
{
	if (pMessage->_uMessageType == UI_KEYDOWN)
	{
		if (m_tagSetting.nOpenGM == 1)
		{
			if (pMessage->_ch == VK_F11)
			{
				IEditorText *pEdit = NULL;
				GETCTRL(IEditorText, pEdit, m_pUI, CTN_EDITORTEXT_GM);
				if (pEdit != NULL)
				{
					pEdit->SetControlVisible(!pEdit->GetVisible());
					pEdit->SetMaxTypeIn(100);
					pEdit->Clear();
					pEdit->SetText(L"");
				}
			}
			else if (pMessage->_ch == VK_RETURN)
			{
				//m_TilesWall.PopFrontTile();
				DrawWall(true);
				IEditorText *pEdit = NULL;
				GETCTRL(IEditorText, pEdit, m_pUI, CTN_EDITORTEXT_GM);
				if (pEdit != NULL)
				{
					if (pEdit->GetVisible())
					{
						OnRequestGMCmd();
					}
				}
			}
		}

		return 0;
	}

	if (pMessage->_uMessageType == UI_LBUTTONDOWN) 
	{
		switch (pMessage->_uControlID)
		{
		case CTN_JSK_BTN_Exit:       // 结算框退出
			{
				OnGameQuiting();
				m_pGameFrame->CloseClient();
			}
			break;
		case CTN_BTN_Start:          // 开始
		case CTN_JSK_BTN_Continue:   // 结算框继续
			{
				//TCMD_NOTIFY_DEAL msg2;
				//msg2.ucDealer = 3;
				//for (int i = 0; i < 13; i++)
				//{
				//	msg2.tiles[i] = 2;
				//}
				//msg2.ucDice[0] = 1;
				//msg2.ucDice[1] = 1;
				//msg2.ucOther[0] = 4;
				//msg2.ucOther[1] = 4;
				//OnNotifyDeal((BYTE*)&msg2, sizeof(TCMD_NOTIFY_DEAL));

				InitData();

				SAFE_CTRL_OPERATE(IButton, CTN_BTN_Start, SetControlVisible(false));

				MSG_GR_S_Agree msg;
				m_pGameFrame->SendGameData(ASS_GM_AGREE_GAME, (void*)&msg, sizeof(msg));
			}
			break;
		case CTN_MJCtrl_HandCtrl_2:  // 自己手牌控件
			{
				if (m_byGameState == MJ_STATE_BLOCK && (m_byStateSelect == MJ_STATE_SELECT_COLLECT || m_byStateSelect == MJ_STATE_SELECT_QUADRUPLET))
				{
					// 吃牌与杠牌选择
					OnRequestBlockSelect(pMessage->_uControlID);
				}
				else
				{
					// 正常出牌与听牌选择
					OnRequestGive(pMessage->_uControlID);
				}
			}
			break;
		case CTN_Block_BTN_Chi:      // 吃、碰、杠、听、胡、取消
		case CTN_Block_BTN_Peng:    
		case CTN_Block_BTN_Gang:    
		case CTN_Block_BTN_Ting:    
		case CTN_Block_BTN_Hu:      
		case CTN_Block_BTN_Cancel:  
			{
				OnRequestBlock(pMessage->_uControlID);
			}
			break;
		case CTN_BTN_TuoGuan:
		case CTN_BTN_Cancel_TuoGuan:
			{
				if (m_byGameState == MJ_STATE_END || m_byGameState == MJ_STATE_NOSTART)
				{
					break; // 非游戏状态中不对拖管进行处理
				}

				MSG_GR_S_UserTruste msg;
				msg.byFlag = !m_bTuoGuan[m_byMyStation];
				m_pGameFrame->SendGameData(ASS_SUB_TRUSTEE, (void*)&msg, sizeof(msg));
			}
			break;
		case CTN_BTN_CloseSound:  // 关闭音乐
			{
				m_bSound = true;
				CString s = CBcfFile::GetAppPath (); // 本地路径
				DWORD dwCfgFile = ::cfgOpenFile(s + SKIN_FOLDER +".bcf");
				if (dwCfgFile >= 0x10) // 文件打开成功
				{
					// 声音开关配置
					::cfgSetValue(dwCfgFile,"Operate","Sound",m_bSound);
					::cfgClose(dwCfgFile);						
				}

				if (m_pSoundEngine != NULL)
				{
					m_pSoundEngine->setAllSoundsPaused(false);
					PlayBackSound();//播放背景音乐
				}

				SAFE_CTRL_OPERATE(IButton, CTN_BTN_OpenSound, SetControlVisible(true));   // 打开音乐按钮
				SAFE_CTRL_OPERATE(IButton, CTN_BTN_CloseSound, SetControlVisible(false)); // 关闭音乐按钮
			}
			break;
		case CTN_BTN_OpenSound:  // 打开音乐
			{
				m_bSound = false;
				CString s = CBcfFile::GetAppPath (); // 本地路径
				DWORD dwCfgFile = ::cfgOpenFile(s + SKIN_FOLDER +".bcf");
				if (dwCfgFile >= 0x10)  // 文件打开成功
				{
					// 声音开关配置
					::cfgSetValue(dwCfgFile, "Operate", "Sound", m_bSound);
					::cfgClose(dwCfgFile);						
				}

				if (m_pSoundEngine != NULL)
				{
					m_pSoundEngine->setAllSoundsPaused(true);
					m_pSoundEngine->stopAllSounds();
				}

				SAFE_CTRL_OPERATE(IButton, CTN_BTN_OpenSound, SetControlVisible(false));  // 打开音乐按钮 隐藏
				SAFE_CTRL_OPERATE(IButton, CTN_BTN_CloseSound, SetControlVisible(true));  // 关闭音乐按钮 显示
				PlayBackSound(false);//播放背景音乐
			}
			break;
		default:
			break;
		}
	}
	else if (pMessage->_uMessageType == UI_CATOONFINISH) 
	{
		OnAnimalFinish(pMessage->_uControlID);  // 动画结束
	}
	else if (pMessage->_uMessageType == UI_TIME_RUN)
	{
		if (pMessage->_uControlID == CTN_Clock_Time)
		{
			// 播放闹钟倒数5秒提示音效
			PlaySound(SOUND_CLOCK);
		}
	}
	else if (pMessage->_uMessageType == UI_MJCTRL_MOVEONMJ)
	{
		if (pMessage->_uControlID == CTN_MJCtrl_HandCtrl_2)
		{
			// 拦牌信息移动提示选择  
			OnBlockMoveSelect(pMessage->_uControlID, pMessage->_mouseX);

			// 播放鼠标移动麻将子上的音效
			PlaySound(SOUND_SEL);
		}
	}
	else if (pMessage->_uMessageType == UI_MOUSEENTER)
	{
		if (pMessage->_uControlID == CTN_MJCtrl_HandCtrl_2)
		{
			if (pMessage->_ch == 1)
			{
				// 鼠标移动手牌控件
				OnBlockMoveSelect(pMessage->_uControlID);
			}
		}
	}

	return 0;
}

/** 
 * @brief 动画结束
 * @param[in] uControlID 控件ID
 */
void CMJFrameLogic::OnAnimalFinish(UINT32 uControlID)
{
	switch (uControlID) 
	{	
	case ExAnimate_0_Saizi_1:   // 第一个骰子播放完毕
	case ExAnimate_1_Saizi_1:
	case ExAnimate_2_Saizi_1:
	case ExAnimate_3_Saizi_1:
	case ExAnimate_0_Saizi_2:   // 第二个骰子播放完毕
	case ExAnimate_1_Saizi_2:
	case ExAnimate_2_Saizi_2:
	case ExAnimate_3_Saizi_2:
		{
			OnAnimalDiceFinish(uControlID);
		}
		break;
	case CTN_Clock_Time:        // 定时器结束
		{
			OnAnimalClockFinish(uControlID);
		}
		break;
	default:
		break;
	}
}

/** 
 * @brief 动画骰子结束
 * @param[in] uControlID 控件ID
 */
void CMJFrameLogic::OnAnimalDiceFinish(UINT32 uControlID)
{
	IExAnimate * pExAnimate = NULL;
	GETCTRL(IExAnimate, pExAnimate, m_pUI, uControlID);	
	if (pExAnimate != NULL)
	{
		if (uControlID < ExAnimate_0_Saizi_2)
		{
			// 第1棵骰子
			pExAnimate->SetShowFrame(m_byCurDice[0] + 14);   // 骰子花色的起始值 1为 0 + 14, 2为 1 + 14
		}
		else
		{
			// 第2棵骰子
			pExAnimate->SetShowFrame(m_byCurDice[1] + 14);   // 骰子花色的起始值 1为 0 + 14, 2为 1 + 14
		}

		// 设置骰子最终显示位置
		pExAnimate->SetGameXY(pExAnimate->GetEnd().x, pExAnimate->GetEnd().y);

		m_byDiceFinishCount++;

		if (m_byDiceFinishCount == DICE_NBR)
		{
			m_byDiceFinishCount = 0;

			m_pGameFrame->SetTimer(TIME_DICE_FINISH, 2000);
		}
	}
}

/** 
 * @brief 动画定时器结束
 * @param[in] uControlID 控件ID
 */
void CMJFrameLogic::OnAnimalClockFinish(UINT32 uControlID)
{
	switch (m_byGameState)
	{
	case MJ_STATE_NOSTART: // 准备时间结束
	case MJ_STATE_END:
		{
			m_pGameFrame->CloseClient();
		}
		break;
	case MJ_STATE_GIVE:    // 出牌时间结束
		{
			OnTimeOutGive(uControlID);
		}
		break;
	case MJ_STATE_BLOCK:   // 拦牌时间结束
		{
			OnTimeOutBlock(uControlID);
		}
		break;
	default:
		break;
	}
}

/** 
 * @brief 出牌超时
 * @param[in] uControlID 控件ID
 */
void CMJFrameLogic::OnTimeOutGive(UINT32 uControlID)
{
	if (m_byTurnStation == m_byMyStation)
	{
		OnRequestGive(TILE_BEGIN, true);

		m_nTimeOutCount++;  // 记录超时次数, 超时3次将进入托管状态

		TCHAR szText[250];
		if (m_nTimeOutCount == 3)
		{
			wsprintf(szText, "您已经超时%d次, 系统自动为您托管!", m_nTimeOutCount);
			m_pGameFrame->InsertSystemMessage(szText);

			// 请求服务器托管
			MSG_GR_S_UserTruste msg;
			msg.byFlag = 1;
			m_pGameFrame->SendGameData(ASS_SUB_TRUSTEE, (void*)&msg, sizeof(msg));

			m_nTimeOutCount = 0;
		}
		else
		{
			wsprintf(szText, "您已经超时%d次, 超时3次系统将会自动为您托管!", m_nTimeOutCount);
			m_pGameFrame->InsertSystemMessage(szText);
		}
	}
}

/** 
 * @brief 拦牌超时
 * @param[in] uControlID 控件ID
 */
void CMJFrameLogic::OnTimeOutBlock(UINT32 uControlID)
{
	if (m_byTurnStation == m_byMyStation)
	{
		// 隐藏吃碰杠面板
		SAFE_CTRL_OPERATE(IContain, CTN_Block, SetControlVisible(false));

		// 取消禁用的牌控件
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_MJCtrl_HandCtrl_2, SetInvalidTile(NULL, 0));

		// 隐藏拦牌提示信息
		SAFE_CTRL_OPERATE(IContain, CTN_Block_Panel, SetControlVisible(false));

		// 取消拦牌, 如果可以胡牌，则默认胡牌
		TCMD_REQUEST_BLOCK cmd;
		cmd.ucFlag = (m_stBlockInfo.byFlag & 0x10) ? ACTION_WIN : ACTION_EMPTY;
		m_pGameFrame->SendGameData(0, (void*)&cmd, sizeof(cmd));

		if (cmd.ucFlag == ACTION_EMPTY && m_TilesHand[m_byMyStation].CurrentLength() % 3 == 2)
		{
			m_byGameState = MJ_STATE_GIVE;
			SetGameTimer(m_byTurnStation, m_tagSetting.nTimerGive);
		}
	}
}


/** 
 * @brief 重置游戏
 */
int CMJFrameLogic::ResetGameFrame(void)
{
	return 0;
}

/** 
 * @brief 玩家进入
 */
int CMJFrameLogic::GameUserCome(void)
{	
	m_byMyStation = m_pGameFrame->GetMyDeskStation();
	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		if (m_pGameFrame->GetUserInfo(i, m_tmpUserInfo))
		{
			DrawUserInfo(i, m_tmpUserInfo, true);
		}
	}
	return 0;
}

/** 
 * @brief 玩家进入
 * @param [in] nIndex 游戏桌子位置
 */
int CMJFrameLogic::GameUserCome(int nIndex)
{
	return 0;
}

/** 
 * @brief 玩家离开
 */
int CMJFrameLogic::GameUserLeft(int nIndex)
{
	m_bReady[nIndex] = false;

	DrawReady(nIndex, false);

	// 隐藏离开玩家信息
	DrawUserInfo(nIndex, m_tmpUserInfo, false);

	return 0;
}

/** 
 * @brief 玩家断线
 */
int CMJFrameLogic::GameUserOffLine(void)
{
	return 0;
}

/** 
 * @brief 是否正在游戏，由游戏实现
 * @return 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
 */
bool CMJFrameLogic::IsPlayingGame()
{
	return m_byGameState ;
}

/** 
 * @brief 定时器消息
 * @param [in] nTimerID 定时器ID
 */
void CMJFrameLogic::OnTimer(int nTimerID)
{
	switch (nTimerID)
	{
	case TIME_DEAL:          // 发牌
		{
			m_pGameFrame->KillTimer(TIME_DEAL);
			OnDeal();
		}
		break;
	case TIME_DICE_FINISH:   // 骰子完成
		{
			m_pGameFrame->KillTimer(TIME_DICE_FINISH);
			OnDice();
		}
		break;
	case TIME_FIND_POS:      // 查找位置定时器
		{
			m_pGameFrame->KillTimer(TIME_FIND_POS);
			
			// 使用小手指进行被牌位置确定
			m_nSmallHandCount = 0;

			m_pGameFrame->SetTimer(TIME_HAND_MOVE, 300);
		}
		break;
	case TIME_HAND_MOVE:     // 小手指移动定时器
		{
			m_pGameFrame->KillTimer(TIME_HAND_MOVE);
			OnSmallHandMove();
		}
		break;
	case TIME_SHOW_FINISH:   // 显示结算框定时器
		{
			m_pGameFrame->KillTimer(TIME_SHOW_FINISH);
			OnShowFinish();
		}
		break;
	default:
		break;
	}
}

/** 
 * @brief 发牌操作函数
 */
void CMJFrameLogic::OnDeal()
{
	m_TilesWall.ReleaseAll();
	m_TilesWall.CurrentLength(m_tagSetting.nTilesNumber);

	// 发牌
	BYTE byStation = m_pGameFrame->DeskStation2View(m_byDealer);     // 抓牌方向
	int nPos = (m_byDice[0] + m_byDice[1]) * 2;                      // 从第几墩开始抓
	int i, j, nStartPos, nCount = 0;

	for (i = 0; i < PLAYER_NBR; i++)
	{
		for (j = 0; j < m_tagSetting.nTilesHandNbr; j++)
		{
			nStartPos = byStation * MJ_WALL_NUMBER + nPos + nCount;
			if (nStartPos >= m_tagSetting.nTilesNumber)
			{
				nStartPos = nStartPos - m_tagSetting.nTilesNumber; 
			}

			m_TilesWall.SetTile(nStartPos,  -1);

			nCount++;
		}

		if (i == m_byDealer)
		{
			nStartPos = byStation * MJ_WALL_NUMBER + nPos + nCount;
			if (nStartPos >= m_tagSetting.nTilesNumber)
			{
				nStartPos = nStartPos - m_tagSetting.nTilesNumber; 
			}

			// 庄家多抓一张
			m_TilesWall.SetTile(nStartPos,  -1);
			nCount++;
		}
	}

	// 设置牌墙的抓牌与补牌位置
	m_TilesWall.SetPos(byStation * MJ_WALL_NUMBER + nPos + nCount, byStation * MJ_WALL_NUMBER + nPos - 1);

	// 显示牌墙
	DrawWall(true);

	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		DrawHand(i, m_TilesHand[i], m_TilesHand[i].CurrentLength() % 3 == 2);
	}

	// 发牌完成, 需要通知服务端
	TCMD_REQUEST_DEAL_FINISH cmd; 
	m_pGameFrame->SendGameData(0, (void*)&cmd, sizeof(cmd));
}

/** 
 * @brief 骰子操作函数
 */
void CMJFrameLogic::OnDice()
{
	switch (m_enDiceState)
	{
	case En_Dice_1:   // 第1次骰子, 确定下一位打骰子的玩家
		{
			//// 进入第二次骰子
			//m_enDiceState = En_Dice_2;

			//// 播放骰子动画
			//DrawDice((m_byDealer + 1) % PLAYER_NBR, m_byDice, true);

			// 骰子完成后, 进入发牌
			m_pGameFrame->SetTimer(TIME_DEAL, 800);
		}
		break;
	case En_Dice_2:   // 第2次骰子, 确定抓牌位置
		{
			m_pGameFrame->SetTimer(TIME_DEAL, 800);
		}
		break;
	case En_Dice_3:   // 第3次骰子, 确定精牌的位置
		{
			m_pGameFrame->SetTimer(TIME_FIND_POS, 800);
		}
		break;
	case En_Dice_4:   // 第4次骰子, 杠牌骰子确定补牌位置
		{
			m_pGameFrame->SetTimer(TIME_FIND_POS, 800);
		}
		break;
	}
}

/** 
 * @brief 小手移动操作函数
 */
void CMJFrameLogic::OnSmallHandMove()
{
	// 播放小手移动声音
	PlaySound(SOUND_SEL);

	if (m_nSmallHandCount < m_byCurDice[0] + m_byCurDice[1])
	{
		// m_nSmallHandCount需要先加, 不能从0开始.
		DrawSmallHand(++m_nSmallHandCount, true);

		m_pGameFrame->SetTimer(TIME_HAND_MOVE, 200);
	}
	else
	{
		// 隐藏小手
		m_nSmallHandCount = 0;
		DrawSmallHand(0, false);

		// 隐藏骰子
		DrawDice(0, m_byCurDice, false);

		if (m_enDiceState == En_Dice_4)
		{
			// 杠牌位置确定
			// 从牌墙指定位置被一张牌
			int nPos = m_TilesWall.PopFrontTile(m_stDrawInfo.byDice[0] + m_stDrawInfo.byDice[1]);

			if (m_stDrawInfo.chair == m_byMyStation)
			{
				m_TilesHand[m_stDrawInfo.chair].AddTile(m_stDrawInfo.t);
			}
			else
			{
				m_TilesHand[m_stDrawInfo.chair].AddTile(TILE_BEGIN);
			}

			// 绘制牌墙
			DrawWall(true);

			// 绘制手牌区域
			DrawHand(m_stDrawInfo.chair, m_TilesHand[m_stDrawInfo.chair], true);

			// 出牌状态
			m_byGameState = MJ_STATE_GIVE;

			m_byTurnStation = m_stDrawInfo.chair;

			// 判断是否有拦牌操作
			if (m_stDrawInfo.ucFlag == ACTION_EMPTY)
			{
				// 设置出牌定时器
				SetGameTimer(m_byTurnStation, m_tagSetting.nTimerGive);

				if (m_byTurnStation == m_byMyStation && m_bTuoGuan[m_byMyStation])
				{
					// 自动处理
					OnRequestGive(TILE_BEGIN, true);
				}
			}
			else
			{
				OnNotifyBlock((BYTE*)&m_stDrawInfo, sizeof(m_stDrawInfo));
			}
		}
	}
}

/** 
 * @brief 显示结算操作函数
 */
void CMJFrameLogic::OnShowFinish()
{
	m_pGameFrame->InsertNormalMessage("------------------");
	m_pGameFrame->InsertNormalMessage("游戏结算信息:");
	TCHAR szText[MAX_PATH];
	TCHAR szName[MAX_PATH];
	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		memset(szName, 0, sizeof(szName));
		W2C(m_wNickName[i], szName);
		wsprintf(szText, "%s: 牌型分: %d, 杠分: %d 总计输赢: %d", 
			szName, m_stFinishInfo.nFanPoint[i], m_stFinishInfo.nGangFen[i], m_stFinishInfo.nMoney[i]);
		m_pGameFrame->InsertNormalMessage(szText);
	}
	m_pGameFrame->InsertNormalMessage("------------------");

	// 显示结算
	DrawFinish(&m_stFinishInfo, true);

	// 刷新玩家信息
	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		if (m_pGameFrame->GetUserInfo(i, m_tmpUserInfo))
		{
			DrawUserInfo(i, m_tmpUserInfo, true);
		}
	}

}


/** 
 * @brief 设置状态
 * @param [in] bGameStation 游戏状态
 */
void CMJFrameLogic::SetStationParameter(BYTE bGameStation)
{

}

/** 
 * @brief 退出事件
 */
bool CMJFrameLogic::OnGameQuiting()
{
	if(m_pGameFrame->GetIsWatching())
	{
		return true ; 
	}

	CString szAppPath = CINIFile::GetAppPath();
	szAppPath.SetAt(szAppPath.GetLength() - 1, TCHAR('\0'));
	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
	CString szAppPathParent = szAppPath.Left(nPosOfSlash);


	CString strKeyName  ; 
	strKeyName.Format("%d", NAME_ID) ; 
	CBcfFile File(szAppPathParent + "\\SpecialRule.bcf") ; 

	bool bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto", strKeyName, 1) ;		
	if (m_byGameState != MJ_STATE_NOSTART && m_byGameState != MJ_STATE_END)
	{
		if (bForceQuitAsAuto)
		{
			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开游戏将会托管处理 ，是否要离开？", "提示",AFC_YESNO)) 
			{
				return false ; 
			}
		}
		else
		{
			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开将会被扣罚，是否真的要离开？", "提示",AFC_YESNO)) 
			{
				return false ; 
			}

			m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
/** 
 * @brief 绘制玩家信息
 * @param[in] byChair    玩家位置
 * @param[in] stUserInfo 玩家信息
 * @param[in] bShow      是否显示
 */
void CMJFrameLogic::DrawUserInfo(BYTE byChair, UserInfoForGame stUserInfo, bool bShow)
{
	BYTE byViewStation = m_pGameFrame->DeskStation2View(byChair);
	wchar_t wText[MAX_PATH];
	IText * pText = NULL;
	IImage * pImage = NULL;
	IExAnimate * pExAnimate = NULL;

	// 显示背景底图
	SAFE_CTRL_OPERATE(IImage, CTN_IMG_BK_0 + byViewStation, SetControlVisible(bShow));

	// 显示昵称
	GETCTRL(IText, pText, m_pUI, CTN_TEXT_NickName_0 + byViewStation);
	if (pText != NULL)
	{
		C2W(stUserInfo.szNickName, wText);
		pText->SetText(wText);
		pText->SetControlVisible(bShow);
	}

	// 显示金币
	GETCTRL(IText, pText, m_pUI, CTN_TEXT_Money_0 + byViewStation);
	if (pText != NULL)
	{
		// 游戏过程中中途被扣的金币在m_nTempMoney数组中表现出来
		swprintf(wText, MAX_PATH, L"%d", stUserInfo.i64Money + m_nTempMoney[byChair]);
		pText->SetText(wText);
		pText->SetControlVisible(bShow);
	}

	// 显示IP
	//GETCTRL(IText, pText, m_pUI, CTN_TEXT_IP_0 + byViewStation);
	//if (pText != NULL)
	//{
	//	C2W("深圳市", wText);
	//	pText->SetText(wText);
	//	pText->SetControlVisible(bShow);
	//}

	// 显示玩家自己的信息
	if (byChair == m_byMyStation)
	{
		// 自己的金币底图
		SAFE_CTRL_OPERATE(IImage, CTN_Self_Money_BG, SetControlVisible(true));

		// 显示用户ID
		GETCTRL(IText, pText, m_pUI,CTN_TEXT_UserID);
		if (pText != NULL)
		{
			swprintf(wText, MAX_PATH, L"%d", stUserInfo.dwUserID);
			pText->SetText(wText);
			pText->SetControlVisible(bShow);
		}
		
		// 自己的等级底图
		//SAFE_CTRL_OPERATE(IImage, CTN_Self_Level_BG, SetControlVisible(true));

		// 自己的勋章底图
		//SAFE_CTRL_OPERATE(IImage, CTN_Self_Medal_BG, SetControlVisible(true));
	}
	else
	{
		// 显示玩家头像(自己的不需要显示)
		CString strUserPath = "GameGrand\\LogoAnimal\\" ; 
		CString strDir = (byViewStation > 1) ? "left" : "right";
		CString strSex = (stUserInfo.bBoy) ? "boy" : "girl";
		CString strName;
		strName.Format("%s_%d_%s.bani",strSex, stUserInfo.bLogoID % 4, strDir) ; 
		strUserPath += strName;	
		pExAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(CTN_ExAnimate_Logo_0 + byViewStation));
		if(pExAnimate != NULL)
		{
			pExAnimate->SetExtFilePath(CA2W(strUserPath)) ; 
			pExAnimate->LoadFromFile();
			pExAnimate->SetIsLoop(true) ;
			pExAnimate->SetIsSymmetrical(false);
			pExAnimate->SetControlVisible(bShow);
			pExAnimate->SetPlayState(bShow);
		}
	}
}

/** 
 * @brief 绘制准备
 * @param[in] byChair 玩家位置
 * @param[in] bShow   是否显示
 */
void CMJFrameLogic::DrawReady(BYTE byChair, bool bShow)
{
	SAFE_CTRL_OPERATE(IImage, CTN_IMG_Ready_0 + m_pGameFrame->DeskStation2View(byChair), SetControlVisible(bShow));

	if (byChair == m_byMyStation)
	{
		IExAnimate * pExAnimate = NULL;
		GETCTRL(IExAnimate, pExAnimate, m_pUI, ExAnimate_Ready_Wait);
		if (pExAnimate != NULL)
		{
			pExAnimate->SetControlVisible(bShow);
			pExAnimate->SetIsLoop(true);
			pExAnimate->SetPlayState(bShow);
		}
	}
}

/** 
 * @brief 绘制骰子
 * @param[in] byChair 玩家位置
 * @param[in] byDice  骰子
 * @param[in] bShow   是否显示
 */
void CMJFrameLogic::DrawDice(BYTE byChair, BYTE byDice[], bool bShow)
{
	KillAllTimer();

	IExAnimate * pExAnimate = NULL;

	// 先隐藏骰子
	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		GETCTRL(IExAnimate, pExAnimate, m_pUI, ExAnimate_0_Saizi_1 + i);
		if (pExAnimate != NULL)
		{
			pExAnimate->SetControlVisible(false);
			pExAnimate->SetIsLoop(false);
			pExAnimate->SetPlayState(false);
		}

		GETCTRL(IExAnimate, pExAnimate, m_pUI, ExAnimate_0_Saizi_2 + i);
		if (pExAnimate != NULL)
		{
			pExAnimate->SetControlVisible(false);
			pExAnimate->SetIsLoop(false);
			pExAnimate->SetPlayState(false);
		}
	}

	m_byDiceFinishCount = 0;
	BYTE bViewStation = m_pGameFrame->DeskStation2View(byChair);
	
	memcpy(m_byCurDice, byDice, sizeof(m_byCurDice));

	GETCTRL(IExAnimate, pExAnimate, m_pUI, ExAnimate_0_Saizi_1 + bViewStation);
	if (pExAnimate != NULL)
	{
		pExAnimate->SetControlVisible(bShow);
		pExAnimate->SetIsLoop(false);
		pExAnimate->SetPlayState(bShow);
	}

	GETCTRL(IExAnimate, pExAnimate, m_pUI, ExAnimate_0_Saizi_2 + bViewStation);
	if (pExAnimate != NULL)
	{
		pExAnimate->SetControlVisible(bShow);
		pExAnimate->SetIsLoop(false);
		pExAnimate->SetPlayState(bShow);
	}

	if (bShow)
	{
		PlaySound(SOUND_SEZI);
	}
}

/** 
 * @brief 绘制托管
 * @param[in] byChair 玩家位置
 * @param[in] bShow   是否显示
 */
void CMJFrameLogic::DrawTuoGuan(BYTE byChair, bool bShow)
{
	SAFE_CTRL_OPERATE(IImage, CTN_IMG_TuoGuan_0 + m_pGameFrame->DeskStation2View(byChair), SetControlVisible(bShow));
}

/** 
 * @brief 绘制庄家
 * @param[in] byChair 玩家位置
 * @param[in] bShow   是否显示
 */
void CMJFrameLogic::DrawDealer(BYTE byChair, bool bShow)
{
	SAFE_CTRL_OPERATE(IImage, CTN_IMG_NT_0 + m_pGameFrame->DeskStation2View(byChair), SetControlVisible(bShow));
}

/** 
 * @brief 绘制拦牌框
 * @param[in] byFlag 拦牌标识: 吃(0x01), 碰(0x02) 杠(0x04), 听(0x08), 胡(0x10)
 */
void CMJFrameLogic::DrawBlock(BYTE byFlag)
{
	SAFE_CTRL_OPERATE(IContain, CTN_Block, SetControlVisible(byFlag > 0));
	SAFE_CTRL_OPERATE(IButton, CTN_Block_BTN_Cancel, SetEnable(true));

	for (BYTE i = 0; i < 5; i++)
	{
		SAFE_CTRL_OPERATE(IButton, CTN_Block_BTN_Chi + i, SetEnable(byFlag & (1 << i)));
	}
}

/** 
 * @brief 绘制手牌
 * @param[in] byChair         玩家位置
 * @param[in] mjHand          手牌数据
 * @param[in] bIsLaydown      控件是否躺下(摊牌)
 * @param[in] bIsGiveTileMode 最右边一张牌是否分开
 */
void CMJFrameLogic::DrawHand(BYTE byChair, CMJHand mjHand, bool bIsGiveTileMode, bool bIsLaydown)
{
	IMjHandCtrl * pMJCtrl = NULL;
	GETCTRL(IMjHandCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_HandCtrl_0 + m_pGameFrame->DeskStation2View(byChair));
	if (pMJCtrl != NULL)
	{
		int nTiles[20];
		int nTilesCount = mjHand.GetAllTile(nTiles);
		pMJCtrl->SetHandInfo(nTiles, nTilesCount);
		pMJCtrl->SetGiveTileMode(bIsGiveTileMode);
		pMJCtrl->SetIsLaydown(bIsLaydown);
		pMJCtrl->SetControlVisible(nTilesCount > 0);
	}
}

/** 
 * @brief 绘制出牌
 * @param[in] byChair 玩家位置
 * @param[in] mjGive  出牌数据
 */
void CMJFrameLogic::DrawGive(BYTE byChair, CMJGive mjGive)
{
	IMjGiveCtrl * pMJCtrl = NULL;
	GETCTRL(IMjGiveCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_GiveCtrl_0 + m_pGameFrame->DeskStation2View(byChair));
	if (pMJCtrl != NULL)
	{
		int nTiles[50];
		mjGive.GetAllTile(nTiles);
		pMJCtrl->SetGiveInfo(nTiles, mjGive.CurrentLength());
		pMJCtrl->SetControlVisible(mjGive.CurrentLength() > 0);
	}
}

/** 
 * @brief 绘制拦牌
 * @param[in] byChair 玩家位置
 * @param[in] mjSet   拦牌数据
 */
void CMJFrameLogic::DrawSet(BYTE byChair, CMJSet mjSet)
{
	IMjHandCtrl * pMJCtrl = NULL;
	GETCTRL(IMjHandCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_HandCtrl_0 + m_pGameFrame->DeskStation2View(byChair));
	if (pMJCtrl != NULL)
	{
		pMJCtrl->ClearAllBlock();

		TILE_SET tileSet;
		for (BYTE i = 0; i < mjSet.CurrentLength(); i++)
		{
			mjSet.GetSet(i, tileSet);
			switch (tileSet.m_ucFlag)
			{
			case ACTION_COLLECT:               // 吃
				{
					pMJCtrl->AddOneBlock(LT_COLLECT, tileSet.m_ucTile, tileSet.m_ucChair);
				}
				break;
			case ACTION_TRIPLET:               // 碰
				{
					pMJCtrl->AddOneBlock(LT_TRIPLET, tileSet.m_ucTile, tileSet.m_ucChair);
				}
				break;
			case ACTION_QUADRUPLET_PATCH:      // 补杠
			case ACTION_QUADRUPLET_REVEALED:   // 明杠
				{
					pMJCtrl->AddOneBlock(LT_QUADRUPLET_REVEALED, tileSet.m_ucTile, tileSet.m_ucChair);
				}
				break;
			case ACTION_QUADRUPLET_CONCEALED:  // 暗杠
				{
					tileSet.m_ucTile[0] = TILE_BEGIN;
					tileSet.m_ucTile[1] = TILE_BEGIN;
					tileSet.m_ucTile[2] = TILE_BEGIN;
					pMJCtrl->AddOneBlock(LT_QUADRUPLET_CONCEALED, tileSet.m_ucTile, tileSet.m_ucChair);
				}
				break;
			default:
				break;
			}
		}
	}
}

/** 
 * @brief 绘制牌墙
 * @param[in] bShow   是否显示
 */
void CMJFrameLogic::DrawWall(bool bShow)
{
	IMjWallCtrl * pMJCtrl = NULL;
	int i, j, byViewStation;
	int pMjStatus[MJ_WALL_NUMBER] = {0};
	TILE tile;
	for (i = 0; i < PLAYER_NBR; i++)
	{
		byViewStation = m_pGameFrame->DeskStation2View(i);
		GETCTRL(IMjWallCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_WallCtrl_0 + byViewStation);
		if (pMJCtrl != NULL)
		{
			// 记录麻将的状况
			memset(&pMjStatus, 0, sizeof(pMjStatus));
			CString str;
			for (j = 0; j < MJ_WALL_NUMBER; j ++) 
			{
				tile = m_TilesWall.GetTile(byViewStation * MJ_WALL_NUMBER + j);

				if (tile == 255) 
				{		
					// 不可见
					pMJCtrl->SetWallTile(-1, j);
					pMjStatus[j] = -1;
				}
				else
				{																						
					// 可见
					pMJCtrl->SetWallTile(tile, j);
				}
			}

			// 杠牌(花)补牌时, 按照麻将序号会导致麻将先抓下面一张，这样就导致上面一张浮在空中，需要把浮起来的那张牌放到下面一排位置
			for (j = 0; j < MJ_WALL_NUMBER; j += 2) 
			{
				if (pMjStatus[j] == 0 && pMjStatus[j + 1] == -1) 
				{
					tile = m_TilesWall.GetTile(byViewStation * MJ_WALL_NUMBER + j);
					pMJCtrl->SetWallTile(-1, j);
					pMJCtrl->SetWallTile(tile, j + 1);
				}
			}
		}
	}
}

/** 
 * @brief 绘制小手
 * @param[in] nOffsetPos 小手指补第几墩的牌
 * @param[in] bShow      是否显示
 */
void CMJFrameLogic::DrawSmallHand(int nOffsetPos, bool bShow)
{
	int nIndex = 0;
	int nPos = m_TilesWall.GetEndPos(nOffsetPos);
	BYTE byViewStation = 0;
	if (nPos >= 0 && nPos < 34)
	{
		byViewStation = 0;
	}
	else if (nPos >= 34 && nPos < 68)
	{
		byViewStation = 1;
	}
	else if (nPos >= 68 && nPos < 102)
	{
		byViewStation = 2;
	}
	else if (nPos >= 102 && nPos < 136)
	{
		byViewStation = 3;
	}

	IMjWallCtrl * pMJCtrl = NULL;
	GETCTRL(IMjWallCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_WallCtrl_0 + byViewStation);
	if (pMJCtrl == NULL)
	{
		return;
	}

	// 确定当前位置在哪个控件第几墩
	nIndex = nPos - byViewStation * MJ_WALL_NUMBER;

	// 小手指是根据牌墙控件下面一排的麻将坐标偏移的, 所以如果是上面一排的坐标则需要+1转换成下面一排的坐标
	RECT rtLast;
	if (nIndex % 2 == 0)
	{
		rtLast = pMJCtrl->GetTileRectByIndex(MJ_WALL_NUMBER - (nIndex + 1));
	}
	else
	{
		rtLast = pMJCtrl->GetTileRectByIndex(MJ_WALL_NUMBER - nIndex);
	}

	// 小手指针对每个方位的牌墙控件偏移位置
	const POINT ptOffset[PLAY_COUNT] = { {-16, -65}, {-8, -62}, {-10, -55}, {-8, -60} };

	IImage * pImage = NULL;
	GETCTRL(IImage, pImage, m_pUI, CTN_IMG_Hand);
	if (pImage != NULL)
	{
		pImage->SetGameXY(rtLast.left + ptOffset[byViewStation].x, rtLast.top + ptOffset[byViewStation].y);
		pImage->SetControlVisible(bShow);
	}
}

/** 
 * @brief 绘制出牌标记
 * @param[in] byChair 玩家位置
 * @param[in] bShow   是否显示
 */
void CMJFrameLogic::DrawGiveAnimate(BYTE byChair, bool bShow)
{
	IExAnimate *pExAnimate = NULL;
	GETCTRL(IExAnimate, pExAnimate, m_pUI, CTN_ExAnimate_GiveTag);

	if (pExAnimate == NULL)
	{
		return;
	}

	BYTE byViewStation = m_pGameFrame->DeskStation2View(byChair);

	IMjGiveCtrl * pMJCtrl = NULL;
	GETCTRL(IMjGiveCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_GiveCtrl_0 + byViewStation);
	if (pMJCtrl == NULL)
	{
		return;
	}

	const POINT ptOffset[PLAY_COUNT] = 
	{
		{-5, -25},
		{2, -30},
		{-5, -25},
		{2, -30}
	};

	RECT rtLast;
	rtLast = pMJCtrl->GetTileRectByIndex(pMJCtrl->GetTileCount() - 1);   
	pExAnimate->SetGameXY(rtLast.left + ptOffset[byViewStation].x, rtLast.top + ptOffset[byViewStation].y);
	pExAnimate->SetIsLoop(bShow);
	pExAnimate->SetPlayState(bShow);
	pExAnimate->SetControlVisible(bShow);
}

/** 
 * @brief  
 * @param[in] pCmd    结算数据
 * @param[in] bShow   是否显示
 */
void CMJFrameLogic::DrawFinish(TCMD_NOTIFY_FINISH * pCmd, bool bShow)
{
	if (pCmd == NULL)
	{
		SAFE_CTRL_OPERATE(IContain, CTN_JieSuanKuang, SetControlVisible(false));
		return ;
	}

	int i, j, nCount = 0;
	wchar_t wText[MAX_PATH];
	BYTE byWinStation = -1;
	BYTE byViewStation = -1;
	IExAnimate * pExAnimate = NULL;
	IMjHandCtrl * pHandCtrl = NULL;
	IContain * pContain = NULL;
	IText  * pText  = NULL;
	IImage * pImage = NULL;
	INum   * pNum   = NULL;

	SAFE_CTRL_OPERATE(IContain, CTN_JieSuanKuang, SetControlVisible(bShow));

	// 显示自摸或点炮
	if (pCmd->byFlag == MJ_END_SELF)
	{
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_ZiMo, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_DianPao, SetControlVisible(false));

		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_LiuJu, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_Line1, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_Line2, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_pxhj, SetControlVisible(true));
	}
	else if (pCmd->byFlag == MJ_END_GUN || pCmd->byFlag == MJ_END_QIANG)
	{
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_ZiMo, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_DianPao, SetControlVisible(true));

		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_LiuJu, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_Line1, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_Line2, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_pxhj, SetControlVisible(true));
	}
	else
	{
		// 流局
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_ZiMo, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_DianPao, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_LiuJu, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_Line1, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_Line2, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_pxhj, SetControlVisible(false));
	}

	// 播放胜利或失败音效
	PlaySound(pCmd->nMoney[m_byMyStation] >= 0 ? SOUND_WIN : SOUND_LOST);

	for (i = 0; i < PLAYER_NBR; i++)
	{
		if (pCmd->byWhoWin[i])
		{
			byWinStation = i;
			break;
		}
	}

	if (byWinStation != 255)
	{
		// 显示胡牌玩家昵称
		SAFE_CTRL_OPERATE(IText, CTN_JSK_TEXT_Win_NickName, SetText(m_wNickName[byWinStation]));
	}
	else
	{
		SAFE_CTRL_OPERATE(IText, CTN_JSK_TEXT_Win_NickName, SetText(L""));
	}

	// 显示胡牌牌数据
	GETCTRL(IMjHandCtrl, pHandCtrl, m_pUI, CTN_JSK_MJHandCtrl);
	if (pHandCtrl != NULL)
	{
		if (byWinStation != 255)
		{
			int nTiles[20];
			int nTilesCount = m_TilesHand[byWinStation].GetAllTile(nTiles);
			pHandCtrl->SetHandInfo(nTiles, nTilesCount);
			pHandCtrl->SetGiveTileMode(true);
			pHandCtrl->SetIsUseMouse(false);
			pHandCtrl->SetControlVisible(true);

			// 设置精牌
			m_TilesGod.GetAllTile(nTiles);
			pHandCtrl->SetGodTile(nTiles, m_TilesGod.CurrentLength());

			pHandCtrl->ClearAllBlock();

			TILE_SET tileSet;
			for (BYTE i = 0; i < m_TilesSet[byWinStation].CurrentLength(); i++)
			{
				m_TilesSet[byWinStation].GetSet(i, tileSet);
				switch (tileSet.m_ucFlag)
				{
				case ACTION_COLLECT:               // 吃
					{
						pHandCtrl->AddOneBlock(LT_COLLECT, tileSet.m_ucTile, tileSet.m_ucChair);
					}
					break;
				case ACTION_TRIPLET:               // 碰
					{
						pHandCtrl->AddOneBlock(LT_TRIPLET, tileSet.m_ucTile, tileSet.m_ucChair);
					}
					break;
				case ACTION_QUADRUPLET_PATCH:      // 补杠
				case ACTION_QUADRUPLET_REVEALED:   // 明杠
					{
						pHandCtrl->AddOneBlock(LT_QUADRUPLET_REVEALED, tileSet.m_ucTile, tileSet.m_ucChair);
					}
					break;
				case ACTION_QUADRUPLET_CONCEALED:  // 暗杠
					{
						tileSet.m_ucTile[0] = TILE_BEGIN;
						tileSet.m_ucTile[1] = TILE_BEGIN;
						tileSet.m_ucTile[2] = TILE_BEGIN;
						pHandCtrl->AddOneBlock(LT_QUADRUPLET_CONCEALED, tileSet.m_ucTile, tileSet.m_ucChair);
					}
					break;
				default:
					break;
				}
			}
		}
		else
		{
			pHandCtrl->SetHandInfo(NULL, 0);
			pHandCtrl->SetControlVisible(false);
		}
	}

	// 显示各个玩家的相关结算信息
	for (i = 0; i < PLAYER_NBR; i++)
	{
		byViewStation = m_pGameFrame->DeskStation2View(i);

		// 显示昵称
		SAFE_CTRL_OPERATE(IText, CTN_JSK_TEXT_NickName_0 + byViewStation, SetText(m_wNickName[i]));
		
		// 显示胡分
		swprintf(wText, MAX_PATH, L"%d", pCmd->nFanPoint[i]);

		SAFE_CTRL_OPERATE(IText, CTN_JSK_TEXT_HuFen_0 + byViewStation, SetText(wText));

		// 杠分
		swprintf(wText, MAX_PATH, L"%d", pCmd->nGangFen[i]);
		SAFE_CTRL_OPERATE(IText, CTN_JSK_TEXT_SJGangFen_0 + byViewStation, SetText(wText));

		// 显示税收
		swprintf(wText, MAX_PATH, L"%d", pCmd->nTax[i]);
		SAFE_CTRL_OPERATE(IText, CTN_JSK_TEXT_Tax_0 + byViewStation, SetText(wText));

		// 显示胡总分
		GETCTRL(INum, pNum, m_pUI, CTN_JSK_NUM_AllFen_0 + byViewStation);
		if (pNum != NULL)
		{
			// 根据分数加载正负号图片
			if (pCmd->nMoney[i] >= 0)
			{
				pNum->SetImagePath(L"GameGrand\\ying_num1.png");
			}
			else
			{
				pNum->SetImagePath(L"GameGrand\\shu_num.png");
			}
			pNum->SetHaveSymbol(true);
			pNum->SetNum(pCmd->nMoney[i]);
		}

		// 显示庄
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_NT_0 + byViewStation, SetControlVisible(i == m_byDealer));

		// 显示胡
		SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_HU_0 + byViewStation, SetControlVisible(pCmd->byWhoWin[i]));

		// 显示炮
		if (pCmd->byFlag == MJ_END_GUN || pCmd->byFlag == MJ_END_QIANG)
		{
			SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_PAO_0 + byViewStation, SetControlVisible(pCmd->byWhoGun == i));
		}
		else
		{
			SAFE_CTRL_OPERATE(IImage, CTN_JSK_IMG_PAO_0 + byViewStation, SetControlVisible(false));
		}
	}

	nCount = 0;

	// 显示番型
	if (byWinStation != 255)
	{
		nCount = m_tagSetting.nBaseScore;

		for (i = 0, j = 0; i < FAN_XING_COUNT; i++)
		{
			if (pCmd->byFan[byWinStation][i])
			{
				SAFE_CTRL_OPERATE(IContain, CTN_JSK_CTN_FanXing + j * 10, SetControlVisible(true));

				// 显示胡牌玩家昵称
				GETCTRL(IText, pText, m_pUI, CTN_JSK_CTN_FanXing_Name + j * 10);
				if (pText != NULL)
				{
					pText->SetText(g_wFanXingName[i]);
					pText->SetControlVisible(true);
				}
				
				// 显示翻型数值
				GETCTRL(INum, pNum, m_pUI, CTN_JSK_CTN_FanXing_Num + j * 10);
				if (pNum != NULL)
				{
					pNum->SetHaveSymbol(true);
					pNum->SetNum(pCmd->byFanScore[byWinStation][i]);
					pNum->SetControlVisible(true);
				}

				nCount *= pCmd->byFanScore[byWinStation][i];
				j++;
			}
		}

		// 隐藏不需要的翻型控件
		for (i = j; i < FAN_XING_COUNT; i++)
		{
			SAFE_CTRL_OPERATE(IContain, CTN_JSK_CTN_FanXing + i * 10, SetControlVisible(false));
		}
	}

	// 显示番型总分
	GETCTRL(INum, pNum, m_pUI, CTN_JSK_NUM_AllFen);
	if (pNum != NULL)
	{
		pNum->SetHaveSymbol(false);
		pNum->SetNum(nCount);
		pNum->SetControlVisible(nCount > 0);
	}
}
//////////////////////////////////////////////////////////////////////////////
/** 
 * @brief 设置定时器
 * @param[in] byChair 玩家位置
 * @param[in] uTime   时间秒数
 */
void CMJFrameLogic::SetGameTimer(BYTE byChair, UINT uTime)
{
	SAFE_CTRL_OPERATE(IContain, CTN_Clock, SetControlVisible(uTime > 0));

	if (uTime > 0)
	{
		ITime * pTime = NULL;
		GETCTRL(ITime, pTime, m_pUI, CTN_Clock_Time);
		if (pTime != NULL)
		{
			pTime->SetControlVisible(true);
			pTime->SetShowMaxNum(uTime);
			pTime->SetShowMinNum(0);
			pTime->SetPlaySoundNum(5);
			pTime->SetPLay(true);
		}

		// 设置箭头显示方向
		for (BYTE i = 0; i < PLAYER_NBR; i++)
		{
			SAFE_CTRL_OPERATE(IImage, CTN_Clock_IMG_Arrows_0 + m_pGameFrame->DeskStation2View(i), SetControlVisible(i == byChair));
		}
	}
}

/** 
 * @brief 停止所有定时器
 */
void CMJFrameLogic::KillAllTimer()
{
	SAFE_CTRL_OPERATE(IContain, CTN_Clock, SetControlVisible(false));

	ITime * pTime = NULL;
	GETCTRL(ITime, pTime, m_pUI, CTN_Clock_Time);
	if (pTime != NULL)
	{
		pTime->SetControlVisible(false);
		pTime->SetShowMaxNum(0);
		pTime->SetShowMinNum(0);
		pTime->SetPLay(false);
	}
}

/** 
 * @brief 播放声音
 * @param[in] nSoundID 播放类型ID
 * @param[in] byChair  玩家位置
 */
void CMJFrameLogic::PlaySound(int nSoundID, BYTE byChair)
{
	if (!m_bSound || m_pSoundEngine == NULL)
	{
		return;
	}

	char chSoundName[MAX_PATH] = {0};
	char chSoundPath[MAX_PATH] = {0};
	char szPlay[MAX_PATH] = "";

	if (byChair >= 0 && byChair < PLAYER_NBR)
	{
		// 男女音效
		m_pGameFrame->GetUserInfo(byChair, m_tmpUserInfo);
		sprintf(chSoundPath, "music/putong/%s", m_tmpUserInfo.bBoy ? "Man" : "Wom");
	}
	else
	{
		// 公用音效
		sprintf(chSoundPath, "music");
	}

	if (m_SoundMap[nSoundID].nCount > 0)
	{
		sprintf(chSoundName, "%s", m_SoundMap[nSoundID].szFileName[(m_SoundMap[nSoundID].nCount - 1) % MAX_SOUND_RAND]);
		sprintf(szPlay, "%s/%s", chSoundPath, chSoundName);

		m_pSoundEngine->play2D(szPlay, false);
	}
}

/** 
 * @brief 播放声音
 * @param[in] pSoundName 声音名称
 */
void CMJFrameLogic::PlaySound(char * pSoundName)
{
	if (pSoundName == NULL)
	{
		return ;
	}

	if (!m_bSound || m_pSoundEngine == NULL)
	{
		return;
	}

	char szPlay[MAX_PATH] = "";

	// 公用音效
	sprintf(szPlay, "music/%s", pSoundName);

	m_pSoundEngine->play2D(szPlay, false);
}

/** 
 * @brief 播放背景音乐
 * @param[in] bPlay 播放开关(true: 播放,  false: 停止)
 */
void CMJFrameLogic::PlayBackSound(bool bPlay)
{
	if (!m_bSound)
	{		
		return ;
	}

	if (m_pSoundEngine == NULL)
	{		
		return;
	}

	char szFileName[MAX_PATH];
	srand((unsigned)time(NULL));
	int iSoundType = rand() % 3 + 1;
	sprintf(szFileName, "music\\bg%d.ogg", iSoundType);

	if (bPlay)
	{
		m_pSoundEngine->play2D(szFileName, true);
	}
	else
	{
		m_pSoundEngine->stopAllSounds();
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  @brief 房间基础信息
 */	
void CMJFrameLogic::OnNotifyBaseInfo(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_MJ_BASE_INFO * pCmd = (TCMD_NOTIFY_MJ_BASE_INFO*)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	m_tagSetting.nOpenGM      = pCmd->nOpenGM;                 // GM命令开关
	m_tagSetting.nTimerGive   = pCmd->ucTimerGive;			   // 出牌时间
	m_tagSetting.nTimerFlower = pCmd->ucTimerFlower;		   // 补花时间
	m_tagSetting.nTimerBlock  = pCmd->ucTimerBlock;		       // 拦牌时间
	m_tagSetting.nTimerReady  = pCmd->ucTimerReady;		       // 准备倒计时

	m_tagSetting.nTimerCutCard= pCmd->ucTimerCutCard;          // 切牌操作时间
	m_tagSetting.nTimerDice   = pCmd->ucTimerDice;             // 骰子操作时间
	m_tagSetting.nTimerDeal   = pCmd->ucTimerDeal;             // 发牌操作时间
	m_tagSetting.nTimerJP     = pCmd->ucTimerJP;               // 精牌操作时间

	m_tagSetting.nTilesHandNbr = pCmd->ucTilesHandNbr;         // 手牌张数
	m_tagSetting.nTilesNumber = pCmd->ucTilesNumber;           // 牌总张数

	m_tagSetting.nGangScore = pCmd->ucGangScore;               // 杠牌的底分
	m_tagSetting.nMinFan    = pCmd->ucMinFan;                  // 最小番
	m_tagSetting.nBaseScore = pCmd->ucBaseScore;               // 游戏底分
	m_tagSetting.nMoneyType = pCmd->ucMoneyType;               // 扣费类型
}

/**
 *  @brief 接受到进入、重回、数据恢复消息
 */	
void CMJFrameLogic::OnNotifyGameInfo(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_VIEW_INFO * pCmd = (TCMD_NOTIFY_VIEW_INFO *)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	m_byMyStation = m_pGameFrame->GetMyDeskStation();

	m_byGameState = pCmd->ucGameState;

	if (m_byGameState == MJ_STATE_NOSTART || m_byGameState == MJ_STATE_END)
	{
		// 显示准备信息
		for (BYTE i = 0; i < PLAYER_NBR; i++)
		{
			m_bReady[i] = pCmd->ucReady[i];
			DrawReady(i, pCmd->ucReady[i]);
		}

		// 准备按钮
		SAFE_CTRL_OPERATE(IButton, CTN_BTN_Start, SetControlVisible(pCmd->ucReady[m_byMyStation] == 0));

		if (!pCmd->ucReady[m_byMyStation])
		{
			SetGameTimer(m_byMyStation, m_tagSetting.nTimerReady);
		}

		return ;
	}

	int i, j;
	
	// 当前操作玩家
	m_byTurnStation = pCmd->ucCurTurn;

	// 庄家
	m_byDealer = pCmd->ucDealer;
	DrawDealer(m_byDealer, true);

	for (i = 0; i < DICE_NBR; i++)
	{
		// 骰子
		m_byDice[i] = pCmd->ucDice[i];
	}

	// 托管标志还原
	for (i = 0; i < PLAYER_NBR; i++)
	{
		m_bTuoGuan[i] = pCmd->ucTrusteeFlag[i];
		DrawTuoGuan(i, m_bTuoGuan[i]);
	}

	// 万能牌还原
	m_TilesGod.ReleaseAll();
	for (i = 0; i < pCmd->ucGodTilesLength; i++)
	{
		m_TilesGod.AddTile(pCmd->ucGodTiles[i]);
	}

	// 手牌数据还原
	for (i = 0; i < PLAYER_NBR; i++)
	{
		m_TilesHand[i].ReleaseAll();

		if (i == m_byMyStation)
		{
			for (j = 0; j < pCmd->ucHandTilesLength[i]; j++)
			{
				m_TilesHand[i].AddTile(pCmd->ucHandTiles[j]);
			}

			m_TilesHand[i].Sort();
		}
		else
		{
			m_TilesHand[i].CurrentLength(pCmd->ucHandTilesLength[i]);
		}
		
		// 绘制手牌
		DrawHand(i, m_TilesHand[i], m_TilesHand[i].CurrentLength() % 3 == 2);
	}

	// 出牌数据还原
	for (i = 0; i < PLAYER_NBR; i++)
	{
		m_TilesGive[i].ReleaseAll();

		for (j = 0; j < pCmd->ucGiveTilesLength[i]; j++)
		{
			m_TilesGive[i].AddTile(pCmd->ucGiveTiles[i][j]);
		}

		// 绘制出牌
		DrawGive(i, m_TilesGive[i]);
	}

	// 拦牌数据还原
	TILE_SET tileSet;
	for (i = 0; i < PLAYER_NBR; i++)
	{
		m_TilesSet[i].ReleaseAll();

		for (j = 0; j < pCmd->ucSetTilesLength[i]; j++)
		{
			tileSet.m_ucFlag = pCmd->ucBlockTilesFlag[i][j];
			tileSet.m_ucChair = pCmd->ucBlockTilesWhich[i][j];
			::CopyMemory(&tileSet.m_ucTile, &pCmd->ucBlockTiles[i][j * 4], sizeof(tileSet.m_ucTile));
			m_TilesSet[i].AddSet(tileSet);
		}

		// 绘制拦牌
		DrawSet(i, m_TilesSet[i]);
	}

	// 牌墙还原
	m_TilesWall.ReleaseAll();
	m_TilesWall.CurrentLength(m_tagSetting.nTilesNumber);
	BYTE byStation = m_pGameFrame->DeskStation2View(m_byDealer);   // 抓牌方向
	int nPos = (m_byDice[0] + m_byDice[1]) * 2;                    // 从第几墩开始抓
	int nStartPos, nCount = 0;

	for (i = 0; i < PLAYER_NBR; i++)
	{
		for (j = 0; j < m_tagSetting.nTilesHandNbr; j++)
		{
			nStartPos = byStation * MJ_WALL_NUMBER + nPos + nCount;
			if (nStartPos >= m_tagSetting.nTilesNumber)
			{
				nStartPos = nStartPos - m_tagSetting.nTilesNumber; 
			}

			m_TilesWall.SetTile(nStartPos,  -1);

			nCount++;
		}

		if (i == m_byDealer)
		{
			nStartPos = byStation * MJ_WALL_NUMBER + nPos + nCount;
			if (nStartPos >= m_tagSetting.nTilesNumber)
			{
				nStartPos = nStartPos - m_tagSetting.nTilesNumber; 
			}

			// 庄家多抓一张
			m_TilesWall.SetTile(nStartPos,  -1);
			nCount++;
		}
	}

	// 设置牌墙的抓牌与补牌位置
	m_TilesWall.SetPos(byStation * MJ_WALL_NUMBER + nPos + nCount, byStation * MJ_WALL_NUMBER + nPos - 1);

	
	//  判断状态, 发牌状态时精牌还未表现, 非发牌状态时需要设置精牌在牌墙中的位置
	if (pCmd->ucGameState != MJ_STATE_DEAL)
	{
		////////////////////////////////////////////////////////////////////////////////////////
		// 正常摸牌数(不包括杠牌)
		for (i = 0; i < pCmd->ucDarwTileLength; i++)
		{
			m_TilesWall.PopTile();
		}

		// 正常杠牌数
		for (i = 0; i < pCmd->ucGangTileLength; i++)
		{
			m_TilesWall.PopFrontTile();
		}

		//////////////////////////////////////////////////////////////////////////////////////////
		// 设置手牌与出牌控件精牌
		int nGodTiles[2];
		m_TilesGod.GetAllTile(nGodTiles);

		IMjHandCtrl * pHandCtrl = NULL;
		IMjGiveCtrl * pGiveCtrl = NULL;
		for (BYTE i = 0; i < PLAY_COUNT; i++)
		{
			GETCTRL(IMjHandCtrl, pHandCtrl, m_pUI, CTN_MJCtrl_HandCtrl_0 + i);
			if (pHandCtrl != NULL)
			{
				pHandCtrl->SetGodTile(nGodTiles, m_TilesGod.CurrentLength());
			}

			GETCTRL(IMjHandCtrl, pHandCtrl, m_pUI, CTN_MJCtrl_GiveCtrl_0 + i);
			if (pGiveCtrl != NULL)
			{
				pGiveCtrl->SetGodTile(nGodTiles, m_TilesGod.CurrentLength());
			}
		}
	}
	
	// 绘制牌墙
	DrawWall(true);

	if (pCmd->ucGameState == MJ_STATE_DEAL)
	{
		// 发牌完成, 需要通知服务端
		TCMD_REQUEST_DEAL_FINISH cmd; 
		m_pGameFrame->SendGameData(0, (void*)&cmd, sizeof(cmd));
	}
	else
	{
		SetGameTimer(pCmd->ucCurTurn, pCmd->ucNowTime);
	}
}

/**
 *  @brief 处理设置连局信息(游戏开始发送的的第一条游戏消息)
 */	
void CMJFrameLogic::OnNotifyRunNbr(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_RUN_NBR * pCmd = (TCMD_NOTIFY_RUN_NBR *)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	m_byGameState = MJ_STATE_DEAL;

	// 保存玩家昵称
	memset(&m_wNickName, 0, sizeof(m_wNickName));
	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		C2W(m_pGameFrame->GetUserNickName(i), m_wNickName[i]);
	}
}

/**
 *  @brief 接受到通知庄家位置消息
 */	
void CMJFrameLogic::OnNotifyDealer(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_DEALER * pCmd = (TCMD_NOTIFY_DEALER *)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	m_byDealer = pCmd->byDealer;

	DrawDealer(m_byDealer, true);
}

/**
 *  @brief 接受到开局发牌消息后
 */	
void CMJFrameLogic::OnNotifyDeal(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_DEAL * pCmd = (TCMD_NOTIFY_DEAL*)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	CopyMemory(&m_stDealInfo, pCmd, sizeof(TCMD_NOTIFY_DEAL));

	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		DrawReady(i, false);
	}

	// 发牌状态
	m_byGameState = MJ_STATE_DEAL;

	CMJHand mjHand;
	for (BYTE i = 0; i < 17; i++)
	{
		if (pCmd->tiles[i] != TILE_BEGIN)
		{
			mjHand.AddTile(pCmd->tiles[i]);
		}
	}

	for (BYTE i = 0; i < PLAYER_NBR; i++)
	{
		m_TilesHand[i].ReleaseAll();
		if (i == m_byMyStation)
		{
			m_TilesHand[i].AddTiles(mjHand);
			m_TilesHand[i].Sort();
		}
		else
		{
			if (i == m_byDealer)
			{
				m_TilesHand[i].AddTile(TILE_BEGIN, m_tagSetting.nTilesHandNbr + 1);
			}
			else
			{
				m_TilesHand[i].AddTile(TILE_BEGIN, m_tagSetting.nTilesHandNbr);
			}
		}

		//DrawHand(i, m_TilesHand[i]);
	}

	m_byDealer = pCmd->ucDealer;

	// 骰子
	m_byDice[0] = pCmd->ucDice[0];
	m_byDice[1] = pCmd->ucDice[1];

	// 第1次打骰子
	m_enDiceState = En_Dice_1;

	// 显示庄家
	DrawDealer(m_byDealer, true);

	// 播放骰子动画
	DrawDice(pCmd->ucDealer, m_byDice, true);
}


/**
 *  @brief 处理补花结束消息
 */	
void CMJFrameLogic::OnNotifyFlowerOver(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_FLOWER_OVER * pCmd = (TCMD_NOTIFY_FLOWER_OVER *)buffer;
	if (buffer == NULL)
	{
		return ;
	}

	// 隐藏骰子
	DrawDice(m_byDealer, m_byDice, false);

	// 判断是否有拦牌操作
	if (pCmd->ucFlag == ACTION_EMPTY)
	{
		// 出牌状态
		m_byGameState = MJ_STATE_GIVE;

		m_byTurnStation = m_byDealer;

		// 设置出牌定时器
		SetGameTimer(m_byTurnStation, m_tagSetting.nTimerGive);

		if (m_byTurnStation == m_byMyStation && m_bTuoGuan[m_byMyStation])
		{
			// 自动处理
			OnRequestGive(TILE_BEGIN, true);
		}
	}
	else
	{
		OnNotifyBlock(buffer, nLen);
	}
}

/**
 *  @brief 接受到摸牌消息
 */	
void CMJFrameLogic::OnNotifyDraw(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_DRAW * pCmd = (TCMD_NOTIFY_DRAW *)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	m_byStateSelect = MJ_STATE_SELECT_NONE;

	if (pCmd->chair == m_byMyStation)
	{
		m_TilesHand[pCmd->chair].AddTile(pCmd->t);
	}
	else
	{
		m_TilesHand[pCmd->chair].AddTile(TILE_BEGIN);
	}

	// 牌墙减少
	if (pCmd->t_target == MJ_GANG_STATE_DRAW)
	{
		// 杠牌或补花补牌
		m_TilesWall.PopFrontTile();
	}
	else
	{
		// 正常摸牌
		m_TilesWall.PopTile();
	}
	
	// 绘制牌墙
	DrawWall(true);

	// 绘制手牌区域
	DrawHand(pCmd->chair, m_TilesHand[pCmd->chair], true);

	// 当前操作者
	m_byTurnStation = pCmd->chair;

	// 出牌状态
	m_byGameState = MJ_STATE_GIVE;

	// 判断是否有拦牌操作
	if (pCmd->ucFlag == ACTION_EMPTY)
	{
		// 设置出牌定时器
		SetGameTimer(m_byTurnStation, m_tagSetting.nTimerGive);

		if (m_byTurnStation == m_byMyStation && m_bTuoGuan[m_byMyStation])
		{
			// 自动处理
			OnRequestGive(TILE_BEGIN, true);
		}

		// 播放抓牌音效
		PlaySound(SOUND_ZHUAPAI);
	}
	else
	{
		OnNotifyBlock(buffer, nLen);
	}
}

/**
 *  @brief 接受到请求出牌结果消息
 */	
void CMJFrameLogic::OnNotifyGive(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_GIVE * pCmd = (TCMD_NOTIFY_GIVE *)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	// 隐藏吃碰杠面板
	SAFE_CTRL_OPERATE(IContain, CTN_Block, SetControlVisible(false));

	// 取消禁用的牌控件
	SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_MJCtrl_HandCtrl_2, SetInvalidTile(NULL, 0));

	// 隐藏拦牌提示信息
	SAFE_CTRL_OPERATE(IContain, CTN_Block_Panel, SetControlVisible(false));

	m_TilesGive[pCmd->chair].AddTile(pCmd->t);

	m_LastTile = pCmd->t;

	m_byGameState = MJ_STATE_WAIT;

	// 播放牌值音效
	PlaySound(SOUND_OUTCARDRAND[rand() % 3]);
	PlaySound(pCmd->t, pCmd->chair);

	// 绘制出牌区域
	DrawGive(pCmd->chair, m_TilesGive[pCmd->chair]);

	// 绘制出牌标记
	DrawGiveAnimate(pCmd->chair, true);

	if (pCmd->chair == m_byMyStation)
	{
		m_TilesHand[pCmd->chair].DelTile(pCmd->t);
		m_TilesHand[pCmd->chair].Sort();
	}
	else
	{
		m_TilesHand[pCmd->chair].DelTile(TILE_BEGIN);
	}

	// 绘制手牌区域
	DrawHand(pCmd->chair, m_TilesHand[pCmd->chair]);
}

/**
 *  @brief 接受到拦牌消息
 */	
void CMJFrameLogic::OnNotifyBlock(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_BLOCK * pCmd = (TCMD_NOTIFY_BLOCK *)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	if (pCmd->ucFlag == ACTION_EMPTY)
	{
		// 其他没有拦牌玩家定时器重置时间
		SetGameTimer(m_byTurnStation, m_tagSetting.nTimerBlock);
		m_byTurnStation = -1;
		return ;
	}

	// 拦牌状态
	m_byGameState = MJ_STATE_BLOCK;

	m_byTurnStation = pCmd->ucChair;

	// 清空拦牌数据
	m_stBlockInfo.ReleaseAll();

	m_stBlockInfo.byFlag = pCmd->ucFlag;

	TILE_SET tileSet;

	// 吃
	if (pCmd->ucFlag & 0x01)
	{ 
		tileSet.m_ucFlag = ACTION_COLLECT;

		for (int i = 0; i < 4; i++)
		{
			if (pCmd->ucTile[0][i * 3] != TILE_BEGIN)
			{
				memset(&tileSet.m_ucTile, 0, sizeof(tileSet.m_ucTile));
				CopyMemory(tileSet.m_ucTile, &pCmd->ucTile[0][i * 3], sizeof(BYTE) * 3);
				m_stBlockInfo.tilesSet.AddSet(tileSet);
			}
		}
	}

	// 碰
	if (pCmd->ucFlag & 0x02)
	{
		tileSet.m_ucFlag = ACTION_TRIPLET;
		CopyMemory(tileSet.m_ucTile, &pCmd->ucTile[1][0], sizeof(BYTE) * 3);
		m_stBlockInfo.tilesSet.AddSet(tileSet);
	}

	// 杠
	if (pCmd->ucFlag & 0x04)
	{
		tileSet.m_ucFlag = ACTION_QUADRUPLET_REVEALED;
		for (int i = 0; i < 3; i++)
		{
			if (pCmd->ucTile[2][i * 4] != 0)
			{
				memset(&tileSet.m_ucTile, 0, sizeof(tileSet.m_ucTile));
				CopyMemory(tileSet.m_ucTile, &pCmd->ucTile[2][i * 4], sizeof(BYTE) * 4);
				m_stBlockInfo.tilesSet.AddSet(tileSet);
			}
		}
	}

	// 听
	if (pCmd->ucFlag & 0x08)
	{
		for (int i = 0; i < 12; i++)
		{
			if (pCmd->ucTile[3][i] != TILE_BEGIN)
			{
				m_stBlockInfo.tingGive.AddTile(pCmd->ucTile[3][i]);

				for (int j = 0; j < 5; j++)
				{
					if (pCmd->ucTingTile[i][j] != TILE_BEGIN)
					{
						m_stBlockInfo.tingWin[i].AddTile(pCmd->ucTingTile[i][j]);
					}
				}
			}
		}
	}

	// 绘制拦牌框
	DrawBlock(pCmd->ucFlag);

	// 设置拦牌定时器
	SetGameTimer(pCmd->ucChair, m_tagSetting.nTimerBlock);

	if (m_byTurnStation == m_byMyStation && m_bTuoGuan[m_byMyStation])
	{
		// 自动处理
		OnRequestGive(TILE_BEGIN, true);
	}
	else
	{
		// 播放拦牌提示音效
		PlaySound(SOUND_BLOCK);
	}
}

/**
 *  @brief 接受到请求拦牌结果消息
 */	
void CMJFrameLogic::OnNotifyBlockResult(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_BLOCK_RESULT * pCmd = (TCMD_NOTIFY_BLOCK_RESULT *)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	SAFE_CTRL_OPERATE(IContain, CTN_Block, SetControlVisible(false));

	switch (pCmd->ucFlag)
	{
	case ACTION_COLLECT:  // 吃
		{
			PlaySound(SOUND_CHI, pCmd->ucChair);

			// 删除手牌
			if (pCmd->ucChair == m_byMyStation)
			{
				for (int i = 0; i < 3; i++)
				{
					if (pCmd->ucTile[i] != m_LastTile)
					{
						m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[i]);
					}
				}
			}
			else
			{
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
			}

			// 删除出牌区域的牌
			m_TilesGive[pCmd->ucWhich].CurrentLength(m_TilesGive[pCmd->ucWhich].CurrentLength() - 1);
			DrawGive(pCmd->ucWhich, m_TilesGive[pCmd->ucWhich]);

			// 隐藏出牌标记
			DrawGiveAnimate(pCmd->ucWhich, false);
		}
		break;
	case ACTION_TRIPLET:  // 碰
		{
			PlaySound(SOUND_PENG, pCmd->ucChair);

			// 删除手牌
			if (pCmd->ucChair == m_byMyStation)
			{
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[0]);
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[0]);
			}
			else
			{
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
			}

			// 删除出牌区域的牌
			m_TilesGive[pCmd->ucWhich].CurrentLength(m_TilesGive[pCmd->ucWhich].CurrentLength() - 1);
			DrawGive(pCmd->ucWhich, m_TilesGive[pCmd->ucWhich]);

			// 隐藏出牌标记
			DrawGiveAnimate(pCmd->ucWhich, false);
		}
		break;
	case ACTION_QUADRUPLET_CONCEALED: // 暗杠
	case ACTION_QUADRUPLET_DNXB:      // 东南西北杠
	case ACTION_QUADRUPLET_ZFB:       // 中发白X杠
		{
			PlaySound(SOUND_GANG, pCmd->ucChair);

			// 删除手牌
			if (pCmd->ucChair == m_byMyStation)
			{
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[0]);
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[1]);
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[2]);
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[3]);
			}
			else
			{
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
			}

			m_byGangCount++;
		}
		break;
	case ACTION_QUADRUPLET_REVEALED:  // 明杠
		{
			PlaySound(SOUND_GANG, pCmd->ucChair);

			// 删除手牌
			if (pCmd->ucChair == m_byMyStation)
			{
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[0]);
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[1]);
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[2]);
			}
			else
			{
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
			}

			// 删除出牌区域的牌
			m_TilesGive[pCmd->ucWhich].CurrentLength(m_TilesGive[pCmd->ucWhich].CurrentLength() - 1);
			DrawGive(pCmd->ucWhich, m_TilesGive[pCmd->ucWhich]);

			// 隐藏出牌标记
			DrawGiveAnimate(pCmd->ucWhich, false);

			m_byGangCount++;
		}
		break;
	case ACTION_QUADRUPLET_PATCH:     // 补杠
		{
			PlaySound(SOUND_GANG, pCmd->ucChair);

			// 删除手牌
			if (pCmd->ucChair == m_byMyStation)
			{
				m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile[0]);
			}
			else
			{
				m_TilesHand[pCmd->ucChair].DelTile(TILE_BEGIN);
			}

			TILE_SET tileSet;
			for (int i = 0; i < 5; i++)
			{
				m_TilesSet[pCmd->ucChair].GetSet(i, tileSet);
				if (tileSet.m_ucFlag == ACTION_TRIPLET && tileSet.m_ucTile[0] == pCmd->ucTile[0])
				{
					tileSet.m_ucFlag = ACTION_QUADRUPLET_PATCH;
					tileSet.m_ucTile[3] = pCmd->ucTile[0];
					m_TilesSet[pCmd->ucChair].SetSet(i, tileSet);
				}
			}

			m_byGangCount++;
		}
		break;
	case ACTION_WIN:  // 胡
		{
			return ;
		}
		break;
	default:
		break;
	}

	// 补杠是由碰能升级而成, 前面已经处理, 下面增加非补杠拦牌
	if (pCmd->ucFlag != ACTION_QUADRUPLET_PATCH)
	{
		m_TilesSet[pCmd->ucChair].AddSet(pCmd->ucTile, pCmd->ucFlag, pCmd->ucWhich);
	}

	// 播放动作音效
	PlaySound(SOUND_BLOCK);

	// 出牌状态
	m_byGameState = MJ_STATE_GIVE;

	m_byTurnStation = pCmd->ucChair;

	// 绘制手牌区域
	DrawHand(pCmd->ucChair, m_TilesHand[pCmd->ucChair]);

	// 绘制拦牌区域
	DrawSet(pCmd->ucChair, m_TilesSet[pCmd->ucChair]);

	// 设置出牌时间
	SetGameTimer(m_byTurnStation, m_tagSetting.nTimerGive);
}

/**
 *  @brief 接受到结束消息
 */	
void CMJFrameLogic::OnNotifyFinish(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_FINISH * pCmd = (TCMD_NOTIFY_FINISH *)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	// 结束状态
	m_byGameState = MJ_STATE_END;

	// 停止定时器
	KillAllTimer();

	CopyMemory(&m_stFinishInfo, pCmd, sizeof(TCMD_NOTIFY_FINISH));

	
	if (pCmd->byFlag == MJ_END_SELF || pCmd->byFlag == MJ_END_GUN || pCmd->byFlag == MJ_END_QIANG)
	{
		// 自摸或点炮, 播放胡牌动画
		IExAnimate * pExAnimate = NULL;
		GETCTRL(IExAnimate, pExAnimate, m_pUI, ExAnimate_HuPai);
		if (pExAnimate != NULL)
		{
			pExAnimate->SetControlVisible(true);
			pExAnimate->SetIsSymmetrical(false);
			pExAnimate->SetIsLoop(false);
			pExAnimate->SetPlayState(true);
		}
	
		for (BYTE i = 0; i < PLAYER_NBR; i++)
		{
			if (pCmd->byWhoWin[i])
			{
				// 自摸与点炮音效
				PlaySound(pCmd->byFlag == MJ_END_SELF ? SOUND_ZIMO : SOUND_DIAN_PAO, i);
				break;
			}
		}
	}
	else
	{
		// 流局
		PlaySound(SOUND_LIUJU);
	}

	int i, j;
	for (i = 0; i < PLAYER_NBR; i++)
	{
		// 设置所有玩家手牌
		m_TilesHand[i].ReleaseAll();

		for (j = 0; j < 17; j++)
		{
			if (pCmd->tile[i][j] != TILE_BEGIN)
			{
				m_TilesHand[i].AddTile(pCmd->tile[i][j]);
			}
		}

		if (pCmd->byWhoWin[i] && m_TilesHand[i].CurrentLength() % 3 == 2)
		{
			// 把胡的那张牌调到最右边显示
			m_TilesHand[i].DelTile(pCmd->tLast);
			m_TilesHand[i].Sort();
			m_TilesHand[i].AddTile(pCmd->tLast);
		}

		DrawHand(i, m_TilesHand[i], m_TilesHand[i].CurrentLength() % 3 == 2, true);


		// 设置所有玩家拦牌
		m_TilesSet[i].ReleaseAll();
		for (j = 0; j < 5; j++)
		{
			if (pCmd->byBlockTilesFlag[i][j] != ACTION_EMPTY)
			{
				TILE_SET tileSet;
				tileSet.m_ucFlag = pCmd->byBlockTilesFlag[i][j];
				tileSet.m_ucChair = pCmd->byBlockTilesWhich[i][j];
				CopyMemory(tileSet.m_ucTile, &pCmd->byBlockTiles[i][j * 4], sizeof(tileSet.m_ucTile));
				m_TilesSet[i].AddSet(tileSet);
			}
		}

		DrawSet(i, m_TilesSet[i]);
	}

	// 2秒后显示结算框
	m_pGameFrame->SetTimer(TIME_SHOW_FINISH, 2000);
}

/**
 *  @brief 接受到修改旁观状态消息
 */	
void CMJFrameLogic::OnNotifyChangeLookOn(BYTE * buffer, int nLen)
{
}

/**
 *  @brief 接受到刷新手牌消息后
 */	
void CMJFrameLogic::OnNotifyUpdateHandTiles(BYTE * buffer, int nLen)
{
	TCMD_NOTIFY_UPDATEHANDTILES * pCmd = (TCMD_NOTIFY_UPDATEHANDTILES *)buffer;
	if (pCmd == NULL)
	{
		return ;
	}

	if (pCmd->ucChair != m_byMyStation)
	{
		return;
	}

	m_TilesHand[pCmd->ucChair].CurrentLength(pCmd->ucHandCount);

	for (int i = 0; i < pCmd->ucHandCount; ++i )
	{
		m_TilesHand[pCmd->ucChair].SetTile(i, pCmd->ucHandTiles[i]);
	}

	m_TilesHand[pCmd->ucChair].Sort();

	// 绘制手牌区域
	DrawHand(pCmd->ucChair, m_TilesHand[pCmd->ucChair], m_TilesHand[pCmd->ucChair].CurrentLength() % 3 == 2);
}

/**
 *  @brief 接受到GM命令消息后执行的事件
 */
void CMJFrameLogic::OnNotifyGMCmd(BYTE * buffer, int nLen)
{
    TCMD_NOTIFY_GMCMD * pCmd = (TCMD_NOTIFY_GMCMD *)buffer;
    if (pCmd == NULL)
    {
        return ;
    }

    //m_pGameFrame->InsertNormalMessage(pCmd->strGMResult);
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/** 
 * @brief 请求GM命令
 */
void CMJFrameLogic::OnRequestGMCmd()
{
	if (m_tagSetting.nOpenGM == 1)
	{
		IEditorText *pEdit = NULL;
		GETCTRL(IEditorText, pEdit, m_pUI, CTN_EDITORTEXT_GM);
		if (pEdit != NULL)
		{
			TCMD_REQUEST_GMCMD cmd;
			swprintf(cmd.strGMCmd, MAX_PATH, L"%s", pEdit->GetText());
			m_pGameFrame->SendGameData(0, (void *)&cmd, sizeof(cmd));
		}
	}
}

/** 
 * @brief 请求出牌                
 * @param [in] uControlID 控件ID
 * @param [in] bAutoGive  是否自动出牌
 */
void CMJFrameLogic::OnRequestGive(UINT32 uControlID, bool bAutoGive)
{
	if (m_byTurnStation != m_byMyStation)
	{
		return ;  // 非当前出牌玩家不能出牌
	}

	if (m_byGameState == MJ_STATE_BLOCK && m_byStateSelect == MJ_STATE_SELECT_TING)
	{
		// 听牌选择
		TCMD_REQUEST_BLOCK cmd;
		cmd.ucFlag = ACTION_TING;
		m_pGameFrame->SendGameData(0, (void*)&cmd, sizeof(cmd));

		// 取消禁用的牌控件
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_MJCtrl_HandCtrl_2, SetInvalidTile(NULL, 0));

		// 隐藏拦牌提示信息
		SAFE_CTRL_OPERATE(IContain, CTN_Block_Panel, SetControlVisible(false));

		m_byStateSelect = MJ_STATE_SELECT_NONE;
		m_byGameState = MJ_STATE_GIVE;
	}

	if (m_byGameState != MJ_STATE_GIVE && m_byGameState != MJ_STATE_BLOCK)
	{
		return ;  // 出牌状态错误
	}

	if (m_byGameState == MJ_STATE_BLOCK)
	{
		// 如果出牌时, 还在拦牌状态
		// 隐藏吃碰杠面板
		SAFE_CTRL_OPERATE(IContain, CTN_Block, SetControlVisible(false));

		// 取消拦牌, 如果可以胡牌，则默认胡牌
		TCMD_REQUEST_BLOCK cmd;
		cmd.ucFlag = ((m_stBlockInfo.byFlag & 0x10) && bAutoGive) ? ACTION_WIN : ACTION_EMPTY;
		m_pGameFrame->SendGameData(0, (void*)&cmd, sizeof(cmd));
	}

	TILE tile = TILE_BEGIN;
	IMjHandCtrl * pMJCtrl = NULL;
	GETCTRL(IMjHandCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_HandCtrl_2);
	if (pMJCtrl != NULL)
	{
		tile = pMJCtrl->GetUpTile();
	}

	TCMD_REQUEST_GIVE cmd;
	cmd.t = tile;
	cmd.ucTileLength = m_TilesHand[m_byMyStation].CurrentLength() - 1;  

    if (bAutoGive || tile == TILE_BEGIN)
	{
		cmd.t = m_TilesHand[m_byMyStation].GetTile(cmd.ucTileLength);
	}
	else
	{
		m_nTimeOutCount = 0;   // 手牌出牌将清空超时记录
	}

	m_pGameFrame->SendGameData(0, (void*)&cmd, sizeof(cmd));
}

/** 
 * @brief 请求拦牌              
 * @param [in] uControlID 控件ID
 */
void CMJFrameLogic::OnRequestBlock(UINT32 uControlID)
{
	SAFE_CTRL_OPERATE(IContain, CTN_Block, SetControlVisible(false));

	if (m_byTurnStation != m_byMyStation)
	{
		return ;  // 非当前出牌玩家不能出牌
	}

	if (m_byGameState != MJ_STATE_BLOCK)
	{
		return ;  // 出牌状态错误
	}

	CMJHand mjHand;
	int nTiels[20];
	int i, j, nCount = 0;
	TILE tile;
	TILE_SET tileSet;

	TCMD_REQUEST_BLOCK cmd;
	cmd.ucFlag = ACTION_EMPTY;

	// 吃、碰、杠、听、胡、取消
	switch (uControlID)
	{
	case CTN_Block_BTN_Chi:   // 吃
		{
			cmd.ucFlag = ACTION_COLLECT;

			for (i = 0; i < m_stBlockInfo.tilesSet.CurrentLength(); i++)
			{
				m_stBlockInfo.tilesSet.GetSet(i, tileSet);
				if (tileSet.m_ucFlag == ACTION_COLLECT)
				{
					for (j = 0; j < 3; j++)
					{
						mjHand.AddTile(tileSet.m_ucTile[j]);
						cmd.ucTile[j] = tileSet.m_ucTile[j];
					}

					nCount++;
				}
			}

			if (nCount > 1)
			{
				// 存在多种吃法, 进入吃牌选择状态
				m_byStateSelect = MJ_STATE_SELECT_COLLECT;

				nCount = 0;
				for (i = 0; i < m_TilesHand[m_byMyStation].CurrentLength(); i++)
				{
					tile = m_TilesHand[m_byMyStation].GetTile(i);
					if (!mjHand.IsHave(tile))
					{
						nTiels[nCount++] = tile;   // 需要禁用的牌
					}
				}

				IMjHandCtrl * pMJCtrl = NULL;
				GETCTRL(IMjHandCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_HandCtrl_2);
				if (pMJCtrl != NULL)
				{
					// 设置禁用
					pMJCtrl->SetInvalidTile(nTiels, nCount);
				}

				return ;
			}
		}
		break;
	case CTN_Block_BTN_Peng:  // 碰
		{
			cmd.ucFlag = ACTION_TRIPLET;
			for (i = 0; i < m_stBlockInfo.tilesSet.CurrentLength(); i++)
			{
				m_stBlockInfo.tilesSet.GetSet(i, tileSet);
				if (tileSet.m_ucFlag == ACTION_TRIPLET)
				{
					cmd.ucTile[0] = tileSet.m_ucTile[0];
					cmd.ucTile[1] = tileSet.m_ucTile[1];
					cmd.ucTile[2] = tileSet.m_ucTile[2];
					break;
				}
			}
		}
		break;
	case CTN_Block_BTN_Gang:  // 杠
		{
			for (i = 0; i < m_stBlockInfo.tilesSet.CurrentLength(); i++)
			{
				m_stBlockInfo.tilesSet.GetSet(i, tileSet);
				if (tileSet.m_ucFlag == ACTION_QUADRUPLET_CONCEALED
				 || tileSet.m_ucFlag == ACTION_QUADRUPLET_REVEALED
				 || tileSet.m_ucFlag == ACTION_QUADRUPLET_PATCH)
				{
					for (j = 0; j < 4; j++)
					{
						mjHand.AddTile(tileSet.m_ucTile[j]);
						cmd.ucTile[j] = tileSet.m_ucTile[j];
					}

					nCount++;
				}
			}

			if (nCount > 1)
			{
				// 存在多种杠, 进入杠牌选择状态
				m_byStateSelect = MJ_STATE_SELECT_QUADRUPLET;

				nCount = 0;
				for (i = 0; i < m_TilesHand[m_byMyStation].CurrentLength(); i++)
				{
					tile = m_TilesHand[m_byMyStation].GetTile(i);
					if (!mjHand.IsHave(tile))
					{
						nTiels[nCount++] = tile;   // 需要禁用的牌
					}
				}

				IMjHandCtrl * pMJCtrl = NULL;
				GETCTRL(IMjHandCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_HandCtrl_2);
				if (pMJCtrl != NULL)
				{
					// 设置禁用
					pMJCtrl->SetInvalidTile(nTiels, nCount);
				}

				return ;
			}
			else
			{
				if (m_TilesHand[m_byMyStation].IsHave(cmd.ucTile[0], 4))
				{
					cmd.ucFlag = ACTION_QUADRUPLET_CONCEALED; // 暗杠
				}
				else if (m_TilesHand[m_byMyStation].IsHave(cmd.ucTile[0], 3))
				{
					cmd.ucFlag = ACTION_QUADRUPLET_REVEALED;  // 明杠
				}
				else
				{
					cmd.ucFlag = ACTION_QUADRUPLET_PATCH;     // 补杠
				}
			}
		}
		break;
	case CTN_Block_BTN_Ting:  // 听
		{
			// 进入听牌选择状态
			m_byStateSelect = MJ_STATE_SELECT_TING;

			for (i = 0; i < m_TilesHand[m_byMyStation].CurrentLength(); i++)
			{
				tile = m_TilesHand[m_byMyStation].GetTile(i);
				if (!m_stBlockInfo.tingGive.IsHave(tile))
				{
					nTiels[nCount++] = tile;   // 听牌后，不可以出的牌都必须禁用
				}
			}

			IMjHandCtrl * pMJCtrl = NULL;
			GETCTRL(IMjHandCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_HandCtrl_2);
			if (pMJCtrl != NULL)
			{
				// 设置禁用
				pMJCtrl->SetInvalidTile(nTiels, nCount);
			}
			
			return ;
		}
		break;
	case CTN_Block_BTN_Hu:    // 胡
		{
			cmd.ucFlag = ACTION_WIN;
		}
		break;
	case CTN_Block_BTN_Cancel:// 取消  
		{
			if (m_TilesHand[m_byMyStation].CurrentLength() % 3 == 2)
			{
				m_byGameState = MJ_STATE_GIVE;
				SetGameTimer(m_byTurnStation, m_tagSetting.nTimerGive);
			}
		}
		break;
	}

	m_byStateSelect = MJ_STATE_SELECT_NONE;
	m_pGameFrame->SendGameData(0, (void*)&cmd, sizeof(cmd));

	// 重置状态
	m_byGameState = MJ_STATE_NONE;
}

/** 
 * @brief 请求拦牌              
 * @param [in] uControlID 控件ID
 */
void CMJFrameLogic::OnRequestBlockSelect(UINT32 uControlID)
{
	TILE tile = TILE_BEGIN;
	IMjHandCtrl * pMJCtrl = NULL;
	GETCTRL(IMjHandCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_HandCtrl_2);
	if (pMJCtrl != NULL)
	{
		tile = pMJCtrl->GetUpTile();
	}

	if (tile == TILE_BEGIN)
	{
		return ;
	}

	int i, j;
	TILE_SET tileSet;
	CMJHand mjHand;
	TCMD_REQUEST_BLOCK cmd;

	if (m_byStateSelect == MJ_STATE_SELECT_COLLECT)
	{
		// 吃牌
		for (i = 0; i < m_stBlockInfo.tilesSet.CurrentLength(); i++)
		{
			m_stBlockInfo.tilesSet.GetSet(i, tileSet);
			if (tileSet.m_ucFlag == ACTION_COLLECT)
			{
				mjHand.ReleaseAll();
				for (j = 0; j < 3; j++)
				{
					cmd.ucTile[j] = tileSet.m_ucTile[j];
					mjHand.AddTile(tileSet.m_ucTile[j]);
				}

				// 判断是否是这个吃牌组合
				if (mjHand.IsHave(tile))
				{
					cmd.ucFlag = ACTION_COLLECT;
					break;
				}
			}
		}
	}
	else if (m_byStateSelect == MJ_STATE_SELECT_QUADRUPLET)
	{
		// 杠牌
		for (i = 0; i < m_stBlockInfo.tilesSet.CurrentLength(); i++)
		{
			m_stBlockInfo.tilesSet.GetSet(i, tileSet);
			if (tileSet.m_ucFlag == ACTION_QUADRUPLET_CONCEALED
			 || tileSet.m_ucFlag == ACTION_QUADRUPLET_REVEALED
			 || tileSet.m_ucFlag == ACTION_QUADRUPLET_PATCH)
			{
				mjHand.ReleaseAll();
				for (j = 0; j < 4; j++)
				{
					mjHand.AddTile(tileSet.m_ucTile[j]);
					cmd.ucTile[j] = tileSet.m_ucTile[j];
				}

				// 判断是否是这个杠组合
				if (mjHand.IsHave(tile))
				{
					if (m_TilesHand[m_byMyStation].IsHave(cmd.ucTile[0], 4))
					{
						cmd.ucFlag = ACTION_QUADRUPLET_CONCEALED; // 暗杠
					}
					else if (m_TilesHand[m_byMyStation].IsHave(cmd.ucTile[0], 3))
					{
						cmd.ucFlag = ACTION_QUADRUPLET_REVEALED;  // 明杠
					}
					else
					{
						cmd.ucFlag = ACTION_QUADRUPLET_PATCH;     // 补杠
					}

					break;
				}
			}
		}
	}

	m_pGameFrame->SendGameData(0, (void*)&cmd, sizeof(cmd));

	// 取消禁用的牌控件
	SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_MJCtrl_HandCtrl_2, SetInvalidTile(NULL, 0));

	// 隐藏拦牌提示信息
	SAFE_CTRL_OPERATE(IContain, CTN_Block_Panel, SetControlVisible(false));

	// 进入出牌状态
	m_byGameState = MJ_STATE_GIVE;

	// 重置拦牌操作状态
	m_byStateSelect = MJ_STATE_SELECT_NONE;

	return ;
}

/** 
 * @brief 拦牌信息移动提示选择             
 * @param [in] uControlID 控件ID
 * @param [in] nIndex     牌索引位置
 */
void CMJFrameLogic::OnBlockMoveSelect(UINT32 uControlID, int nIndex)
{
	TILE tile = TILE_BEGIN;
	IMjHandCtrl * pMJHandCtrl = NULL;
	GETCTRL(IMjHandCtrl, pMJHandCtrl, m_pUI, CTN_MJCtrl_HandCtrl_2);
	if (pMJHandCtrl != NULL)
	{
		tile = pMJHandCtrl->GetUpTile();
	}

	if ((nIndex < 0 || nIndex > 14) || tile == TILE_BEGIN || (m_byStateSelect != MJ_STATE_SELECT_COLLECT && m_byStateSelect != MJ_STATE_SELECT_QUADRUPLET && m_byStateSelect != MJ_STATE_SELECT_TING))
	{
		// 当前状态不处于吃、杠、听牌选择状态
		SAFE_CTRL_OPERATE(IContain, CTN_Block_Panel, SetControlVisible(false));
		return ;
	}

	int i, j, k;
	TILE_SET tileSet;
	CMJHand mjHand;
	MJ_TILE_UIINFO tileUserInfo[20];
	IMjCtrl * pMJCtrl = NULL;
	SAFE_CTRL_OPERATE(IMjCtrl, CTN_MJCtrl_Block_3, SetControlVisible(false));
	SAFE_CTRL_OPERATE(IMjCtrl, CTN_MJCtrl_Block_4, SetControlVisible(false));

	IContain * pContain = NULL;
	GETCTRL(IContain, pContain, m_pUI, CTN_Block_Panel);
	if (pContain != NULL)
	{
		// 吃杠听牌框跟着当前选择的麻将子移动
		RECT rtTile = pMJHandCtrl->GetTileRectByIndex(m_TilesHand[m_byMyStation].CurrentLength() % 3 == 2 ? nIndex - 1: nIndex);
		pContain->SetGameXY(rtTile.left, pContain->GetGameY());
	}

	if (m_byStateSelect == MJ_STATE_SELECT_COLLECT)
	{
		GETCTRL(IMjCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_Block_3);
		if (pMJCtrl == NULL)
		{
			return ;
		}

		pMJCtrl->SetIsUseMouse(false);
		pMJCtrl->GetTilesInfo(tileUserInfo, 20);

		// 吃牌状态选择
		for (i = 0; i < m_stBlockInfo.tilesSet.CurrentLength(); i++)
		{
			m_stBlockInfo.tilesSet.GetSet(i, tileSet);
			if (tileSet.m_ucFlag == ACTION_COLLECT)
			{
				mjHand.ReleaseAll();
				for (j = 0; j < 3; j++)
				{
					mjHand.AddTile(tileSet.m_ucTile[j]);
				}

				if (mjHand.IsHave(tile))
				{
					tileUserInfo[0].ucTiles[0] = mjHand.GetTile(0);
					tileUserInfo[1].ucTiles[0] = mjHand.GetTile(1);
					tileUserInfo[2].ucTiles[0] = mjHand.GetTile(2);
					pMJCtrl->SetTilesInfo(tileUserInfo, 3);
					pMJCtrl->SetControlVisible(true);
					SAFE_CTRL_OPERATE(IContain, CTN_Block_Panel, SetControlVisible(true));
					return ;
				}
			}
		}
	}
	else if (m_byStateSelect == MJ_STATE_SELECT_QUADRUPLET)
	{
		GETCTRL(IMjCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_Block_4);
		if (pMJCtrl == NULL)
		{
			return ;
		}

		pMJCtrl->SetIsUseMouse(false);
		pMJCtrl->GetTilesInfo(tileUserInfo, 20);

		// 杠牌状态选择
		for (i = 0; i < m_stBlockInfo.tilesSet.CurrentLength(); i++)
		{
			m_stBlockInfo.tilesSet.GetSet(i, tileSet);
			if (tileSet.m_ucFlag == ACTION_QUADRUPLET_CONCEALED
			 || tileSet.m_ucFlag == ACTION_QUADRUPLET_REVEALED
			 || tileSet.m_ucFlag == ACTION_QUADRUPLET_PATCH)
			{
				mjHand.ReleaseAll();
				for (j = 0; j < 4; j++)
				{
					mjHand.AddTile(tileSet.m_ucTile[j]);
				}

				if (mjHand.IsHave(tile))
				{
					tileUserInfo[0].ucTiles[0] = mjHand.GetTile(0);
					tileUserInfo[1].ucTiles[0] = mjHand.GetTile(1);
					tileUserInfo[2].ucTiles[0] = mjHand.GetTile(2);
					tileUserInfo[3].ucTiles[0] = mjHand.GetTile(3);
					pMJCtrl->SetTilesInfo(tileUserInfo, 4);
					pMJCtrl->SetControlVisible(true);
					SAFE_CTRL_OPERATE(IContain, CTN_Block_Panel, SetControlVisible(true));
					return ;
				}
			}
		}
	}
	else if (m_byStateSelect == MJ_STATE_SELECT_TING)
	{
		GETCTRL(IMjCtrl, pMJCtrl, m_pUI, CTN_MJCtrl_Block_4);
		if (pMJCtrl == NULL)
		{
			return ;
		}

		pMJCtrl->SetIsUseMouse(false);
		pMJCtrl->GetTilesInfo(tileUserInfo, 20);

		// 听牌状态选择
		for (i = 0; i < m_stBlockInfo.tingGive.CurrentLength(); i++)
		{
			tile = m_stBlockInfo.tingGive.GetTile(i);

			for (j = 0; j < m_TilesHand[m_byMyStation].CurrentLength(); j++)
			{
				if (tile == m_TilesHand[m_byMyStation].GetTile(j))
				{
					mjHand.ReleaseAll();
					for (k = 0; k < m_stBlockInfo.tingWin[j].CurrentLength(); k++)
					{
						tileUserInfo[k].ucTiles[0] = m_stBlockInfo.tingWin[j].GetTile(k);
					}

					pMJCtrl->SetTilesInfo(tileUserInfo, m_stBlockInfo.tingWin[j].CurrentLength());
					pMJCtrl->SetControlVisible(true);
					SAFE_CTRL_OPERATE(IContain, CTN_Block_Panel, SetControlVisible(true));
					return ;
				}
			}
		}
	}

	SAFE_CTRL_OPERATE(IContain, CTN_Block_Panel, SetControlVisible(false));
}