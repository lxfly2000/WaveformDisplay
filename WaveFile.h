#pragma once
#include"DxLib.h"

#define USE_DATA_FROM_DXLIB//http://dxlib.o.oo7.jp/cgi/patiobbs/patio.cgi?mode=view&no=3385
#define MAX_CHANNELS 2

class WaveFile
{
public:
	//����һ�� DxLib ��Ƶ����
	WaveFile();
	~WaveFile();
	//����һ�� DxLib ��Ƶ���󲢼����ļ�
	WaveFile(const TCHAR*);
	//�����ļ�
	bool LoadFile(const TCHAR*);
	//�ͷ���Դ
	bool Release();
	//�����ļ�������Ϊ�Ƿ��ͷ��ʼ����
	bool Play(int resetPos = TRUE);
	//��ͣ����
	void Pause();

	//��ȡ��Ƶ���
	int GetSoundHandle();
	//��ȡ���޸���Ƶ���
	int GetSoftSoundHandle();
	//��ȡ��ǰ�Ĳ���λ�ã������㣩
	int GetCurrentPlaySamplePos();
	//��ȡ�������еĵ�ƽ����ֵ����Ϊ GetSampleVarBitsLength() λ
	int GetSampleVar(int sample, int channel);
	//��ȡ�������ƽ��ֵ�ĳ���
	int GetSampleVarBitsLength();
	//��ȡ������
	int GetChannelNum();
	//��ȡ����Ƶ��
	int GetSampleRate();
	//��ȡ������
	int GetSampleCount();
	//��ȡƵ����ֵ��calc_length_in_sample ֻ���� 256��16384 ֮��� 2 �ı���
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
