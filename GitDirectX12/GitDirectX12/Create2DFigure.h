#pragma once

#include <vector>
#include <d3d12.h>
//数学ライブラリのインクルード
#include <DirectXMath.h>
using namespace DirectX;
//D3Dコンパイラのインクルード
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")

//定数バッファー用データ構造体
struct ConstBufferData
{
	XMFLOAT4 color;//色(RGBA)
};

class Create2DFigure
{
public:
	//コンストラクタ
	Create2DFigure(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	//デストラクタ
	~Create2DFigure();

	//初期化
	void Initialize();

	//頂点バッファの作成
	void CreateVertBuff();

	//定数バッファの作成
	void CreateConstBuff();

	//インデックスバッファの作成
	void CreateIndexBuff();

	//多角形を作る
	void CreateFigure(float Radius);

	//シェーダーファイルの読み込み
	HRESULT ReadShaderFile();

	//グラフィックパイプラインの初期化
	void InitGraphicsPipeLine();

	//描画コマンド
	void Draw2D();

private:

	HRESULT result;
	ID3D12Device* _dev = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;
	ID3DBlob* vsBlob = nullptr;   //頂点シェーダーオブジェクト
	ID3DBlob* psBlob = nullptr;   //ピクセルシェーダーオブジェクト
	ID3DBlob* errorBlob = nullptr;//エラーオブジェクト
	//グラフィクスパイプラインの生成
	ID3D12PipelineState* pipelinestate = nullptr;
	//ルートシグネチャの宣言
	ID3D12RootSignature* rootsignature;
	//頂点バッファービューの作成
	D3D12_VERTEX_BUFFER_VIEW vbview{};
	//頂点・インデックスのリソースの設定構造体
	D3D12_RESOURCE_DESC resdesc{};
	//頂点・インデックスのヒープ設定構造体
	D3D12_HEAP_PROPERTIES heapprop{};//HeapProperties
	//インデックスバッファービューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//定数バッファに送るデータ
	ConstBufferData* cbData = nullptr;
	//定数バッファー用のディスクリプタヒープ
	ID3D12DescriptorHeap* basicDescHeap = nullptr;


	//ウィンドウサイズ
	const int window_width = 1280;
	const int window_height = 720;
};

