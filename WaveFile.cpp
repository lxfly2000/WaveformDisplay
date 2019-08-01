#include "WaveFile.h"
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
	handleSound = LoadSoundMemFromSoftSound(handleSoftSound); //handleSound = LoadSoundMem(filepath);//��֪Ϊ������ռ���ڴ淴����
	//��ȡ������������Ƶ�ʣ�ͨ����
	samplesCount = GetSoftSoundSampleNum(handleSoftSound);
	GetSoftSoundFormat(handleSoftSound, &nChannels, &bitsPerVarInSample, &sampleRate, &isFloatType);

	//��ȡ��¼ѭ�����TXT����Ϊû��ʹ��LoadSoundMem�����������޷�ʹ���Զ�����ѭ���㹦�ܣ�
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
	//���ز�������((short*)pWaveData)[sample*nChannels + channel]
	ReadSoftSoundData(handleSoftSound, sample, sampleVar, sampleVar + 1);
	return sampleVar[channel];
}

int WaveFile::GetSampleVarBitsLength()
{
	return bitsPerVarInSample;
}

int WaveFile::GetChannelNum()
{
	return nChannels;
}

int WaveFile::GetSampleRate()
{
	return sampleRate;
}

int WaveFile::GetSampleCount()
{
	return samplesCount;
}

void WaveFile::GetFFTVar(int sample, int channel, int calc_length_in_sample, float * outbuf, int nbuf)
{
	GetFFTVibrationSoftSound(GetSoftSoundHandle(), channel, sample, calc_length_in_sample, outbuf, nbuf);
}
