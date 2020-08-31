#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include "DirectXManager.h"
#include "Create2DFigure.h"


//����ǉ�
////���w���C�u�����̃C���N���[�h
//#include <DirectXMath.h>
//using namespace DirectX;
////D3D�R���p�C���̃C���N���[�h
//#include <d3dcompiler.h>
//#pragma comment(lib, "d3dcompiler.lib")


using namespace std;

//�E�B���h�E�N���X�̐ݒ�
WNDCLASSEX w{};
//�E�B���h�E�T�C�Y
const int window_width = 1280;
const int window_height = 720;
HWND hwnd;


//����N���X�����ĂĂȂ��������
//HRESULT result;
//ID3D12Device* _dev = nullptr;
//ID3D12GraphicsCommandList* _cmdList = nullptr;


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
#ifdef _DEBUG
	//�f�o�b�O���C���[��ON�ɂ���
	ID3D12Debug* debugController;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif // DEBUG

	//�E�B���h�E�̐���
	WindowCreate();

	//DirectX�̏���������
	DirectXManager* dxManager = new DirectXManager();
	dxManager->Initialize(hwnd);

	Create2DFigure* _create2D =
		new Create2DFigure(dxManager->GetDev(), dxManager->getCmdList());


	//����ǉ�
//#pragma region �`�揉��������
//
//	//���_�f�[�^(3���_���̍��W)
//	XMFLOAT3 vertices[] =
//	{
//		{-0.5f, -0.5f, 0.0f},//����
//		{-0.5f, +0.5f, 0.0f},//����
//		{+0.5f, -0.5f, 0.0f},//�E��
//	};
//
//#pragma region ���_�o�b�t�@�̊m��
//
//	//���_�q�[�v�ݒ�\����
//	D3D12_HEAP_PROPERTIES heapprop{};//HeapProperties
//	//GPU�ւ̃f�[�^�]���p
//	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
//
//	//���\�[�X�̐ݒ�\����
//	D3D12_RESOURCE_DESC resdesc{};
//
//	//�o�b�N�o�b�t�@�[���g���̂�Buffer���w�肷��
//	resdesc.Dimension =
//		D3D12_RESOURCE_DIMENSION_BUFFER;
//	//���_��񂪓��镪�̃T�C�Y���m�ۂ���
//	resdesc.Width = sizeof(vertices);
//	resdesc.Height = 1;
//	resdesc.DepthOrArraySize = 1;
//	resdesc.MipLevels = 1;
//	resdesc.SampleDesc.Count = 1;
//	resdesc.Layout =
//		D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	//GPU���\�[�X�̐���
//	ID3D12Resource* vertBuff = nullptr;
//
//	//���_�o�b�t�@�̐���
//	result = _dev->CreateCommittedResource(
//		&heapprop,           //�q�[�v�ݒ�
//		D3D12_HEAP_FLAG_NONE,
//		&resdesc,            //���\�[�X�ݒ�
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff)
//	);
//
//	/*���_�o�b�t�@�[�́AGPU�ɒ��_�����󂯎�邽�߂̃��\�[�X(�̈�)*/
//
//#pragma endregion
//
//#pragma region ���_���̃R�s�[
//
//	//GPU��̃o�b�t�@�ɑΉ����ĉ��z���������擾
//	XMFLOAT3* _vertMap = nullptr;
//
//	//�R�s�[�J�n
//	//�o�b�t�@�[�ɒ��_�����R�s�[����
//	result = vertBuff->Map(0, nullptr, (void**)&_vertMap);
//
//	//�S���_�̃f�[�^�ɑ΂��čs��
//	for (int i = 0; i < _countof(vertices); i++)
//	{
//		//���W�̃R�s�[
//		_vertMap[i] = vertices[i];
//	}
//
//	//�R�s�[�I��
//	vertBuff->Unmap(0, nullptr);
//
//#pragma endregion
//
//#pragma region ���_�o�b�t�@�[�r���[�̍쐬
//
//	//���_�o�b�t�@�[�r���[�̍쐬
//	D3D12_VERTEX_BUFFER_VIEW vbview{};
//
//	//�o�b�t�@�[�̉��z�A�h���X�̎擾
//	vbview.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	//�S���ŉ��o�C�g���𒲂ׂ�
//	vbview.SizeInBytes = sizeof(vertices);
//	//1���_������̃o�C�g���𒲂ׂ�
//	vbview.StrideInBytes = sizeof(vertices[0]);
//
//	/*���_�o�b�t�@�[�r���[�́A�f�[�^�̑傫����m�点�����*/
//
//#pragma endregion
//
//#pragma region �V�F�[�_�[�t�@�C���̓ǂݍ��݂ƃR���p�C��
//
//	ID3DBlob* vsBlob = nullptr;   //���_�V�F�[�_�[�I�u�W�F�N�g
//	ID3DBlob* psBlob = nullptr;   //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
//	ID3DBlob* errorBlob = nullptr;//�G���[�I�u�W�F�N�g
//
//	//���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
//	result = D3DCompileFromFile(
//		L"BasicVertexShader.hlsl",                      //�V�F�[�_�[���w��
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,              //�C���N���[�h���\�ɂ���
//		"VSmain",                                       //�G���g���[�|�C���g�̎w��
//		"vs_5_0",                                       //�V�F�[�_�[���f���̎w��
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�w��
//		0,
//		&vsBlob,
//		&errorBlob
//	);
//
//	//�s�N�Z���V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
//	result = D3DCompileFromFile(
//		L"BasicPixcelShader.hlsl",                      //�V�F�[�_�[���w��
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,              //�C���N���[�h���\�ɂ���
//		"PSmain",                                       //�G���g���[�|�C���g�̎w��
//		"ps_5_0",                                       //�V�F�[�_�[���f���̎w��
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�w��
//		0,
//		&psBlob,
//		&errorBlob
//	);
//
//	//���V�F�[�_�[�̓ǂݍ��݂Ɏ��s�����ꍇ�ɓ��e��\������
//	if (FAILED(result))
//	{
//		//errorBlob�̒�����A�G���[�̓��e��string�^�ɃR�s�[����
//		string error;
//		error.resize(errorBlob->GetBufferSize());
//
//		copy_n((char*)errorBlob->GetBufferPointer(),
//			errorBlob->GetBufferSize(),
//			error.begin());
//		error += "\n";
//
//		//�G���[�̓��e���o�̓E�B���h�E�ɕ\������
//		OutputDebugStringA(error.c_str());
//		exit(1);
//	}
//
//#pragma endregion
//
//#pragma region ���_���C�A�E�g�ݒ�
//
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
//	{
//		{
//			"POSITION",
//			0,
//			DXGI_FORMAT_R32G32B32_FLOAT,
//			0,
//			D3D12_APPEND_ALIGNED_ELEMENT,
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
//			0,
//		},
//	};
//
//#pragma endregion
//
//
//#pragma region �O���t�B�N�X�p�C�v���C���̍쐬
//
//	//�O���t�B�N�X�p�C�v���C���̐ݒ�\����
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
//
//	//�V�F�[�_�[�̃Z�b�g--------------------------------------
//	//���_�V�F�[�_�[���p�C�v���C���ɐݒ�
//	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
//	gpipeline.VS.BytecodeLength =  vsBlob->GetBufferSize();
//	//�s�N�Z���V�F�[�_�[���p�C�v���C���ɐݒ�
//	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
//	gpipeline.PS.BytecodeLength =  psBlob->GetBufferSize();
//	//--------------------------------------------------------
//
//	//�T���v���}�X�N�ƃ��X�^���C�Y�X�e�[�g�̐ݒ�--------------
//	//�f�t�H���g�̐ݒ�
//	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
//	//�w�ʃJ�����O���s��Ȃ�
//	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
//	//�|���S������h��Ԃ�
//	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
//	//�[�x�N���b�s���O��L���ɂ���
//	gpipeline.RasterizerState.DepthClipEnable = true;
//	//--------------------------------------------------------
//
//	//�u�����h�X�e�[�g�̐ݒ�----------------------------------
//	//RGBA �S�Ẵ`�����l����`��
//	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask =
//		D3D12_COLOR_WRITE_ENABLE_ALL;
//	//--------------------------------------------------------
//
//	//�}�`�̌`��̐ݒ�
//	gpipeline.PrimitiveTopologyType = 
//		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//
//	//�`��Ώۂ̐�
//	gpipeline.NumRenderTargets = 1;
//
//	//0�`1�ɐ��K�����ꂽRGBA
//	gpipeline.RTVFormats[0] =
//		DXGI_FORMAT_R8G8B8A8_UNORM;
//
//	//1�s�N�Z���ɂ�1��̃T���v�����O
//	gpipeline.SampleDesc.Count = 1;
//
//
//#pragma region ���[�g�V�O�l�`���̐���
//
//	//���[�g�V�O�l�`���̐錾
//	ID3D12RootSignature* rootsignature;
//
//	//���[�g�V�O�l�`���̐ݒ�\����
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
//
//	//���_���̑��݂�`����
//	rootSignatureDesc.Flags =
//		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//	//�I�u�W�F�N�g���m�ۂ���
//	ID3DBlob* rootSigBlob = nullptr;
//
//	//�o�C�i���R�[�h(�\�[�X�R�[�h���@�B��ɕϊ��������)�̍쐬
//	result = D3D12SerializeRootSignature(
//		&rootSignatureDesc,            //���[�g�V�O�l�`���̐ݒ�
//		D3D_ROOT_SIGNATURE_VERSION_1_0,//���[�g�V�O�l�`���̃o�[�W����
//		&rootSigBlob, 				   //�V�F�[�_�[�̎��Ɠ���
//		&errorBlob);				   //�G���[�̎��͂����Ƀ��b�Z�[�W������
//
//	//���[�g�V�O�l�`���̐���
//	result = _dev->CreateRootSignature(
//		0,
//		rootSigBlob->GetBufferPointer(),
//		rootSigBlob->GetBufferSize(),
//		IID_PPV_ARGS(&rootsignature)
//	);
//
//	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
//	gpipeline.pRootSignature = rootsignature;
//
//#pragma endregion
//
//	//�O���t�B�N�X�p�C�v���C���̐���
//	ID3D12PipelineState* pipelinestate = nullptr;
//	result = _dev->CreateGraphicsPipelineState(
//		&gpipeline,
//		IID_PPV_ARGS(&pipelinestate)
//	);
//
//#pragma endregion
//
//
//#pragma endregion


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

		dxManager->Start();

		_create2D->Draw2D();

		//����ǉ�
