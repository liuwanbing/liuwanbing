#ifndef __CMD_GAMELOGIC_H_
#define __CMD_GAMELOGIC_H_

#include "..\..\MJFrame\Include\Common\CommonStructs.h"
#include "..\..\MJFrame\Include\Common\Frame\mjframebasedata.h"


//开发库版本
#define DEV_LIB_VER				        1				///开发库版本


using namespace MyGame;

namespace MyGame
{

#pragma pack(1)

    // 开发人员定义的GameMessage的ID需要在51以后
    #define SC_MJ_NOTIFY_DEALER       52
    #define CS_MJ_REQUEST_DEAL_FINISH 56
 
	/**
	* @ingroup MJFrame
	* @brief 通知庄家位置
	*/	
	struct NCMJ_TCMD_NOTIFY_DEALER : public TCMD_HEADER
	{
		BYTE byDealer;	  // 庄家位置

		NCMJ_TCMD_NOTIFY_DEALER ()
		{
			memset(this, 0, sizeof(NCMJ_TCMD_NOTIFY_DEALER));
			cCmdPara = SC_MJ_NOTIFY_DEALER;
			cCmdID = MJFRAME_MOUDLE_MSG;
			nDataLen = sizeof(NCMJ_TCMD_NOTIFY_DEALER);
		}
	};

    /**
    * @ingroup MJFrame
    * @brief 请求发牌完成
    */	
    struct NCMJ_TCMD_REQUEST_DEAL_FINISH : public TCMD_HEADER
    {
        NCMJ_TCMD_REQUEST_DEAL_FINISH ()
        {
            memset(this, 0, sizeof(NCMJ_TCMD_REQUEST_DEAL_FINISH));
            cCmdPara = CS_MJ_REQUEST_DEAL_FINISH;
            cCmdID = MJFRAME_MOUDLE_MSG;
            nDataLen = sizeof(NCMJ_TCMD_REQUEST_DEAL_FINISH);
        }
    };


#pragma pack()
	
}

#endif


