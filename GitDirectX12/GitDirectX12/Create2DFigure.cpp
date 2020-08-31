#include "Create2DFigure.h"

using namespace std;

//頂点データ(3頂点分の座標)
XMFLOAT3 vertices[] =
{
	{-0.5f, -0.5f, 0.0f},//左下
	{-0.5f, +0.5f, 0.0f},//左上
	{+0.5f, -0.5f, 0.0f},//右下
};

//エラーの内容をまとめて、すぐに質問できるようにする
/*資料2-1を作ってクラス化を使用としたのですが、
シェーダーファイルの読み込みの部分で、PSBlobがnullだと
言われてしまいます。資料1の時点からクラス化していたので
実行確認してからクラス化しているわけではありません。
*/

Create2DFigure::Create2DFigure(ID3D12Device * device, ID3D12GraphicsCommandList * commandList)
	:_dev(device),
	 _cmdList(commandList)
{
	Initialize();
}

Create2DFigure::~Create2DFigure()
{
}

void Create2DFigure::Initialize()
{
	ReadShaderFile();
	//基本的にパイプラインは最初に作る!
	InitGraphicsPipeLine();
	CreateVertBuff();
}

void Create2DFigure::CreateVertBuff()
{
#pragma region 頂点バッファの確保

	//頂点ヒープ設定構造体
	D3D12_HEAP_PROPERTIES heapprop{};//HeapProperties
	//GPUへのデータ転送用
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//リソースの設定構造体
	D3D12_RESOURCE_DESC resdesc{};

	//バックバッファーを使うのでBufferを指定する
	resdesc.Dimension =
		D3D12_RESOURCE_DIMENSION_BUFFER;
	//頂点情報が入る分のサイズを確保する
	resdesc.Width = sizeof(vertices);
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout =
		D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPUリソースの生成
	ID3D12Resource* vertBuff = nullptr;

	//頂点バッファの生成
	result = _dev->CreateCommittedResource(
		&heapprop,           //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resdesc,            //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	/*頂点バッファーは、GPUに頂点情報を受け取るためのリソース(領域)*/

#pragma endregion

#pragma region 頂点情報のコピー

	//GPU上のバッファに対応して仮想メモリを取得
	XMFLOAT3* _vertMap = nullptr;

	//コピー開始
	//バッファーに頂点情報をコピーする
	result = vertBuff->Map(0, nullptr, (void**)&_vertMap);

	//全頂点のデータに対して行う
	for (int i = 0; i < _countof(vertices); i++)
	{
		//座標のコピー
		_vertMap[i] = vertices[i];
	}

	//コピー終了
	vertBuff->Unmap(0, nullptr);

#pragma endregion

#pragma region 頂点バッファービューの作成

	//バッファーの仮想アドレスの取得
	vbview.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//全部で何バイトかを調べる
	vbview.SizeInBytes = sizeof(vertices);
	//1頂点当たりのバイト数を調べる
	vbview.StrideInBytes = sizeof(vertices[0]);

	/*頂点バッファービューは、データの大きさを知らせるもの*/

#pragma endregion
}

HRESULT Create2DFigure::ReadShaderFile()
{
#pragma region シェーダーファイルの読み込みとコンパイル

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVertexShader.hlsl",                      //シェーダー名指定
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,              //インクルードを可能にする
		"VSmain",                                       //エントリーポイントの指定
		"vs_5_0",                                       //シェーダーモデルの指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用指定
		0,
		&vsBlob,
		&errorBlob
	);

	//ピクセルシェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPixelShader.hlsl",                      //シェーダー名指定
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,              //インクルードを可能にする
		"PSmain",                                       //エントリーポイントの指定
		"ps_5_0",                                       //シェーダーモデルの指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用指定
		0,
		&psBlob,
		&errorBlob
	);

	//↓シェーダーの読み込みに失敗した場合に内容を表示する
	if (FAILED(result))
	{
		//errorBlobの中から、エラーの内容をstring型にコピーする
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";

		//エラーの内容を出力ウィンドウに表示する
		OutputDebugStringA(error.c_str());
		exit(1);
	}

#pragma endregion

	return S_OK;
}

void Create2DFigure::InitGraphicsPipeLine()
{

#pragma region 頂点レイアウト設定

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0,
		},
	};

#pragma endregion

#pragma region グラフィクスパイプラインの作成

	//グラフィクスパイプラインの設定構造体
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//シェーダーのセット--------------------------------------
	//頂点シェーダーをパイプラインに設定
	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
	//ピクセルシェーダーをパイプラインに設定
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();
	//--------------------------------------------------------

	//サンプルマスクとラスタライズステートの設定--------------
	//デフォルトの設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//背面カリングを行わない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//ポリゴン内を塗りつぶす
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//深度クリッピングを有効にする
	gpipeline.RasterizerState.DepthClipEnable = true;
	//--------------------------------------------------------

	//ブレンドステートの設定----------------------------------
	//RGBA 全てのチャンネルを描画
	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	//--------------------------------------------------------

	//頂点レイアウトの設定------------------------------------
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	//--------------------------------------------------------

	//図形の形状の設定
	gpipeline.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//描画対象の数
	gpipeline.NumRenderTargets = 1;

	//0～1に正規化されたRGBA
	gpipeline.RTVFormats[0] =
		DXGI_FORMAT_R8G8B8A8_UNORM;

	//1ピクセルにつき1回のサンプリング
	gpipeline.SampleDesc.Count = 1;


#pragma region ルートシグネチャの生成

	//ルートシグネチャの設定構造体
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};

	//頂点情報の存在を伝える
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//オブジェクトを確保する
	ID3DBlob* rootSigBlob = nullptr;

	//バイナリコード(ソースコードを機械語に変換するもの)の作成
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,            //ルートシグネチャの設定
		D3D_ROOT_SIGNATURE_VERSION_1_0,//ルートシグネチャのバージョン
		&rootSigBlob, 				   //シェーダーの時と同じ
		&errorBlob);				   //エラーの時はここにメッセージが入る

	//ルートシグネチャの生成
	result = _dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature)
	);

	//パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootsignature;

#pragma endregion

	//グラフィクスパイプラインの生成
	result = _dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&pipelinestate)
	);

#pragma endregion
}

void Create2DFigure::Draw2D()
{
#pragma region 描画コマンド

	//パイプラインステートの設定
	_cmdList->SetPipelineState(pipelinestate);

	//ビューポートの設定----------------------------------
	D3D12_VIEWPORT viewport{};
	viewport.Width = window_width;
	viewport.Height = window_height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	_cmdList->RSSetViewports(1, &viewport);
	//----------------------------------------------------

	//シザー矩形の設定------------------------------------
	D3D12_RECT scissorret{};
	scissorret.left = 0;
	scissorret.right = scissorret.left + window_width;
	scissorret.top = 0;
	scissorret.bottom = scissorret.top + window_height;
	_cmdList->RSSetScissorRects(1, &scissorret);
	//----------------------------------------------------

	//ルートシグネチャの設定
	_cmdList->SetGraphicsRootSignature(rootsignature);

	//プリミティブ形状の設定
	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点バッファの設定
	_cmdList->IASetVertexBuffers(0, 1, &vbview);

	//描画
	_cmdList->DrawInstanced(3, 1, 0, 0);

#pragma endregion
}
