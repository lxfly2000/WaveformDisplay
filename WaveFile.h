#pragma once
#include"DxLib.h"
#define USE_XAUDIO2
#ifdef USE_XAUDIO2
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#endif

//http://soundfile.sapp.org/doc/WaveFormat/
struct WaveFileHeader
{
	char RIFF[4];
	int fileLength_minus8;
	char WAVE[4];
	char fmt[4];
	int fmtLength_minus8;
	short audioFormat;//1=PCM, 大于等于2=其他格式
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
	//获取采样点中的电平
	short GetSampleVar16(int sample, int channel);

	WaveFileHeader header;
	BYTE *pWaveData;
	int samplesCount;
#ifndef USE_XAUDIO2
	int handleSound;
#else
private:
	IXAudio2 *xAudio2Engine = nullptr;
	IXAudio2MasteringVoice *masterVoice = nullptr;
	IXAudio2SourceVoice *srcVoice = nullptr;
	WAVEFORMATEX waveformat;

	XAUDIO2_BUFFER buffer = { 0 };
	XAUDIO2_VOICE_STATE state;
	int &wavsize = header.waveDataSize;
	BYTE *&wavdata = pWaveData;
#endif
};
