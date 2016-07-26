#include <Windows.h>

#ifdef AUDIO_EXPORTS
#define AUDIO_API __declspec(dllexport)
#else
#define AUDIO_API __declspec(dllimport)
#endif AUDIO_EXPORTS

interface IAudioPlay
{
public:
	virtual ~IAudioPlay(void) {};
public:
	///播放声音
	virtual void Play() = 0;
	///播放声音
	virtual void Play(const char* stFileName) = 0;
	///停止播放
	virtual void Stop() = 0;
	///暂停
	virtual void Pause() = 0;
	///继续播放
	virtual void Continue() = 0;
	//获取音量
	virtual int GetVolume() = 0;
	///设置音量
	virtual void SetVolume(int iVolume) = 0;
	///是否在播放中
	virtual bool IsPlaying() = 0;
	///重新播放
	virtual void ResetPlay() = 0;
	///跳帧
	virtual void SetPosition(int iPosition) = 0;
	///获取当前帧
	virtual int GetPosition() = 0;
	///获取播放文件长度
	virtual int GetLength() = 0;
	///是否循环播放
	virtual void SetLoopPlay(bool isLoop) = 0;
	///是否循环播放
	virtual int GetLoopPlay() = 0;
public:
	virtual void Next() = 0;
	virtual void Prep() = 0;
	virtual void Add(const char* stFileName) = 0;
	virtual void Delete(const char* stFileName) = 0;
};
AUDIO_API IAudioPlay * CreateAudioPlay();
AUDIO_API void DeleteAudioPlay(IAudioPlay** pAudioPlay);

extern "C" AUDIO_API IAudioPlay * CreateAudioPlayC();
extern "C" AUDIO_API void DeleteAudioPlayC(IAudioPlay** pAudioPlay);

typedef IAudioPlay *(*CREATEAUDIOPLAYC)();
typedef void (*DELETEAUDIOPLAYC)(IAudioPlay** pAudioPlay);
