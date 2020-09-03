#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include"dxgi1_6.h"
#include <vector>

#include "Input.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;

class DirectXManager
{
public:
	//�R���X�g
	DirectXManager();

	//�f�X�g
	~DirectXManager();

	//������
	void Initialize(HWND hwnd);

	//�X�V�J�n
	void Start();

	//�X�V�I��
	void End();

	//�f�o�C�X�󂯎��p
	ID3D12Device* GetDev() const;

	//�R�}���h���X�g�󂯎��p
	ID3D12GraphicsCommandList* getCmdList() const;

private:

	HRESULT result;
	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;
	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;
	//�R�}���h���X�g�ɂ��܂������߂�GPU�ɏ��ԂɎ��s������d�g��
	ID3D12CommandQueue* _cmdQueue = nullptr;
	ID3D12DescriptorHeap* _rtvHeaps = nullptr;//RenderTargetViewHeap
	ID3D12Fence* _fence = nullptr;

	UINT64 fenceVal = 0;

	//��ʂ̐F
	float r, g, b, a = 0.0f;

	//�����_�[�^�[�Q�b�g�r���[�̐ݒ�\����
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	//������(2)����������ǂ��g������G���[�ɂȂ����B
	vector<ID3D12Resource*> _backBuffers{2};
	//���\�[�X�o���A�̐ݒ�\����
	D3D12_RESOURCE_BARRIER barrierDesc{};

	//�E�B���h�E�T�C�Y
	const int window_width = 1280;
	const int window_height = 720;


	Input* input;
};

