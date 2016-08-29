#include "DxLib.h"
#include "WaveFile.h"
#include "E:\Codes\UseVisualStyle.h"

struct WaveformScreen
{
	const int color[2] = { 0x00FF0000,0x0000FF00 };
	const int betweenX = 1;//每隔多少像素画一个波形采样点
	int x, y, w, h;
	int samplesPerFrame;
	int samplesPerPixel;
	WaveFile *wavfile;
	void ConfigureVar(WaveFile*wav, int fps)
	{
		wavfile = wav;
		x = 0;
		y = 0;
		GetDrawScreenSize(&w, &h);
		if (!wavfile)return;
		samplesPerFrame = wavfile->header.sampleRate / fps;
		samplesPerPixel = samplesPerFrame / w;
		if (samplesPerPixel == 0)samplesPerPixel = 1;
		for (i = 0; i < 2; i++)
			baseY[i] = (int)((i + 0.5f)*h / wavfile->header.nChannels);
	}
	void DrawSingleChannel(int ch)
	{
		for (i = 0; i < w;)//i是像素的X座标
		{
			i += betweenX;
			tempSmpVar[i / betweenX % 2] = wavfile->GetSampleVar16(curSmp + i*samplesPerPixel, ch);
			DrawLine(i - betweenX, baseY[ch] - tempSmpVar[(i / betweenX + 1) % 2] * (h / wavfile->header.nChannels / 2) / SHRT_MAX,
				i, baseY[ch] - tempSmpVar[i / betweenX % 2] * (h / wavfile->header.nChannels / 2) / SHRT_MAX, color[ch]);
		}
	}
	void Draw()
	{
		if (!wavfile)return;
		curSmp = wavfile->GetCurrentPlaySamplePos();
		for (c = 0; c < wavfile->header.nChannels; c++)
			DrawSingleChannel(c);
		DrawFormatString(0, 0, 0x00FFFFFF, L"Cur:%10d Smp All:%10d Smps", curSmp, wavfile->samplesCount);
	}
private:
	int baseY[2];
	int tempSmpVar[2] = { 0 };
	int curSmp;
	int i, c;
};

BOOL SelectFile(TCHAR *filepath, TCHAR *filename)
{
	OPENFILENAME ofn = { sizeof OPENFILENAME };
	ofn.lStructSize = sizeof OPENFILENAME;
	ofn.hwndOwner = GetActiveWindow();
	ofn.hInstance = nullptr;
	ofn.lpstrFilter = L"波形音频 (wav)\0*.wav\0所有文件\0*\0\0";
	ofn.lpstrFile = filepath;
	ofn.lpstrTitle = L"选择文件";
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = filename;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"wav";
	return GetOpenFileName(&ofn);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	WaveFile wave;
	int status = 0;
	TCHAR filepath[MAX_PATH] = L"";
	WaveformScreen ws;
	int pressed = FALSE;

	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);
	if (_strcmpi(lpCmdLine, "720p") == 0)
		SetGraphMode(1280, 720, 32);
	if (_strcmpi(lpCmdLine, "kaiki") == 0)
		SetGraphMode(1152, 720, 32);
	SetDrawScreen(DX_SCREEN_BACK);
	ChangeFont(L"SimSun");
	SetFontSize(14);

	ws.ConfigureVar(nullptr, 60);

	if (DxLib_Init() == -1)    // ＤＸライブラリ初期化処理
		return -1;    // エラーが起きたら直ちに終了

	while (1)
	{
		if (ProcessMessage() == -1)break;
		if (ClearDrawScreen() == -1)break;

		if (CheckHitKey(KEY_INPUT_ESCAPE))break;

		if (CheckHitKey(KEY_INPUT_SPACE))
		{
			if (!pressed)
			{
				switch (status)
				{
				case 0://停止
				case 1://暂停
					if (!wave.Play(status == 0))
						break;
					status = 2;
					break;
				case 2://播放
					wave.Pause();
					status = 1;
					break;
				}
				pressed = TRUE;
			}
		}
		else pressed = FALSE;
		if (CheckHitKey(KEY_INPUT_S))
		{
			wave.Pause();
			status = 0;
		}
		if (CheckHitKey(KEY_INPUT_O))
		{
			if (SelectFile(filepath, NULL))
			{
				wave.LoadFile(filepath);
				ws.ConfigureVar(&wave, 60);
				status = 0;
			}
		}
		ws.Draw();

		DrawFormatString(0, ws.h - 36, 0x00FFFFFF,
			L"Space:播放/暂停 S:停止 O:打开文件 Esc:退出\n%s：%s", status == 2 ? L"正在播放" : L"当前文件",
			filepath[0] == L'\0' ? L"[未选择]" : filepath);
		if (ScreenFlip() == -1)break;
	}

	DxLib_End();        // ＤＸライブラリ使用の終了処理

	return 0;        // ソフトの終了
}