//#pragma region �`��R�}���h
//
//		//�p�C�v���C���X�e�[�g�̐ݒ�
//		_cmdList->SetPipelineState(pipelinestate);
//
//		//�r���[�|�[�g�̐ݒ�----------------------------------
//		D3D12_VIEWPORT viewport{};
//		viewport.Width = window_width;
//		viewport.Height = window_height;
//		viewport.TopLeftX = 0;
//		viewport.TopLeftY = 0;
//		viewport.MinDepth = 0.0f;
//		viewport.MaxDepth = 1.0f;
//		_cmdList->RSSetViewports(1, &viewport);
//		//----------------------------------------------------
//
//		//�V�U�[��`�̐ݒ�------------------------------------
//		D3D12_RECT scissorret{};
//		scissorret.left = 0;
//		scissorret.right = scissorret.left + window_width;
//		scissorret.top = 0;
//		scissorret.bottom = scissorret.top + window_height;
//		_cmdList->RSSetScissorRects(1, &scissorret);
//		//----------------------------------------------------
//
//		//���[�g�V�O�l�`���̐ݒ�
//		_cmdList->SetGraphicsRootSignature(rootsignature);
//
//		//�v���~�e�B�u�`��̐ݒ�
//		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//		//���_�o�b�t�@�̐ݒ�
//		_cmdList->IASetVertexBuffers(0, 1, &vbview);
//
//		//�`��
//		_cmdList->DrawInstanced(3, 1, 0, 0);
//
//#pragma endregion


		dxManager->End();

#pragma endregion

	}

	//�E�B���h�E�N���X�̓o�^������
	UnregisterClass(w.lpszClassName, w.hInstance);

	delete dxManager;

	return 0;
}

//�E�B���h�E�̐���
void WindowCreate()
{
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
	hwnd = CreateWindow(
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