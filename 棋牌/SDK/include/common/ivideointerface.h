#pragma once

#define BRVIDEOIMPL_DLL
#ifdef BRVIDEOIMPL_DLL
#define BRVIDEOIMPL_EXP __declspec(dllexport)
#else
#define BRVIDEOIMPL_EXP __declspec(dllimport)
#endif

#define VIDEO_MAX_PEOPLE 16
/*
视频接口
*/
class ivideointerface
{
public:
    virtual ~ivideointerface(){}
    virtual void show_video(int userid, bool bshow)=0;
    virtual void mute(int userid)=0;
    // pos按平台规则来
    virtual void set_user_id(int roomid,int userid, int pos)=0;
    //
    virtual void set_self_user_id(int roomid, int userid)=0;
    //
    virtual void reset_pos(const std::string filename)=0;
    //
    virtual void set_video_pos(int pos, int x, int y)=0;
    //
    virtual void exit()=0;
};




extern "C" BRVIDEOIMPL_EXP ivideointerface *create_video_interface(int playercount, long long parentwndid, std::string ip, int videoport, int audioport);
extern "C" BRVIDEOIMPL_EXP void free_video_interface(ivideointerface *p);

