#include "StdAfx.h"
#include "UpGradeLogic.h"

/**
 * 构造函数
 */
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	
}


/*
//是否可以出牌
BOOL CUpGradeGameLogic::CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], 
								   int iBaseCount, BYTE iHandCard[], int iHandCount)
{
	return TRUE;
}
*/

/**
 * 查找棋子是什么棋
 * @return 若不为0，则返回棋ID，若为0则是无棋。
 */
int  CUpGradeGameLogic::GetChess(int xChess,int yChess,int chessArray[][9])
{
	::CopyMemory(m_iQiZi,chessArray,sizeof(chessArray));
	if (xChess > 0 && xChess < 10)
	{
		if (yChess > 0 && yChess < 9)
		{
			return m_iQiZi[xChess][yChess];
		}
	}
	return XQ_QIZI_NONE;
}

void CUpGradeGameLogic::InitBoard(int iMeFlag, int qi[][9])
{
	m_iSideFlag = iMeFlag;
	for (int i = 0;i < 10;i++)
		for (int j = 0;j < 9;j++)
			qi[i][j] = XQ_QIZI_NONE;


	if (iMeFlag == XQ_FLAG_RED)
	{
		qi[0][0] = qi[0][8] = XQ_BLACK_JU;
		qi[0][1] = qi[0][7] = XQ_BLACK_MA;
		qi[0][2] = qi[0][6] = XQ_BLACK_XIANG;
		qi[0][3] = qi[0][5] = XQ_BLACK_SHI;
		qi[0][4]		    = XQ_BLACK_JIANG;
		qi[2][1] = qi[2][7] = XQ_BLACK_PAO;
		qi[3][0] = qi[3][2] = qi[3][4] = qi[3][6] = qi[3][8] =XQ_BLACK_BIN;


		qi[6][0] = qi[6][2] = qi[6][4] = qi[6][6] = qi[6][8] =XQ_RED_BIN;
		qi[9][0] = qi[9][8] = XQ_RED_JU;
		qi[9][1] = qi[9][7] = XQ_RED_MA;
		qi[9][2] = qi[9][6] = XQ_RED_XIANG;
		qi[9][3] = qi[9][5] = XQ_RED_SHI;
		qi[9][4]		    = XQ_RED_JIANG;
		qi[7][1] = qi[7][7] = XQ_RED_PAO;

	}

	else if (iMeFlag == XQ_FLAG_BLACK)
	{
		qi[0][0] = qi[0][8] = XQ_RED_JU;
		qi[0][1] = qi[0][7] = XQ_RED_MA;
		qi[0][2] = qi[0][6] = XQ_RED_XIANG;
		qi[0][3] = qi[0][5] = XQ_RED_SHI;
		qi[0][4]		    = XQ_RED_JIANG;
		qi[2][1] = qi[2][7] = XQ_RED_PAO;
		qi[3][0] = qi[3][2] = qi[3][4] = qi[3][6] = qi[3][8] =XQ_RED_BIN;


		qi[6][0] = qi[6][2] = qi[6][4] = qi[6][6] = qi[6][8] =XQ_BLACK_BIN;
		qi[9][0] = qi[9][8] = XQ_BLACK_JU;
		qi[9][1] = qi[9][7] = XQ_BLACK_MA;
		qi[9][2] = qi[9][6] = XQ_BLACK_XIANG;
		qi[9][3] = qi[9][5] = XQ_BLACK_SHI;
		qi[9][4]			= XQ_BLACK_JIANG;
		qi[7][1] = qi[7][7] = XQ_BLACK_PAO;
	}


	return;
}

/**
 * 能否走子
 */
