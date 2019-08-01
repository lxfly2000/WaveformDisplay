#include <cmath>
#include "DxLib.h"
#include "WaveFile.h"
#include "UseVisualStyle.h"

class DPIInfo
{
public:
	DPIInfo()
	{
		HDC h = GetDC(0);
		sx = GetDeviceCaps(h, LOGPIXELSX);
		sy = GetDeviceCaps(h, LOGPIXELSY);
	}
	template<typename Tnum>Tnum X(Tnum n)const { return n * sx / odpi; }
	template<typename Tnum>Tnum Y(Tnum n)const { return n * sy / odpi; }
private:
	const int odpi = USER_DEFAULT_SCREEN_DPI;
	int sx, sy;
};

struct WaveformScreen
{
	void ConfigureVar(WaveFile*wav, int fps = 60)
	{
		wavfile = wav;
		multiplyX = w / log(w + 1);
		if (!wavfile)return;
		samplesPerFrame = wavfile->GetSampleRate() / fps;
		samplesPerPixel = samplesPerFrame / w;
		sampleMaxSignedVar = (1 << (wavfile->GetSampleVarBitsLength() - 1)) - 1;
		if (samplesPerPixel == 0)samplesPerPixel = 1;
		for (i = 0; i < 2; i++)
			baseY[i] = (int)((i + 0.5f)*h / wavfile->GetChannelNum());
#pragma region CheckSign
		//因为目前的 DxLib 中无法获取音频是否是有符号的，所以只能自己检测。
		bool issigned = false;
		for (i = 0; i<wavfile->GetSampleCount(); i++)
			if (wavfile->GetSampleVar(i, 0) < 0)
			{
				issigned = true;
				break;
			}
		if (!issigned)for (i = 0; i < 2; i++)
			baseY[i] += h / 2 / wavfile->GetChannelNum();
#pragma endregion
	}
	void DrawSingleChannel(int ch)
	{
		switch (viewform)
		{
		case 0:
			for (i = 0; i < w; i += betweenX)//i是像素的X座标
			{
				tempSmpVar[i / betweenX % 2] = baseY[ch] - wavfile->GetSampleVar(curSmp + i*samplesPerPixel, ch) *
					h / wavfile->GetChannelNum() / 2 / sampleMaxSignedVar;
				if (i)DrawLine(i - betweenX, tempSmpVar[(i / betweenX + 1) % 2], i, tempSmpVar[i / betweenX % 2], color[ch]);
			}
			break;
		case 1:
			wavfile->GetFFTVar(curSmp, ch, 4096, freqviewBuffer, ARRAYSIZE(freqviewBuffer));
			for (i = 0; i < ARRAYSIZE(freqviewBuffer); i++)
				DrawBox((int)(log(i*w / ARRAYSIZE(freqviewBuffer) + 1)*multiplyX), (ch + 1)*h / wavfile->GetChannelNum(),
				(int)(log((i + 1)*w / ARRAYSIZE(freqviewBuffer) + 1)*multiplyX),
					(int)((ch + 1 - powf(freqviewBuffer[i], 0.25f))*h / wavfile->GetChannelNum()), color[ch], TRUE);
			break;
		}
	}
	void Draw()
	{
		if (!wavfile)return;
		curSmp = wavfile->GetCurrentPlaySamplePos();
		for (c = 0; c < wavfile->GetChannelNum(); c++)
			DrawSingleChannel(c);
		snprintfDx(szSmp, ARRAYSIZE(szSmp), TEXT("Smp:%10d / %d"), curSmp, wavfile->GetSampleCount());
		DrawString(0, 0, szSmp, 0x00FFFFFF);
	}
	void SetViewForm(int n)
	{
		viewform = n;
	}
	void SetRectangle(int _x, int _y, int _w = 0, int _h = 0)
	{
		x = _x;
		y = _y;
		if (_w)w = _w;
		if (_h)h = _h;
	}
	int x, y, w, h;
private:
	const int color[2] = { 0x00FF0000,0x0000FF00 };
	const int betweenX = 1;//每隔多少像素画一个波形采样点（即解析度）
	double multiplyX;
	int samplesPerFrame;
	int samplesPerPixel;
	int sampleMaxSignedVar;
	int viewform = 0;//0=波形，1=频谱
	WaveFile *wavfile;
	TCHAR szSmp[32];
	int baseY[2];
	int tempSmpVar[2] = { 0 };
	int curSmp;
	int i;//Sample 的临时变量
	int c;//Channel 的临时变量
	float freqviewBuffer[512];
};

