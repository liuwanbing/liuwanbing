#include "StdAfx.h"
#include "ServerManage.h"
#include "../GameMessage/GameAlgorithm.h"
#include "../../../../sdk/include/Common/writelog.h"
#include <math.h>

TCHAR szNumber[][10] = {"","2","3","4","5","6","7","8","9","10","J","Q","K","A"};
TCHAR szShape[][10] = {"黑桃", "红桃", "梅花" ,"方块"};
TCHAR szName[][20] = {"", "散牌", "对子", "两对", "三条", "最小顺子", "顺子", "同花", "葫芦", "四条", "最小同花顺", "同花顺", "皇家同花顺"};


/*
* 打印日志文件
*/
void DebugPrintf(int uRoomID,const char *p, ...)
{
	//return;	// 发布时不做任何事	By Zxd 20090727
	char szFilename[1024];
	CString strPath = CINIFile::GetAppPath() +"\\GameLog\\";

	SHCreateDirectoryEx(NULL, strPath, NULL);

	CTime time = CTime::GetCurrentTime();

	wsprintf( szFilename, "%s%d_%d_%d%d%d_Server.txt",strPath ,  NAME_ID,uRoomID ,time.GetYear(), time.GetMonth(), time.GetDay());
	FILE *fp = fopen( szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start( arg, p );
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}

//游戏配置

//游戏扩展配置

//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{
	//for (int i = 0; i < PLAY_COUNT; i++)
	//{
	//	m_emUserAgree[i] = US_UNKNOWN;
	//}
	m_bGameFinished = true;

    m_bGameStation = GS_WAIT_SETGAME;
	m_bFaiPaiFinish = false;
}

//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}


/*-----------------------------------------------------------------------------------------------------------
一系列平台提供的需要重载来驱动游戏服务的函数实现，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
----------------------------------------------------------------------------------------------------------*/

//读取配置文件
bool CServerGameDesk::InitDeskGameStation()
{
	//LoadDeskCfg();
	LoadDeskCfg(m_pDataManage->m_InitData.uRoomID);
	//LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}

//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (GetPlayerCount() < 2)
	{
		OutputDebugString("dxh: GameBegin, 人数不足");
		return false;
	}

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			if (m_GameData.m_UserData[i].m_bIn == false)
			{
				OutputDebugString("dxh: GameBegin, 有玩家未代入金币");
				return false;
			}
		}	
	}

	OutputDebugString("dxh: GameBegin");
	if (__super::GameBegin(bBeginFlag) == false) 
	{
		GameFinish(0, GF_SALE);
		return false;
	}

	m_bGameStation = GS_PLAY_GAME;

	m_bGameFinished = false;


	//启动MyTimer，意味游戏的状态流程随之启动
	SetTimer(IDT_MYTIMER,MY_ELAPSE);

	//关闭事件游戏保护
	CloseEventProtection();

	//初始分组表
	InitGroupTable();

	//初始服务端游戏数据
	m_GameData.Init();

	// 游戏数据对象的半初始化
	m_GameData.HalfInit();

	for (int i = 0; i < PLAY_COUNT; i++) 
	{
		m_iUserStation[i] = 255;
		if (m_pUserInfo[i] != NULL)
		{
			m_iUserStation[i] = TYPE_PLAY_GAME;
			m_GameData.m_bGiveUp[i] = false;
		}
	}

	//下发游戏开始信号，通知客户端开启游戏状态机
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			SendGameData(i,MDM_GM_GAME_NOTIFY,ASS_GAME_BEGIN,0);
		}
	}
	SendWatchData(m_bMaxPeople,MDM_GM_GAME_NOTIFY,ASS_GAME_BEGIN,0);

	//启动系统设置桌面配置流程，主要完成（时间配置，规则配置的任务）
	ActionRedirect(IDEV_SYS_CFG_DESK, 1);

	OutputDebugString("DZPKServer:OnUserSubMoney2");
	return true;
}

// 游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	CString str;
	str.Format("Server: GameFinish() 游戏结束标志: %d",bCloseFlag);
	OutputDebugString(str);

	KillMyTimer(IDT_MYTIMER);

	switch (bCloseFlag)
	{
	case GF_NORMAL:
		{
			TResult tagResult;

			memset(&tagResult,0,sizeof(tagResult));

			int i, j, k;

			CString str;

			CByteArray arrayResult;

			for (i = 0; i < PLAY_COUNT; i++)
			{
				if (m_pUserInfo[i] != NULL)
				{
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
					tagResult.nbyUserID[i] = m_pUserInfo[i]->m_UserData.dwUserID;
					memcpy(tagResult.szName[i],m_pUserInfo[i]->m_UserData.nickName,sizeof(m_pUserInfo[i]->m_UserData.nickName));
					tagResult.bOrder[i] = i;
				}
			}

			int nWinCount = 0;
			__int64 nScore[PLAY_COUNT] = {0};
			__int64 nRecoverMoney[PLAY_COUNT][8] = {0}; // 回收下注的金币

			if (!m_GameData.m_bGiveUpOver)
			{
				OutputDebugString("Server: 比牌结束");
				
				// 整个游戏8个下注池, 需要检测每个下注池胜利玩家
				for (i = 0; i <= m_GameData.m_nBetPool && i < 8; i++)
				{
					// 计算每一个下注池赢家 // 赢的玩家与相等玩家
					CalculateWin(i, arrayResult);
					
					// 获取数组大小
					nWinCount = arrayResult.GetSize();

					str.Format("Server: 当前赢家数量: %d", nWinCount);
					OutputDebugString(str);

					if (nWinCount > 0)
					{
						bool bIsEqual = true;
						for (j = 0; j < PLAY_COUNT; j++)
						{
							if (m_pUserInfo[j] == NULL || m_GameData.m_nBetMoney[j][i] <= 0)
							{
								continue;
							}

							bool bIsWin = false;

							for (k = 0; k < nWinCount; k++)
							{
								if (j == arrayResult.GetAt(k))
								{
									bIsWin = true;
									break;
								}
							}

							if (bIsWin)
							{
								// 胜利玩家跳过
								continue;
							}

							str.Format("Server: 第%d个下注池, 玩家ID: %d, 位置:%d, 输: %I64d", i, m_pUserInfo[j]->m_UserData.dwUserID, j, m_GameData.m_nBetMoney[j][i]);
							OutputDebugString(str);

							// 输家扣金币
							nScore[j] -= m_GameData.m_nBetMoney[j][i];

							bIsEqual = false;

							for (k = 0; k < nWinCount; k++)
							{
								int nStation = arrayResult.GetAt(k);  // 赢的玩家

								str.Format("Server: 赢家人数:%d, 赢家位置: %d", nWinCount, nStation);
								OutputDebugString(str);

								// 胜利玩家加金币
								nScore[nStation] += m_GameData.m_nBetMoney[j][i] / nWinCount;

								// 记录玩家赢的下注池金币
								tagResult.nWinPoolsMoney[nStation][i] += m_GameData.m_nBetMoney[j][i] / nWinCount;

								// 记录玩家赢的下注池
								tagResult.bWinBetPools[nStation][i] = true;

								// 回收自己下注的金币
								if (nRecoverMoney[nStation][i] <= 0)
								{
									nRecoverMoney[nStation][i] = m_GameData.m_nBetMoney[nStation][i];
									CString str;
									str.Format("Server: 比牌结束1---玩家:%d 回收自己下注金币:%I64d, 第%d个下注池", m_pUserInfo[nStation]->m_UserData.dwUserID,nRecoverMoney[nStation][i], i);
									OutputDebugString(str);
								}

								str.Format("Server: 第%d个下注池, 玩家ID: %d, 位置:%d, 赢: %I64d", i, m_pUserInfo[nStation]->m_UserData.dwUserID, nStation, m_GameData.m_nBetMoney[j][i]);
								OutputDebugString(str);
							}
						}

						if (bIsEqual)
						{
							// 玩家牌型相等，没有输家，需要把自己下注的金币回收
							for (k = 0; k < nWinCount; k++)
							{
								int nStation = arrayResult.GetAt(k);  // 赢的玩家

								// 回收自己下注的金币
								if (nRecoverMoney[nStation][i] <= 0)
								{
									tagResult.bWinBetPools[nStation][i] = true;
									nRecoverMoney[nStation][i] = m_GameData.m_nBetMoney[nStation][i];
									CString str;
									str.Format("Server: 比牌结束2---玩家:%d 回收自己下注金币:%I64d, 第%d个下注池", m_pUserInfo[nStation]->m_UserData.dwUserID,nRecoverMoney[nStation][i], i);
									OutputDebugString(str);
								}
							}
						}
					}
					else
					{
						str.Format("Server: 第%d个下注池没有找到胜利玩家", i);
						OutputDebugString(str);
					}
				}
			}
			else
			{
				// 弃牌结束, 只有一个玩家赢
				OutputDebugString("Server: 弃牌结束");
				BYTE bWinStation = -1;
				for (int i = 0; i < PLAY_COUNT; i++)
				{
					if (m_pUserInfo[i] == NULL)
					{
						continue;
					}

					// 先查找到最后一个未弃牌的玩家
					if (!m_GameData.m_bGiveUp[i])
					{
						bWinStation = i;

						str.Format("Server: bWinStation=%d未弃牌", bWinStation);
						OutputDebugString(str);

						for (int j = 0; j <= m_GameData.m_nBetPool && j < 8; j++)
						{
							// 记录玩家赢的下注池
							tagResult.bWinBetPools[bWinStation][j] = true;

							// 回收自己下注的金币
							nRecoverMoney[bWinStation][j] = m_GameData.m_nBetMoney[bWinStation][j];

							CString str;
							str.Format("Server: 弃牌结束---玩家:%d 回收自己下注金币:%I64d, 第%d个下注池", m_pUserInfo[bWinStation]->m_UserData.dwUserID,nRecoverMoney[bWinStation][j], j);
							OutputDebugString(str);
						}

						break;
					}
				}

				for (int i = 0; i < PLAY_COUNT; i++)
				{
					if (m_pUserInfo[i] == NULL || bWinStation == i)
					{
						continue;
					}

					// 输家扣金币
					nScore[i] -= m_GameData.GetBetMoney(i);

					str.Format("Server: 玩家: %d 输金币: %I64d", i, nScore[i]);
					OutputDebugString(str);

					// 赢家金币
					nScore[bWinStation] += _abs64(nScore[i]);

					for (int j = 0; j <= m_GameData.m_nBetPool && j < 8; j++)
					{
						// 记录玩家赢的下注池金币
						tagResult.nWinPoolsMoney[bWinStation][j] = _abs64(nScore[i]);
					}


					str.Format("Server: 玩家: %d 赢金币: %I64d", bWinStation, nScore[bWinStation]);
					OutputDebugString(str);
				}
			}

			OutputDebugString("Server: 处理玩家积分");

			for (i = 0; i < PLAY_COUNT; i++)
			{
				if (m_pUserInfo[i] == NULL)
				{
					continue;
				}

				// 玩家输赢积分
				tagResult.nScore[i] = nScore[i];

				if (nScore[i] > 0)
				{
					tagResult.bWin[i] = true;
				}

				for (j = 0; j <= m_GameData.m_nBetPool && j < 8; j++)
				{
					// 玩家每个下注池数据
					tagResult.nUserBet[i][j] = m_GameData.m_nBetMoney[i][j];
				}

				str.Format("Server: 位置: %d，输赢积分: %I64d", i, tagResult.nScore[i]);
				OutputDebugString(str);
			}

			for (i = 0; i <= m_GameData.m_nBetPool && j < 8; i++)
			{
				// 把每个下注池的金币
				tagResult.nBetPools[i] = m_GameData.GetBetMoney(i, false);
			}

			// 写入数据库				
			bool bNetCutUser[PLAY_COUNT];
			memset(bNetCutUser, 0, sizeof(bNetCutUser));

			// 进行金币扣除
			__super::ChangeUserPointint64(tagResult.nScore,bNetCutUser);

			memset(tagResult.nMoney,0,sizeof(tagResult.nMoney));

			// 获取扣税后金币
			__super::RecoderGameInfo(tagResult.nMoney);

			// 自己下注金币回收处理
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (m_pUserInfo[i] == NULL)
				{
					continue;
				}

				// 加减相应的代入金币, 输的不需要再处理，赢的时候需要把赢的金币加入到代入金币数组中
				if (tagResult.nMoney[i] > 0)
				{
					m_GameData.m_UserMoney[i] += tagResult.nMoney[i];
				}
				else
				{
					// 下注的钱 + 输了的钱 = 本局游戏返还的钱 2012-10-11 duanxiaohui alter 
					m_GameData.m_UserMoney[i] += (m_GameData.GetBetMoney(i) + tagResult.nMoney[i]);

					if (m_GameData.m_UserMoney[i] < 0)
					{
						m_GameData.m_UserMoney[i] = 0;
					}
				}

                // 客户端自身金币更新
                tagResult.nSelfMoney[i] = m_pUserInfo[i]->m_UserData.i64Money;

				tagResult.nMoneyEx[i] = tagResult.nMoney[i];

				str.Format("Server: nScore[%d]=%I64d,  nMoneyEx[%d]=%I64d,  nMoney[%d]=%I64d ", i, tagResult.nScore[i], i, tagResult.nMoneyEx[i], i, tagResult.nMoney[i]);
				OutputDebugString(str);

				for (int j = 0; j < 8; j++)
				{
					if (tagResult.bWinBetPools[i][j])
					{
						CString str;
						str.Format("Server: 玩家:%d 赢金币:%I64d, 回收自己下注金币:%I64d, 第%d个下注池", m_pUserInfo[i]->m_UserData.dwUserID,tagResult.nMoney[i], nRecoverMoney[i][j], j);
						OutputDebugString(str);

						// 把自己下注的金币回收
						//m_GameData.m_UserMoney[i] += nRecoverMoney[i][j];

						// 赢的金币数据也加上自己下注的金币
						tagResult.nWinPoolsMoney[i][j] += nRecoverMoney[i][j];

						if (tagResult.nMoneyEx[i] < 0)
						{
							// 赢的金币数据也加上自己下注的金币
							//tagResult.nMoneyEx[i] = nRecoverMoney[i][j];
							// 2012-10-11 duanxiaohui alter
							tagResult.nMoneyEx[i] = (m_GameData.GetBetMoney(i) + tagResult.nMoney[i]);
						}
						else
						{
							// 把自己下注的金币回收 2012-10-11 duanxiaohui alter
							m_GameData.m_UserMoney[i] += nRecoverMoney[i][j];

							// 赢的金币数据也加上自己下注的金币
							tagResult.nMoneyEx[i] += nRecoverMoney[i][j];
						}
					}
				}
			}

            // 更新客户端代入金币
            memcpy(tagResult.nSubMoney, m_GameData.m_UserMoney, sizeof(tagResult.nSubMoney));

			BYTE temp = 0;
			// 金币排序(赢最多的玩家排前名, 依次排名)
			for (i = 0; i < PLAY_COUNT; i++)
			{
				for (j = 0  ; j < PLAY_COUNT - i - 1; j++)
				{
					if (m_pUserInfo[i] == NULL)
					{
						continue;
					}

					if (tagResult.nMoney[j] > tagResult.nMoney[j + 1])
					{
						temp = tagResult.bOrder[j];
						tagResult.bOrder[j] = tagResult.bOrder[j+1];
						tagResult.bOrder[j + 1] = temp;
					}
				}
			}

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] == NULL)
				{
					tagResult.bOrder[i] = 255;
					tagResult.nMoney[i] = 0;
				}
			}

			CString strbOrder;

			// 通知客户端
			for (i = 0; i < PLAY_COUNT; i++)
			{
 				m_GameData.m_UserData[i].m_bIn = false;

				if (m_pUserInfo[i] != NULL)
 				{
					strbOrder.Format("Server:bOrder[%d] == %d",i,tagResult.bOrder[i]);
					OutputDebugString(strbOrder);

 					SendGameData(i, &tagResult, sizeof(TResult), MDM_GM_GAME_NOTIFY, ASS_RESULT, 0);

					str.Format("Server:游戏结算数据-Server:玩家ID:%d,玩家昵称:%s,玩家代入金额:%I64d,玩家真实金额1:%I64d,玩家真实金额2:%I64d,玩家实际输赢金额:%I64d,玩家输赢金额:%I64d",
						m_pUserInfo[i]->m_UserData.dwUserID,m_pUserInfo[i]->m_UserData.szName,tagResult.nSubMoney[i],
						m_pUserInfo[i]->m_UserData.i64Money,tagResult.nSelfMoney[i],tagResult.nMoney[i],tagResult.nMoneyEx[i]);

				   	OutputDebugString(str);
 				}
			}

			SendWatchData(m_bMaxPeople, &tagResult, sizeof(TResult), MDM_GM_GAME_NOTIFY, ASS_RESULT, 0);

            m_bGameStation = GS_WAIT_NEXT_ROUND;

			// 清除断线玩家信息 duanxiaohui
			for (i = 0; i < PLAY_COUNT; i++)
			{
				if (m_pUserInfo[i] != NULL)
				{
					if (m_GameData.m_UserData[i].m_bNetCut)
					{
						CString str;
						str.Format("dxh: 清空断线玩家:%d", i);
						OutputDebugString(str);

						//MakeUserOffLine(i);   // 强退清空该玩家信息
						OnUserMoneyLack(i);   // 通知客户端删除该玩家信息
					}
				}
			}
			// 断流，等待每个玩家确认继续游戏消息(ASS_NEXT_ROUND_REQ)，在此需要完善断流风险管理机制			
			//OpenEventProtection(IDEV_NEXT_ROUND_BEGIN, /*m_GameData.m_tagDeskCfg.Time.byNextRound +*/ 30 * 1000);)

			// 游戏结束
			//GameFinish(bDeskStation,bCloseFlag);
			//return true;
			
		}
		break;
	case GFF_SAFE_FINISH:
	case GF_SALE:			//游戏安全结束
		{
			//设置数据
			m_bGameStation = GS_WAIT_ARGEE;//GS_WAIT_SETGAME;

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}
			/*GameCutStruct CutEnd;
			::memset(&CutEnd,0,sizeof(CutEnd));

			for (int i = 0; i < PLAY_COUNT; i ++) 
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SAFE_END,0);
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SAFE_END,0);*/

			bCloseFlag = GFF_SAFE_FINISH;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
		break;
	default:
		break;
	}

	// 重置数据
	ReSetGameState(bCloseFlag);

	return __super::GameFinish(bDeskStation,bCloseFlag);
}

