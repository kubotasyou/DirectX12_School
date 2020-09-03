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

	//�L�[��������Ă��邩
	bool KeyDown(int keyNum);

	//�L�[���������u��
	bool KeyTrigger(int keyNum);

private:

	HRESULT result;
	//�L�[�{�[�h�f�o�C�X�̐���
	IDirectInputDevice8* devkeybord = nullptr;
	//�L�[�̓��͏��
	BYTE key[256] = {};
	//�O�t���[���̃L�[���
	BYTE oldkey[256] = {};
};