BOOL CUpGradeGameLogic::CanMoveQiZi(int fromi, int fromj, int toi, int toj, int qi[][9],bool check)
{
	::CopyMemory(m_iQiZi,qi,sizeof(m_iQiZi));
	if (
		(toi == fromi && toj == fromj) ||
		(toi < 0 || toj < 0)		   ||
		(toi > 9 || toj > 8)		   ||
		(fromi < 0 || fromj < 0)       ||
		(fromi > 9 || fromj > 8)
		)
		
	{
		
		return FALSE;
	}
	
	int qizi = m_iQiZi[fromi][fromj];
	if (qizi == XQ_QIZI_NONE)
		return FALSE;
	int flag;
	if (qizi > 0) 
		flag = XQ_FLAG_RED;
	else 
		if (qizi < 0) 
			flag = XQ_FLAG_BLACK;
	else
		return FALSE;
	
	if (flag * m_iQiZi[toi][toj] > 0) return FALSE;

	BOOL ret = FALSE;
	
	if (qizi == XQ_RED_JU || qizi == XQ_BLACK_JU)
	{
		ret = MoveZhu(fromj,fromi,toj,toi);
		goto retu;
	}
	if (qizi == XQ_RED_MA || qizi == XQ_BLACK_MA)
	{
		ret = MoveMa(fromj,fromi,toj,toi);
		goto retu;
	}
	if (qizi == XQ_RED_PAO || qizi == XQ_BLACK_PAO)
	{
		ret = MovePao(fromj,fromi,toj,toi,flag);
		goto retu;
	}
	if (qizi == XQ_RED_JIANG || qizi == XQ_BLACK_JIANG)
	{
		ret = MoveShuai(fromj,fromi,toj,toi);
		goto retu;
	}
	if (qizi == XQ_RED_SHI || qizi == XQ_BLACK_SHI)
	{
		ret = MoveShi(fromj,fromi,toj,toi);
		
		goto retu;
	}
	if (qizi == XQ_RED_XIANG || qizi == XQ_BLACK_XIANG)
	{
		ret = MoveXiang(fromj,fromi,toj,toi);
		goto retu;
	}
	if (qizi == XQ_RED_BIN || qizi == XQ_BLACK_BIN)
	{
		ret = MoveBin(fromj,fromi,toj,toi);
		
		goto retu;
	}

retu:
	
//	if (qizi == XQ_RED_JIANG || qizi == XQ_BLACK_JIANG)
//	{
//		TCHAR sz[200];
//		wsprintf(sz,"qizi=%d,ret=%d",qizi,ret);
//		WriteStr(sz,3,3);
//	}
	if (!ret) return FALSE;
	if(check)
		return true;
//	if(CheckOtherJiangJun(fromi,fromj,flag,m_iQiZi))
//		return 0;
	////AfxMessageBox("MoveShi");
/*	m_iQiZi[toi][toj] = qizi;
	m_iQiZi[fromi][fromj] = XQ_QIZI_NONE;
	::CopyMemory(qi,m_iQiZi,sizeof(m_iQiZi));
*/	return TRUE;
}

/**
 * 设置棋子值(用于回退)
 */
BOOL CUpGradeGameLogic::MoveQiZi(int fromi, int fromj, int toi, int toj, int qi[][9])
{
	::CopyMemory(m_iQiZi,qi,sizeof(m_iQiZi));
	int qizi = m_iQiZi[fromi][fromj];
	m_iQiZi[toi][toj] = qizi;
	m_iQiZi[fromi][fromj] = XQ_QIZI_NONE;
	::CopyMemory(qi,m_iQiZi,sizeof(m_iQiZi));	
	return true;
}

/**
 * 设置棋子值(用于回退)
 */
BOOL CUpGradeGameLogic::SetQiZi(int fx,int fy,int value)
{
	m_iQiZi[fx][fy] = value;
	return true;
}

/**
 * 获取棋子值
 */
int CUpGradeGameLogic::GetQiZiValue(int fx, int fy)
{
	return m_iQiZi[fy][fx];
}

BOOL CUpGradeGameLogic::MoveBin(int fx, int fy, int tx, int ty)
{
	if (abs(tx - fx) > 1 || abs(ty - fy) > 1) return FALSE;	//只能走一格
	if ((tx != fx) && (ty != fy)) return FALSE;	//只能横走或竖走
	
	//TCHAR sz[200];
	////wsprintf(sz,"fx=%d,fy=%d,源%d,目的%d,%d",fx,fy,GetQiZiValue(fx,fy),GetQiZiValue(tx,ty),m_iSideFlag);
//	WriteStr(sz,m_iSideFlag);

	//判别是谁的兵
	if(GetQiZiValue(fx,fy) * m_iSideFlag > 0)
	{
		if (ty > fy) return FALSE;	//只能前进不能后退
		if (fy > 4)	//没过河
		{
			if (tx != fx) return FALSE;	//只能竖走
		}
	}
	else//对方的兵
	{
		//if(GetQiZiValue(tx,ty)*m_iSideFlag == 6 )//||GetQiZiValue(tx,ty) == 6) //本方王
		//{
			//TCHAR sz[200];
		//	wsprintf(sz,"fx=%d,fy=%d,源%d,目的%d,%d",fx,fy,GetQiZiValue(fx,fy),GetQiZiValue(tx,ty),m_iSideFlag);
		//	WriteStr(sz,m_iSideFlag);
		//}
		if(ty < fy) return FALSE;	//不能后退
		if(fy < 5)					//敌家兵未过河
		{
			if( tx != fx) return FALSE;
		}
	}
	return TRUE;
}