//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	if (m_iUserStation[bDeskStation] != TYPE_PLAY_GAME ||m_GameData.m_bGiveUp[bDeskStation] == true)
	{
		return false;
	}
	else
	{
		return m_bGameStation >= GS_PLAY_GAME && m_bGameStation < GS_WAIT_NEXT_ROUND;
	}
	
	//return __super::IsPlayGame(bDeskStation);//m_bGameStation >= GS_SEND_CARD && m_bGameStation < GS_WAIT_NEXT;
}

//根据游戏判断是否在游戏中
BOOL CServerGameDesk::IsPlayingByGameStation() 
{
	return m_bGameStation >= GS_PLAY_GAME && m_bGameStation < GS_WAIT_NEXT_ROUND;
	//return 	__super::IsPlayingByGameStation();//m_bGameStation >= GS_SEND_CARD && m_bGameStation < GS_WAIT_NEXT;
}

bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:
		{
			bool bForceQuitAsAuto = true; 
			///查找上层路径
			CString szAppPath = CINIFile::GetAppPath();

			CString strBCFFile; 
			CString strKeyName; 
			strKeyName.Format("%d", NAME_ID); 
			strBCFFile.Format("%s\\SpecialRule.bcf", szAppPath); 

			CBcfFile File(strBCFFile);

			if (File.IsFileExist(strBCFFile))
			{
				bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto", strKeyName, 0); 
			}

			if (bForceQuitAsAuto == true)
			{
				return true; 
			}
			else
			{
				if (m_GameData.m_byTokenUser != bDeskStation 
					&& m_pUserInfo[bDeskStation] != NULL
					&&!m_GameData.m_bGiveUp[bDeskStation] 
				&&m_iUserStation[bDeskStation] == TYPE_PLAY_GAME)
				{
					m_GameData.m_bGiveUp[bDeskStation] = true;

					if (m_GameData.m_byNTUser == bDeskStation)
					{
						m_GameData.m_byNTUser = 255;
					}
					else if (m_GameData.m_bSmallBlind == bDeskStation)
					{
						// 小盲注玩家弃牌, 把小盲注至为255
						m_GameData.m_bSmallBlind = 255;
					}
					else if (m_GameData.m_bBigBlind == bDeskStation)
					{
						// 大盲注玩家弃牌, 把大盲注至为255
						m_GameData.m_bBigBlind = 255;
					}
				}
				else
				{
					TBet tagVerb;
					tagVerb.nType = ET_FOLD;
					OnUserBet(bDeskStation, &tagVerb, true);
				}
			}

			int iCount = 0; 
			for(int  i = 0; i < PLAY_COUNT; i++)
			{
				if(NULL != m_pUserInfo[i] && m_iUserStation[i] == TYPE_PLAY_GAME && m_GameData.m_bGiveUp[i] == false)
				{
					iCount ++ ;
				}
			}

			if(iCount < 2)
			{
				m_GameData.m_bGiveUpOver = true;
				GameFinish(0, GF_NORMAL);
			}

			return true; 
		}
	default:
		{
			break; 
		}
	}

	return __super::HandleFrameMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{

		// 	case ASS_GM_AGREE_GAME:		
		// 		{
		// 			//用户同意游戏
		// 			return OnUserAgreeGame(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		// 		}

	case ASS_CFG_DESK:
		{
			//代入金币限制
			if (bWatchUser)
			{
				return false;
			}
			//处理玩家主设置底注
			return OnUserCfgDesk(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
		break;

	case ASS_CFG_DESK_TIMEOUT:
		{
			//代入金币限制
			if (bWatchUser)
			{
				return true;
			}
			//设置底注超时
			return OnUserCfgDeskTimeout(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
		break;

	case ASS_AGREE_DESK_CFG:
		{
			//代入金币限制
			if (bWatchUser)
			{
				return false;
			}
			//处理玩家主同意底注
			return OnUserAgreeDeskCfg(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
		break;

	case ASS_SUB_MENOY:
		{
			//代入金币限制
			if (bWatchUser)
			{
				return false;
			}

			if (uSize != sizeof(TSubMoney))
			{
				return false;
			}

			if (m_bGameStation == GS_PLAY_GAME)
			{
				return true;
			}

			// 玩家代入金币设置
			return OnUserSubMoney(bDeskStation, pData);
		}
		break;
		
	case ASS_BET:
		{
			if (bWatchUser)
				return false;
			if( uSize != sizeof(TBet))
				return false;

			//处理玩家主不同意底注
			return OnUserBet(bDeskStation, pData);
		}
		break;

	case ASS_SEND_CARD_FINISH:
		{
			//代入金币限制
			if (bWatchUser)
			{
				return true;
			}
			//处理玩家发牌完成
			return OnUserSendCardFinish(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
		break;

	case ASS_NEXT_ROUND_REQ:
		{
			//代入金币限制
			if (bWatchUser)
			{
				return false;
			}
			//处理玩家对下一回合准备的信号
			return OnUserNextRoundReq(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
		break;
	case ASS_MONEY_LACK:
		{
			if (bWatchUser)
			{
				return true;
			}

			// 处理玩家本身金币不足于最少带入金额
			return OnUserMoneyLack(bDeskStation);
		}
		break;

#ifdef CLOSE_CARD
	case ASS_LOOK_CARD:		//玩家看牌
		{
			if(bWatchUser)
			{
				return true;
			}

			UserLookCardStruct s_userlookcard;
			s_userlookcard.bDeskStation = bDeskStation;

			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				if(m_pUserInfo[i] == NULL || i == bDeskStation)
					continue;

				SendGameData(i,&s_userlookcard,sizeof(UserLookCardStruct),MDM_GM_GAME_NOTIFY,ASS_LOOK_CARD,0);
			}

			return true;
		}
	case ASS_SHOW_CARD:	//展示牌消息
		{
			if(bWatchUser)
			{
				return true;
			}

			//展示牌数据
			ShowCardStruct s_showcard;
			s_showcard.byDeskStation = bDeskStation;
			s_showcard.iCardCount = m_GameData.m_UserData[bDeskStation].m_iCardsNum;
			::CopyMemory(s_showcard.byCardList,m_GameData.m_UserData[bDeskStation].m_byMyCards,sizeof(BYTE)*s_showcard.iCardCount);

			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				if(m_pUserInfo[i] == NULL)
					continue;

				SendGameData(i,&s_showcard,sizeof(ShowCardStruct),MDM_GM_GAME_NOTIFY,ASS_SHOW_CARD,0);
			}
			return true;
		}
#endif
		//游戏调试的消息处理
#ifdef CD_CAN_SETPAI

	case ASS_FINISH_ROUND_REQ:
		{
			//结束本轮游戏
			return OnTestFinishRoundRequest(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
		break;
#endif

	}

	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);

}

//玩家坐下
BYTE CServerGameDesk::UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo)
{
	//如果玩家桌桌后，桌面没有其他人，则获取桌面的配置
	int iSeatCount = 0;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pUserInfo[i])
		{
			iSeatCount++;
		}
	}

	if (iSeatCount == 0)
	{
		//清除原来的注额信息
		m_GameData.m_tagDeskCfg.Clear();

		LoadDeskCfg(m_pDataManage->m_InitData.uRoomID);
	}

	return __super::UserSitDesk(pUserSit,pUserInfo);
}

//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	/*CString str;
	str.Format("Server:-Server: 玩家%d, 离开游戏", bDeskStation);
	OutputDebugString(str);*/

	if (!IsPlayingByGameStation())
	{
		// 清除玩家
		m_GameData.m_UserMoney[bDeskStation] = 0;
		m_GameData.m_UserData[bDeskStation].m_bIn = false;
	}
	
	bool bShow = false;
	int nCount = 0;
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		// 判断存在的玩家并且不是当前退出的玩家
		if (m_pUserInfo[i] != NULL && i != bDeskStation)
		{
			nCount++;
		}
		else
		{
			bShow = true;
		}
	}

	if (nCount == 0 && IsPlayingByGameStation())
	{
		m_GameData.m_iDealPeople = 255;
		// 当前玩家退出后, 没有玩家了, 解散本桌
		OutputDebugString("Server: 当前玩家退出后, 没有玩家了, 解散本桌");
		GameFinish(bDeskStation,GF_SALE);
	}


	UserLeftDeskStruct  UserLeft;
	UserLeft.bDeskStation = bDeskStation ; 

	UserLeft.bClearLogo = (m_iUserStation[bDeskStation] == TYPE_PLAY_GAME /*&& m_bGameStation == GS_PLAY_GAME || m_bGameStation == GS_WAIT_ARGEE*/ && bShow) ; 

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL && i != bDeskStation)
		{
			SendGameData(i,&UserLeft,sizeof(UserLeft),MDM_GM_GAME_NOTIFY ,ASS_USER_LEFT_DESK,0);
		}
	}

	 return __super::UserLeftDesk(bDeskStation,pUserInfo);
}

///名称：UserReCome
///描述：用来截取玩家断线回来信息
bool CServerGameDesk::UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo)
{
	CString str;
	str.Format("Server: 玩家%d, 玩家断线重回", bDeskStation);
	OutputDebugString(str);

	m_GameData.m_UserData[bDeskStation].m_bNetCut = false;

	return __super::UserReCome(bDeskStation, pNewUserInfo);
}

//名称：UserNetCut
///描述：用来截取玩家断线信息
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo)
{
	m_GameData.m_UserData[bDeskStation].m_bNetCut = true;

	bool bRet = __super::UserNetCut(bDeskStation, pLostUserInfo);


	CString str;
	str.Format("Server: 玩家%d, 玩家断线", bDeskStation);
	OutputDebugString(str);

	int iPlayerCount = 0;
	int iNetCutCount = 0;
	
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pUserInfo[i])
		{
			iPlayerCount++;
		}
	}

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pUserInfo[i])
		{
			if (m_GameData.m_UserData[i].m_bNetCut)
			{
				iNetCutCount++;
			}
		}
	}

	if (iNetCutCount >= iPlayerCount)
	{
		m_GameData.m_iDealPeople = 255;
		OutputDebugString("Server:重置数据");
		// 重置数据
		ReSetGameState(GF_SALE);
		__super::GameFinish(bDeskStation,GF_SALE);
	}
	
	//switch(m_bGameStation)
	//{
	//case GS_PLAY_GAME:
	//	{
	//		
	//		;
	//	}
	//	break;
	//case GS_WAIT_NEXT_ROUND:
	//	{
	//		TNextRoundReq NextRoundReq;
	//		NextRoundReq.nType = TNextRoundReq::EXIT;
	//		OnUserNextRoundReq(bDeskStation,NULL,&NextRoundReq,sizeof(TNextRoundReq),0,false);
	//	}
	//	break;
	//default:
	//	break;
	//}

	return bRet;
}

