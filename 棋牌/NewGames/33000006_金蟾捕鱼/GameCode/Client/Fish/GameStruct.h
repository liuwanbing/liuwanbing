//各种结构体定义
//子弹结构

struct FilesParmesA
{
    int                             m_PngSize;                              //文件大小
};
struct Bullet
{
	 //子弹是否存在
	 bool	m_Have;
	 //子弹速度
	 int	m_Speed;
	 //子弹角度
	 float	m_Roation;
	 //子弹坐标
	 POINT	m_Point;
	 //发射玩家
	 int	m_Senduser;
	 //子弹状态
	 bool	m_Stop;
	 //网状序列
	 int	m_Netindex;
	 //
	 float	m_ptx;
	 //
	 float	m_pty;
	 //是否超级子弹
	 bool	m_issuper;
	 //子弹3可
	 bool	m_isThree;
	 //是否机器人发射的
	 bool	m_isAndroid;
	 //发射真实
	 int	m_RealUser;
};
//鱼儿数据
struct Fish
{
     //鱼儿序列
	 int m_index;
	 //鱼儿ID
	 int m_fishID;
	 //当前动作序列
	 int m_actionindex;
	 //当前是否被捕获
	 bool m_isHit;
	 //是否存在
	 bool m_Have;
	 //当前坐标
	 POINT m_Point;
	 //当前角度
	 float m_Roation;
	 //死亡金币,在鱼儿死亡时间由服务端发送过来的
	 int m_DeadMoney;
	 //捕获玩家,捕获这鱼的玩家，由服务器发送过来
	 int m_HitUser;
	 //私人
	 int m_Nowindex;
	 //
	 int m_PtIndex;
	 //
	 int m_Currtime;
	 //
	 int m_speed;
	 //
	 float m_ptx;
	 //
	 float m_pty;
	 //附带小鱼
	 int  m_smallFish;
	 //
	 int  m_traceIndex;
	 //
	 int  m_creatTime;


};
//飘落金币图片
struct Gold
{
	 //飘落玩家
	 int m_Player;
	 //是否存在
	 bool m_Have;
	 //当前坐标
	 POINT m_Point;
	 //当前角度
	 float m_Roation;
	 //死亡金币,在鱼儿死亡时间由服务端发送过来的
	 int m_DeadMoney;
	  //当前动作序列
	 int m_actionindex;
	 //
	 float m_ptx;
	 //
	 float m_pty;


};
//撒钱动画
struct MuchMoneyaction
{   
	 //是否存在
     bool m_have;
	 //坐标X
	 int m_ptx;
	 //
	 int m_pty;
	 //
	 int m_nowindex;
	 //撒多少钱
	 int m_DiuMoney;
	 //转换角度
	 int m_Roation;


};
//定住动画
struct Dingaction
{   
	 //是否存在
     bool m_have;
	 //坐标X
	 int m_ptx;
	 //
	 int m_pty;
	 //
	 int m_nowindex;
	
};
//玩家钱堆数组,每个玩家应该有3个这样的数据结构
struct MoneyDui
{
	 int m_Have;
	 //
	 int m_Money;
	 //
	 int m_Time;
	 //
	 //
	 int m_FishScore;
	 //
	 int m_Nowscore;
    
};
//浪潮切换数据
struct WaterWavestruct
{
	  //浪潮坐标
      int m_ptx;
	  //
	  int m_pty;
	  //时间索引
	  int m_Time;
	  //当前图片
	  int m_currImgIndex;
}
;
//打死鱼后显示数字
struct Numstruct
{
	  int m_Have;
	  //浪潮坐标
      int m_ptx;
	  //
	  int m_pty;
	  //时间索引
	  int m_Time;
	  //当前图片
	  int m_currImgIndex;
	  //
	  int m_Numcount;
	  //
	  int  m_HitUser;
	  //
	  float m_beishu;

}
;
//打死鱼后显示数字
struct Soundmstruct
{
	  int m_Have;
	  int m_Time;
      LPCTSTR m_path;
	 

}
;
struct CursorStruct
{
      int m_ptx;
	  int m_pty;
	  int m_have;
};

