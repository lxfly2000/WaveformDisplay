#include "WaveFile.h"
#include"E:\Codes\MyUtilities.h"
#include<cassert>

WaveFile::WaveFile()
{
#ifdef USE_XAUDIO2
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = 0;
	buffer.LoopBegin = 0;
	buffer.LoopLength = 0;
	buffer.PlayBegin = 0;
	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
	{
		MessageBox(NULL, TEXT("CoInitializeEx ʧ�ܡ�"), NULL, MB_ICONERROR);
		return;
	}
	if (FAILED(XAudio2Create(&xAudio2Engine)))
	{
		MessageBox(NULL, TEXT("δ�ܴ��� XAudio2 ���档"), TEXT("XAudio2"), MB_ICONERROR);
		return;
	}
	if (FAILED(xAudio2Engine->CreateMasteringVoice(&masterVoice)))
	{
		MessageBox(NULL, TEXT("δ�ܴ�����������"), TEXT("XAudio2"), MB_ICONERROR);
		return;
	}
#endif
}

WaveFile::~WaveFile()
{
	Release();
#ifdef USE_XAUDIO2
	masterVoice->DestroyVoice();
	xAudio2Engine->Release();
	CoUninitialize();
#endif
}

WaveFile::WaveFile(const TCHAR *filepath) :WaveFile()
{
	LoadFile(filepath);
}

bool WaveFile::LoadFile(const TCHAR *filepath)
{
	int h = 0;
	if (pWaveData)Release();

	//��ȡWAV�ļ�
	h = FileRead_open(filepath);
	if (h == -1 || h == 0)
	{
		FileRead_close(h);
		return false;
	}
	FileRead_read(&header, sizeof header, h);
	pWaveData = new BYTE[header.waveDataSize];
	FileRead_read(pWaveData, header.waveDataSize, h);
	FileRead_close(h);

	//��ȡ��¼ѭ�����TXT
	TCHAR loopfilepath[MAX_PATH];
	strcpyDx(loopfilepath, filepath);
	strcatDx(loopfilepath, TEXT(".txt"));
	h = FileRead_open(loopfilepath);
#ifndef USE_XAUDIO2
	int a, b;
#endif
	if (h != -1 && h != 0)
	{
#ifndef USE_XAUDIO2
		FileRead_scanf(h, TEXT("%d %d"), &a, &b);
#else
		FileRead_scanf(h, TEXT("%d %d"), &buffer.LoopBegin, &buffer.LoopLength);
		if (buffer.LoopLength)buffer.LoopLength -= buffer.LoopBegin;
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
#endif
	}
	FileRead_close(h);

	samplesCount = header.waveDataSize / header.bitsPerVarInSample / header.nChannels * 8;
#ifndef USE_XAUDIO2
	handleSound = LoadSoundMem(filepath);
	SetLoopAreaSamplePosSoundMem(a, b, handleSound);
#else
	//https://msdn.microsoft.com/zh-cn/library/windows/desktop/dd390970(v=vs.85).aspx
	waveformat.nAvgBytesPerSec = header.sampleRate*header.blockAlign;
	waveformat.nBlockAlign = header.blockAlign;
	MyAssert(waveformat.nBlockAlign == header.nChannels*header.bitsPerVarInSample / 8);
	waveformat.nChannels = header.nChannels;
	waveformat.nSamplesPerSec = header.sampleRate;
	waveformat.wBitsPerSample = header.bitsPerVarInSample;
	MyAssert(waveformat.wBitsPerSample == 8 || waveformat.wBitsPerSample == 16);
	waveformat.wFormatTag = WAVE_FORMAT_PCM;
	waveformat.cbSize = 0;

	//if (!pWaveData)return false;
	if (FAILED(xAudio2Engine->CreateSourceVoice(&srcVoice, &waveformat)))return false;
	buffer.pAudioData = wavdata;
	buffer.AudioBytes = wavsize;
#endif
	return true;
}

bool WaveFile::Release()
{
#ifndef USE_XAUDIO2
	if (DeleteSoundMem(handleSound) == -1)return false;
#else
	if (srcVoice)
	{
		Pause();
		srcVoice->FlushSourceBuffers();
		srcVoice->DestroyVoice();
		//srcVoice = nullptr;
	}
#endif
	delete[header.waveDataSize]pWaveData;
	pWaveData = nullptr;
	//ZeroMemory(this, sizeof(*this));
	return true;
}

bool WaveFile::Play(int resetPos)
{
	if (header.bitsPerVarInSample != 16)
		if (MessageBox(NULL, L"���ļ��ļ�¼��ʽ����16λ��������ʾ���ܻ�����⣬�Ƿ������",
			L"����", MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
			return false;
#ifndef USE_XAUDIO2
	PlaySoundMem(handleSound, DX_PLAYTYPE_LOOP, resetPos);
#else
	if (!srcVoice)return false;
	if (resetPos)
	{
		srcVoice->FlushSourceBuffers();
		if (FAILED(srcVoice->SubmitSourceBuffer(&buffer)))
		{
			MessageBox(NULL, TEXT("ִ�� SubmitSourceBuffer ʱ����"), TEXT("XAudio2"), MB_ICONERROR);
			return false;
		}
	}
	srcVoice->Start();
#endif
	return true;
}

void WaveFile::Pause()
{
#ifndef USE_XAUDIO2
	StopSoundMem(handleSound);
#else
	srcVoice->Stop();
#endif
}

int WaveFile::GetCurrentPlaySamplePos()
{
#ifdef USE_XAUDIO2
	srcVoice->GetState(&state);
	return (int)(state.SamplesPlayed < buffer.LoopBegin ? state.SamplesPlayed : (state.SamplesPlayed - buffer.LoopBegin) %
		(buffer.LoopLength ? buffer.LoopLength : samplesCount - buffer.LoopBegin) + buffer.LoopBegin);
#else
	return GetCurrentPositionSoundMem(handleSound);
#endif
}

short WaveFile::GetSampleVar16(int sample, int channel)
{
	if (sample >= samplesCount)return 0;
	return ((short*)pWaveData)[sample*header.nChannels + channel];
}