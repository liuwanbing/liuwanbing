#pragma once
#include "GameLanchSDK/bzwUiInclude/bzwUI.h"
//#include "defines.h"
#define DLG_MSG_MB_OK     1
#define DLG_MSG_MB_YESNO  2
#define DLG_MSG_MB_OK_NEW  3
#define DLG_MSG_MB_YESNO_NEW  4
// 消息提示对话框定义 
#define IDD_MSG_DLG_RES_CONFIRM		"resources\\config\\Confirm.xml"	/**< 确认对话框路径 */ 
#define IDD_MSG_DLG_RES_SELECT		"resources\\config\\Select.xml"		/**< 选择对话框路径  */
#define IDD_MSG_DLG_RES_CONFIRM_NEW		"resources\\config\\Confirm_new.xml"	/**< 确认对话框路径 */ 
#define IDD_MSG_DLG_RES_SELECT_NEW		"resources\\config\\Select_new.xml"		/**< 选择对话框路径  */
#define IDD_DLG_BTN_OK				10001	/**< 确定按钮 */
#define IDD_DLG_BTN_CANCLE			10005	/**< 取消按钮 */
#define IDD_DLG_TIP_LABLE			12345	/**< 标签控件 */

class CAFCMessageBox : public BModalContainer
{
public:
	CAFCMessageBox(void);

	~CAFCMessageBox(void);

	virtual bool OnPostMessage(UINT pId, BBaseParameter* pParam);

	void SetMessage(CString message);

    void SetMessage_New(CString message);

};

//extern UINT AFCMessageBox(CString strInfo,CString strTitel,CWnd* pParentWnd,UINT style=DLG_MSG_MB_OK);

