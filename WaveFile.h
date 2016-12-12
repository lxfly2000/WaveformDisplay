#pragma once
#include"DxLib.h"

#define USE_DATA_FROM_DXLIB//http://dxlib.o.oo7.jp/cgi/patiobbs/patio.cgi?mode=view&no=3385
#define MAX_CHANNELS 2

class WaveFile
{
public:
	WaveFile();
	~WaveFile();
	WaveFile(const TCHAR*);
	bool LoadFile(const TCHAR*);
	bool Release();
	bool Play(int resetPos = TRUE);
	void Pause();

	int GetSoundHandle();
	int GetSoftSoundHandle();
	int GetCurrentPlaySamplePos();
	//获取采样点中的电平，有效数值：-32768～32767
	int GetSampleVar(int sample, int channel);
	//获取频谱数值，calc_length_in_sample 只能是 256～16384 之间的 2 的倍数
	void GetFFTVar(int sample, int channel, int calc_length_in_sample, float* outbuf, int nbuf);
	int sampleRate;
	int nChannels;
	int samplesCount;
private:
	int handleSound;
	int handleSoftSound;
	int bitsPerVarInSample;
	int isFloatType;

	int sampleVar[MAX_CHANNELS];
};
