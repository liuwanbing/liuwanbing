#include "Stdafx.h"

#include "BankService.h"
#include "GameMainManage.h"
#include "commonuse.h"


CBankService::CBankService(CWnd *pWnd)
{
	m_pWnd = pWnd;
}


CBankService::~CBankService(void)
{

}


bool CBankService::OnNetMessage(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	CGameMainManage* pGameMainManage = (CGameMainManage*)m_pWnd;
	if (pGameMainManage == NULL) return true;

	if (pNetHead->bMainID == MDM_BANK)
	{
		switch(pNetHead->bAssistantID)
		{
		case ASS_BANK_NORMAL:
			{
				bank_op_normal *p = (bank_op_normal*)pData;
				if (p)
				{
					db_bank_op_normal_in _in;
					_in._data = *p;

					CGameUserInfo* pUserInfo = pGameMainManage->m_UserManage.GetOnLineUserInfo(uIndex);
					const CGameDesk* pDesk = pGameMainManage->GetGameDesk(pUserInfo->m_UserData.bDeskNO);

					if (p->_operate_type == 1) //取钱
					{
						/// 如果玩家正在游戏中，非百人类游戏或庄家(可配)，则不让取钱 add by lxl 2010-12-6
						if (pUserInfo->m_UserData.bUserState == USER_PLAY_GAME || pUserInfo->m_UserData.bUserState == USER_CUT_GAME)
						{
							if(pUserInfo->m_UserData.bDeskNO != 255)
							{
								if(!pDesk->IsBJLGameType())
								{		
									pGameMainManage->m_TCPSocket.SendData(uIndex,MDM_BANK,ASS_BANK_NORMAL,HC_BANK_OP_CHECKOUT_PLAYING, dwHandleID);
									return true;
								}

								CBcfFile f(CBcfFile::GetAppPath() + "Function.bcf");
								int drawMoneyInGame = f.GetKeyVal("DrawMoneyInGame","Available",0);
								if(drawMoneyInGame)
								{
									if(pDesk->IsZhuangjia(pUserInfo->m_UserData.dwUserID))
									{
										pGameMainManage->m_TCPSocket.SendData(uIndex,MDM_BANK,ASS_BANK_NORMAL,HC_BANK_OP_CHECKOUT_PLAYING,dwHandleID);
										return true;
									}
								}
							}
						}
					}
					else if (p->_operate_type == 2) //存钱
					{
						/// 如果玩家正在游戏中，则不让存钱
						if (pUserInfo->m_UserData.bUserState == USER_PLAY_GAME || pUserInfo->m_UserData.bUserState == USER_CUT_GAME)
						{
							pGameMainManage->m_TCPSocket.SendData(uIndex,MDM_BANK,ASS_BANK_NORMAL,HC_BANK_OP_CHECKIN_PLAYING,dwHandleID);
							return true;
						}

						/// 百家乐类游戏，如果玩家是庄家，则不让存钱 add by lxl 2010-11-5
						//判断是否在游戏桌子，只有在游戏中才有可能是庄家
						if(pUserInfo->m_UserData.bDeskNO != 255)
						{
							if(pDesk->IsZhuangjia(pUserInfo->m_UserData.dwUserID))
							{
								pGameMainManage->m_TCPSocket.SendData(uIndex,MDM_BANK,ASS_BANK_NORMAL,HC_BANK_OP_CHECKIN_ZHUANG,dwHandleID);
								return true;
							}
						}
					}

					pGameMainManage->m_SQLDataManage.PushLine(&_in.DataBaseHead, sizeof(db_bank_op_normal_in), DTK_GR_BANK_NORMAL, uIndex, dwHandleID);
				}
				break;
			}
		case ASS_BANK_TRANSFER:
			{
				bank_op_transfer *p = (bank_op_transfer*)pData;
				if (p)
				{
					db_bank_op_transfer_in _in;
					_in._data = *p;
					pGameMainManage->m_SQLDataManage.PushLine(&_in.DataBaseHead, sizeof(db_bank_op_transfer_in), DTK_GR_BANK_TRANSFER, uIndex, dwHandleID);
				}
				break;
			}
		case ASS_BANK_DEPOSIT:
			{
				bank_op_deposit *p = (bank_op_deposit*)pData;
				if (p)
				{
					db_bank_op_deposit_in _in;
					_in._data = *p;
					pGameMainManage->m_SQLDataManage.PushLine(&_in.DataBaseHead, sizeof(db_bank_op_deposit_in), DTK_GR_BANK_DEPOSIT, uIndex, dwHandleID);
				}
				break;
			}
		case ASS_BANK_WALLET:
			{
				db_bank_op_wallet_in _in;
				_in._UserID = *((int*)pData);
				pGameMainManage->m_SQLDataManage.PushLine(&_in.DataBaseHead, sizeof(db_bank_op_deposit_in), DTK_GR_BANK_WALLET, uIndex, dwHandleID);
				break;
			}
		case ASS_BANK_QUERY_DEPOITS:
			{
				db_bank_op_query_deposit_in _in;
				CGameUserInfo* pUserInfo = pGameMainManage->m_UserManage.GetOnLineUserInfo(uIndex);
				_in._UserID = pUserInfo->m_UserData.dwUserID;
				pGameMainManage->m_SQLDataManage.PushLine(&_in.DataBaseHead, sizeof(db_bank_op_query_deposit_in), DTK_GR_BANK_QUERY_DEPOSIT, uIndex, dwHandleID);
				break;
			}
		default:
			break;
		}
	}
	return true;
}

