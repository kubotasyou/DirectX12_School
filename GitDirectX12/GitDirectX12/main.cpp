#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include "DirectXManager.h"

using namespace std;



//ウィンドウクラスの設定
WNDCLASSEX w{};
//ウィンドウサイズ
const int window_width = 1280;
const int window_height = 720;
HWND hwnd;


//ウィンドウプロシージャ作成
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージで分岐
	switch (msg)
	{
	case WM_DESTROY://ウィンドウが破棄された
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//標準の初期化を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WindowCreate();

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
	//デバッグレイヤーをONにする
	ID3D12Debug* debugController;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif // DEBUG

	//ウィンドウの生成
	WindowCreate();

	//DirectXの初期化処理
	DirectXManager* dxManager = new DirectXManager();
	dxManager->Initialize(hwnd);

//	HRESULT result;
//	ID3D12Device* _dev = nullptr;
//	IDXGIFactory6* _dxgiFactory = nullptr;
//	IDXGISwapChain4* _swapchain = nullptr;
//	ID3D12CommandAllocator* _cmdAllocator = nullptr;
//	ID3D12GraphicsCommandList* _cmdList = nullptr;
//	//コマンドリストにたまった命令をGPUに順番に実行させる仕組み
//	ID3D12CommandQueue* _cmdQueue = nullptr;
//	ID3D12DescriptorHeap* _rtvHeaps = nullptr;//RenderTargetViewHeap
//
//#pragma region グラフィックボードのアダプタを列挙
//
//	//DXGIファクトリーの生成
//	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
//
//	//アダプターの列挙用
//	vector<IDXGIAdapter*> _adapters;
//
//	//アダプターオブジェクトを格納する変数
//	IDXGIAdapter* _tmpAdapter = nullptr;
//
//	for (int i = 0;
//		_dxgiFactory->EnumAdapters(i, &_tmpAdapter) != DXGI_ERROR_NOT_FOUND;
//		i++)
//	{
//		//利用可能なアダプターを格納する
//		_adapters.push_back(_tmpAdapter);
//	}
//
//	for (int i = 0; i < _adapters.size(); i++)
//	{
//		//アダプターの設定構造体
//		DXGI_ADAPTER_DESC adDesc{};//AdapterDesc
//
//		//アダプターの情報を取得
//		_adapters[i]->GetDesc(&adDesc);
//
//		//アダプター名
//		wstring strDesc = adDesc.Description;
//
//		//探したいアダプターの名前を確認
//		if (strDesc.find(L"NAVIDATA") != string::npos)
//		{
//			//見つかったら_tmpAdapterに格納
//			_tmpAdapter = _adapters[i];
//			break;
//		}
//	}
//
//#pragma endregion
//
//#pragma region デバイスの生成
//
//	//使用できるレベルを一つ一つ調べる
//	D3D_FEATURE_LEVEL levels[] =
//	{
//		D3D_FEATURE_LEVEL_12_1,
//		D3D_FEATURE_LEVEL_12_0,
//		D3D_FEATURE_LEVEL_11_1,
//		D3D_FEATURE_LEVEL_11_0,
//	};
//
//	//フューチャーレベルの初期化
//	D3D_FEATURE_LEVEL _featureLevel;
//
//	for (int i = 0; i < _countof(levels); i++)
//	{
//		//採用したアダプターでデバイスを生成
//		result = D3D12CreateDevice(_tmpAdapter, levels[i], IID_PPV_ARGS(&_dev));
//
//		//デバイスを生成できたらループを抜ける
//		if (result == S_OK)
//		{
//			_featureLevel = levels[i];
//			break;
//		}
//	}
//
//#pragma endregion
//
//#pragma region コマンドリストとアロケータの生成
//
//
//	//コマンドアロケータの生成
//	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
//		IID_PPV_ARGS(&_cmdAllocator));
//
//	//コマンドリストの生成
//	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
//		_cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
//
//	/*コマンドアロケータはコマンドリストに格納する、
//	　命令用のメモリを管理する。
//	  命令が実行されるまでは、解放されることはない。*/
//
//	/*コマンドリストは描画命令用のインターフェース。
//	　それを、コマンドアロケータに格納する。*/
//
//#pragma endregion
//
//#pragma region コマンドキューの生成
//
//	//コマンドキューの設定構造体
//	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
//	_dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));
//
//#pragma endregion
//
//#pragma region スワップチェーンの生成
//
//	//スワップチェーンの設定構造体
//	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
//
//	//↓数値を直接入れるところをおしゃぶってるよ
//	swapchainDesc.Width = window_width;
//	swapchainDesc.Height = window_height;
//
//	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;       //色情報の書式
//	swapchainDesc.SampleDesc.Count = 1;                      //サンプリングをしない→1
//	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;      //バックバッファを使用
//	swapchainDesc.BufferCount = 2;                           //バックバッファをしようするため2
//	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//フリップ後は破棄する
//	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
//
//	//設定の反映
//	_dxgiFactory->CreateSwapChainForHwnd(
//		_cmdQueue,
//		hwnd,
//		&swapchainDesc,
//		nullptr,
//		nullptr,
//		(IDXGISwapChain1**)&_swapchain
//	);
//
//#pragma endregion
//
//#pragma region レンダーターゲットビューの生成
//
//	//レンダーターゲットビューの設定構造体
//	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
//
//	//レンダーターゲットビューを指定
//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//
//	//バックバッファを使うため、表と裏の二つ
//	heapDesc.NumDescriptors = 2;
//
//	//設定の反映
//	_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeaps));
//
//
//	//----------------------------------------
//
//	//表と裏の二つ分について
//	vector<ID3D12Resource*> _backBuffers(2);
//
//	for (int i = 0; i < 2; i++)
//	{
//		//スワップチェーンからバッファを取得
//		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));
//
//		//ディスクリプタヒープの先頭ハンドルを取得
//		D3D12_CPU_DESCRIPTOR_HANDLE _handle =
//			_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
//
//		//表と裏でポインターを1つずらす
//		_handle.ptr += i * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
//
//		//レンダーターゲットビューの生成
//		_dev->CreateRenderTargetView(
//			_backBuffers[i],
//			nullptr,
//			_handle
//		);
//	}
//
//	//----------------------------------------
//
//	/*バックバッファに描画結果を書き込むための仕組み*/
//
//	/*ディスクリプタは、扱うデータの種類や、保存場所、サイズなどの情報。*/
//	/*ディスクリプタヒープは、それらディスクリプタを保存する配列。*/
//
//#pragma endregion
//
//#pragma region フェンスの生成
//
//	ID3D12Fence* _fence = nullptr;
//
//	UINT64 fenceVal = 0;
//
//	//フェンスの生成
//	result = _dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE,
//		IID_PPV_ARGS(&_fence));
//
//#pragma endregion

	//メッセージ
	MSG msg{};

	while (true)
	{
		//メッセージがあるか?
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			//キー入力メッセージの処理
			TranslateMessage(&msg);
			//プロシージャにメッセージを送る
			DispatchMessage(&msg);
		}

		//終了メッセージが来たらループを抜ける
		//(ここにescapeで終了の処理を加えたらいいのかな？)
		if (msg.message == WM_QUIT)
		{
			break;
		}

#pragma region Update処理

		dxManager->Start();

//#pragma region リソースバリア変更復帰
//
//		//バックバッファの番号を取得(表と裏しかないので0or1)
//		UINT _bbIndex = _swapchain->GetCurrentBackBufferIndex();
//
//		//リソースバリアの設定構造体
//		D3D12_RESOURCE_BARRIER barrierDesc{};
//
//		//バックバッファから指定
//		barrierDesc.Transition.pResource = _backBuffers[_bbIndex];
//
//		//表示してる方から指定
//		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
//
//		//この先からレンダーターゲット状態
//		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
//
//		//バリアの実行
//		_cmdList->ResourceBarrier(1, &barrierDesc);
//
//#pragma endregion
//
//#pragma region 画面のクリア
//
//		//レンダーターゲットビューの先頭アドレスを取得
//		D3D12_CPU_DESCRIPTOR_HANDLE _rtvH =
//			_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
//
//		_rtvH.ptr += _bbIndex * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
//
//		//(レンダーターゲットビューの数、rtvHeapsの先頭アドレス,マルチレンダーターゲット,深度)
//		_cmdList->OMSetRenderTargets(1, &_rtvH, false, nullptr);
//
//		//画面の色設定
//		float clearColor[] = { 0.1f,0.25f,0.5f,0.0f };
//
//		//画面のクリア
//		_cmdList->ClearRenderTargetView(_rtvH, clearColor, 0, nullptr);
//
//#pragma endregion

#pragma region 描画コマンド



#pragma endregion

		dxManager->End();

//#pragma region コマンドのフラッシュ
//
//		//リソースバリアを戻す
//		//描画状態から...
//		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
//		//表示状態に戻す
//		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
//
//		//実行
//		_cmdList->ResourceBarrier(1, &barrierDesc);
//
//		//命令のクローズ
//		_cmdList->Close();
//
//		//コマンドリストの実行
//		ID3D12CommandList* _cmdLists[] = { _cmdList };//コマンドリストの配列
//		_cmdQueue->ExecuteCommandLists(1, _cmdLists);
//
//		//コマンドリストの実行が終了するのを待つ
//		_cmdQueue->Signal(_fence, ++fenceVal);
//
//		//フェンスの呼び出し
//		if (_fence->GetCompletedValue() != fenceVal)
//		{
//			//イベントハンドルの取得
//			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
//
//			_fence->SetEventOnCompletion(fenceVal, event);
//			WaitForSingleObject(event, INFINITE);
//			CloseHandle(event);
//		}
//
//		//コマンドのクリア
//		_cmdAllocator->Reset();
//		//再びコマンドリストをため込む準備
//		_cmdList->Reset(_cmdAllocator, nullptr);
//		//バックバッファのフリップ
//		_swapchain->Present(1, 0);
//
//#pragma endregion


#pragma endregion

	}

	//ウィンドウクラスの登録を解除
	UnregisterClass(w.lpszClassName, w.hInstance);

	delete dxManager;

	return 0;
}

//ウィンドウの生成
void WindowCreate()
{
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;                //ウィンドウプロシージャの設定
	w.lpszClassName = L"DirectXGame";//ウィンドウ名
	w.hInstance = GetModuleHandle(nullptr);             //ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);            //カーソル指定

	//ウィンドウクラスをOSに登録
	RegisterClassEx(&w);

	//ウィンドウサイズ(X座標, Y座標, 横幅, 縦幅)
	RECT windowRect = { 0,0,window_width,window_height };
	//自動でサイズを補正する
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(
		w.lpszClassName,                   //クラス名
		L"Re:ゼロから始めるDirectXの課題", //タイトルバーの文字
		WS_OVERLAPPEDWINDOW,               //タイトルバーと画面に境界があるウィンドウ
		CW_USEDEFAULT,                     //表示X座標(OSに任せる)
		CW_USEDEFAULT,                     //表示Y座標(OSに任せる)
		windowRect.right - windowRect.left,//ウィンド横幅
		windowRect.bottom - windowRect.top,//ウィンド縦幅
		nullptr,                           //親のウィンドウハンドル
		nullptr,                           //メニューハンドル
		w.hInstance,                       //呼び出しアプリケーションハンドル
		nullptr,                           //追加パラメーター
		);

	//ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);
}