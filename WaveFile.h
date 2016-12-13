#pragma once
#include"DxLib.h"

#define USE_DATA_FROM_DXLIB//http://dxlib.o.oo7.jp/cgi/patiobbs/patio.cgi?mode=view&no=3385
#define MAX_CHANNELS 2

class WaveFile
{
public:
	//创建一个 DxLib 音频对象
	WaveFile();
	~WaveFile();
	//创建一个 DxLib 音频对象并加载文件
	WaveFile(const TCHAR*);
	//加载文件
	bool LoadFile(const TCHAR*);
	//释放资源
	bool Release();
	//播放文件，参数为是否从头开始播放
	bool Play(int resetPos = TRUE);
	//暂停播放
	void Pause();

	//获取音频句柄
	int GetSoundHandle();
	//获取可修改音频句柄
	int GetSoftSoundHandle();
	//获取当前的播放位置（采样点）
	int GetCurrentPlaySamplePos();
	//获取采样点中的电平，数值长度为 GetSampleVarBitsLength() 位
	int GetSampleVar(int sample, int channel);
	//获取采样点电平数值的长度
	int GetSampleVarBitsLength();
	//获取声道数
	int GetChannelNum();
	//获取采样频率
	int GetSampleRate();
	//获取采样数
	int GetSampleCount();
	//获取频谱数值，calc_length_in_sample 只能是 256～16384 之间的 2 的倍数
	void GetFFTVar(int sample, int channel, int calc_length_in_sample, float* outbuf, int nbuf);
private:
	int sampleRate;
	int nChannels;
	int samplesCount;
	int handleSound;
	int handleSoftSound;
	int bitsPerVarInSample;
	int isFloatType;

	int sampleVar[MAX_CHANNELS];
};