BOOL CUpGradeGameLogic::MoveMa(int fx, int fy, int tx, int ty)
{
	if (abs(tx - fx) > 2 || abs(ty - fy) > 2) return FALSE;
	if (abs(ty - fy) == 2)
	{
		if (abs(tx - fx) != 1) return FALSE;
		if (fy > ty)
		{
			if (m_iQiZi[fy - 1][fx] != XQ_QIZI_NONE) return FALSE;	//蹩足马

		}
		else
		{
			if (m_iQiZi[fy + 1][fx] != XQ_QIZI_NONE) return FALSE; //蹩足马
		}
	}
	else if (abs(tx - fx) == 2)
	{
		if (abs(ty - fy) != 1) return FALSE;
		if (fx > tx)
		{
			if (m_iQiZi[fy][fx - 1] != XQ_QIZI_NONE) return FALSE; //蹩足马
		}
		else
		{
			if (m_iQiZi[fy][fx + 1] != XQ_QIZI_NONE) return FALSE; //蹩足马
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CUpGradeGameLogic::MovePao(int fx, int fy, int tx, int ty, int flag)
{
	int has = 0;
	if (tx != fx && ty != fy) return FALSE;
	if (ty != fy)
	{
		if (fy > ty)
		{
			for (int i = ty + 1;i < fy;i++)
			{
				if (m_iQiZi[i][fx] != XQ_QIZI_NONE) has++;
			}
			if (has > 1) return FALSE;
			if (has == 1)
			{
				if (flag * m_iQiZi[ty][tx] < 0) return TRUE;
				else return FALSE;
			}
			if (m_iQiZi[ty][tx] != XQ_QIZI_NONE) return FALSE;
		}

		else
		{
			for (int i = fy + 1;i < ty;i++)
			{
				if (m_iQiZi[i][fx] != XQ_QIZI_NONE) has++;
			}
			if (has > 1) return FALSE;
			if (has == 1) 
			{
				if (flag * m_iQiZi[ty][tx] < 0) return TRUE;
				else return FALSE;
			}
			if (m_iQiZi[ty][tx] != XQ_QIZI_NONE) return FALSE;
		}
	}

	else
	{
		if (fx > tx)
		{
			for (int i = tx + 1;i < fx;i++)
			{
				if (m_iQiZi[fy][i] != XQ_QIZI_NONE) has++;
			}
			if (has > 1) return FALSE;
			if (has == 1)
			{
				if (flag * m_iQiZi[ty][tx] < 0) return TRUE;
				else return FALSE;
			}
			if (m_iQiZi[ty][tx] != XQ_QIZI_NONE) return FALSE;
		}
		else
		{
			for (int i = fx + 1;i < tx;i++)
			{
				if (m_iQiZi[fy][i] != XQ_QIZI_NONE) has++;
			}
			if (has > 1) return FALSE;
			if (has == 1)
			{
				if (flag * m_iQiZi[ty][tx] < 0) return TRUE;
				else return FALSE;
			}
			if (m_iQiZi[ty][tx] != XQ_QIZI_NONE) return FALSE;
		}
	}
	
	return TRUE;
}

/**
 * 士的行棋路线只能是九宫内的斜线
 */
BOOL CUpGradeGameLogic::MoveShi(int fx, int fy, int tx, int ty)
{	
	
	if (abs(tx - fx) > 1 || abs(ty - fy) > 1) return FALSE;
	
	if (ty < 7 || tx < 3 || tx > 5) return FALSE;
	if (tx == fx || ty == fy) return FALSE;
	return TRUE;
}

/**
 * 将只能在九宫内走动，可走横竖线
 */
BOOL CUpGradeGameLogic::MoveShuai(int fx, int fy, int tx, int ty)
{
	if (ty < 7 && m_iQiZi[ty][tx] + m_iQiZi[fy][fx] == 0 && tx == fx)//將對將(吃對家將)
	{
		for (int i = ty + 1;i < fy;i++)
		{
			if (m_iQiZi[i][fx] != XQ_QIZI_NONE) return FALSE;
		}
		return TRUE;
	}
	if (fy < 3 && m_iQiZi[ty][tx] + m_iQiZi[fy][fx] == 0 && tx == fx)//將對將(對家吃本家將)
	{
		for (int i = fy + 1;i < ty;i++)
		{
			if (m_iQiZi[i][fx] != XQ_QIZI_NONE) return FALSE;
		}
		return TRUE;
	}

	if (ty < 7 || tx < 3 || tx > 5) return FALSE;
	if (abs(tx - fx) > 1 || abs(ty - fy) > 1) return FALSE;
	if (tx != fx && ty != fy) return FALSE;
	return TRUE;
}

/**
 * 相不能过河，只能走“田”
 */
BOOL CUpGradeGameLogic::MoveXiang(int fx, int fy, int tx, int ty)
{
	if (ty < 5) return FALSE;
	if (abs(tx - fx) != 2 || abs(ty - fy) != 2) return FALSE;
	if (m_iQiZi[(fy + ty) / 2][(fx + tx) / 2] != XQ_QIZI_NONE) return FALSE;
	return TRUE;
}

BOOL CUpGradeGameLogic::MoveZhu(int fx, int fy, int tx, int ty)
{
	if (tx != fx && ty != fy) return FALSE;
	if (ty != fy)
	{
		if (fy > ty)
		{
			for (int i = ty + 1;i < fy;i++)
			{
				if (m_iQiZi[i][fx] != XQ_QIZI_NONE) return FALSE;
			}
		}
		else
		{
			for (int i = fy + 1;i < ty;i++)
			{
				if (m_iQiZi[i][fx] != XQ_QIZI_NONE) return FALSE;
			}
		}
	}
	else
	{
		if (fx > tx)
		{
			for (int i = tx + 1;i < fx;i++)
			{
				if (m_iQiZi[fy][i] != XQ_QIZI_NONE) return FALSE;
			}
		}
		else
		{
			for (int i = fx + 1;i < tx;i++)
			{
				if (m_iQiZi[fy][i] != XQ_QIZI_NONE) return FALSE;
			}
		}
	}
	return TRUE;
}

/**
 * 本方將還是敵方將
 */
int CUpGradeGameLogic::CheckOtherOrMeJiang(int Startx, int Starty,int Endx,int Endy,int meflag,int MeBoard[][9])
{
	//敵方將返回2
	if(CheckOtherJiangJun( Startx,  Starty, Endx, Endy, meflag, MeBoard,false))
		return 2;
	//本方將返回1
	if(CheckMeJiangJun(Startx,  Starty, Endx, Endy, meflag, MeBoard))
		return 1;
	return 0;
}

/**
 * 检测是否将军(原理,获取敌方将或帅位置,将本方棋子遍历一次,看是否有子可以达到对方将帅的位置)
 */
int CUpGradeGameLogic::CheckMeJiangJun(int Startx, int Starty,int Endx,int Endy,int meflag,int MeBoard[][9])
{
	int endi,endj,icount=0;

	//查找敌方将
	CPoint point = GetQiZiKing(-1*meflag,MeBoard);
	endi=point.x;
	endj=point.y;
	if(endi >= 10 ||endj >=9)		//无效王位
		return 0;
	CPoint MePoint[16];				//一共16个子
	icount=GetMeQiZi(MePoint,meflag,MeBoard);

	if(icount<=0)					//无效子
		return 0;

	for(int i=0;i<icount;i++)
	{
		if(CanMoveQiZi(MePoint[i].x,MePoint[i].y,endi,endj,MeBoard,true))		//本方将
			return 1;
	}
	return 0;
}

/**
 * 敌方将本方(用以确定是否可以移动某些子)本方移动造成对方将军
 */
int CUpGradeGameLogic::CheckOtherJiangJun(int Startx, int Starty,int Endx,int Endy,int meflag,int MeBoard[][9],BOOL bTest)
{
	int TestBoard[10][9];
	::CopyMemory(TestBoard,MeBoard,sizeof(TestBoard));

	if(bTest)	//测试值
	{
		TestBoard[Endx][Endy] = TestBoard[Startx][Starty];
		TestBoard[Startx][Starty] = 0;
	}
	int endi,endj,icount=0;
	//本方将位置
	CPoint point = GetQiZiKing(meflag,TestBoard);
	endi=point.x;
	endj=point.y;
	if(endi == 10 ||endj >=9)		//无效王位
		return 0;
	CPoint MePoint[16];				//一共16个子

	icount=GetMeQiZi(MePoint,-1*meflag,TestBoard);//敌方棋子

	if(icount <= 0)					//无效子
		return 0;
	for(int i = 0;i < icount;i ++)
	{
		/*if(MePoint[i].y == 4)
		{
			TCHAR sz[200];
			wsprintf(sz,"源x=%d,y=%d,%d,目标x=%d,y=%d,%d,BOOL = %d",
				MePoint[i].x,MePoint[i].y,TestBoard[MePoint[i].x][MePoint[i].y],
				endi,endj,TestBoard[endi][endj],
				CanMoveQiZi(MePoint[i].x,MePoint[i].y,endi,endj,TestBoard,true));
			WriteStr(sz);
		}*/
		if(CanMoveQiZi(MePoint[i].x,MePoint[i].y,endi,endj,TestBoard,true))		//敌方将
			return 2;
	}
	return 0;
}

/**
 * 查找王的位置
 */
CPoint CUpGradeGameLogic::GetQiZiKing(int meflag,int meboard[][9])
{
	CPoint point(10,9);

	for(int i=0;i<10;i++)
	{
		for(int j=0;j<9;j++)
		{
			if(meboard[i][j]*meflag == 6)
			{
				point=CPoint(i,j);
				break;
			}
		}
	}
	return  point;
}

/**
 * 查找本方所有棋子位置
 */
int CUpGradeGameLogic::GetMeQiZi(CPoint point[],int meflag,int MeBoard[][9])
{
	int count=0;
	for(int i = 0;i < 10;i++ )
	{
		for(int j=0;j<9;j++)
		{
			if(MeBoard[i][j]*meflag > 0)		//本方棋子
			{
				point[count++]=CPoint(i,j);
			}
		}
	}
	return count;
}

/**
 * 查找本方是否有棋可动
 */
BOOL CUpGradeGameLogic::HaveQiZiMove(int meflag,int MeBoard[][9])
{
	if( IsLose( meflag,MeBoard)) return false;	//帅已经被吃
	//if( IsLive(meflag,MeBoard) )	 return true;							//有棋子可动
	//将自己的牌保存起来,对棋盘遍历一次
	if( IsDieCheck(meflag,MeBoard)) return false;							//死棋
	//if(	CanEatKing(meflag,MeBoard)) return false;						//可以吃对方王
	return true;// CheckOtherJiangJun(0,0,0,0,meflag,MeBoard,false);
	//return true;
}

/**
 * 胜负
 */
BOOL CUpGradeGameLogic::IsLose(int meflag, int MeBoard[][9])
{
	//本方将位置
	CPoint point = GetQiZiKing(meflag,MeBoard);
	int endi=point.x;
	int endj=point.y;
	if(endi == 10 ||endj >=9)		//无效王位
		return true;
	return false;	
}

/**
 * 是否有活动棋,且不会移动不会造成对方将军
 */
BOOL CUpGradeGameLogic::IsLive(int meflag,int MeBoard[][9])
{
	return true;
}

/**
 * 是否有活动棋,且不会移动不会造成对方将军
 */
BOOL CUpGradeGameLogic::CanEatKing(int meflag,int MeBoard[][9])
{
	int endi,endj,icount=0;

	//查找敌方将
	CPoint point = GetQiZiKing(-1*meflag,MeBoard);
	endi=point.x;
	endj=point.y;
	if(endi >= 10 ||endj >=9)		//无效王位
		return 0;
	CPoint MePoint[16];				//一共16个子
	icount=GetMeQiZi(MePoint,meflag,MeBoard);

	if(icount<=0)					//无效子
		return 0;

	for(int i=0;i<icount;i++)
	{
		if(CanMoveQiZi(MePoint[i].x,MePoint[i].y,endi,endj,MeBoard,true))		//本方将
			return 1;
	}
	return false;
}

/**
 * 是否死棋
 */
BOOL CUpGradeGameLogic::IsDieCheck(int meflag,int MeBoard[][9])
{
	int iCount = 0;
	CPoint MePoint[16];				//一共16个子
	iCount=GetMeQiZi(MePoint,meflag,MeBoard);

	if(iCount<=0)					//无效子
		return 0;

	//将自己的棋子对棋盘进行一次遍历.再差别是否还是他人将军状态
	for(int i=0;i<iCount;i++)
	{
		for(int x=0;x<10;x++)
			for( int y=0;y<9;y++)
			{
				//能否移动到此位置
				if(CanMoveQiZi(MePoint[i].x,MePoint[i].y,x,y,MeBoard,false))
				{
					//移动某一个子造子不将军状态
					if(!CheckOtherJiangJun(MePoint[i].x,MePoint[i].y,x,y,meflag,MeBoard,true))
						return false;
				}
			}

	}
	return true;
}

void CUpGradeGameLogic::SetBoard(int iMeFlag, int qi[][9])
{
	m_iSideFlag = iMeFlag;
	::CopyMemory(m_iQiZi,qi,sizeof(m_iQiZi));
}