///名称：CanNetCut
///描述：用来改变用户断线条件的函数
///@param bDeskStation 要断线玩家的位置
///@return 
bool CServerGameDesk::CanNetCut(BYTE bDeskStation)
{
	return true;
}


//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	//基本状态
	TGSBase tagGSBase;

	//获得游戏的基本代码
	GetGSBase(&tagGSBase,bDeskStation);

	CString str;
	str.Format("dxh: 玩家%d, 获取游戏状态信息", m_bGameStation);
	OutputDebugString(str);

	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		
	case GS_WAIT_ARGEE:
	case GS_WAIT_NEXT_ROUND:
		{
			m_GameData.m_UserMoney[bDeskStation] = 0;
			m_GameData.m_UserData[bDeskStation].m_bIn = false;
			//等待玩家开始状态
			return OnGetGSWaitAgree(bDeskStation,uSocketID,bWatchUser,&tagGSBase);
		}
		break;
	case GS_PLAY_GAME:
		{
			//游戏进行的状态中
			return OnGetGSPlaying(bDeskStation,uSocketID,bWatchUser,&tagGSBase);
		}
		break;
	//case GS_WAIT_NEXT_ROUND:
	//	{
	//		m_GameData.m_UserMoney[bDeskStation] = 0;
	//		m_GameData.m_UserData[bDeskStation].m_bIn = false;

	//		//等待下一轮游戏的开始
	//		return OnGetGSWaitNextRound(bDeskStation,uSocketID,bWatchUser,&tagGSBase);
	//	}
		break;
	}

	return false;
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	//KillAllTimer();

	//记得把定时器（MyTimer）完全清理掉，否则会严重影响这桌下一次游戏！
	KillTimer(IDT_MYTIMER);
	memset(m_arTimer,0,sizeof(m_arTimer));

	//事件流保护也需要关掉
	CloseEventProtection();

	m_GameData.Init();
	m_bGameStation = GS_WAIT_SETGAME;

	return TRUE;
}

//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	//用MyTimer托管MFC的Timer

	if (uTimerID == IDT_MYTIMER)
	{
		for (int i = 0; i < TIMER_COUNT; ++i)
		{
			if (m_arTimer[i].uiID == 0)
			{
				continue;
			}

			m_arTimer[i].nTickCount++;

			if (m_arTimer[i].nTickCount >= m_arTimer[i].nTriggerCount)
			{
				m_arTimer[i].nTickCount = 0;
				OnMyTimer(m_arTimer[i].uiID);
			}
		}
	}

	return __super::OnTimer(uTimerID);
}


/*-----------------------------------------------------------------------------------------------------------
一系列桌子特有机制函数实现，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
函括配置机制、计时机制、动作流机制、游戏状态的管理
----------------------------------------------------------------------------------------------------------*/

//加载限注
BOOL CServerGameDesk::LoadDeskCfg()
{
	TCHAR szIniName[MAX_PATH];
	GET_SERVER_INI_NAME(szIniName);

	CINIFile f(szIniName);

	CString strSeg = "game";
	CString strKey;

	TDeskCfg* pDeskCfg = &m_GameData.m_tagDeskCfg;

	//时间相关////////////////////////////////////////////////////////////////////////////

	//玩家设置玩家操作时间(秒)
	strKey = _TEXT("OperateTime");
	pDeskCfg->Time.byOperate = f.GetKeyVal(strSeg,strKey,10);
	if(pDeskCfg->Time.byOperate<10)
	{
		pDeskCfg->Time.byOperate=10;
	}

	//规则相关////////////////////////////////////////////////////////////////////////////

	//其它共性////////////////////////////////////////////////////////////////////////////

	strKey = _TEXT("ShowIdOrPlay");
	pDeskCfg->bShowUserInfo = f.GetKeyVal(strSeg, strKey, pDeskCfg->bShowUserInfo); 

	return TRUE;
}

//根据房间ID加载配置文件
BOOL CServerGameDesk::LoadDeskCfg(int iRoomID)
{
	// 加载通用配置
	LoadDeskCfg();

	TCHAR szIniName[MAX_PATH];
	GET_SERVER_INI_NAME(szIniName);

	CINIFile f(szIniName);

	CString strSeg;
	strSeg.Format("%d_%d",NAME_ID,iRoomID);

	CString strKey;

	TDeskCfg* pDeskCfg = &m_GameData.m_tagDeskCfg;

	//底注相关////////////////////////////////////////////////////////////////////////////
	__int64 iLowers[5] = {2, 5, 10, 20, 50};
	::memcpy(pDeskCfg->dz.iLowers,iLowers,sizeof(iLowers));

	pDeskCfg->dz.iRoomMultiple = f.GetKeyVal(strSeg, "RoomMultiple", 1); // 房间倍数
	if(pDeskCfg->dz.iRoomMultiple<1)
	{
		pDeskCfg->dz.iRoomMultiple=1;
	}
	pDeskCfg->dz.iLower = f.GetKeyVal(strSeg, "MinLimit", 100);			 // 游戏最低额
	pDeskCfg->dz.iUpper = f.GetKeyVal(strSeg, "MaxLimit", 200000000);	 // 游戏最高额
	pDeskCfg->dz.iBigBlindNote = pDeskCfg->dz.iLower;		             // 大盲注底注
	pDeskCfg->dz.iSmallBlindNote = pDeskCfg->dz.iLower / 2;		         // 小盲注底注

	pDeskCfg->dz.bIsRoomSubMoney = f.GetKeyVal(strSeg, "RoomSubMoney", 0);  // 房间代入金币  0: 此房间不需要代入金币  1: 此房间需要代入金币
	pDeskCfg->dz.iSubMinMoney = f.GetKeyVal(strSeg, "SubMinMoney",  1000);  // 房间最低代入金币
	pDeskCfg->dz.iSubMaxMoney = f.GetKeyVal(strSeg, "SubMaxMoney", 10000);  // 房间最高代入金币
	pDeskCfg->dz.iSubPlanMoney = f.GetKeyVal(strSeg, "SubPlanMoney",3000);  // 当前准备代入金币
	pDeskCfg->dz.iMinusMoney  = f.GetKeyVal(strSeg, "MinusMoney",100);		// 修改注额加减金币
	pDeskCfg->dz.iMinMoney   = f.GetKeyVal(strSeg, "MinMoney",100);			// 最小下注金币
	pDeskCfg->dz.iUntenMoney   = f.GetKeyVal(strSeg, "UntenMoney ",0);		// 少于多少金额时带入

	pDeskCfg->dz.iUserClose = f.GetKeyVal(strSeg, "UserClose", true);		// 一局后10秒内将玩家强退
	pDeskCfg->dz.iUserCloseTime = f.GetKeyVal(strSeg, "UserCloseTime", 10); // 关闭客户端时间
	pDeskCfg->dz.iUserInMoney = f.GetKeyVal(strSeg, "iUserInMoney", 10);	// 游戏金额带入时间

	for (int i = 0; i < 5; ++i)
	{
		pDeskCfg->dz.iLowers[i] *= pDeskCfg->dz.iLower * pDeskCfg->dz.iRoomMultiple;
	}

	//时间相关////////////////////////////////////////////////////////////////////////////

	//玩家设置桌子时间(秒)
	strKey = _TEXT("OperateTime");
	pDeskCfg->Time.byOperate = f.GetKeyVal(strSeg, strKey, pDeskCfg->Time.byOperate);
	
	//规则相关////////////////////////////////////////////////////////////////////////////

	//其它共性////////////////////////////////////////////////////////////////////////////
	strKey = _TEXT("ShowIdOrPlay");
	pDeskCfg->bShowUserInfo = f.GetKeyVal(strSeg,strKey,pDeskCfg->bShowUserInfo); 

	return TRUE;
}

BOOL CServerGameDesk::LoadChangCard(void)
{
	return true;
}

//初始分组表
void CServerGameDesk::InitGroupTable(void)
{
	return;
}

//创建游戏内部计时器
int CServerGameDesk::SetMyTimer(UINT nTimeID, int uElapse)
{
	int nElapse = uElapse / MY_ELAPSE;
	if (nElapse < 1)
	{
		nElapse = 1;
	}
	int nIdx = nTimeID - IDMYT_BASE;
	if (nIdx < 0 || nIdx > TIMER_COUNT - 1)
	{
		CString strCNOutput;
		strCNOutput.Format("SJServer：定时器[%d]创建不成功.",nTimeID);
		OutputDebugString(strCNOutput);
		return -1;
	}
	m_arTimer[nIdx].uiID = nTimeID;
	m_arTimer[nIdx].nTriggerCount = nElapse;
	return 0;
}


//删除游戏内部计时器
void CServerGameDesk::KillMyTimer(UINT nTimeID)
{
	int nIdx = nTimeID - IDMYT_BASE;
	if (nIdx < 0 || nIdx > TIMER_COUNT-1)
	{
		CString strCNOutput;
		strCNOutput.Format("SJServer：删除定时器[%d]创建不成功.",nTimeID);
		OutputDebugString(strCNOutput);
		return;
	}
	memset(&m_arTimer[nIdx],0,sizeof(TMyTimerNode));
}

//定时器消息
bool CServerGameDesk::OnMyTimer(UINT uTimerID)
{
	switch(uTimerID)
	{

		//处理事件流机制/////////////////////////////////////////////
	case IDEV_SYS_CFG_DESK: //系统设定桌面配置
		{
			KillMyTimer(IDEV_SYS_CFG_DESK);
			return OnEventSysCfgDesk();
		}
		break;

	case IDEV_USER_CFG_DESK: //玩家设定桌面配置
		{
			KillMyTimer(IDEV_USER_CFG_DESK);
			return OnEventUserCfgDesk();
		}
		break;

	case IDEV_RAND_CARD:	//洗牌
		{
			KillMyTimer(IDEV_RAND_CARD);
			return OnEventRandCard();
		}
		break;

	case IDEV_RANDOM:	//随机一个庄
		{
			KillMyTimer(IDEV_RANDOM);
			return OnEventRandom();
		}
		break;

	case IDEV_SENDACARD:	 //下发2底牌
		{
			KillMyTimer(IDEV_SENDACARD);
			return OnEventSendACard();
		}
		break;

	case IDEV_SEND_3_CARD: //下发3张公共底牌
		{
			KillMyTimer(IDEV_SEND_3_CARD);
			return OnEventSend3Card();
		}
		break;

	case IDEV_SEND_4_5_CARD: //下发3张公共底牌
		{
			KillMyTimer(IDEV_SEND_4_5_CARD);
			return OnEventSend1Card();
		}
		break;

	case IDEV_BET_START:	//首次下注事件
		{
			KillMyTimer(IDEV_BET_START);
			return OnEventBetStart();
		}
		break;

	case IDEV_BET_BLINDS_AUTO:	//大小盲注同时下注事件
		{
			KillMyTimer(IDEV_BET_BLINDS_AUTO);
			return OnEventBetBlindsAuto();
		}
		break;

	case IDEV_BET_NEXT:	//下位玩家下注事件
		{
			KillMyTimer(IDEV_BET_NEXT);
			return OnEventBetNext();
		}
		break;

	case IDEV_SMALL_BLIND:   //小肓注下注
		{
			KillMyTimer(IDEV_SMALL_BLIND);
		}
		break;

	case IDEV_BIGBLIND:   //大肓注下注
		{
			KillMyTimer(IDEV_BIGBLIND);

		}
		break;

	case IDEV_ROUND_FINISH: //回合结束
		{
			KillMyTimer(IDEV_ROUND_FINISH);
			//return OnEventRoundFinish();
		}
		break;
	case IDEV_COMPARE_CARD: //游戏比牌
		{
			KillMyTimer(IDEV_COMPARE_CARD);
			return OnCompareCard();
		}
		break;
	case IDEV_RESULT: //游戏结算
		{
			KillMyTimer(IDEV_RESULT);
			//return OnEventResult();
			return GameFinish(0, GF_NORMAL);
		}
		break;

	case IDEV_NEXT_ROUND_BEGIN: //下一回合开始
		{
			KillMyTimer(IDEV_NEXT_ROUND_BEGIN);
			return OnEventNextRoundBegin();
		}
		break;

    case IDEV_AUTO_OPERATE:   // 自动操作
        {
            KillMyTimer(IDEV_AUTO_OPERATE);
            return OnEventAutoOperate();
        }
        break;

		//事件流保护相关/////////////////////////////////////////////

	case IDMYT_EVENT_PROTECT:  //事件流保护定时器
		{
			return EventGoing();
		}
		break;
	}

	return __super::OnTimer(uTimerID);
}


//动作流控制
bool CServerGameDesk::ActionRedirect(UINT uiActID, int iDelay)
{
	switch(uiActID)
	{
	case IDEV_SYS_CFG_DESK:
		{
			//系统设定桌面配置
			OutputDebugString("DZPKServer：动作转向[IDEV_SYS_CFG_DESK.系统设定桌面配置].");
		}
		break;

	case IDEV_USER_CFG_DESK:
		{
			//玩家设定桌面配置
			OutputDebugString("DZPKServer：动作转向[IDEV_USER_CFG_DESK.玩家设定底注额].");
		}
		break;
		
	case IDEV_RANDOM:
		{
			//随机一个庄
			OutputDebugString("DZPKServer：动作转向[IDEV_RANDOM.随机一个庄].");
		}
		break;

	case IDEV_RAND_CARD:
		{
			//洗牌
			OutputDebugString("DZPKServer：动作转向[IDEV_RAND_CARD.洗牌].");
		}
		break;

	case IDEV_SENDACARD:
		{
			//发牌
			OutputDebugString("DZPKServer：动作转向[IDEV_SEND_CARD.发牌].");
		}
		break;

	case IDEV_SEND_3_CARD: 
		{
			//发3张公共牌
			OutputDebugString("DZPKServer：动作转向[IDEV_SEND_3_CARD.发3张公共牌].");
		}
		break;

	case IDEV_BET_START:
		{
			//首次下注
			OutputDebugString("DZPKServer：动作转向[IDEV_BET_START.新的一轮下注].");
		}
		break;

	case IDEV_BET_BLINDS_AUTO:
		{
			//大小盲注同时下注事件
			OutputDebugString("DZPKServer：动作转向[IDEV_BET_BLINDS_AUTO.大小盲注同时下注事件].");
		}
		break;

	case IDEV_BET_NEXT:
		{
			//下位玩家下注事件
			OutputDebugString("DZPKServer：动作转向[IDEV_BET_NEXT.下位玩家下注事件].");
		}
		break;

	case IDEV_ROUND_FINISH:
		{
			//一回合结束
			OutputDebugString("DZPKServer：动作转向[IDEV_ROUND_FINISH.回合结束].");
		}
		break;

	case IDEV_RESULT:
		{
			//回合结算
			OutputDebugString("DZPKServer：动作转向[IDEV_RESULT.回合结算].");
		}
		break;

	case IDEV_NEXT_ROUND_BEGIN:
		{
			//开始下一回合游戏
			OutputDebugString("DZPKServer：动作转向[IDEV_NEXT_ROUND_BEGIN.开始下一回合游戏].");
		}
	}

	SetMyTimer(uiActID,iDelay);

	return true;
}