bool CBankService::OnDataBaseResult(DataBaseResultLine * pResultData)
{
	CGameMainManage* pGameMainManage = (CGameMainManage*)m_pWnd;
	if (pGameMainManage == NULL) return true;

	switch(pResultData->uHandleKind)
	{
	case DTK_GR_BANK_NORMAL:
		{
			db_bank_op_normal_out *p = (db_bank_op_normal_out*)pResultData;
			if (p != NULL)
			{
				if (pResultData->uHandleRusult == 0)
				{
					CGameUserInfo* puser = pGameMainManage->m_UserManage.GetOnLineUserInfo(pResultData->uIndex);
					if (p->_data._operate_type == 1)
					{
						puser->m_UserData.i64Bank -= p->_data._money;

						if (p->_data._game_id == pGameMainManage->m_KernelData.uNameID)
						{
							puser->m_UserData.i64Money += p->_data._money;
						}

// 						CString str;
// 						str.Format("DServer: 取钱 puser = 0x%08x uNameID = %d i64Money = %I64d, i64Bank = %I64d",\
// 							puser,pGameMainManage->m_KernelData.uNameID,puser->m_UserData.i64Money,puser->m_UserData.i64Bank);
// 						OutputDebugString(str);

						MSG_GR_CheckMoney CheckOutMoney;
						memset(&CheckOutMoney,0,sizeof(CheckOutMoney));
						CheckOutMoney.i64CheckOut = p->_data._money;
						CheckOutMoney.i64MoneyInBank = puser->m_UserData.i64Bank;
						CheckOutMoney.i64MoneyInRoom = puser->m_UserData.i64Money;
						CheckOutMoney.dwUserID = puser->m_UserData.dwUserID;
						CheckOutMoney.uGameID = p->_data._game_id;
						pGameMainManage->m_TCPSocket.SendDataBatch(&CheckOutMoney,sizeof(CheckOutMoney),MDM_GR_MONEY,ASS_GR_CHECK_OUT_INGAME,ASS_GR_CHECKOUT_SUC);

					}
					else if (p->_data._operate_type == 2)
					{
						puser->m_UserData.i64Bank += p->_data._money;

						if (p->_data._game_id == pGameMainManage->m_KernelData.uNameID)
						{
							puser->m_UserData.i64Money -= p->_data._money;
						}

// 						CString str;
// 						str.Format("DServer: 存钱 puser = 0x%08x uNameID = %d i64Money = %I64d, i64Bank = %I64d",\
// 							puser,pGameMainManage->m_KernelData.uNameID,puser->m_UserData.i64Money,puser->m_UserData.i64Bank);
// 						OutputDebugString(str);

						MSG_GR_CheckMoney CheckInMoney;
						memset(&CheckInMoney,0,sizeof(CheckInMoney));
						CheckInMoney.i64CheckOut = p->_data._money;
						CheckInMoney.i64MoneyInBank = puser->m_UserData.i64Bank;
						CheckInMoney.i64MoneyInRoom = puser->m_UserData.i64Money;
						CheckInMoney.dwUserID = puser->m_UserData.dwUserID;
						CheckInMoney.uGameID = p->_data._game_id;
						pGameMainManage->m_TCPSocket.SendDataBatch(&CheckInMoney,sizeof(CheckInMoney),MDM_GR_MONEY,ASS_GR_CHECK_IN_INGAME,ASS_GR_CHECKIN_SUC);
					}
				}
				pGameMainManage->m_TCPSocket.SendData(pResultData->uIndex, &p->_data, sizeof(bank_op_normal), MDM_BANK, ASS_BANK_NORMAL, pResultData->uHandleRusult, 0);
			}
			break;
		}
	case DTK_GR_BANK_TRANSFER:
		{
			db_bank_op_transfer_out *p = (db_bank_op_transfer_out*)pResultData;
			if (p != NULL)
			{
				
			}
			break;
		}
	case DTK_GR_BANK_DEPOSIT:
		{
			db_bank_op_deposit_out *p = (db_bank_op_deposit_out*)pResultData;
			if (p != NULL)
			{
				if (pResultData->uHandleRusult == 0)
				{
					if (p->_data._operate_type == 1) //创建充值卡
					{
						CGameUserInfo* puser = pGameMainManage->m_UserManage.GetOnLineUserInfo(pResultData->uIndex);
						puser->m_UserData.i64Bank = p->_data._money;
					}
				}
				pGameMainManage->m_TCPSocket.SendData(pResultData->uIndex, &p->_data, sizeof(bank_op_deposit), MDM_BANK, ASS_BANK_DEPOSIT, pResultData->uHandleRusult, 0);
			}
			break;
		}
	case DTK_GR_BANK_WALLET:
		{
			db_bank_game_wallet_out *p = (db_bank_game_wallet_out*)pResultData;
			if (p != NULL)
			{
				pGameMainManage->m_TCPSocket.SendData(pResultData->uIndex, &p->_data, sizeof(bank_game_wallet), MDM_BANK, ASS_BANK_WALLET, pResultData->uHandleRusult, 0);
			}
			break;
		}
	case DTK_GR_BANK_QUERY_DEPOSIT:
		{
			db_bank_op_query_deposit_out *p = (db_bank_op_query_deposit_out*)pResultData;
			if (p != NULL)
			{
				pGameMainManage->m_TCPSocket.SendData(pResultData->uIndex, &p->_data, sizeof(bank_op_deposit_node), MDM_BANK, ASS_BANK_QUERY_DEPOITS, pResultData->uHandleRusult, 0);
			}
			break;
		}
	default:
		break;
	}
	return true;
}

