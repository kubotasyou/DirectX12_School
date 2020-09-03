#include "Input.h"



Input::Input()
{
}

Input::~Input()
{
    //devkeybord = nullptr;
}

void Input::Initialize(HWND hwnd)
{
	//Inputオブジェクトの生成
	IDirectInput8* dinput = nullptr;

	//インプット初期化
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,(void**)&dinput,nullptr);

	//キーボードデバイスの生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeybord, NULL);

	//入力データ形式のセット
	result = devkeybord->SetDataFormat(&c_dfDIKeyboard);//標準形式

	//排他制御レベルのセット
	result = devkeybord->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

void Input::Updatae()
{
	//キーボード動作開始
	result = devkeybord->Acquire();

	//前回のキー入力情報のコピー
	for (int i = 0; i < 256; i++)
	{
		//配列をコピーする
		oldkey[i] = key[i];
	}
	
	//キーの入力
	result = devkeybord->GetDeviceState(sizeof(key), key);
}

bool Input::KeyDown(int keyNum)
{
	//引数が配列の範囲外の値だった場合は弾く(エラー対策)
	if (keyNum < 0x00) return false;
	if (keyNum > 0xff) return false;

	if (key[keyNum])
	{
		return true;
	}

	return false;
}

bool Input::KeyTrigger(int keyNum)
{
	//引数が配列の範囲外の値だった場合は弾く(エラー対策)
	if (keyNum < 0x00) return false;
	if (keyNum > 0xff) return false;

	//前回押されていない & 今回押されている
	if (!oldkey[keyNum] && key[keyNum])
	{
		return true;
	}

	return false;
}
