#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;

class DirectXManager
{
public:
	//コンスト
	DirectXManager();

	//デスト
	~DirectXManager();

	//初期化
	void Initialize(HWND hwnd);

	//更新開始
	void Start();

	//更新終了
	void End();

private:

	HRESULT result;
	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;
	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;
	//コマンドリストにたまった命令をGPUに順番に実行させる仕組み
	ID3D12CommandQueue* _cmdQueue = nullptr;
	ID3D12DescriptorHeap* _rtvHeaps = nullptr;//RenderTargetViewHeap
	ID3D12Fence* _fence = nullptr;

	UINT64 fenceVal = 0;


	//レンダーターゲットビューの設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	vector<ID3D12Resource*> _backBuffers{2};
	//リソースバリアの設定構造体
	D3D12_RESOURCE_BARRIER barrierDesc{};

	//ウィンドウサイズ
	const int window_width = 1280;
	const int window_height = 720;
};