//打开动作流保护程序
bool CServerGameDesk::OpenEventProtection(UINT uiEvID,int iLifeTime)
{
	if (m_EventProtecter.uiEvID > 0)
	{
		return false;
	}

	m_EventProtecter.uiEvID = uiEvID;
	m_EventProtecter.iLiftTime = iLifeTime;
	m_EventProtecter.iPassTime = 0;

	SetMyTimer(IDMYT_EVENT_PROTECT,1000);

	return true;
}

//处理预算已发牌数量的定时器
bool CServerGameDesk::EventGoing()
{
	if (m_EventProtecter.uiEvID == 0)
	{
		return true;
	}

	m_EventProtecter.iPassTime++;

	if (m_EventProtecter.iPassTime < m_EventProtecter.iLiftTime) 
	{
		return true;
	}

	//事件将要毁灭，根据m_epDescr.uiEvID提供必要的保护

	switch(m_EventProtecter.uiEvID)
	{
	case IDEV_SENDACARD:
		{
			//代管玩家发牌完成
			OnAgentSendCardsFinish();
		}
		break;
	case IDEV_NEXT_ROUND_BEGIN:
		{
			//下一局开始事件流代理
			OnAgentNextRoundBegin();
		}
		break;
	case IDEV_BET_NEXT:
		{
			// 自动操作
			OnEventAutoOperate();
		}
		break;
	}

	//自然毁灭事件
	CloseEventProtection();

	return false;
}

//消灭事件
bool CServerGameDesk::CloseEventProtection()
{
	//uiEvID事件保护利用完毕，销毁事件保护定时器
	KillMyTimer(IDMYT_EVENT_PROTECT);

	//清空事件保护上下文，以便为一下个事件保护提供资源
	::memset(&m_EventProtecter,0,sizeof(TEvProtectDescr));

	return false;
}

//获得游戏的基本的状态
void CServerGameDesk::GetGSBase(TGSBase* pGSBase,BYTE byDeskstation)
{
	//GSBase基本属性
	//当前事件已消耗的时间
	pGSBase->iEvPassTime = m_EventProtecter.iPassTime + 2; //考虑网络延时，加2秒

	//桌面基本配置
	::memcpy(&pGSBase->tagDeskCfg,&m_GameData.m_tagDeskCfg,sizeof(TDeskCfg));

	// 将游戏状态发至客户端
	pGSBase->bGameStation = m_bGameStation;

	CString strUserInfo;
	//获取各座位是否存在玩家
	for (int i = 0; i < PLAY_COUNT; ++i)
	{	
		pGSBase->bHaveUser[i] = (m_pUserInfo[i] != NULL);
	}

	return;
}

//获得游戏的基本的状态
void CServerGameDesk::CopyGSBase(TGSBase* pTarget,const TGSBase* pSource)
{
	if (!pTarget || !pSource)
	{
		return;
	}

	::memcpy(pTarget,pSource,sizeof(TGSBase));

	return;
}

/*-----------------------------------------------------------------------------------------------------------
游戏事件响应函数实现，功能函数实现，不同游戏差异较大
----------------------------------------------------------------------------------------------------------*/

//系统设定桌面的事件处理（主要设定时间、游戏规则等）
bool CServerGameDesk::OnEventSysCfgDesk()
{
	//清除原来的注额信息
	m_GameData.m_tagDeskCfg.Clear();

	LoadDeskCfg(m_pDataManage->m_InitData.uRoomID);

	// 广播设定底注的消息
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SendGameData(i,&m_GameData.m_tagDeskCfg,sizeof(TDeskCfg),MDM_GM_GAME_NOTIFY,ASS_SYS_DESK_CFG,0);
	}

	SendWatchData(m_bMaxPeople,&m_GameData.m_tagDeskCfg,sizeof(TDeskCfg),MDM_GM_GAME_NOTIFY,ASS_SYS_DESK_CFG,0);

	//启动用户设置桌面配置流程，主要完成（时间配置，规则配置的任务）

	ActionRedirect(IDEV_RAND_CARD, 1);

	return true;
}

//注额设定
bool CServerGameDesk::OnEventUserCfgDesk()
{

	//随机选择一个玩家，让TA设置底注，其余玩家等待设置

	int iPlayerNums = 0;
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			iPlayerNums++;
		}
	}

	if (iPlayerNums >= 2)
	{
		::srand((unsigned)::time(NULL)); 
		int iUserSel = rand()%iPlayerNums;

		iPlayerNums = 0;
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if (m_pUserInfo[i] != NULL)
			{
				if (iPlayerNums++ == iUserSel)
				{
					iUserSel = i;
					break;
				}
			}
		}

		//记录是那一个玩家需要设置底注
		//m_GameData.m_tagDeskCfg.dz.byUser = iUserSel;


		//可选择性的判断：由于四个玩家中有可能会有玩家的金币数小于顶注，那么这些顶注对应的底注不能被选择
		bool bBreak = false;
		for (int i = 0; i < 5; ++i)
		{
			for (int j = 0; j < PLAY_COUNT; ++j)
			{
				if (m_pUserInfo[j] != NULL)
				{
					if (m_pUserInfo[j]->m_UserData.i64Money < m_GameData.m_tagDeskCfg.dz.iUppers[i])
					{
						bBreak = true;
						break;
					}
				}
			}

			if (bBreak)
			{
				break;
			}

			//m_GameData.m_tagDeskCfg.dz.bSelects[i] = true;
		}


		//广播下注提示
		TCfgDesk tagCfgDesk;
		//复制可选的注值
		::memcpy(tagCfgDesk.dz.iLowers,m_GameData.m_tagDeskCfg.dz.iLowers,sizeof(tagCfgDesk.dz.iLowers));
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if (m_pUserInfo[i] != NULL)
			{
				tagCfgDesk.dz.bCanSet = (i == iUserSel);
				SendGameData(i,&tagCfgDesk,sizeof(TCfgDesk),MDM_GM_GAME_NOTIFY,ASS_CFG_DESK,0);
			}
		}

		tagCfgDesk.dz.bCanSet = false;
		SendWatchData(m_bMaxPeople,&tagCfgDesk,sizeof(TCfgDesk),MDM_GM_GAME_NOTIFY,ASS_CFG_DESK,0);

		//广播下注后，标志着游戏进入GS_CONFIG_NOTE状态（设置底注状态）
		//m_bGameStation = GS_CONFIG_NOTE;
	}

	return true;
}

//随机一个庄事件
bool CServerGameDesk::OnEventRandom()
{	
	::srand((unsigned)::time(NULL)); 

	if (m_GameData.m_iDealPeople == 255)
	{
		// 第一次随机庄
		m_GameData.m_iDealPeople = rand() % PLAY_COUNT;
		
		if (m_pUserInfo[m_GameData.m_iDealPeople] == NULL)
		{
			m_GameData.m_iDealPeople = GetNextUserStation(m_GameData.m_iDealPeople);
		}
	}
	else
	{
		// 轮流做庄
		m_GameData.m_iDealPeople = GetNextUserStation(m_GameData.m_iDealPeople);
	}

	m_GameData.m_byNTUser = m_GameData.m_iDealPeople;

	////////////////////////////////////////////////////

	if (GetPlayerCount() == 2)
	{
		m_GameData.m_bSmallBlind = m_GameData.m_byNTUser;
		m_GameData.m_bBigBlind = GetNextUserStation(m_GameData.m_byNTUser);		// 获取大盲注位置
	}
	else
	{
		m_GameData.m_bSmallBlind = GetNextUserStation(m_GameData.m_byNTUser);	// 获取小盲注位置
		m_GameData.m_bBigBlind = GetNextUserStation(m_GameData.m_bSmallBlind);	// 获取大盲注位置
	}

	// 广播新回合开始包
	TNextRoundBeginInfo tagNextRoundBeginInfo;
	tagNextRoundBeginInfo.byNTUser = m_GameData.m_byNTUser;
	tagNextRoundBeginInfo.bSmallBlind = m_GameData.m_bSmallBlind;
	tagNextRoundBeginInfo.bBigBlind = m_GameData.m_bBigBlind;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		// 通知游戏开始
		SendGameData(i,&tagNextRoundBeginInfo,sizeof(TNextRoundBeginInfo),MDM_GM_GAME_NOTIFY,ASS_NEW_ROUND_BEGIN,0);
	}

	SendWatchData(m_bMaxPeople,&tagNextRoundBeginInfo,sizeof(TNextRoundBeginInfo),MDM_GM_GAME_NOTIFY,ASS_NEW_ROUND_BEGIN,0);

	//启动洗牌流程
	ActionRedirect(IDEV_SENDACARD, 1);

	return true;
}

//洗牌事件
bool CServerGameDesk::OnEventRandCard()
{
	int iCardsNum = sizeof(m_GameData.m_byCards);
	GAlg::RandCard(m_GameData.m_byCards,m_GameData.m_tagDeskCfg.Rule.nPokeNum,m_bDeskIndex);

	//BYTE bCard[] = {0x08, 0x36, 0x03, 0x0A, 0x01, 0x05, 0x0C, 0x32, 0x17, 0x34, 0x14, 0x1B, 0x07, 0x21, 0x22};
	//memcpy(m_GameData.m_byCards, bCard, sizeof(bCard));

	//启动发牌流程
	ActionRedirect(IDEV_RANDOM, 1);
	
	return true;
}

//为每个玩家下发2张底牌的事件
bool CServerGameDesk::OnEventSendACard()
{
	TCards tagCards;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL && TYPE_PLAY_GAME == m_iUserStation[i]) //合法玩家
		{
			//为玩家i取2张牌
			m_GameData.DealCardsTo(i, MAX_DEAL_CARDS);
		}
	}

#ifdef SUPER
	OutputDebugString("dwjlog11302800::SUPER");
	//配制好牌
	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(CanGetGoodCard(i))
		{
			m_GameData.GetGoodCard(i);
			break;
		}
	}
#endif
	
	tagCards.iCardsNum = MAX_DEAL_CARDS;
	tagCards.nTypeCard = SEND_A_CAND;

	//广播发牌数据
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pUserInfo[i] != NULL && TYPE_PLAY_GAME == m_iUserStation[i]) //合法玩家
		{
			tagCards.byUser = i;

			//得到玩家i的手牌
			m_GameData.m_UserData[i].GetHandCards(tagCards.byCards);
			SendGameData(i, &tagCards,sizeof(TCards), MDM_GM_GAME_NOTIFY, ASS_SEND_A_CARD, 0);
		}
	}

	memset(tagCards.byCards,0,sizeof(tagCards.byCards));
	SendWatchData(m_bMaxPeople, &tagCards,sizeof(TCards),MDM_GM_GAME_NOTIFY, ASS_SEND_A_CARD, 0);

	OutputDebugString("dxh: 为每个玩家下发2张底牌的事件");
	//ActionRedirect(IDEV_BET_START, 1000);
	m_bFaiPaiFinish = false;

	OpenEventProtection(IDEV_SENDACARD, GetPlayerCount() * 2);

	return true;

}

//为发公牌的事件
bool CServerGameDesk::OnEventSend3Card()
{
	TCards tagCards;

	CString str;
	str.Format("dxh : 当前发牌位置: %d", m_GameData.m_iGetCardPos);
	OutputDebugString(str);
	memcpy(m_GameData.m_iCards, &m_GameData.m_byCards[m_GameData.m_iGetCardPos], MAX_THREE_CARDS);
	m_GameData.m_iGetCardPos += MAX_THREE_CARDS;
	m_GameData.m_iCardsCount += MAX_THREE_CARDS;

	memcpy(tagCards.byCards, m_GameData.m_iCards, sizeof(BYTE) * m_GameData.m_iCardsCount);
	tagCards.iCardsNum += m_GameData.m_iCardsCount;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SendGameData(i, &tagCards, sizeof(tagCards), MDM_GM_GAME_NOTIFY, ASS_SEND_3_CARD, 0);
	}	
	
	SendWatchData(m_bMaxPeople,&tagCards, sizeof(tagCards), MDM_GM_GAME_NOTIFY, ASS_SEND_3_CARD, 0);

	OutputDebugString("dxh: 为发公牌的事件");


	if (m_GameData.m_bAutoSendCard)
	{
		m_GameData.m_nTurnNums++;

		// 发一张公共牌
		ActionRedirect(IDEV_SEND_4_5_CARD, 1200);
	}
	else
	{
		ActionRedirect(IDEV_BET_START, 500);
	}

	return true;
}

