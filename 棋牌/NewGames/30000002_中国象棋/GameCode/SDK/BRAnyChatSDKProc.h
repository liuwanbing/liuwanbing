// BRAnyChatSDKProc.h : header file
//
#ifndef _BR_ANY_CHAT_SDK_PROC_H__
#define _BR_ANY_CHAT_SDK_PROC_H__

#include "BRAnyChatCoreSDK.h"

class CBRAnyChatSDKProc
{
public:
	CBRAnyChatSDKProc()
	{
		RegAnyChatSDKCallback();
	}
	virtual ~CBRAnyChatSDKProc()
	{
		UnRegAnyChatSDKCallback();
	}

private:
	// 注册SDK回调函数
	void RegAnyChatSDKCallback(void)
	{
		BRAC_SetVideoDataCallBack(BRAC_PIX_FMT_RGB24,VideoData_CallBack,this);
		BRAC_SetAudioDataCallBack(AudioData_CallBack,this);
		BRAC_SetVolumeChangeCallBack(VolumeChange_CallBack,this);
		BRAC_SetTextMessageCallBack(TextMessage_CallBack,this);
		BRAC_SetTransBufferExCallBack(TransBufferEx_CallBack,this);
		BRAC_SetTransBufferCallBack(TransBuffer_CallBack,this);
		BRAC_SetTransFileCallBack(TransFile_CallBack,this);
		BRAC_SetRecordSnapShotCallBack(RecordSnapShot_CallBack,this);
		BRAC_SetSDKFilterDataCallBack(SDKFilterData_CallBack,this);
		BRAC_SetNotifyMessageCallBack(NotifyMessage_CallBack,this);
	}
	// 注销SDK回调函数
	void UnRegAnyChatSDKCallback(void)
	{
		BRAC_SetVideoDataCallBack(BRAC_PIX_FMT_RGB24,VideoData_CallBack,NULL);
		BRAC_SetAudioDataCallBack(AudioData_CallBack,NULL);
		BRAC_SetVolumeChangeCallBack(VolumeChange_CallBack,NULL);
		BRAC_SetTextMessageCallBack(TextMessage_CallBack,NULL);
		BRAC_SetTransBufferExCallBack(TransBufferEx_CallBack,NULL);
		BRAC_SetTransBufferCallBack(TransBuffer_CallBack,NULL);
		BRAC_SetTransFileCallBack(TransFile_CallBack,NULL);
		BRAC_SetRecordSnapShotCallBack(RecordSnapShot_CallBack,NULL);
		BRAC_SetSDKFilterDataCallBack(SDKFilterData_CallBack,NULL);
		BRAC_SetNotifyMessageCallBack(NotifyMessage_CallBack,NULL);
	}

protected:
	// 连接服务器消息
	virtual void OnAnyChatConnectMessage(BOOL bSuccess) {}
	// 用户登陆消息
	virtual void OnAnyChatLoginMessage(DWORD dwUserId, DWORD dwErrorCode) {}
	// 用户进入房间消息
	virtual void OnAnyChatEnterRoomMessage(DWORD dwRoomId, DWORD dwErrorCode) {}
	// 房间在线用户消息
	virtual void OnAnyChatOnlineUserMessage(DWORD dwUserNum, DWORD dwRoomId) {}
	// 用户得到/释放mic消息
	virtual void OnAnyChatMicStateChgMessage(DWORD dwUserId, BOOL bGetMic) {}
	// 用户进入/退出房间消息
	virtual void OnAnyChatUserAtRoomMessage(DWORD dwUserId, BOOL bEnter) {}
	//网络断开消息
	virtual void OnAnyChatLinkCloseMessage(DWORD dwErrorCode) {}

	// 用户摄像头状态改变消息
	virtual void OnAnyChatCameraStateChgMessage(DWORD dwUserId, DWORD dwState) {}
	// 用户聊天模式改变消息
	virtual void OnAnyChatChatModeChgMessage(DWORD dwUserId, BOOL bPublicChat) {}
	// 用户活动状态发生变化消息
	virtual void OnAnyChatActiveStateChgMessage(DWORD dwUserId, DWORD dwState) {}

