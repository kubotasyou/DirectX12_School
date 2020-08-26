#include "DirectXManager.h"
#include <Windows.h>

//↓なんか型識別子が必要とかでヘッダーで作れなった。
//std::vector<ID3D12Resource*> _backBuffers(2);

DirectXManager::DirectXManager()
{
}

DirectXManager::~DirectXManager()
{
}

void DirectXManager::Initialize(HWND hwnd)
{
#pragma region グラフィックボードのアダプタを列挙

	//DXGIファクトリーの生成
	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));

	//アダプターの列挙用
	vector<IDXGIAdapter*> _adapters;

	//アダプターオブジェクトを格納する変数
	IDXGIAdapter* _tmpAdapter = nullptr;

	for (int i = 0;
		_dxgiFactory->EnumAdapters(i, &_tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		//利用可能なアダプターを格納する
		_adapters.push_back(_tmpAdapter);
	}

	for (int i = 0; i < _adapters.size(); i++)
	{
		//アダプターの設定構造体
		DXGI_ADAPTER_DESC adDesc{};//AdapterDesc

		//アダプターの情報を取得
		_adapters[i]->GetDesc(&adDesc);

		//アダプター名
		wstring strDesc = adDesc.Description;

		//探したいアダプターの名前を確認
		if (strDesc.find(L"NAVIDATA") != string::npos)
		{
			//見つかったら_tmpAdapterに格納
			_tmpAdapter = _adapters[i];
			break;
		}
	}

#pragma endregion

#pragma region デバイスの生成

	//使用できるレベルを一つ一つ調べる
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	//フューチャーレベルの初期化
	D3D_FEATURE_LEVEL _featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(_tmpAdapter, levels[i], IID_PPV_ARGS(&_dev));

		//デバイスを生成できたらループを抜ける
		if (result == S_OK)
		{
			_featureLevel = levels[i];
			break;
		}
	}

#pragma endregion

#pragma region コマンドリストとアロケータの生成


	//コマンドアロケータの生成
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&_cmdAllocator));

	//コマンドリストの生成
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		_cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));

	/*コマンドアロケータはコマンドリストに格納する、
	　命令用のメモリを管理する。
	  命令が実行されるまでは、解放されることはない。*/

	  /*コマンドリストは描画命令用のインターフェース。
	  　それを、コマンドアロケータに格納する。*/

#pragma endregion

#pragma region コマンドキューの生成

	   //コマンドキューの設定構造体
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	_dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

#pragma endregion

#pragma region スワップチェーンの生成

	//スワップチェーンの設定構造体
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};

	//↓数値を直接入れるところをおしゃぶってるよ
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;

	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;       //色情報の書式
	swapchainDesc.SampleDesc.Count = 1;                      //サンプリングをしない→1
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;      //バックバッファを使用
	swapchainDesc.BufferCount = 2;                           //バックバッファをしようするため2
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//フリップ後は破棄する
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//設定の反映
	_dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&_swapchain
	);

#pragma endregion

#pragma region レンダーターゲットビューの生成

	//レンダーターゲットビューを指定
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	//バックバッファを使うため、表と裏の二つ
	heapDesc.NumDescriptors = 2;

	//設定の反映
	_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeaps));


	//----------------------------------------

	//表と裏の二つ分について

	for (int i = 0; i < 2; i++)
	{
		//スワップチェーンからバッファを取得
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));

		//ディスクリプタヒープの先頭ハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE _handle =
			_rtvHeaps->GetCPUDescriptorHandleForHeapStart();

		//表と裏でポインターを1つずらす
		_handle.ptr += i * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);

		//レンダーターゲットビューの生成
		_dev->CreateRenderTargetView(
			_backBuffers[i],
			nullptr,
			_handle
		);
	}

	//----------------------------------------

	/*バックバッファに描画結果を書き込むための仕組み*/

	/*ディスクリプタは、扱うデータの種類や、保存場所、サイズなどの情報。*/
	/*ディスクリプタヒープは、それらディスクリプタを保存する配列。*/

#pragma endregion

#pragma region フェンスの生成

	//フェンスの生成
	result = _dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&_fence));

#pragma endregion
}

void DirectXManager::Start()
{
#pragma region リソースバリア変更復帰

	//バックバッファの番号を取得(表と裏しかないので0or1)
	UINT _bbIndex = _swapchain->GetCurrentBackBufferIndex();

	

	//バックバッファから指定
	barrierDesc.Transition.pResource = _backBuffers[_bbIndex];

	//表示してる方から指定
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

	//この先からレンダーターゲット状態
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	//バリアの実行
	_cmdList->ResourceBarrier(1, &barrierDesc);

#pragma endregion

#pragma region 画面のクリア

	//レンダーターゲットビューの先頭アドレスを取得
	D3D12_CPU_DESCRIPTOR_HANDLE _rtvH =
		_rtvHeaps->GetCPUDescriptorHandleForHeapStart();

	_rtvH.ptr += _bbIndex * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);

	//(レンダーターゲットビューの数、rtvHeapsの先頭アドレス,マルチレンダーターゲット,深度)
	_cmdList->OMSetRenderTargets(1, &_rtvH, false, nullptr);

	//画面の色設定
	float clearColor[] = { 0.1f,0.25f,0.5f,0.0f };

	//画面のクリア
	_cmdList->ClearRenderTargetView(_rtvH, clearColor, 0, nullptr);

#pragma endregion
}

void DirectXManager::End()
{
#pragma region コマンドのフラッシュ

	//リソースバリアを戻す
	//描画状態から...
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//表示状態に戻す
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	//実行
	_cmdList->ResourceBarrier(1, &barrierDesc);

	//命令のクローズ
	_cmdList->Close();

	//コマンドリストの実行
	ID3D12CommandList* _cmdLists[] = { _cmdList };//コマンドリストの配列
	_cmdQueue->ExecuteCommandLists(1, _cmdLists);

	//コマンドリストの実行が終了するのを待つ
	_cmdQueue->Signal(_fence, ++fenceVal);

	//フェンスの呼び出し
	if (_fence->GetCompletedValue() != fenceVal)
	{
		//イベントハンドルの取得
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);

		_fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//コマンドのクリア
	_cmdAllocator->Reset();
	//再びコマンドリストをため込む準備
	_cmdList->Reset(_cmdAllocator, nullptr);
	//バックバッファのフリップ
	_swapchain->Present(1, 0);

#pragma endregion
}