//为发1张公共底牌的事件
bool CServerGameDesk::OnEventSend1Card()
{
	CString str;
	str.Format("dxh : 当前发牌位置: %d", m_GameData.m_iGetCardPos);
	OutputDebugString(str);

	if (m_GameData.m_iCardsCount >= 5)
	{
		OutputDebugString("Server: 公共牌最多5张牌");
		return true;
	}

	TCards tagCards;
	memcpy(&m_GameData.m_iCards[m_GameData.m_iCardsCount], &m_GameData.m_byCards[m_GameData.m_iGetCardPos], MAX_THREE_CARDS);
	m_GameData.m_iGetCardPos += MAX_FIRST_CARDS;
	m_GameData.m_iCardsCount += MAX_FIRST_CARDS;

	memcpy(tagCards.byCards, m_GameData.m_iCards, sizeof(BYTE) * m_GameData.m_iCardsCount);
	tagCards.iCardsNum = m_GameData.m_iCardsCount;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SendGameData(i, &tagCards, sizeof(tagCards), MDM_GM_GAME_NOTIFY, ASS_SEND_4_5_CARD, 0);
	}	

	SendWatchData(m_bMaxPeople,&tagCards, sizeof(tagCards), MDM_GM_GAME_NOTIFY, ASS_SEND_4_5_CARD, 0);

	if (m_GameData.m_bAutoSendCard)
	{
		if (m_GameData.m_nTurnNums < 3)
		{
			m_GameData.m_nTurnNums++;

			// 发一张公共牌
			ActionRedirect(IDEV_SEND_4_5_CARD, 1200);
		}
		else 
		{
			// 游戏结束, 进行结算比牌
			OutputDebugString("dxh:  OnEventSend1Card中, 游戏结束, 进行结算比牌");
			ActionRedirect(IDEV_COMPARE_CARD, 500);
			return true;
		}
	}
	else
	{
		ActionRedirect(IDEV_BET_START, 500);
	}

	return true;
}

//首次下注事件
bool CServerGameDesk::OnEventBetStart()
{
	OutputDebugString("Server:首次下注事件");
	//首位玩家下注意味当前游戏的活动轮数加1
	m_GameData.m_nTurnNums++;
	for (int i = 0; i < m_GameData.m_nTurnNums; i++)
	{
		SendGameData(i, MDM_GM_GAME_NOTIFY, ASS_SORT_OUT, 0);
	}

	SendWatchData(m_bMaxPeople, MDM_GM_GAME_NOTIFY, ASS_SORT_OUT, 0);

	// 进行小肓注下注
	//如果当前是第一轮下注，转向大小盲注自动下注流程\
	从第二轮开始则转向下位玩家下注流程(也就从小盲注开始)
	m_GameData.m_byTokenUser = GetLastUserStation(m_GameData.m_bSmallBlind);

	CString str;
	str.Format("YY:m_GameData.m_byTokenUser == %d",m_GameData.m_byTokenUser);
	OutputDebugString(str);

	m_GameData.m_iBetCounts = 0;
	m_GameData.m_iCallCounts = 0;
	m_GameData.m_iRaiseCounts = 0;
	//m_GameData.m_iAllinUser = 0;
	m_GameData.m_iCheckCount = 0;
	//m_GameData.m_iFoldCount = 0;
	m_GameData.m_bFirstStation = 255;

	OutputDebugString("Server: 首次下注事件");

    // 新的一轮开始
    m_GameData.m_bNewTurn = true;
	// 新的一轮开始
	m_GameData.m_bNewTurns = true;
	

	if(m_GameData.m_nTurnNums == 1)
	{
		// 第一轮只可以跟注、加注、全下、弃牌
		m_GameData.MakeVerbs(UD_VF_CALL|UD_VF_ADD|UD_VF_FOLD|UD_VF_ALLIN);

		// 进行大小盲注下注
		ActionRedirect(IDEV_BET_BLINDS_AUTO, 500);
	}
	else
	{
		// 第一轮之后每轮开始可以下注、弃牌、过牌、全下
		m_GameData.MakeVerbs(UD_VF_BET|UD_VF_FOLD|UD_VF_CHECK|UD_VF_ALLIN);

		// 新的一轮开始
		ActionRedirect(IDEV_BET_NEXT, 500);
	}

	return true;
}

//大小盲注强制同时下注事件
bool CServerGameDesk::OnEventBetBlindsAuto()
{
	TBet tagBet;
	tagBet.nType = ET_AUTO;

	//小盲注强制下注
	m_GameData.m_byTokenUser = m_GameData.m_bSmallBlind;

	//小盲注强制下注
	tagBet.byUser = m_GameData.m_byTokenUser;
	tagBet.nMoney = m_GameData.m_tagDeskCfg.dz.iSmallBlindNote;

	// 模拟下注消息
	OnUserBet(m_GameData.m_bSmallBlind, &tagBet);
	CString str;
	str.Format("dxh: ------------- 下注玩家: %d", m_GameData.m_byTokenUser);
	OutputDebugString(str);

	//大盲注强制下注
	m_GameData.m_byTokenUser = m_GameData.m_bBigBlind;

	//大盲注强制下注
	tagBet.byUser = m_GameData.m_byTokenUser;
	tagBet.nMoney = m_GameData.m_tagDeskCfg.dz.iBigBlindNote;

	// 模拟下注消息
	OnUserBet(m_GameData.m_bBigBlind, &tagBet);

	str.Format("dxh: ------------- 下注玩家: %d", m_GameData.m_byTokenUser);
	OutputDebugString(str);

	// 通知大盲注下一个玩家开始下注
	ActionRedirect(IDEV_BET_NEXT, 500);

	return true;
}

//下一位玩家下注事件
bool CServerGameDesk::OnEventBetNext()
{
	// 移交令牌到下一个玩家
	m_GameData.m_byTokenUser = GetNextUserStation(m_GameData.m_byTokenUser);

	//广播令牌
	TToken tagToken;
	tagToken.byUser = m_GameData.m_byTokenUser;
	tagToken.byVerbFlag = m_GameData.m_byVerbFlag;
    tagToken.bNewTurn = m_GameData.m_bNewTurn;
	tagToken.bNewTurns = m_GameData.m_bNewTurns;
	tagToken.nTurnNums = m_GameData.m_nTurnNums;
	tagToken.nCallMoney = m_GameData.m_nMaxBetMoney - m_GameData.GetBetMoney(m_GameData.m_byTokenUser);
	
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			SendGameData(i,&tagToken,sizeof(TToken),MDM_GM_GAME_NOTIFY,ASS_TOKEN,0);
		}
	}	

	SendWatchData(m_bMaxPeople,&tagToken,sizeof(TToken),MDM_GM_GAME_NOTIFY,ASS_TOKEN,0);

    // 开启自动操作流程, 客户端在规则的时间内操作, 则服务器自动处理操作
	OpenEventProtection(IDEV_BET_NEXT,m_GameData.m_tagDeskCfg.Time.byOperate + 10);
    //ActionRedirect(IDEV_AUTO_OPERATE, (m_GameData.m_tagDeskCfg.Time.byOperate + 10) * 1000);
	return true;
}

//开始下一回合游戏事件处理
bool CServerGameDesk::OnEventNextRoundBegin()
{
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			if (!m_GameData.m_UserData[i].m_bIn)
			{
				return true;
			}

			m_pUserInfo[i]->m_UserData.bUserState = USER_ARGEE;
		}
	}

	OutputDebugString("dxh: OnEventNextRoundBegin");
	GameBegin(ALL_ARGEE);
	return true;
}

// 比牌
bool CServerGameDesk::OnCompareCard()
{
	OutputDebugString("dxh: 进行比牌流程");

	// 进行比牌流程
	TCompareCard cmd;
	memset(&cmd, 0, sizeof(TCompareCard));

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] == NULL || m_GameData.m_bGiveUp[i])
		{
			continue;
		}

		// 得到玩家手牌
		memcpy(cmd.bHandCards[i], m_GameData.m_UserData[i].m_byMyCards, sizeof(cmd.bHandCards[i]));
		cmd.nHandCardCount[i] = 2;

		CString str,str1;
		str.Format("jan: 玩家位置: %d, 用户ID:%d, 手牌: ", i, m_pUserInfo[i]->m_UserData.dwUserID);

		for (int j = 0; j < 2; j++)
		{
			BYTE bNumber = m_GameData.m_UserData[i].m_byMyCards[j] & 0x0F;
			BYTE bShape = (m_GameData.m_UserData[i].m_byMyCards[j] & 0xF0) >> 4;
			str1.Format("%s%s(0x%x) ", szShape[bShape], szNumber[bNumber],  m_GameData.m_UserData[i].m_byMyCards[j]);
			str += str1;
		}

		str += "  公共牌:";
		for (int j = 0; j < 5; j++)
		{
			BYTE bNumber = m_GameData.m_iCards[j] & 0x0F;
			BYTE bShape = (m_GameData.m_iCards[j] & 0xF0)>>4;
			str1.Format("%s%s(0x%x) ", szShape[bShape], szNumber[bNumber], m_GameData.m_iCards[j]);
			str += str1;
		}

		OutputDebugString(str);

		BYTE byResultCard[7];
		memset(byResultCard, 0, sizeof(byResultCard));

		// 分析牌型
		cmd.nCardKind[i] = m_Logic.AnalysisCard(m_GameData.m_UserData[i].m_byMyCards, 2, m_GameData.m_iCards, 5, byResultCard);
		cmd.nCardCount = 5;
		memcpy(m_GameData.m_bResultCards[i],byResultCard, sizeof(BYTE) * cmd.nCardCount);  // 五张最大的牌
		memcpy(cmd.bCardsEx[i], &byResultCard[cmd.nCardCount], sizeof(BYTE) * 2);           // 剩余二张没用的牌

		memcpy(cmd.bCards[i], m_GameData.m_bResultCards[i], sizeof(BYTE) * cmd.nCardCount);
		
		
		str.Format("jan: 玩家位置: %d, 用户昵称:%s,  牌型: ID:%d  名称:%s, 牌值: ", i, m_pUserInfo[i]->m_UserData.nickName,  cmd.nCardKind[i], szName[cmd.nCardKind[i]]);
		OutputDebugString(str);

		for (int j = 0; j < 5; j++)
		{
			BYTE bNumber = cmd.bCards[i][j] & 0x0F;
			BYTE bShape = (cmd.bCards[i][j] & 0xF0) >> 4;
			str1.Format("%s%s(0x%x) ", szShape[bShape], szNumber[bNumber], cmd.bCards[i][j]);
			str += str1;
		}

		OutputDebugString(str);
	}

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		// 通知客户端
		SendGameData(i, &cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_COMPARE_CARD, 0);
	}
	
	SendWatchData(m_bMaxPeople,&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_COMPARE_CARD, 0);



	OutputDebugString("Server: 进入结算流程");

	// 进入结算流程
	ActionRedirect(IDEV_RESULT, 1500);
	return true;
}

///名称：OnUserAgreeGame
///描述：处理玩家同意游戏
///@param bDeskStation 位置, pNetHead 消息头,pData 消息数据,uSize 消息包长度,uSocketID id,bWatchUser 是否旁观
///@return 
bool CServerGameDesk::OnUserAgreeGame(BYTE bDeskStation,NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser)
{
	//只要有玩家同意游戏，意味着游戏进入等待其它玩家同意的状态
	//m_bGameStation = GS_WAIT_ARGEE;
	OutputDebugString("dxh: OnUserAgreeGame");
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}


///名称：OnUserSetDiZhu
///描述：处理玩家主设置底注
///@param bDeskStation 位置, pNetHead 消息头,pData 消息数据,uSize 消息包长度,uSocketID id,bWatchUser 是否旁观
///@return 
bool CServerGameDesk::OnUserCfgDesk(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	TCfgDesk* pCfgDesk = (TCfgDesk*)pData;

	//这时该玩家应被认定同意游戏
	m_GameData.SetAgreeStateTo(bDeskStation);

	//记录底注值，通过底注计算顶注
	m_GameData.m_tagDeskCfg.dz.iLower = m_GameData.m_tagDeskCfg.dz.iLowers[pCfgDesk->dz.bySelect];
	m_GameData.m_tagDeskCfg.dz.iUpper = m_GameData.m_tagDeskCfg.dz.iUppers[pCfgDesk->dz.bySelect];

	//广播设定底注的消息
	for (int i  = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			SendGameData(i,&m_GameData.m_tagDeskCfg,sizeof(TDeskCfg),MDM_GM_GAME_NOTIFY,ASS_DESK_CFG,0);
		}
	}

	SendWatchData(m_bMaxPeople,&m_GameData.m_tagDeskCfg,sizeof(TDeskCfg),MDM_GM_GAME_NOTIFY,ASS_DESK_CFG,0);

	//广播下注后，标志着游戏进入GS_AGREE_NOTE状态（同意底注状态）
	//m_bGameStation = GS_AGREE_NOTE;

	return true;
}

///名称：OnUserSetNoteTimeout
///描述：处理玩家主设置底注超时
///@param bDeskStation 位置, pNetHead 消息头,pData 消息数据,uSize 消息包长度,uSocketID id,bWatchUser 是否旁观
///@return 
bool CServerGameDesk::OnUserCfgDeskTimeout(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	//这时该玩家应被认定不同意游戏，修改用户对象，设用户状态
	m_GameData.SetDisAgreeStateTo(bDeskStation);

	//下发定注超时消息，并解散桌子
	for (int i  = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL && i != bDeskStation)
		{
			SendGameData(i,MDM_GM_GAME_NOTIFY,ASS_CFG_DESK_TIMEOUT,0);
		}
	}

	GameFinish(0,GF_SALE);

	return true;
}

