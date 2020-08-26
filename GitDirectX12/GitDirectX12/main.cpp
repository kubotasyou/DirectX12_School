#include <Windows.h>







//�E�B���h�E�N���X�̐ݒ�
WNDCLASSEX w{};


//�E�B���h�E�v���V�[�W���쐬
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ŕ���
	switch (msg)
	{
	case WM_DESTROY://�E�B���h�E���j�����ꂽ
		//OS�ɑ΂��āA�A�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}
	//�W���̏��������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WindowCreate();

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//�E�B���h�E�̐���
	WindowCreate();

	//���b�Z�[�W
	MSG msg{};

	while (true)
	{
		//���b�Z�[�W�����邩?
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			//�L�[���̓��b�Z�[�W�̏���
			TranslateMessage(&msg);
			//�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
			DispatchMessage(&msg);
		}

		//�I�����b�Z�[�W�������烋�[�v�𔲂���
		//(������escape�ŏI���̏������������炢���̂��ȁH)
		if (msg.message == WM_QUIT)
		{
			break;
		}

#pragma region Update����

#pragma endregion

	}

	//�E�B���h�E�N���X�̓o�^������
	UnregisterClass(w.lpszClassName, w.hInstance);

}

//�E�B���h�E�̐���
void WindowCreate()
{
	//�E�B���h�E�T�C�Y
	const int window_width = 1280;
	const int window_height = 720;

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;                //�E�B���h�E�v���V�[�W���̐ݒ�
	w.lpszClassName = L"DirectXGame";//�E�B���h�E��
	w.hInstance = GetModuleHandle(nullptr);             //�E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW);            //�J�[�\���w��

	//�E�B���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&w);

	//�E�B���h�E�T�C�Y(X���W, Y���W, ����, �c��)
	RECT windowRect = { 0,0,window_width,window_height };
	//�����ŃT�C�Y��␳����
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(
		w.lpszClassName,                   //�N���X��
		L"Re:�[������n�߂�DirectX�̉ۑ�", //�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,               //�^�C�g���o�[�Ɖ�ʂɋ��E������E�B���h�E
		CW_USEDEFAULT,                     //�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,                     //�\��Y���W(OS�ɔC����)
		windowRect.right - windowRect.left,//�E�B���h����
		windowRect.bottom - windowRect.top,//�E�B���h�c��
		nullptr,                           //�e�̃E�B���h�E�n���h��
		nullptr,                           //���j���[�n���h��
		w.hInstance,                       //�Ăяo���A�v���P�[�V�����n���h��
		nullptr,                           //�ǉ��p�����[�^�[
		);

	//�E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
}