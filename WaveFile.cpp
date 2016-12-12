#include "WaveFile.h"
#include"..\MyCodes\MyUtilities.h"
#include<cassert>

WaveFile::WaveFile() :handleSound(0), handleSoftSound(0)
{
}

WaveFile::~WaveFile()
{
	Release();
}

WaveFile::WaveFile(const TCHAR *filepath) :WaveFile()
{
	LoadFile(filepath);
}

bool WaveFile::LoadFile(const TCHAR *filepath)
{
	if (handleSound || handleSoftSound)Release();
	handleSoftSound = LoadSoftSound(filepath);
	handleSound = LoadSoundMemFromSoftSound(handleSoftSound); //handleSound = LoadSoundMem(filepath);//不知为何这样占的内存反而少
	//获取采样数，采样频率，通道数
	samplesCount = GetSoftSoundSampleNum(handleSoftSound);
	GetSoftSoundFormat(handleSoftSound, &nChannels, &bitsPerVarInSample, &sampleRate, &isFloatType);

	//读取记录循环点的TXT（因为没有使用LoadSoundMem函数，所以无法使用自动加载循环点功能）
	TCHAR loopfilepath[MAX_PATH];
	strcpyDx(loopfilepath, filepath);
	strcatDx(loopfilepath, TEXT(".txt"));
	int h = FileRead_open(loopfilepath);
	if (h != -1 && h != 0)
	{
		int a, b;
		FileRead_scanf(h, TEXT("%d %d"), &a, &b);
		SetLoopAreaSamplePosSoundMem(a, b, handleSound);
	}
	FileRead_close(h);

	return true;
}

bool WaveFile::Release()
{
	bool ret = true;
	if (DeleteSoundMem(handleSound) == -1)ret = false;
	if (DeleteSoftSound(handleSoftSound) == -1)ret = false;
	handleSound = handleSoftSound = 0;
	//ZeroMemory(this, sizeof(*this));
	return ret;
}

bool WaveFile::Play(int resetPos)
{
	PlaySoundMem(handleSound, DX_PLAYTYPE_LOOP, resetPos);
	return true;
}

void WaveFile::Pause()
{
	StopSoundMem(handleSound);
}

int WaveFile::GetSoundHandle()
{
	return handleSound;
}

int WaveFile::GetSoftSoundHandle()
{
	return handleSoftSound;
}

int WaveFile::GetCurrentPlaySamplePos()
{
	return GetCurrentPositionSoundMem(handleSound);
}

int WaveFile::GetSampleVar(int sample, int channel)
{
	if (sample >= samplesCount)return 0i16;
	//返回采样数据((short*)pWaveData)[sample*nChannels + channel]
	ReadSoftSoundData(handleSoftSound, sample, sampleVar, sampleVar + 1);
	return sampleVar[channel];
}

void WaveFile::GetFFTVar(int sample, int channel, int calc_length_in_sample, float * outbuf, int nbuf)
{
	GetFFTVibrationSoftSound(GetSoftSoundHandle(), channel, sample, calc_length_in_sample, outbuf, nbuf);
}