///名称：OnUserAgreeDiZhu
///描述：处理玩家主同意底注
///@param bDeskStation 位置, pNetHead 消息头,pData 消息数据,uSize 消息包长度,uSocketID id,bWatchUser 是否旁观
///@return 
bool CServerGameDesk::OnUserAgreeDeskCfg(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{

	//if (m_pUserInfo[bDeskStation] == NULL)
	//{
	//	return false;
	//}

	////修改用户对象，设用户状态
	//m_GameData.SetAgreeStateTo(bDeskStation);

	////广播玩家bDeskStation同意消息

	//TAgree tagAgree;
	//tagAgree.byUser = bDeskStation;
	//tagAgree.bAgree = true;

	////是否所有人都同意游戏
	//tagAgree.bAllAgree = (m_GameData.GetAgreeNums() == PLAY_COUNT);

	//for (int i = 0; i < PLAY_COUNT; ++i)
	//{
	//	if (m_pUserInfo[i])
	//	{
	//		SendGameData(i,&tagAgree,sizeof(TAgree),MDM_GM_GAME_NOTIFY,ASS_AGREE,0);
	//	}
	//}

	////如果所有玩家都同意底注的设定，则启动发牌流程
	//if (tagAgree.bAllAgree)
	//{
	//	//广播所有玩家准备好（tagAgree.bAllAgree = true）的通知后，标志着游戏进入GS_PLAY_GAME状态（游戏过程状态）\
	//	  也标志着游戏的进入第一回合，回合计数要加1
	//	
	//	m_bGameStation = GS_PLAY_GAME;

	//	//启动洗牌流程
	//	ActionRedirect(IDEV_RAND_CARD,500);
	//}

	return true;
}

// 玩家代入金币
bool CServerGameDesk::OnUserSubMoney(BYTE bDeskStation, void* pData)
{
	TSubMoney *pCmd = (TSubMoney*)pData;

	if (pCmd == NULL)
	{
		return false;
	}

	if (m_pUserInfo[bDeskStation] == NULL)
	{
		return false;
	}

	if (pCmd->nMoney[bDeskStation] <= m_GameData.m_tagDeskCfg.dz.iUntenMoney
	|| m_pUserInfo[bDeskStation]->m_UserData.i64Money < pCmd->nMoney[bDeskStation]
	|| m_pUserInfo[bDeskStation]->m_UserData.i64Money < m_GameData.m_tagDeskCfg.dz.iSubMinMoney)
	{
		// 不合法设置
		return true;
	}

	if (pCmd->nMoney[bDeskStation] < m_GameData.m_tagDeskCfg.dz.iSubMinMoney) 
	{
		// 玩家当前带入金额少于最低带入值时，将最低带入值赋予当前带前带入金额
		pCmd->nMoney[bDeskStation] = m_GameData.m_tagDeskCfg.dz.iSubMinMoney;
	}
	else if (pCmd->nMoney[bDeskStation] > m_GameData.m_tagDeskCfg.dz.iSubMaxMoney)
	{
		// 玩家当前带入金额大于最高带入值时，将最高带入值赋予当前带前带入金额
		pCmd->nMoney[bDeskStation] = m_GameData.m_tagDeskCfg.dz.iSubMaxMoney;
	}

	if (pCmd->nMoney[bDeskStation] > m_pUserInfo[bDeskStation]->m_UserData.i64Money)
	{
		pCmd->nMoney[bDeskStation] = m_pUserInfo[bDeskStation]->m_UserData.i64Money;
	}

	m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_ARGEE;

	m_GameData.m_UserData[bDeskStation].m_bIn = true;

	// 存入代入金额
	m_GameData.m_UserMoney[bDeskStation] = pCmd->nMoney[pCmd->bDeskStation];
	
	// 设置当前可用金币
	TSubMoney pBag;
	pBag.bDeskStation = bDeskStation;
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			pBag.bIn[i] = m_GameData.m_UserData[i].m_bIn;
			
			CString str,str1;

			str.Format("Server游戏代入数据-Server:游戏桌号:%d,玩家ID:%d,玩家昵称:%s,玩家代入金额:%I64d,玩家真实金额:%I64d",
				m_bDeskIndex,m_pUserInfo[i]->m_UserData.dwUserID,m_pUserInfo[i]->m_UserData.szName,pCmd->nMoney[i],
				m_pUserInfo[i]->m_UserData.i64Money);

			
			str1.Format("Server: 玩家:%d(位置:%d), 代入金币: %I64d, m_bIn: %d", m_pUserInfo[i]->m_UserData.dwUserID, i, m_GameData.m_UserMoney[i],  m_GameData.m_UserData[i].m_bIn);
			OutputDebugString(str1);
		}
	}

	memcpy(pBag.nMoney,m_GameData.m_UserMoney,sizeof(m_GameData.m_UserMoney));
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SendGameData(i, &pBag,sizeof(pBag),  MDM_GM_GAME_NOTIFY, ASS_SHOW_IN_MONEY, 0);
	}
	
	SendWatchData(m_bMaxPeople,&pBag,sizeof(pBag),  MDM_GM_GAME_NOTIFY, ASS_SHOW_IN_MONEY, 0);

	int nCount = 0;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			nCount++;
			if (false == m_GameData.m_UserData[i].m_bIn)
			{
				OutputDebugString("Server: 有玩家未代入金币");
				return true;
			}
		}	
	}

	if (nCount >= 2)
	{
		// 游戏开始
		GameBegin(ALL_ARGEE);
	}

	return true;
}

///名称：OnUserBet
///描述：处理玩家下注信息
///@param bDeskStation 位置, pData 消息数据
///@return 
bool CServerGameDesk::OnUserBet(BYTE bDeskStation, void* pData ,bool bUserforceExit)
{
	OutputDebugString("Server: 处理玩家下注信息");
	
	// 不是当前操作玩家
	if (m_GameData.m_byTokenUser != bDeskStation)
	{
		OutputDebugString("Server: 不是当前操作玩家");
		return false;
	}
	// 玩家不存在 或 玩家已弃牌 或 不可下注玩家 或 中途进入玩家
	if (m_pUserInfo[bDeskStation] == NULL || m_GameData.m_bGiveUp[bDeskStation] || m_GameData.m_bNoBet[bDeskStation])
	{
		OutputDebugString("Server: 玩家不存在或玩家已弃牌");
		return true;
	}

	if (m_GameData.m_bAutoSendCard)
	{
		OutputDebugString("Server: 已经发所有牌, 所有玩家都不可进行操作, 等待游戏结束操作");
		return true;
	}

	CString str;

    // 停止自动定时操作
    KillMyTimer(IDEV_AUTO_OPERATE);

	TBet * pCmd = (TBet *)pData;

	switch (pCmd->nType)
	{
	case ET_BET:     // 下注
	case ET_AUTO:
		{
			if (pCmd->nMoney < 0 || pCmd->nMoney > m_GameData.m_UserMoney[bDeskStation])
			{
				// 下注额超过本人自身金币, 则把自己自身金币全下
				pCmd->nMoney = m_GameData.m_UserMoney[bDeskStation];
				str.Format("Server:下注额超过本人自身金币, 则把自己自身金币全下:%I64d",pCmd->nMoney);
				OutputDebugString(str);
			}

			m_GameData.m_UserMoney[bDeskStation] -= pCmd->nMoney;
			m_GameData.m_nBetMoney[bDeskStation][m_GameData.m_nBetPool] += pCmd->nMoney;

			if (m_GameData.m_UserMoney[bDeskStation] == 0)
			{
				// 当作全下处理
				m_GameData.m_bNoBet[bDeskStation] = true;
				pCmd->nType = ET_ALLIN;
				m_GameData.m_bAllBetStation = bDeskStation;
				m_GameData.m_iAllinUser++;
				m_GameData.m_byAllPools[m_GameData.m_nBetPool]++;
				m_GameData.m_bAllBet[bDeskStation][m_GameData.m_nBetPool] = true;
			}

			int nAllBetMoney = m_GameData.GetBetMoney(bDeskStation);
			if (m_GameData.m_nMaxBetMoney < nAllBetMoney)
			{
				m_GameData.m_nMaxBetMoney = nAllBetMoney;  // 记录最多的金币
			}

			m_GameData.m_bBetStation = bDeskStation;
			m_GameData.m_iBetCounts++;
			CString str;
			str.Format("Server:m_GameData.m_bBetStation == %d,bDeskStation == %d",m_GameData.m_bBetStation,bDeskStation);
			OutputDebugString(str);
		}
		break;
	case ET_CALL:    // 跟注
		{
			// 2012-10-11 duanxiaohui add 防止客户端乱跟金币, 保证跟注为一轮中最大的下注
			int nAllMoeny = m_GameData.GetBetMoney(bDeskStation);
			if ((nAllMoeny + pCmd->nMoney) < m_GameData.m_nMaxBetMoney)
			{
				pCmd->nMoney = m_GameData.m_nMaxBetMoney - nAllMoeny;   // 跟注异常了, 这里计算出应该要跟的金币
			}		
			// end duanxiaohui

			if (pCmd->nMoney < 0 || pCmd->nMoney > m_GameData.m_UserMoney[bDeskStation])
			{
				// 跟注额超过本人自身金币, 则把自己自身金币全跟
				pCmd->nMoney = m_GameData.m_UserMoney[bDeskStation];
			}

			m_GameData.m_UserMoney[bDeskStation] -= pCmd->nMoney;
			m_GameData.m_nBetMoney[bDeskStation][m_GameData.m_nBetPool] += pCmd->nMoney;

			if (m_GameData.m_UserMoney[bDeskStation] == 0)
			{
				// 当作全下处理
				m_GameData.m_bNoBet[bDeskStation] = true;
				pCmd->nType = ET_ALLIN;
				m_GameData.m_bAllBetStation = bDeskStation;
				m_GameData.m_iAllinUser++;
				m_GameData.m_byAllPools[m_GameData.m_nBetPool]++;
				m_GameData.m_bAllBet[bDeskStation][m_GameData.m_nBetPool] = true;
			}
			else
			{
				m_GameData.m_iCallCounts++;
			}

			int nAllBetMoney = m_GameData.GetBetMoney(bDeskStation);
			if (m_GameData.m_nMaxBetMoney < nAllBetMoney)
			{
				m_GameData.m_nMaxBetMoney = nAllBetMoney;  // 记录最多的金币
			}
		}
		break;
	case ET_ADDNOTE: // 加注    
		{ 
			if (pCmd->nMoney < 0 || pCmd->nMoney > m_GameData.m_UserMoney[bDeskStation])
			{
				// 加注额超过本人自身金币, 则把自己自身金币全加
				pCmd->nMoney = m_GameData.m_UserMoney[bDeskStation];
			}

			m_GameData.m_UserMoney[bDeskStation] -= pCmd->nMoney;
			m_GameData.m_nBetMoney[bDeskStation][m_GameData.m_nBetPool] += pCmd->nMoney;

			if (m_GameData.m_UserMoney[bDeskStation] == 0)
			{
				// 当作全下处理
				m_GameData.m_bNoBet[bDeskStation] = true;
				pCmd->nType = ET_ALLIN;
				m_GameData.m_bAllBetStation = bDeskStation;
				m_GameData.m_iAllinUser++;
				m_GameData.m_byAllPools[m_GameData.m_nBetPool]++;
				m_GameData.m_bAllBet[bDeskStation][m_GameData.m_nBetPool] = true;
			}
			else
			{
				m_GameData.m_iRaiseCounts++;
			}

			int nAllBetMoney = m_GameData.GetBetMoney(bDeskStation);
			if (m_GameData.m_nMaxBetMoney < nAllBetMoney)
			{
				m_GameData.m_nMaxBetMoney = nAllBetMoney;  // 记录最多的金币
			}

			m_GameData.m_bRaiseStation = bDeskStation;

			CString str;
			str.Format("Server:m_GameData.m_bRaiseStation == %d,bDeskStation == %d",m_GameData.m_bRaiseStation,bDeskStation);
			OutputDebugString(str);
		}
		break;
	case ET_ALLIN:   // 全下
		{
			pCmd->nMoney = m_GameData.m_UserMoney[bDeskStation];
			
			str.Format("Server:m_UserMoney = %I64d",m_GameData.m_UserMoney[bDeskStation]);
			OutputDebugString(str);

			m_GameData.m_bNoBet[bDeskStation] = true;
			m_GameData.m_bAllBetStation = bDeskStation;
			m_GameData.m_UserMoney[bDeskStation] -= pCmd->nMoney;
			m_GameData.m_nBetMoney[bDeskStation][m_GameData.m_nBetPool] += pCmd->nMoney; 
			m_GameData.m_iAllinUser++;
			m_GameData.m_byAllPools[m_GameData.m_nBetPool]++;
			m_GameData.m_bAllBet[bDeskStation][m_GameData.m_nBetPool] = true;

			__int64 nAllBetMoney = m_GameData.GetBetMoney(bDeskStation);
			if (m_GameData.m_nMaxBetMoney < nAllBetMoney)
			{
				m_GameData.m_nMaxBetMoney = nAllBetMoney;  // 记录最多的金币
			}
		}
		break; 
	case ET_CHECK:   // 过牌
		{
			if (m_GameData.m_bFirstStation == 255)
			{
				m_GameData.m_bFirstStation = bDeskStation;  // 记录第一个过牌玩家
			}

			m_GameData.m_iCheckCount++;
		}
		break;
	case ET_FOLD:    // 弃牌
		{
			m_GameData.m_bGiveUp[bDeskStation] = true;

			if(!bUserforceExit)
			{
				m_GameData.m_iFoldCount++;
			}

			if (m_GameData.m_byNTUser == bDeskStation)
			{
				m_GameData.m_byNTUser = 255;
			}
			else if (m_GameData.m_bSmallBlind == bDeskStation)
			{
				// 小盲注玩家弃牌, 把小盲注至为255
				m_GameData.m_bSmallBlind = 255;
			}
			else if (m_GameData.m_bBigBlind == bDeskStation)
			{
				// 大盲注玩家弃牌, 把大盲注至为255
				m_GameData.m_bBigBlind = 255;
			}
			
		}
		break;
	default:
		break;
	}

	str.Format("Server: 玩家:%d, 下注:%I64d, 第%I64d个下注池, 剩余金币:%I64d",bDeskStation, pCmd->nMoney,m_GameData.m_nBetPool+1, m_GameData.m_UserMoney[bDeskStation]);
	OutputDebugString(str);

	// 将下注数据发送到客户端显示
	TBet tagAnteVerb;
	tagAnteVerb.byUser = bDeskStation;
	tagAnteVerb.nType = pCmd->nType;
	tagAnteVerb.nMoney = pCmd->nMoney;
	//memcpy(tagAnteVerb.bUserman,m_GameData.m_bGiveUp,sizeof(m_GameData.m_bGiveUp));
	tagAnteVerb.nBetMoney = m_GameData.m_nMaxBetMoney;

	str.Format("Server:下注金额：%I64d",tagAnteVerb.nBetMoney);
	OutputDebugString(str);

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			SendGameData(i, &tagAnteVerb, sizeof(tagAnteVerb), MDM_GM_GAME_NOTIFY, ASS_BET_INFO,0);
		}
	}

	SendWatchData(m_bMaxPeople,&tagAnteVerb, sizeof(tagAnteVerb), MDM_GM_GAME_NOTIFY, ASS_BET_INFO,0);
	CloseEventProtection();

	// 判断下一步操作
	if (pCmd->nType != ET_AUTO)
	{
		if (m_GameData.m_iFoldCount >= GetPlayerCount(true) - 1)
		{
			OutputDebugString("Server: 所有玩家弃牌, 游戏结束");
			m_GameData.m_bGiveUpOver = true;

#ifdef CLOSE_CARD
			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				//玩家在游戏中 并且没有 放弃的玩家 发送能展示牌的消息
				if(NULL != m_pUserInfo[i] && !m_GameData.m_bGiveUp[i])
				{
					SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,ASS_CAN_SHOW_CARD,0);
				}
			}

			// 所有玩家弃牌, 游戏结束
			ActionRedirect(IDEV_RESULT, 4000);
