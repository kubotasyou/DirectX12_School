#include "DirectXManager.h"
#include <Windows.h>

//���Ȃ񂩌^���ʎq���K�v�Ƃ��Ńw�b�_�[�ō��Ȃ����B
//std::vector<ID3D12Resource*> _backBuffers(2);

DirectXManager::DirectXManager()
{
}

DirectXManager::~DirectXManager()
{
}

void DirectXManager::Initialize(HWND hwnd)
{
#pragma region �O���t�B�b�N�{�[�h�̃A�_�v�^���

	//DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));

	//�A�_�v�^�[�̗񋓗p
	vector<IDXGIAdapter*> _adapters;

	//�A�_�v�^�[�I�u�W�F�N�g���i�[����ϐ�
	IDXGIAdapter* _tmpAdapter = nullptr;

	for (int i = 0;
		_dxgiFactory->EnumAdapters(i, &_tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		//���p�\�ȃA�_�v�^�[���i�[����
		_adapters.push_back(_tmpAdapter);
	}

	for (int i = 0; i < _adapters.size(); i++)
	{
		//�A�_�v�^�[�̐ݒ�\����
		DXGI_ADAPTER_DESC adDesc{};//AdapterDesc

		//�A�_�v�^�[�̏����擾
		_adapters[i]->GetDesc(&adDesc);

		//�A�_�v�^�[��
		wstring strDesc = adDesc.Description;

		//�T�������A�_�v�^�[�̖��O���m�F
		if (strDesc.find(L"NAVIDATA") != string::npos)
		{
			//����������_tmpAdapter�Ɋi�[
			_tmpAdapter = _adapters[i];
			break;
		}
	}

#pragma endregion

#pragma region �f�o�C�X�̐���

	//�g�p�ł��郌�x���������ׂ�
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	//�t���[�`���[���x���̏�����
	D3D_FEATURE_LEVEL _featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(_tmpAdapter, levels[i], IID_PPV_ARGS(&_dev));

		//�f�o�C�X�𐶐��ł����烋�[�v�𔲂���
		if (result == S_OK)
		{
			_featureLevel = levels[i];
			break;
		}
	}

#pragma endregion

#pragma region �R�}���h���X�g�ƃA���P�[�^�̐���


	//�R�}���h�A���P�[�^�̐���
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&_cmdAllocator));

	//�R�}���h���X�g�̐���
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		_cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));

	/*�R�}���h�A���P�[�^�̓R�}���h���X�g�Ɋi�[����A
	�@���ߗp�̃��������Ǘ�����B
	  ���߂����s�����܂ł́A�������邱�Ƃ͂Ȃ��B*/

	  /*�R�}���h���X�g�͕`�施�ߗp�̃C���^�[�t�F�[�X�B
	  �@������A�R�}���h�A���P�[�^�Ɋi�[����B*/

#pragma endregion

#pragma region �R�}���h�L���[�̐���

	   //�R�}���h�L���[�̐ݒ�\����
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	_dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

#pragma endregion

#pragma region �X���b�v�`�F�[���̐���

	//�X���b�v�`�F�[���̐ݒ�\����
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

	//�����l�𒼐ړ����Ƃ����������Ԃ��Ă��
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;

	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;       //�F���̏���
	swapchainDesc.SampleDesc.Count = 1;                      //�T���v�����O�����Ȃ���1
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;      //�o�b�N�o�b�t�@���g�p
	swapchainDesc.BufferCount = 2;                           //�o�b�N�o�b�t�@�����悤���邽��2
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//�t���b�v��͔j������
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//�ݒ�̔��f
	_dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&_swapchain
	);

#pragma endregion

