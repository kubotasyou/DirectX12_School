#pragma once
#include <Windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class Input
{
public:

	Input();

	~Input();

	void Initialize(HWND hwnd);

	void Updatae();

	//キーが押されているか
	bool KeyDown(int keyNum);

	//キーを押した瞬間
	bool KeyTrigger(int keyNum);

private:

	HRESULT result;
	//キーボードデバイスの生成
	IDirectInputDevice8* devkeybord = nullptr;
	//キーの入力情報
	BYTE key[256] = {};
	//前フレームのキー情報
	BYTE oldkey[256] = {};
};