#else
			// 所有玩家弃牌, 游戏结束
			ActionRedirect(IDEV_RESULT, 1000);
#endif
			return true;
		}

		bool bEqual = false;

		//if (m_GameData.m_bBigBlind == bDeskStation)		// 玩家位置等于大盲注
		{
			/*CString str;
			str.Format("Server:玩家位置等于大盲注__玩家%d全下%I64d",m_GameData.m_nBetMoney[bDeskStation][m_GameData.m_nBetPool]);
			OutputDebugString(str);*/

			bEqual = true;

			__int64 nMoney = m_GameData.m_nBetMoney[bDeskStation][m_GameData.m_nBetPool];

			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (i == bDeskStation || m_pUserInfo[i] == NULL || m_GameData.m_bGiveUp[i] || m_GameData.m_bNoBet[i])
				{
					continue;
				}
			
				if (nMoney != m_GameData.m_nBetMoney[i][m_GameData.m_nBetPool])
				{
					bEqual = false;
					break;
				}	
			}

			CString str;
			if (!bEqual)
			{
				int nCount = 0;
				BYTE byStation = -1;
				for (int i = 0; i < PLAY_COUNT; i++)
				{
					if (m_pUserInfo[i] == NULL || m_GameData.m_bGiveUp[i] || m_GameData.m_bNoBet[i])
					{
						continue;
					}

					nCount++;
					byStation = i;
				}

				// 如果只剩下一个人, 判断是否还需要继续下注操作
				if (nCount == 1)
				{
					bEqual = true;
					nMoney = m_GameData.m_nBetMoney[byStation][m_GameData.m_nBetPool];

					for (int i = 0; i < PLAY_COUNT; i++)
					{
						if (m_pUserInfo[i] == NULL || m_GameData.m_bGiveUp[i] || (m_GameData.m_bNoBet[i] && !m_GameData.m_bAllBet[i][m_GameData.m_nBetPool]))
						{
							continue;
						}

						// 判断最后一个玩家下注的金币是否大于全下的玩家下注金币, 大于则直接结算，小于需要继续下注
						if (nMoney < m_GameData.m_nBetMoney[i][m_GameData.m_nBetPool])
						{
							bEqual = false;
							break;
						}	
					}
				}
			
				str.Format("Server: ============nCount =  %d", nCount);
				OutputDebugString(str);
			}

			str.Format("Server: ============bEqual   %d", bEqual);
			OutputDebugString(str);
		}

		// 新的一轮标志置为false
		m_GameData.m_bNewTurn = false;
		
		// 加注玩家的上家结束, 开始新的一轮下注或轮到大盲注时(前面没有人加注), 开始新的一轮下注
		BYTE bNextStation = GetNextUserStation(bDeskStation);

		str.Format("Server: m_GameData.m_bBetStation=%d,m_GameData.m_bRaiseStation=%d, 下注人: %d, 大: %d, bEqual=%d, m_GameData.m_iBetCounts: %d, 当前下注第%d轮,第一个过牌玩家%d,GetNextUserStation(bDeskStation) == %d,bNextStation == %d",
			m_GameData.m_bBetStation,m_GameData.m_bRaiseStation, bDeskStation, m_GameData.m_bBigBlind, bEqual, m_GameData.m_iBetCounts, m_GameData.m_nTurnNums, m_GameData.m_bFirstStation,GetNextUserStation(bDeskStation),bNextStation);
		OutputDebugString(str);

		bool temp = false;
		if (m_GameData.m_nTurnNums == 1)
		{
			if (m_GameData.m_iBetCounts >= 2 && m_GameData.m_bBigBlind == bDeskStation)
			{
				temp = true;
			}
			else if (m_GameData.m_bBigBlind != bNextStation)
			{
				temp = true;
			}
			else
			{
				temp = false;
			}
		}
		else
		{
			if (m_GameData.m_iBetCounts > 1)
			{
				temp = true;
			}
			else if (m_GameData.m_bBigBlind != bNextStation && (m_GameData.m_bFirstStation == bNextStation
				|| m_GameData.m_bRaiseStation == bNextStation
				|| m_GameData.m_bBetStation == bNextStation))
			{
				temp = true;
			}
			else
			{
				temp = false;
			}
		}

		if ((m_GameData.m_bRaiseStation != 255 && bNextStation == m_GameData.m_bRaiseStation)
			//|| (bEqual && m_GameData.m_bRaiseStation == 255 && m_GameData.m_bBigBlind == bDeskStation && m_GameData.m_iCheckCount > 2)
			|| (bEqual && temp)
			//(m_GameData.m_iBetCounts > 2)
			|| (m_GameData.m_iCheckCount > 1 && m_GameData.m_bFirstStation == bNextStation && bEqual) // 全过
			|| bNextStation == 255)   // 全下  
		{
			m_GameData.m_iCheckCount = 0;

			// 新的一轮标志置为false
			m_GameData.m_bNewTurns = false;
			OutputDebugString("Server:加注玩家的上家结束, 开始新的一轮下注或轮到大盲注时(前面没有人加注), 开始新的一轮下注");
			// 有人全下之后, 需要计算边池
			if (m_GameData.m_byAllPools[m_GameData.m_nBetPool] > 0)
			{
				// 进行边池计算
				str.Format("Server: 第%I64d个边池有人全下人数:%d, 需要进行边池产生处理", m_GameData.m_nBetPool+1, m_GameData.m_byAllPools[m_GameData.m_nBetPool]);
				OutputDebugString(str);

				int i, j;
				bool bIsNewPool = false;
				int nBetPool = m_GameData.m_nBetPool;
				for (i = 0; i < m_GameData.m_byAllPools[m_GameData.m_nBetPool]; i++)
				{
					// 查找池中最小下注金币, 做为下注池上限
					__int64 nMinMoney = /*2147483647*/9223372036854775807;
					bIsNewPool = false;

					for (j = 0; j < PLAY_COUNT; j++)
					{
						if (m_pUserInfo[j] == NULL || m_GameData.m_bGiveUp[j] || m_GameData.m_nBetMoney[j][nBetPool] <= 0/* || m_GameData.m_bNoBet[j]*/)
						{
							continue;
						}

						if (m_GameData.m_nBetMoney[j][nBetPool] < nMinMoney)
						{
							nMinMoney = m_GameData.m_nBetMoney[j][nBetPool];
						}
					}

					CString str;
					str.Format("Server: 当前%d边池, 最按最低金币:%I64d 进行分池", nBetPool, nMinMoney);
					OutputDebugString(str);

					for (j = 0; j < PLAY_COUNT; j++)
					{
						if (m_pUserInfo[j] == NULL || m_GameData.m_bGiveUp[j] || m_GameData.m_nBetMoney[j][nBetPool] <= 0/* || m_GameData.m_bNoBet[j]*/)
						{
							continue;
						}

						str.Format("Server: 玩家:%d, 当前下注金币:%I64d", m_pUserInfo[j]->m_UserData.dwUserID, m_GameData.m_nBetMoney[j][nBetPool]);
						OutputDebugString(str);

						// 判断该玩家下注的金币是否大于全压的玩家金币
						if (m_GameData.m_nBetMoney[j][nBetPool] > nMinMoney)
						{
							// 把多出的金币转到边池去
							__int64 nPoolMoney = m_GameData.m_nBetMoney[j][nBetPool] - nMinMoney;
							m_GameData.m_nBetMoney[j][nBetPool] -= nPoolMoney;
							m_GameData.m_nBetMoney[j][nBetPool+1] = nPoolMoney;

							str.Format("Server: 玩家:%d, 分池后: %I64d, %I64d", m_pUserInfo[j]->m_UserData.dwUserID, m_GameData.m_nBetMoney[j][nBetPool], nPoolMoney);
							OutputDebugString(str);

							str.Format("Server: 分池金币: %I64d", nPoolMoney);
							OutputDebugString(str);
							
							bIsNewPool = true;
						}
					}

					if (!bIsNewPool)
					{
						// 没有新边池产生, 则跳出
						break;
					}

					// 边池位置移到下一个边池
					nBetPool++;
				}

				m_GameData.m_nBetPool = nBetPool;

				TBetPool tagTbetPool;

				// 更新下池信息到客户端
				for (int i = 0; i <= m_GameData.m_nBetPool; i++)
				{
					// 把8个下注池的金币发到客户端
					tagTbetPool.iBetPools[i] = m_GameData.GetBetMoney(i, false);
					CString str;
					str.Format("Server: 第%d个边池金币: %I64d", i, tagTbetPool.iBetPools[i]);
					OutputDebugString(str);
				}
				
				for (int i = 0; i < PLAY_COUNT; i++)
				{
					// 发送
					SendGameData(i, &tagTbetPool, sizeof(tagTbetPool), MDM_GM_GAME_NOTIFY, ASS_BETPOOL_UP, 0);
				}

				SendWatchData(m_bMaxPeople,&tagTbetPool, sizeof(tagTbetPool), MDM_GM_GAME_NOTIFY, ASS_BETPOOL_UP, 0);
			}

			OutputDebugString("Server: 进入下一轮发牌操作");

			int nCount = 0;
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (m_pUserInfo[i] == NULL || m_GameData.m_bGiveUp[i] || m_GameData.m_bNoBet[i]/* || i == bDeskStation*/)
				{
					continue;
				}

				nCount++;
			}

			if (nCount <= 1)
			{
				// 全下之后，没人可以下注了，需要把剩余的牌全发了
				OutputDebugString("Server: 全下之后，没人可以下注了，需要把剩余的牌全发了");
				m_GameData.m_bAutoSendCard = true;
			}

			// 没有加注, 下一轮开始
			if (m_GameData.m_nTurnNums == 1)
			{
				// 发三张公共牌
				ActionRedirect(IDEV_SEND_3_CARD, 500);
			}
			else if (m_GameData.m_nTurnNums < 4)
			{
				// 发一张公共牌
				ActionRedirect(IDEV_SEND_4_5_CARD, 500);
			}
			else 
			{
				// 游戏结束, 进行结算比牌
				OutputDebugString("Server:  游戏结束, 进行结算比牌");
				ActionRedirect(IDEV_COMPARE_CARD, 500);
				return true;
			}

			// 重置下注位置
			m_GameData.m_bRaiseStation = -1;
			m_GameData.m_bBetStation = 255;
		}
		else
		{
			OutputDebugString("Server: 下一个玩家操作");

			int nCount = 0;
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (m_pUserInfo[i] == NULL || m_GameData.m_bGiveUp[i] || m_GameData.m_bNoBet[i] || i == bDeskStation)
				{
					continue;
				}

				nCount++;
			}

			if (nCount <= 0)
			{
				OutputDebugString("Server:  下一个玩家操作后, 其他玩家放弃或全下(没有可操作玩家了), 游戏结束");
				m_GameData.m_bAutoSendCard = true;

				// 没有加注, 下一轮开始
				if (m_GameData.m_nTurnNums == 1)
				{
					// 发三张公共牌
					ActionRedirect(IDEV_SEND_3_CARD, 500);
				}
				else if (m_GameData.m_nTurnNums < 4)
				{
					// 发一张公共牌
					ActionRedirect(IDEV_SEND_4_5_CARD, 500);
				}
				else 
				{
					// 游戏结束, 进行结算比牌
					OutputDebugString("Server:  游戏结束, 进行结算比牌");
					ActionRedirect(IDEV_COMPARE_CARD, 500);
				}

				return true;
			}

			bool temps = true;
			if (m_GameData.m_nTurnNums == 1)
			{
				OutputDebugString("Server:m_GameData.m_nTurnNums == 1");
				if (m_GameData.m_iBetCounts > 2)
				{
					temps = true;
				}
				else if (m_GameData.m_bBigBlind == bNextStation)
				{
					temps = false;
				}
			}
			else
			{
				if (m_GameData.m_iBetCounts > 1)
				{
					temps = true;
				}
				else if (m_GameData.m_bBigBlind == bNextStation)
				{
					temps = false;
				}
			}

			// 第一轮之后每轮开始可以跟注、加注、弃牌、过牌、全下
			if (/*m_GameData.m_iBetCounts > 0*/temps)
			{
				//OutputDebugString("Server:第一轮之后每轮开始可以跟注、加注、弃牌、过牌、全下");
				// 有下注的人, 则跟注按钮、加注按钮、弃牌按钮、全下按钮可显示
				m_GameData.MakeVerbs(UD_VF_CALL|UD_VF_ADD|UD_VF_FOLD/*|UD_VF_ALLIN*/);
			}
			else
			{
				//OutputDebugString("Server:第一圈大盲注操作时, 则过片按钮、加注按钮、弃牌按钮可显示");
				// 第一圈大盲注操作时, 则过片按钮、加注按钮、弃牌按钮可显示
				m_GameData.MakeVerbs(UD_VF_CHECK|UD_VF_ADD|UD_VF_FOLD);
				// 还没有人下注: 下注按钮、弃牌按钮、过牌按钮、全下按钮可显示
				// m_GameData.MakeVerbs(UD_VF_BET|UD_VF_FOLD|UD_VF_CHECK|UD_VF_ALLIN);
			}

			// 继续下一个玩家下注
			ActionRedirect(IDEV_BET_NEXT, 500);
		}
	}

	return true;
}


//处理玩家发牌完成
bool CServerGameDesk::OnUserSendCardFinish(BYTE bDeskStation,NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser)
{
	// 不需要客户端同步
	//return true;

	////为第bDeskStation个玩家置发玩牌的标记
	m_GameData.m_UserData[bDeskStation].m_bSendCardFinish = true;

	////如果还有玩家未发完牌，则不能继续流程
	if (!m_GameData.IsAllPlayerSendCardFinish())
	{
		return true;
	}

	////发牌完成开始首次下注事件处理
	OutputDebugString("Server: 处理玩家发牌完成");
	
	if (m_bFaiPaiFinish == false)
	{
		ActionRedirect(IDEV_BET_START,500);
	}
	m_bFaiPaiFinish = true;

	return true;
}