#pragma region �����_�[�^�[�Q�b�g�r���[�̐���

	//�����_�[�^�[�Q�b�g�r���[���w��
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	//�o�b�N�o�b�t�@���g�����߁A�\�Ɨ��̓��
	heapDesc.NumDescriptors = 2;

	//�ݒ�̔��f
	_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeaps));


	//----------------------------------------

	//�\�Ɨ��̓���ɂ���

	for (int i = 0; i < 2; i++)
	{
		//�X���b�v�`�F�[������o�b�t�@���擾
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));

		//�f�B�X�N���v�^�q�[�v�̐擪�n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE _handle =
			_rtvHeaps->GetCPUDescriptorHandleForHeapStart();

		//�\�Ɨ��Ń|�C���^�[��1���炷
		_handle.ptr += i * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);

		//�����_�[�^�[�Q�b�g�r���[�̐���
		_dev->CreateRenderTargetView(
			_backBuffers[i],
			nullptr,
			_handle
		);
	}

	//----------------------------------------

	/*�o�b�N�o�b�t�@�ɕ`�挋�ʂ��������ނ��߂̎d�g��*/

	/*�f�B�X�N���v�^�́A�����f�[�^�̎�ނ�A�ۑ��ꏊ�A�T�C�Y�Ȃǂ̏��B*/
	/*�f�B�X�N���v�^�q�[�v�́A�����f�B�X�N���v�^��ۑ�����z��B*/

#pragma endregion

#pragma region �t�F���X�̐���

	//�t�F���X�̐���
	result = _dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&_fence));

#pragma endregion
}

void DirectXManager::Start()
{
#pragma region ���\�[�X�o���A�ύX���A

	//�o�b�N�o�b�t�@�̔ԍ����擾(�\�Ɨ������Ȃ��̂�0or1)
	UINT _bbIndex = _swapchain->GetCurrentBackBufferIndex();

	

	//�o�b�N�o�b�t�@����w��
	barrierDesc.Transition.pResource = _backBuffers[_bbIndex];

	//�\�����Ă������w��
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

	//���̐悩�烌���_�[�^�[�Q�b�g���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	//�o���A�̎��s
	_cmdList->ResourceBarrier(1, &barrierDesc);

#pragma endregion

#pragma region ��ʂ̃N���A

	//�����_�[�^�[�Q�b�g�r���[�̐擪�A�h���X���擾
	D3D12_CPU_DESCRIPTOR_HANDLE _rtvH =
		_rtvHeaps->GetCPUDescriptorHandleForHeapStart();

	_rtvH.ptr += _bbIndex * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);

	//(�����_�[�^�[�Q�b�g�r���[�̐��ArtvHeaps�̐擪�A�h���X,�}���`�����_�[�^�[�Q�b�g,�[�x)
	_cmdList->OMSetRenderTargets(1, &_rtvH, false, nullptr);

	//��ʂ̐F�ݒ�
	float clearColor[] = { 0.1f,0.25f,0.5f,0.0f };

	//��ʂ̃N���A
	_cmdList->ClearRenderTargetView(_rtvH, clearColor, 0, nullptr);

#pragma endregion
}

void DirectXManager::End()
{
#pragma region �R�}���h�̃t���b�V��

	//���\�[�X�o���A��߂�
	//�`���Ԃ���...
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//�\����Ԃɖ߂�
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	//���s
	_cmdList->ResourceBarrier(1, &barrierDesc);

	//���߂̃N���[�Y
	_cmdList->Close();

	//�R�}���h���X�g�̎��s
	ID3D12CommandList* _cmdLists[] = { _cmdList };//�R�}���h���X�g�̔z��
	_cmdQueue->ExecuteCommandLists(1, _cmdLists);

	//�R�}���h���X�g�̎��s���I������̂�҂�
	_cmdQueue->Signal(_fence, ++fenceVal);

	//�t�F���X�̌Ăяo��
	if (_fence->GetCompletedValue() != fenceVal)
	{
		//�C�x���g�n���h���̎擾
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);

		_fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//�R�}���h�̃N���A
	_cmdAllocator->Reset();
	//�ĂуR�}���h���X�g�����ߍ��ޏ���
	_cmdList->Reset(_cmdAllocator, nullptr);
	//�o�b�N�o�b�t�@�̃t���b�v
	_swapchain->Present(1, 0);

#pragma endregion
}
