#pragma once
#include <map>
/**
* @brief 取得成员函数指针地址
*/
template <typename ToType, typename FromType>
void GetMemberFuncAddr_VC6(ToType& addr,FromType f)
{
    union 
    {
        FromType _f;
        ToType   _t;
    }ut;

    ut._f = f;

    addr = ut._t;
}
//调用类成员函数
//callflag:成员函数调用约定类型,0--thiscall,非0--其它类型. (注意， thiscall 是VC的默认类型)，
//funcaddr:成员函数地址.
//This:类对象的地址.
//count:成员函数参数个数.
//...:成员函数的参数列表.
DWORD CallMemberFunc(int callflag,DWORD funcaddr,void *This,int count,...)
{
    DWORD re;

    if(count>0)//有参数,将参数压入栈.
    {
        __asm
        {
            mov  ecx,count;//参数个数,ecx,循环计数器.
            mov  edx,ecx;
            shl  edx,2;    
            add  edx,0x14;  edx = count*4+0x14;

next:	push  dword ptr[ebp+edx];
            sub   edx,0x4;
            dec   ecx  
                jnz   next;
        }
    }

    //处理this指针.
    if(callflag==0) //__thiscall,vc默认的成员函数调用类型.
    {
        __asm mov ecx,This;
    }
    else//__stdcall
    {
        __asm push This;
    }

    __asm//调用函数
    {
        call funcaddr;
        mov  re,eax;
    }

    return re;
}

// 与MFC的BEGIN_MESSAGE_MAP使用方法相同
// 在类头文件中使用DECLARE_UIMESSAGE_MAP
#define DECLARE_UIMESSAGE_MAP() \
    private:\
    std::map<int, DWORD_PTR>       m_MessageMapUILButtonHandler;\
    std::map<int, DWORD_PTR>       m_MessageMapUILButtonDownHandler;\
    std::map<int, DWORD_PTR>       m_MessageMapUIAniFinish;\
    void MessageMapFunc();          
//
#define BEGIN_UIMESSAGE_MAP(CLASS_NAME)\
    void CLASS_NAME::MessageMapFunc(){
// 左键单击宏
#define ON_LBUTTONCLICK(ID, FUNCNAME) \
    GetMemberFuncAddr_VC6(m_MessageMapUILButtonHandler[ID], &FUNCNAME);
// 左键按下宏
#define ON_LBUTTONDOWNID(ID, FUNCNAME) \
    GetMemberFuncAddr_VC6(m_MessageMapUILButtonDownHandler[ID], &FUNCNAME);
// 动画结束宏
#define ON_ANIFINISH(ID, FUNCNAME) \
    GetMemberFuncAddr_VC6(m_MessageMapUIAniFinish[ID], &FUNCNAME);
//
#define END_UIMESSAGE_MAP()\
    };
// 另外还需要以下两个步骤
// 1、在构造函数中调用CALL_UIMESSAGE_MAP()
// 2、在OnUIMessage中调用 CALL_UIMESSAGE(pMessage)
#define CALL_UIMESSAGE_MAP()\
    MessageMapFunc();
#define CALL_UIMESSAGE(pMessage)\
    if(pMessage->_uMessageType==UI_LBUTTONUP)\
{\
    if (m_MessageMapUILButtonHandler[pMessage->_uControlID]!=0)\
    CallMemberFunc(0, m_MessageMapUILButtonHandler[pMessage->_uControlID], this, 1, pMessage);\
}\
    else if(pMessage->_uMessageType==UI_CATOONFINISH)\
{\
    if (m_MessageMapUIAniFinish[pMessage->_uControlID]!=0)\
    CallMemberFunc(0, m_MessageMapUIAniFinish[pMessage->_uControlID], this, 1, pMessage);\
}\
    else if (pMessage->_uMessageType==UI_LBUTTONDOWN)\
{\
    if (m_MessageMapUILButtonDownHandler[pMessage->_uControlID]!=0)\
    CallMemberFunc(0, m_MessageMapUILButtonDownHandler[pMessage->_uControlID], this, 1, pMessage);\
}

////////////////////////////////////////////////////////////////////////////////
// SOCKET消息函数
#define DECLARE_SOCKETMESSAGE_MAP() \
    private:\
    std::map<int, DWORD_PTR>       m_MessageMapSocketHandler;\
    void SocketMessageMapFunc();          
//
#define BEGIN_SOCKETMESSAGE_MAP(CLASS_NAME)\
    void CLASS_NAME::SocketMessageMapFunc(){
// 左键单击宏
#define ON_SOCKETRECV(nSubId, FUNCNAME) \
    GetMemberFuncAddr_VC6(m_MessageMapSocketHandler[nSubId], &FUNCNAME);
//
#define END_SOCKETMESSAGE_MAP()\
    };
// 另外还需要以下两个步骤
// 1、在构造函数中调用CALL_SOCKETMESSAGE_MAP()
// 2、在HandleGameMessage中调用 CALL_SOCKETMESSAGE(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
#define CALL_SOCKETMESSAGE_MAP()\
    SocketMessageMapFunc();
#define CALL_SOCKETMESSAGE(nMainID,nSubId,buffer,nLen)\
    if (m_MessageMapSocketHandler[nSubId]!=0)\
    CallMemberFunc(0, m_MessageMapSocketHandler[nSubId], this, 2, buffer,nLen); 