	// 用户私聊请求消息
	virtual void OnAnyChatPrivateRequestMessage(DWORD dwUserId, DWORD dwRequestId) {}
	// 用户私聊请求回复消息
	virtual void OnAnyChatPrivateEchoMessage(DWORD dwUserId, DWORD dwErrorCode) {}
	// 用户退出私聊消息
	virtual void OnAnyChatPrivateExitMessage(DWORD dwUserId, DWORD dwErrorCode) {}

	// 视频数据回调
	virtual void OnAnyChatVideoDataCallBack(DWORD dwUserid, LPVOID lpBuf, DWORD dwLen, LPBITMAPINFOHEADER lpbmiHeader) {}
	// 音频数据回调
	virtual void OnAnyChatAudioDataCallBack(DWORD dwUserid, LPVOID lpBuf, DWORD dwLen, LPWAVEFORMATEX lpWaveFormatEx) {}
	// 发送文字的回调函数
	virtual void OnAnyChatTextMsgCallBack(DWORD dwFromUserid, DWORD dwToUserid, BOOL bSecret, LPCTSTR lpMsgBuf, DWORD dwLen) {}
	// 透明通道回调函数
	virtual void OnAnyChatTransBufferCallBack(DWORD dwUserid, LPBYTE lpBuf, DWORD dwLen) {}
	// 透明通道数据扩展回调函数
	virtual void OnAnyChatTransBufferExCallBack(DWORD dwUserid, LPBYTE lpBuf, DWORD dwLen, DWORD wParam, DWORD lParam, DWORD dwTaskId) {}
	// 文件传输回调函数
	virtual void OnAnyChatTransFileCallBack(DWORD dwUserid, LPCTSTR lpFileName, LPCTSTR lpTempFilePath, DWORD dwFileLength, DWORD wParam, DWORD lParam, DWORD dwTaskId) {}
	// 音量变化回调函数
	virtual void OnAnyChatVolumeChangeCallBack(BRAC_AudioDevice device, DWORD dwCurrentVolume) {}
	// SDK Filter 通信数据回调函数
	virtual void OnAnyChatSDKFilterDataCallBack(LPBYTE lpBuf, DWORD dwLen) {}
	// 录像、快照任务完成回调函数
	virtual void OnAnyChatRecordSnapShotCallBack(DWORD dwUserid, LPCTSTR lpFileName, DWORD dwParam, BOOL bRecordType) {}
	// 异步消息通知回调函数
	virtual void OnAnyChatNotifyMessageCallBack(DWORD dwNotifyMsg, DWORD wParam, DWORD lParam) {}


