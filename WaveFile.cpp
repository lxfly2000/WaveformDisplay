#include "WaveFile.h"

WaveFile::WaveFile()
{
}

WaveFile::~WaveFile()
{
	Release();
}

WaveFile::WaveFile(const TCHAR *filepath)
{
	LoadFile(filepath);
}

bool WaveFile::LoadFile(const TCHAR *filepath)
{
	int h = 0;
	if (pWaveData)Release();

	h = FileRead_open(filepath);
	if (h == -1)
	{
		FileRead_close(h);
		return false;
	}
	FileRead_read(&header, sizeof header, h);
	pWaveData = new BYTE[header.waveDataSize];
	FileRead_read(pWaveData, header.waveDataSize, h);
	FileRead_close(h);

	samplesCount = header.waveDataSize / header.bitsPerVarInSample / header.nChannels * 8;
	handleSound = LoadSoundMem(filepath);

	return true;
}

bool WaveFile::Release()
{
	delete[header.waveDataSize]pWaveData;
	if (DeleteSoundMem(handleSound) == -1)return false;
	ZeroMemory(this, sizeof*this);
	return true;
}

bool WaveFile::Play(int resetPos)
{
	if (header.bitsPerVarInSample != 16)
		if (MessageBox(NULL, L"该文件的记录格式不是16位，波形显示可能会出问题，是否继续？",
			L"播放", MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
			return false;
	PlaySoundMem(handleSound, DX_PLAYTYPE_BACK, resetPos);
	return true;
}

void WaveFile::Pause()
{
	StopSoundMem(handleSound);
}

int WaveFile::GetCurrentPlaySamplePos()
{
	return GetCurrentPositionSoundMem(handleSound);
}

short WaveFile::GetSampleVar16(int sample, int channel)
{
	if (sample >= samplesCount)return 0;
	return ((short*)pWaveData)[sample*header.nChannels + channel];
}