//处理玩家对下一回合准备的信号
bool CServerGameDesk::OnUserNextRoundReq(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	TNextRoundReq* pNextRoundReq = (TNextRoundReq*)pData;

	if (m_pUserInfo[bDeskStation] == NULL)
	{
		return false;
	}

	TAgree tagAgree;
	tagAgree.byUser = bDeskStation;

	if (pNextRoundReq->nType == TNextRoundReq::READY)
	{
		//m_emUserAgree[bDeskStation] = US_AGREE;
		tagAgree.bAgree = true;
	}
	else if (pNextRoundReq->nType == TNextRoundReq::EXIT)
	{
		//m_emUserAgree[bDeskStation] = US_DISAGREE;
		tagAgree.bAgree = false;
	}
	else
	{
		return true;
	}

	//广播该玩家是否同意继续游戏的消息
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pUserInfo[i] != NULL)
		{
			SendGameData(i,&tagAgree,sizeof(TAgree),MDM_GM_GAME_NOTIFY,ASS_AGREE,0);
		}
	}

	SendWatchData(m_bMaxPeople,&tagAgree,sizeof(TAgree),MDM_GM_GAME_NOTIFY,ASS_AGREE,0);

	//只要有一个人退出游戏，则无法进行游戏，通知在仍在桌上的玩家有玩家退出，并结束游戏

	if (pNextRoundReq->nType == TNextRoundReq::EXIT)
	{
		TNoPlayer tagNoPlayer;
		tagNoPlayer.bGameFinished = false;
		for (int i = 0; i < PLAY_COUNT; ++i)
		{
			if (m_pUserInfo[i])
			{
				SendGameData(i,&tagNoPlayer,sizeof(TNoPlayer),MDM_GM_GAME_NOTIFY,ASS_NO_PLAYER,0);
			}
		}

		SendWatchData(m_bMaxPeople,&tagNoPlayer,sizeof(TNoPlayer),MDM_GM_GAME_NOTIFY,ASS_NO_PLAYER,0);

		GameFinish(bDeskStation,GF_SALE);
		//__super::GameFinish(0,GF_SALE);//这儿使用GameFinish后，客户无法调用踢人接口，故屏蔽

		return true;
	}

	if (GetPlayerCount() >= 2)
	{
		//释放当前正在运行的事件
		CloseEventProtection();

		//开始下一回合游戏事件处理
		//ActionRedirect(IDEV_NEXT_ROUND_BEGIN,500);
		GameBegin(ALL_ARGEE);
	}

	return true;
}

//处理玩家本身金币不足于最少带入金额
bool CServerGameDesk::OnUserMoneyLack(BYTE bDeskStation)
{
	TMoneyLack  cmd;
	cmd.nbyUser = bDeskStation;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SendGameData(i,&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_NOT_ENOUGH_MONEY, 0);
	}

	SendWatchData(m_bMaxPeople,&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_NOT_ENOUGH_MONEY, 0);
	
	if (m_pUserInfo[bDeskStation] != NULL)
	{
		// 踢除断线玩家
		//KickUserByID(m_pUserInfo[bDeskStation]->m_UserData.dwUserID);
	}

	// 清空玩家坐位上的信息，以免下一个玩家再次使用
	m_GameData.m_UserMoney[bDeskStation] = 0;
	m_GameData.m_UserData[bDeskStation].m_bIn = false;

	return true;
}


//处理玩家线束本轮游戏的请求
bool CServerGameDesk::OnTestFinishRoundRequest(BYTE bDeskStation,NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser)
{
	// 	if (m_bGameStation != GS_PLAY_GAME)
	// 	{
	// 		return true;
	// 	}

	//释放当前正在运行的事件
	CloseEventProtection();

	//启动回合结算流程
	ActionRedirect(IDEV_ROUND_FINISH,500);

	return true;
}

//代管玩家发牌完成
bool CServerGameDesk::OnAgentSendCardsFinish()
{
	OutputDebugString("Server:代管玩家发牌完成");
	//查看有没有玩家掉线
	if (m_bFaiPaiFinish == false)	
	{
		for (int i = 0; i < PLAY_COUNT; ++i)
		{
			//if (m_GameData.m_UserData[i].m_bNetCut)

			{
				OnUserSendCardFinish(i,NULL,NULL,0,0,false);
			}
		}
	}
	
	return true;
}

//保护出牌事件流
bool CServerGameDesk::OnAgentOutCard()
{
	return true;
}

//下一局开始事件流代理
bool CServerGameDesk::OnAgentNextRoundBegin()
{
	//查看有没有玩家掉线
	BYTE byNetCutUser = 255;
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_GameData.m_UserData[i].m_bNetCut)
		{
			byNetCutUser = i;
			break;
		}
	}

	if (byNetCutUser != 255)
	{
		//掉线的玩家请求退出
		TNextRoundReq NextRoundReq;
		NextRoundReq.nType = TNextRoundReq::EXIT;

		OnUserNextRoundReq(byNetCutUser,NULL,&NextRoundReq,sizeof(TNextRoundReq),0,false);
	}

	return true;
}

//处理要得到游戏等待开始的请求
bool CServerGameDesk::OnGetGSWaitAgree(BYTE bDeskStation,UINT uSocketID,bool bWatchUser,TGSBase* pGSBase)
{
	TGSWaitAgree GSWaitAgree;
	CopyGSBase(dynamic_cast<TGSWaitAgree*>(&GSWaitAgree),pGSBase);

	//具体的数据获取代码写下此
    memcpy(GSWaitAgree.nSubMoney, m_GameData.m_UserMoney, sizeof(m_GameData.m_UserMoney));

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			GSWaitAgree.bReady[i] = m_GameData.m_UserData[i].m_bIn;
		}
	}

	return SendGameStation(bDeskStation,uSocketID,bWatchUser,&GSWaitAgree,sizeof(TGSWaitAgree));
}

//处理要得到游戏中状态的请求
bool CServerGameDesk::OnGetGSPlaying(BYTE bDeskStation,UINT uSocketID,bool bWatchUser,TGSBase* pGSBase)
{
	TGSPlaying GSPlaying;
	CopyGSBase(dynamic_cast<TGSPlaying*>(&GSPlaying),pGSBase);

	//具体的数据获取代码写下此

	// 获得得到令牌的玩家
	GSPlaying.byTokenUser = m_GameData.m_byTokenUser;

    // 当前可用金币
    memcpy(GSPlaying.nSubMoney, m_GameData.m_UserMoney, sizeof(m_GameData.m_UserMoney));

    // 庄家
    GSPlaying.byNTUser = m_GameData.m_byNTUser;

    // 大小肓家
    GSPlaying.bySmallBlind = m_GameData.m_bSmallBlind;
    GSPlaying.byBigBlind = m_GameData.m_bBigBlind;

    GSPlaying.byVerbFlag = m_GameData.m_byVerbFlag;
    GSPlaying.bNewTurn = m_GameData.m_bNewTurn;
	GSPlaying.bNewTurns = m_GameData.m_bNewTurns;

	// 跟注金币
	GSPlaying.nCallMoney = m_GameData.m_nMaxBetMoney - m_GameData.GetBetMoney(m_GameData.m_byTokenUser);

    // 获取玩家手上的扑克
    GSPlaying.nHandCardNums = m_GameData.m_UserData[bDeskStation].m_iCardsNum;
    memcpy(GSPlaying.byHandCard, m_GameData.m_UserData[bDeskStation].m_byMyCards,  GSPlaying.nHandCardNums * sizeof(BYTE));

    // 获取公共牌
    GSPlaying.nCardsCount = m_GameData.m_iCardsCount;
    memcpy(GSPlaying.byCards, m_GameData.m_iCards, GSPlaying.nCardsCount * sizeof(BYTE));

    // 获取下注金币
    memcpy(GSPlaying.nBetMoney, m_GameData.m_nBetMoney, sizeof(m_GameData.m_nBetMoney));

	/*CString strMoney;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] == NULL)
		{
			continue;
		}
		for (int j = 0; j < PLAY_COUNT; j++)
		{
			strMoney.Format("Server:玩家%d,获取下注金币:%I64d",GSPlaying.nBetMoney[i][j]);
			OutputDebugString(strMoney);
		}
	}*/

    // 获取不能下注玩家
    memcpy(GSPlaying.bNoBet, m_GameData.m_bNoBet, sizeof(m_GameData.m_bNoBet));

    // 获取弃牌玩家
    memcpy(GSPlaying.bGiveUp, m_GameData.m_bGiveUp, sizeof(m_GameData.m_bGiveUp));

	CString str;
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		str.Format("giwo:获取弃牌玩家：%d",m_GameData.m_bGiveUp[i]);
		OutputDebugString(str);
	}

	// 中途进入玩家
	GSPlaying.iUserStation = m_iUserStation[bDeskStation] ; 
		
    // 所有边池数据
    for (int i = 0; i <= m_GameData.m_nBetPool; i++)
    {
        GSPlaying.nBetPools[i] = m_GameData.GetBetMoney(i, false);
    }

	return SendGameStation(bDeskStation,uSocketID,bWatchUser,&GSPlaying,sizeof(TGSPlaying));
}

//处理要得到游戏回合结束的请求
bool CServerGameDesk::OnGetGSWaitNextRound(BYTE bDeskStation,UINT uSocketID,bool bWatchUser,TGSBase* pGSBase)
{
	TGSWaitNextRound GSWaitNextRound;
	//CopyGSBase(dynamic_cast<TGSWaitNextRound*>(&GSWaitNextRound),pGSBase);

	//具体的数据获取代码写下此
    //memcpy(GSWaitNextRound.nSubMoney, m_GameData.m_UserMoney, sizeof(m_GameData.m_UserMoney));

	return SendGameStation(bDeskStation,uSocketID,bWatchUser,&GSWaitNextRound,sizeof(TGSWaitNextRound));
}

//顺时针顺序得到下家位置
BYTE CServerGameDesk::GetNextUserStation(BYTE byStation, BOOL bIsAll)
{
	int nCount = 0;
	int iNextStation = (byStation + PLAY_COUNT + 1) % PLAY_COUNT;

	// 绕过无效玩家与已放弃的玩家 或 已经全下的玩家 或 中途进入游戏玩家
	while (m_pUserInfo[iNextStation] == NULL 
	|| m_GameData.m_bGiveUp[iNextStation] 
	|| (bIsAll && m_GameData.m_bNoBet[iNextStation])
	|| m_iUserStation[iNextStation] != TYPE_PLAY_GAME)
	{
		iNextStation = (iNextStation + PLAY_COUNT + 1) % PLAY_COUNT;
		nCount++;

		if (nCount >= PLAY_COUNT)
		{
			iNextStation = -1;
			break;
		}
	}

	return iNextStation;
}

//逆时针顺序得到上家位置
BYTE CServerGameDesk::GetLastUserStation(BYTE byStation, BOOL bIsAll)
{
	int nCount = 0;
	int iLastStation = (byStation + PLAY_COUNT - 1) % PLAY_COUNT;

	// 绕过无效玩家与已放弃的玩家 或 已经全下的玩家 或 中途进入游戏玩家
	while (m_pUserInfo[iLastStation] == NULL
	|| m_GameData.m_bGiveUp[iLastStation]
	|| (bIsAll && m_GameData.m_bNoBet[iLastStation])
	|| m_iUserStation[iLastStation] != TYPE_PLAY_GAME)
	{
		iLastStation = (iLastStation + PLAY_COUNT - 1) % PLAY_COUNT;

		nCount++;

		if (nCount >= PLAY_COUNT)
		{
			iLastStation = -1;
			break;
		}
	}

	return iLastStation;
}

///名称：GetPlayerCount
///描述：获取游戏玩家数量
///@return 
int CServerGameDesk::GetPlayerCount(bool bFold)
{
	int nCount = 0;
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (bFold)
		{
			if (m_pUserInfo[i] != NULL && m_iUserStation[i] == TYPE_PLAY_GAME)
			{
				nCount++;
			}
		}
		else
		{
			if (m_pUserInfo[i] != NULL)
			{
				nCount++;
			}
		}
		
	}

	return nCount;
}

// 计算个下注池胜利玩家
void CServerGameDesk::CalculateWin(BYTE nSectionCount, CByteArray &arrayResult)
{
	arrayResult.RemoveAll();

	// 找出牌型最大的牌
	// 查找一个可用的玩家开始比牌
	BYTE bMaxStation = -1;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] == NULL || m_GameData.m_bGiveUp[i] || m_GameData.m_nBetMoney[i][nSectionCount] <= 0)
		{
			continue ;
		}

		bMaxStation = i;
		break;
	}

	CString str;
	str.Format("Server: 第%d个下注池, 第一个比牌玩家: %d", nSectionCount, bMaxStation);
	OutputDebugString(str);

	if (bMaxStation == 255)
	{
		return ;
	}

	// 目前为赢家
	arrayResult.Add(bMaxStation);

	// 找出比牌后胜利玩家
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] == NULL || m_GameData.m_bGiveUp[i] || m_GameData.m_nBetMoney[i][nSectionCount] <= 0 || i == bMaxStation)
		{
			continue ;
		}

		int nRet = m_Logic.CompareCard(m_GameData.m_bResultCards[i], 5, m_GameData.m_bResultCards[bMaxStation], 5);

		if (nRet > 0)
		{
			arrayResult.RemoveAll();

			// 赢
			bMaxStation = i;
	
			arrayResult.Add(i);
		}
		else if (nRet == 0)
		{
			// 相等
			arrayResult.Add(i);
		}
	}
}

// 判断玩家是否属于配好牌玩家
bool CServerGameDesk::CanGetGoodCard(BYTE byStation)
{
	if(!m_pUserInfo[byStation])
	{
		return false;
	}

	long int UserID = m_pUserInfo[byStation]->m_UserData.dwUserID;

	TCHAR szIniName[MAX_PATH];
	GET_SERVER_INI_NAME(szIniName);

	CINIFile f(szIniName);

	CString key = TEXT("superUserConfig");

	int	iSuperUserCount = 0;
	iSuperUserCount = f.GetKeyVal(key,"SuperUserCount",0);	//超端用户人数

	if(iSuperUserCount <= 0)
	{
		return false;
	}

	long int iTempUserID = -1;
	CString sTemp = "";
	for(int i = 1; i <= iSuperUserCount; ++i)
	{
		sTemp.Format("SuperUserID%d",i);
		iTempUserID = f.GetKeyVal(key,sTemp,0);		//读取超端用户ID

		//与配置文件中的ID相符 则为超端用户
		if(UserID == iTempUserID)
		{
			return true;
		}
	}
	return false;
	
}

//自动操作游戏事件处理
bool CServerGameDesk::OnEventAutoOperate()
{
    // 自动放弃操作
    TBet tagBet;
    tagBet.nType = ET_FOLD;
    tagBet.byUser = m_GameData.m_byTokenUser;
    tagBet.nMoney = 0;

    // 模拟下注消息
    OnUserBet(m_GameData.m_byTokenUser, &tagBet);

    return true;
}