	// 视频数据回调函数定义
	static void CALLBACK VideoData_CallBack(DWORD dwUserid, LPVOID lpBuf, DWORD dwLen, BITMAPINFOHEADER bmiHeader, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(pAnyChatSDKProc)
			pAnyChatSDKProc->OnAnyChatVideoDataCallBack(dwUserid,lpBuf,dwLen,&bmiHeader);
	}
	// 音频数据回调函数定义
	static void CALLBACK AudioData_CallBack(DWORD dwUserid, LPVOID lpBuf, DWORD dwLen, WAVEFORMATEX waveFormatEx, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(pAnyChatSDKProc)
			pAnyChatSDKProc->OnAnyChatAudioDataCallBack(dwUserid,lpBuf,dwLen,&waveFormatEx);
	}
	// 文字消息回调函数定义
	static void CALLBACK TextMessage_CallBack(DWORD dwFromUserid, DWORD dwToUserid, BOOL bSecret, LPCTSTR lpMsgBuf, DWORD dwLen, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(pAnyChatSDKProc)
			pAnyChatSDKProc->OnAnyChatTextMsgCallBack(dwFromUserid,dwToUserid,bSecret,lpMsgBuf,dwLen);
	}
	// 透明通道数据回调函数定义
	static void CALLBACK TransBuffer_CallBack(DWORD dwUserid, LPBYTE lpBuf, DWORD dwLen, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(pAnyChatSDKProc)
			pAnyChatSDKProc->OnAnyChatTransBufferCallBack(dwUserid,lpBuf,dwLen);
	}
	// 透明通道数据扩展回调函数定义
	static void CALLBACK TransBufferEx_CallBack(DWORD dwUserid, LPBYTE lpBuf, DWORD dwLen, DWORD wParam, DWORD lParam, DWORD dwTaskId, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(pAnyChatSDKProc)
			pAnyChatSDKProc->OnAnyChatTransBufferExCallBack(dwUserid,lpBuf,dwLen,wParam,lParam,dwTaskId);
	}
	// 文件传输回调函数定义
	static void CALLBACK TransFile_CallBack(DWORD dwUserid, LPCTSTR lpFileName, LPCTSTR lpTempFilePath, DWORD dwFileLength, DWORD wParam, DWORD lParam, DWORD dwTaskId, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(pAnyChatSDKProc)
			pAnyChatSDKProc->OnAnyChatTransFileCallBack(dwUserid,lpFileName,lpTempFilePath,dwFileLength,wParam,lParam,dwTaskId);
	}
	// 音量变化回调函数定义
	static void CALLBACK VolumeChange_CallBack(BRAC_AudioDevice device, DWORD dwCurrentVolume, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(pAnyChatSDKProc)
			pAnyChatSDKProc->OnAnyChatVolumeChangeCallBack(device,dwCurrentVolume);
	}
	// SDK Filter 通信数据回调函数定义
	static void CALLBACK SDKFilterData_CallBack(LPBYTE lpBuf, DWORD dwLen, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(pAnyChatSDKProc)
			pAnyChatSDKProc->OnAnyChatSDKFilterDataCallBack(lpBuf,dwLen);
	}
	// 录像、快照任务完成回调函数定义
	static void CALLBACK RecordSnapShot_CallBack(DWORD dwUserid, LPCTSTR lpFileName, DWORD dwParam, BOOL bRecordType, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(pAnyChatSDKProc)
			pAnyChatSDKProc->OnAnyChatRecordSnapShotCallBack(dwUserid,lpFileName,dwParam,bRecordType);
	}
	// 异步消息通知回调函数定义
	static void CALLBACK NotifyMessage_CallBack(DWORD dwNotifyMsg, DWORD wParam, DWORD lParam, LPVOID lpUserValue)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBRAnyChatSDKProc*	pAnyChatSDKProc = (CBRAnyChatSDKProc*)lpUserValue;
		if(!pAnyChatSDKProc)
			return;
		switch(dwNotifyMsg)
		{
		case WM_GV_CONNECT:			pAnyChatSDKProc->OnAnyChatConnectMessage(wParam);				break;
		case WM_GV_LOGINSYSTEM:		pAnyChatSDKProc->OnAnyChatLoginMessage(wParam,lParam);			break;
		case WM_GV_ENTERROOM:		pAnyChatSDKProc->OnAnyChatEnterRoomMessage(wParam,lParam);		break;
		case WM_GV_MICSTATECHANGE:	pAnyChatSDKProc->OnAnyChatMicStateChgMessage(wParam,lParam);	break;
		case WM_GV_USERATROOM:		pAnyChatSDKProc->OnAnyChatUserAtRoomMessage(wParam,lParam);		break;
		case WM_GV_LINKCLOSE:		pAnyChatSDKProc->OnAnyChatLinkCloseMessage(wParam);				break;
		case WM_GV_ONLINEUSER:		pAnyChatSDKProc->OnAnyChatOnlineUserMessage(wParam,lParam);		break;
			
		case WM_GV_CAMERASTATE:		pAnyChatSDKProc->OnAnyChatCameraStateChgMessage(wParam,lParam);	break;
		case WM_GV_CHATMODECHG:		pAnyChatSDKProc->OnAnyChatChatModeChgMessage(wParam,lParam);	break;
		case WM_GV_ACTIVESTATE:		pAnyChatSDKProc->OnAnyChatActiveStateChgMessage(wParam,lParam);	break;
			
		case WM_GV_PRIVATEREQUEST:	pAnyChatSDKProc->OnAnyChatPrivateRequestMessage(wParam,lParam);	break;
		case WM_GV_PRIVATEECHO:		pAnyChatSDKProc->OnAnyChatPrivateEchoMessage(wParam,lParam);	break;
		case WM_GV_PRIVATEEXIT:		pAnyChatSDKProc->OnAnyChatPrivateExitMessage(wParam,lParam);	break;
		default:
			break;
		}
		pAnyChatSDKProc->OnAnyChatNotifyMessageCallBack(dwNotifyMsg,wParam,lParam);
	}
};

#endif // #ifndef _BR_ANY_CHAT_SDK_PROC_H__