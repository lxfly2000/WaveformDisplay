#pragma once
#include"DxLib.h"
//http://soundfile.sapp.org/doc/WaveFormat/
struct WaveFileHeader
{
	char RIFF[4];
	int fileLength_minus8;
	char WAVE[4];
	char fmt[4];
	int fmtLength_minus8;
	short audioFormat;//1=PCM, ���ڵ���2=������ʽ
	short nChannels;
	int sampleRate;
	int byteRate;
	short blockAlign;
	short bitsPerVarInSample;
	char data[4];
	int waveDataSize;//In byte
};
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

	int GetCurrentPlaySamplePos();
	//��ȡ�������еĵ�ƽ
	short GetSampleVar16(int sample, int channel);

	WaveFileHeader header;
	BYTE *pWaveData;
	int samplesCount;
	int handleSound;
};
