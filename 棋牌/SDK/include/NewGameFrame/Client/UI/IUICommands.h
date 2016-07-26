#pragma once

#include "FastDelegate.h"
using namespace fastdelegate;
typedef FastDelegate2<int, int> CMDCallBack;

struct IUserInterface;
struct IBCInterFace;
/** @brief 基本的命令对像
 */
class IUICommand
{
public:
    virtual ~IUICommand(){}
    /** @brief 执行命令
     */
    virtual int Execute()=0;
    /** @brief 撒消执行
     */
    virtual int Undo()=0;
    /** @brief 获取控制控件的ID号
     */
    virtual int GetRecieverID()=0;
};


/** @brief 执行命令接口
 */
class ICmdInterface
{
public:
    /** @brief 向后撒消一步
     */
    virtual int StepRollBack() = 0;
    /** @brief 向前重复一步
     */
    virtual int StepReplyFront() = 0;
    /** @brief 设置某控件的位置
     */
    virtual IUICommand *SetModeXY(int nID, int nX, int nY) = 0;
    /** @brief 设置某控件的宽度
     */
    virtual IUICommand *SetW(int nID, int nW) = 0;
    /** @brief 设置某控件的高度
     */
    virtual IUICommand *SetH(int nID, int nH) = 0;
    /** @brief 加入一个控件
    */
    virtual IUICommand *AddControl(IUserInterface *pUserInterface, IBCInterFace *pCtrl, IBCInterFace *pParent, CMDCallBack CallBack) = 0;
    /** @brief 删除一个控件
    */
    virtual IUICommand *DelControl(IUserInterface *pUserInterface, IBCInterFace *pCtrl, CMDCallBack CallBack) = 0;
    /** @brief 移动一个控件
    */
    virtual IUICommand *MoveControl(IUserInterface *pUserInterface, int nToMoveID, int nDstContain, CMDCallBack CallBack) = 0;

};