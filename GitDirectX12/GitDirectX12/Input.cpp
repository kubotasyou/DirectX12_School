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
	//Input�I�u�W�F�N�g�̐���
	IDirectInput8* dinput = nullptr;

	//�C���v�b�g������
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,(void**)&dinput,nullptr);

	//�L�[�{�[�h�f�o�C�X�̐���
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeybord, NULL);

	//���̓f�[�^�`���̃Z�b�g
	result = devkeybord->SetDataFormat(&c_dfDIKeyboard);//�W���`��

	//�r�����䃌�x���̃Z�b�g
	result = devkeybord->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

void Input::Updatae()
{
	//�L�[�{�[�h����J�n
	result = devkeybord->Acquire();

	//�O��̃L�[���͏��̃R�s�[
	for (int i = 0; i < 256; i++)
	{
		//�z����R�s�[����
		oldkey[i] = key[i];
	}
	
	//�L�[�̓���
	result = devkeybord->GetDeviceState(sizeof(key), key);
}

bool Input::KeyDown(int keyNum)
{
	//�������z��͈̔͊O�̒l�������ꍇ�͒e��(�G���[�΍�)
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
	//�������z��͈̔͊O�̒l�������ꍇ�͒e��(�G���[�΍�)
	if (keyNum < 0x00) return false;
	if (keyNum > 0xff) return false;

	//�O�񉟂���Ă��Ȃ� & ���񉟂���Ă���
	if (!oldkey[keyNum] && key[keyNum])
	{
		return true;
	}

	return false;
}
