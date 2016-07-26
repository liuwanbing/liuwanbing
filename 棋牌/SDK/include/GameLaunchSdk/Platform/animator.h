#ifndef _ANIMATOR_H_INCLUDE
#define _ANIMATOR_H_INCLUDE

#include <string>
#include <vector>

#define CHUNK 16384

typedef unsigned long  uLong; 
typedef unsigned char  Byte;  
typedef Byte  FAR Bytef;

/**
 * 自定义动画类
 */
class Animator
{
public:
	/**
	 * 动画用到的坐标结构
	 */
	struct Point
	{
		Point():x(0),y(0) {}
		Point(int _x,int _y):x(_x),y(_y) {}

		int x,y;                                 // 坐标X,Y
	};

    /**
     * 动画文件头结构
     */
    struct FileHeader
    {
		FileHeader():files_count(0),width(0),height(0),speed(0) {}

        char marker[3];                         // 文件标识 "S","P","R"
        int files_count;                        // 文件数量

	    int width,height;                       // 动画的高宽
		int speed;                              // 动画的播放速度
    };

    /**
     * 文件结构
     */
    struct FileStruct
    {
		FileStruct():startx(0),starty(0),isDisplay(true),startposition(0),uncompressed_size(0),compressed_size(0),
			speed(0)
		{
		
		}

        char  filename[256];                    // 文件完整名称
	    int startx,starty;                      // 动画播放的开始位置
		bool isDisplay;                         // 是否显示动画
		int speed;                              // 每帧动画的播放速度

        uLong startposition;                    // 文件在整个压缩包中的开始位置

        uLong uncompressed_size;                // 正常的文件长度
        uLong compressed_size;                  // 压缩后的文件长度
    };

	/// 构造函数
	Animator();
	/// 析构函数
	~Animator();

	/// 打开指定的动画文件(路径必须为相对于当前目录的路径)
	bool OpenAnimator(std::string filepath); 
	/// 从内存中加载动画文件
	bool OpenAnimatorFromMemory(Byte *data,uLong length);
	/// 关闭动画文件
	void CloseAnimator(void);
	/// 将动画保存到指定的路径(路径必须为绝对路径，如果为空的话，表示使用我们打开的动画文件，如果不存在就抛出错误)
	bool SaveAnimator(std::string filepath="");
	/// 检测当前是否在工作中
	bool IsWorking(void);

	/// 得到当前库的工作路径
	inline std::string GetCurrentPath(void) { return m_CurrentPath + "\\"; }
	/// 得到当前动画的名称
	inline std::string GetName(void) { return m_FileName; }
	/// 得到当前动画的帧数
	inline int GetFrameCount(void) { return (int)m_FileList.size(); }
	/// 设置动画的高宽
	void SetWidthAndHeight(int w,int h);
	/// 得到动画的高宽
	void GetWidthAndHeight(int *w,int *h);
	/// 设置动画播放的速度
	inline void SetSpeed(int s) { m_Header.speed = s; }
	/// 得到动画播放的速度
	inline int GetSpeed(void) { return m_Header.speed; }
	/// 设置指定动画帧的速度
	void SetFrameSpeed(int index,int s);
	/// 得到指定动画帧的速度
	int GetFrameSpeed(int index);
	/// 设置指定帧的显示位置
	void SetStartPosForFrame(int index,Animator::Point pos);
	/// 得到指定帧的显示位置
	Animator::Point GetStartPosForFrame(int index);	
	/// 得到指定帧的信息
	Animator::FileStruct* GetFrameInfoById(int index);
	/// 得到指定帧图片名称的帧信息
	Animator::FileStruct* GetFrameInfoByName(std::string filename);

	/// 添加指定的帧图片到动画文件中
	bool AddFrame(std::string filepath,int sx,int sy,bool isShow,int sp);
	/// 清空当前的动画帧列表
	inline void ClearFrame(void) { m_FileList.clear(); }
	/// 根据指定ID得到帧图片数据
	Byte* GetFrameById(int index,uLong* length);
	/// 根据指定名称得到帧图片数据
	Byte* GetFrameByName(std::string name,uLong* length);
	/// 在指定的路径得到所有的帧图片(路径必须为绝对路径，如果设置成空的话表示使用当前路径)
	bool GetAllFrames(std::string path="");

private:
    /// 用于压缩制定的文件到制定的包文件中
    int def(FILE *source, FILE *dest,uLong *size,int level,bool iswrite=true);
    /// 用于解压压缩包中的所有的文件
    int inf(FILE *source, FILE *dest,uLong length,uLong *clength);
	/// 用于解压压缩包中的制定文件到内存中
	int inf(FILE *source, Byte *dest,uLong length,uLong *clength);
	/// 用于结束指定内存中的数据到指定内存中
	int inf(Byte *src, Byte *dest,uLong startpos,uLong length,uLong *clength);
    /// 拷贝制定大小的内存数据到指定内存中
    void CopyDataToMemory(Bytef *dest,Bytef *src,uLong pos,uLong size);
	/// 根据文件路径名建立相应的目录
	void CreateDirectionByPath(std::string path,std::string filename);

	/// 添加文件头和文件结构组到文件中
	bool AddFileHeaderAndFileStructs(void);
	/// 压缩所有当前系统中拥有的所有帧到动画文件中
	bool CompressAllFileInSystem(void);

private:
    std::string                             m_FileName;     // 要操作的动画文件路径名
	std::string                             m_CurrentPath;  // 当前程序的路径

    FileHeader                              m_Header;       // 动画文件的头
    std::vector<FileStruct>                 m_FileList;     // 动画文件中动画的帧列表

	FILE*                                   m_File;         // 文件读写
	Byte*                                   m_AnimData;     // 动画数据 
};

#endif