BOOL SelectFile(TCHAR *filepath, TCHAR *filename)
{
	OPENFILENAME ofn = { sizeof OPENFILENAME };
	ofn.lStructSize = sizeof OPENFILENAME;
	ofn.hwndOwner = GetActiveWindow();
	ofn.hInstance = nullptr;
	ofn.lpstrFilter = L"所有支持的格式 (wav, mp3, ogg)\0*.wav;*.mp3;*.ogg\0"
		"波形音频 (wav)\0*.wav\0MP3 格式音频\0*.mp3\0Ogg Vorbis\0*.ogg\0所有文件\0*\0\0";
	ofn.lpstrFile = filepath;
	ofn.lpstrTitle = L"选择文件";
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = filename;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"wav";
	return GetOpenFileName(&ofn);
}

void UpdateString(TCHAR *str, int strsize, bool isplaying, const TCHAR *path)
{
	TCHAR path2[60];
	if (strlenDx(path) > ARRAYSIZE(path2))
	{
		path = strrchrDx(path, TEXT('\\')) + 1;
		size_t n = strlenDx(path);
		if (n > ARRAYSIZE(path2))
		{
			strncpyDx(path2, path, ARRAYSIZE(path2));
			path = strrchrDx(path, TEXT('.'));
			n = ARRAYSIZE(path2) - 3 - strlenDx(path);
			sprintfDx(path2 + n, TEXT("..%s"), path);
			path = path2;
		}
	}
	snprintfDx(str, strsize, TEXT("Space:播放/暂停 S:停止 O:打开文件 Esc:退出 1:波形显示 2:频谱显示\n%s：%s"),
		isplaying ? TEXT("正在播放") : TEXT("当前文件"), path[0] == TEXT('\0') ? TEXT("未选择") : path);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	WaveFile wave;
	WaveformScreen ws;
	TCHAR filepath[MAX_PATH] = L"";
	TCHAR szStr[110];
	int status = 0;
	int pressed = FALSE;
	int windowed = TRUE;
	int w, h;

	ChangeWindowMode(windowed);
	SetAlwaysRunFlag(TRUE);
	DPIInfo hdpi;
	if (_strcmpi(lpCmdLine, "720p") == 0)
		SetGraphMode(hdpi.X(1280), hdpi.Y(720), 32);
	else if (_strcmpi(lpCmdLine, "kaiki") == 0)
		SetGraphMode(hdpi.X(1152), hdpi.Y(720), 32);
	else
		SetGraphMode(hdpi.X(DEFAULT_SCREEN_SIZE_X), hdpi.Y(DEFAULT_SCREEN_SIZE_Y), 32);
	SetDrawScreen(DX_SCREEN_BACK);
	ChangeFont(L"SimSun");
	SetFontSize(hdpi.X(14));
	if (hdpi.X(14) > 14)
	{
		ChangeFontType(DX_FONTTYPE_ANTIALIASING);
		SetFontThickness(3);
	}

	GetDrawScreenSize(&w, &h);
	ws.SetRectangle(0, 0, w, h);
	ws.ConfigureVar(nullptr, 60);
	UpdateString(szStr, ARRAYSIZE(szStr), status == 2, filepath);

	if (DxLib_Init() == -1)    // ＤＸライブラリ初期化処理
		return -1;    // エラーが起きたら直ちに終了

	while (1)
	{
		if (ProcessMessage() == -1)break;
		ClearDrawScreen();

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
			UpdateString(szStr, ARRAYSIZE(szStr), status == 2, filepath);
		}
		else pressed = FALSE;
		if (CheckHitKey(KEY_INPUT_S))
		{
			wave.Pause();
			status = 0;
		}
		if (CheckHitKey(KEY_INPUT_O))
		{
			if (!windowed)ChangeWindowMode(windowed = TRUE);
			if (SelectFile(filepath, NULL))
			{
				wave.LoadFile(filepath);
				ws.ConfigureVar(&wave, 60);
				status = 0;
			}
			UpdateString(szStr, ARRAYSIZE(szStr), status == 2, filepath);
		}
		if (CheckHitKey(KEY_INPUT_1))ws.SetViewForm(0);
		if (CheckHitKey(KEY_INPUT_2))ws.SetViewForm(1);
		if (CheckHitKey(KEY_INPUT_F11))
		{
			windowed ^= TRUE;
			ChangeWindowMode(windowed);
		}
		ws.Draw();

		DrawString(0, ws.h - 36, szStr, 0x00FFFFFF);
		ScreenFlip();
	}

	DxLib_End();        // ＤＸライブラリ使用の終了処理

	return 0;        // ソフトの終了
}