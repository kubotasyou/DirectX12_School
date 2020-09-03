#include "Create2DFigure.h"

#define PI 3.14159265359

using namespace std;

//頂点数
const int a = 50;

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
	CreateConstBuff();
	CreateIndexBuff();
}

void Create2DFigure::CreateVertBuff()
{

	//頂点データ
	XMFLOAT3 vertices[a + 1];

	//n角形の表示
	for (int i = 0; i < a; i++)
	{
		vertices[i].x = 0.5f * sinf(XM_2PI / a * i) *
			window_height / window_width;
		vertices[i].y = 0.5f * cosf(XM_2PI / a * i);
		vertices[i].z = 0.0f;
	}
	vertices[a] = { 0,0,0 };

#pragma region 頂点バッファの確保

	//GPUへのデータ転送用
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

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

void Create2DFigure::CreateConstBuff()
{
#pragma region 定数バッファのディスクリプタヒープを作成

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};

	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//シェーダーから見えるようにする
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//バッファーの数は1
	descHeapDesc.NumDescriptors = 1;

	//生成
	result = _dev->CreateDescriptorHeap(
		&descHeapDesc,
		IID_PPV_ARGS(&basicDescHeap)
	);

#pragma endregion

#pragma region 定数バッファの作成

	//ヒープの設定
	D3D12_HEAP_PROPERTIES cbHeapprop{};

	//GPUへの転送用
	cbHeapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//リソースの設定
	D3D12_RESOURCE_DESC cbresDesc{};

	cbresDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//256バイトアラインメント
	cbresDesc.Width = (sizeof(ConstBufferData) + 0xff)& ~0xff;
	cbresDesc.Height = 1;
	cbresDesc.DepthOrArraySize = 1;
	cbresDesc.MipLevels = 1;
	cbresDesc.SampleDesc.Count = 1;
	cbresDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPUリソースの生成
	ID3D12Resource* constBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&cbHeapprop,                      //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbresDesc,                       //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);

#pragma endregion

#pragma region 定数バッファのデータ転送(cpp側での色変更)

	//マッピング(コピー)
	result = constBuff->Map(0, nullptr, (void**)&cbData);

	//こちら側で色を指定
	cbData->color = XMFLOAT4(0, 1, 0, 0.2f);

	//マップ解除
	constBuff->Unmap(0, nullptr);

#pragma endregion

#pragma region 定数バッファービューの生成

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};

	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;

	//生成
	_dev->CreateConstantBufferView(
		&cbvDesc,
		basicDescHeap->GetCPUDescriptorHandleForHeapStart()
	);

	/*定数バッファービューはCreateしないといけない*/

#pragma endregion

}

void Create2DFigure::CreateIndexBuff()
{
	unsigned short indices[a * 3];

	//n角形の表示
	for (int i = 0; i < a; i++)
	{
		indices[i * 3 + 0] = i;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = a;
	}
	indices[_countof(indices) - 1] = 0;

#pragma region インデクスバッファの確保

	ID3D12Resource* indexBuff = nullptr;

	//インデックスの情報を入れる分のサイズのメモリを確保
	resdesc.Width = sizeof(indices);

	//GPUリソースの生成
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

#pragma endregion

#pragma region インデクスバッファのデータ転送

	//GPU上のバッファに対応した仮想メモリを取得
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全てのインデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		//コピーを行う
		indexMap[i] = indices[i];
	}

	//終了
	indexBuff->Unmap(0, nullptr);

#pragma endregion

#pragma region インデックスバッファービューの作成

	//バッファーの仮想アドレスの取得
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	//種別を設定
	ibView.Format = DXGI_FORMAT_R16_UINT;
	//全部で何バイトかを調べる
	ibView.SizeInBytes = sizeof(indices);

	//shortが16ビットなのでR16_UINTになった

#pragma endregion

}

void Create2DFigure::CreateFigure(float Radius)
{
	//頂点配列の宣言
	XMFLOAT3 vert[a + 1];

	//vert[0].x = Radius * sinf(XM_2PI / _countof(vert) * 0);
	//vert[0].y = Radius * cosf(XM_2PI / _countof(vert) * 0);
	//vert[0].z = 0.0f;
	//vert[1].x = Radius * sinf(XM_2PI / _countof(vert) * 1);
	//vert[1].y = Radius * cosf(XM_2PI / _countof(vert) * 1);
	//vert[1].z = 0.0f;
	//vert[2].x = Radius * sinf(XM_2PI / _countof(vert) * 2);
	//vert[2].y = Radius * cosf(XM_2PI / _countof(vert) * 2);
	//vert[2].z = 0.0f;
	//vert[3] = { 0,0,0 };

	//n角形の表示
	for (int i = 0; i < a; i++)
	{
		vert[i].x = Radius * sinf(XM_2PI / a * i) * 
			window_height /  window_width;
		vert[i].y = Radius * cosf(XM_2PI / a * i);
		vert[i].z = 0.0f;
	}

	//インデックス配列
	unsigned short indi[a * 3];

	//indi[0] = 0;
	//indi[1] = 1;
	//indi[2] = 3;
	//	    
	//indi[3] = 1;
	//indi[4] = 2;
	//indi[5] = 3;
	//	    
	//indi[6] = 2;
	//indi[7] = 0;
	//indi[8] = 3;

	//n角形の表示
	for (int i = 0; i < a; i++)
	{
		indi[i * 3 + 0] = i;
		indi[i * 3 + 1] = i + 1;
		indi[i * 3 + 2] = a;
	}
	indi[_countof(indi) - 1] = 0;
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
	//ワイヤーフレーム表示を行う
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//深度クリッピングを有効にする
	gpipeline.RasterizerState.DepthClipEnable = true;

	//ラスタライザとは、頂点のピクセル化
	//--------------------------------------------------------

	//ディスクリプタテーブルの初期化--------------------------
	//ディスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descTblRange{};

	//定数は一つ
	descTblRange.NumDescriptors = 1;
	//種別は定数を指定(ConstBufferView)
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//0番から開始
	descTblRange.BaseShaderRegister = 0;
	//標準設定
	descTblRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootparam = {};

	//種別の設定(DiscriptorTable)
	rootparam.ParameterType = 
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

	//ディスクリプタレンジを指定
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;

	//ディスクリプタレンジの数
	rootparam.DescriptorTable.NumDescriptorRanges = 1;

	//全てのシェーダーから見えるようにする
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//--------------------------------------------------------

	//ブレンドステートの設定----------------------------------
	////RGBA 全てのチャンネルを描画
	//gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask =
	//	D3D12_COLOR_WRITE_ENABLE_ALL;

	//レンダーターゲットのブレンド設定(全部で8個あるらしい)
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
	//標準設定
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//共通の設定(どのブレンドを使うにあたっても必須)
	blendDesc.BlendEnable = true;//ブレンド合成を有効にする
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算合成
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;  //ソースの値を100%使う
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う

#pragma region ブレンドのいろいろ
	//加算合成
	//blendDesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blendDesc.SrcBlend = D3D12_BLEND_ONE; //ソースの値を100%使う
	//blendDesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	//ソース(図形の色)とデスト(背景の色)の100%の値を加算する
	//デスト     ソース    結果
	//(0,0,1) + (1,0,0) = (1,0,1)

	//減算合成
	//blendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//デストからソースを減算
	//blendDesc.SrcBlend = D3D12_BLEND_ONE; //ソースの値を100%使う
	//blendDesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	//デスト(背景の色)の100%からソース(図形の色)の100%の値を減算する
	//デスト     ソース    結果
	//(1,1,1) - (1,0,0) = (0,1,1)

	////色反転
	//blendDesc.BlendOp = D3D12_BLEND_OP_ADD;         //加算
	//blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f - デストのカラーの値
	//blendDesc.DestBlend = D3D12_BLEND_ZERO;         //使用しない
	////デストのカラーを反転した値(1から引いた値)を使用する
	////ソース     デスト    結果
	////(1,1,1) - (1,0,0) = (0,1,1)になるらしい

	//半透明合成(アルファブレンディング)
	blendDesc.BlendOp = D3D12_BLEND_OP_ADD;         //加算
	blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;     //ソースのα値
	blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f - ソースのα値
	//ソースのα値の割合に応じて、ソースとデストのカラーを混ぜる
	//        デスト      ソース          結果
	//例) (0,0,1)*0.2 + (1,1,1)*0.8 = (0.8,0.8,1)
#pragma endregion

	//ブレンドステートの登録
	gpipeline.BlendState.RenderTarget[0] = blendDesc;
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

	//ディスクリプタテーブルの情報を追加
	rootSignatureDesc.Flags = 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//ルートパラメータの先頭アドレス
	rootSignatureDesc.pParameters = &rootparam;
	//ルートパラメータの数
	rootSignatureDesc.NumParameters = 1;

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
	viewport.Width = window_width;  //横幅
	viewport.Height = window_height;//縦幅
	viewport.TopLeftX = 0;          //左上X
	viewport.TopLeftY = 0;          //左上Y
	viewport.MinDepth = 0.0f;       //最小深度(0)
	viewport.MaxDepth = 1.0f;       //最大深度(1)
	//コマンドリストに追加する
	_cmdList->RSSetViewports(1, &viewport);

	//ビューポートは、ウィンドウの中での描画範囲
	//----------------------------------------------------

	//シザー矩形の設定------------------------------------
	D3D12_RECT scissorret{};
	scissorret.left = 0;               //切り抜き座標左
	scissorret.right = 
		scissorret.left + window_width;//切り抜き座標右
	scissorret.top = 0;                //切り抜き座標上
	scissorret.bottom = 
		scissorret.top + window_height;//切り抜き座標下
	//コマンドリストに追加する
	_cmdList->RSSetScissorRects(1, &scissorret);

	//シザー矩形は、ビューポートの中での描画範囲
	//----------------------------------------------------

	//ルートシグネチャの設定
	_cmdList->SetGraphicsRootSignature(rootsignature);

	//ディスクリプタヒープの設定------------------------------
	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap };
	_cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//先頭アドレスを取得
	_cmdList->SetGraphicsRootDescriptorTable(0,
		basicDescHeap->GetGPUDescriptorHandleForHeapStart());
	//--------------------------------------------------------


#pragma region プリミティブ形状の一覧

	////点のリスト
	//_cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	////線のリスト
	//_cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	////線のストリップ
	//_cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	////三角形のリスト
	//_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	////三角形のストリップ
	//_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//STRIPはデータの効率がいいが、くっついてる。
	//LIST はデータの効率は悪いが、独立している。

#pragma endregion
	//プリミティブ形状の設定
	_cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点バッファの設定
	_cmdList->IASetVertexBuffers(0, 1, &vbview);

	//インデックスバッファのセット
	_cmdList->IASetIndexBuffer(&ibView);

	//描画
	//(頂点数・インスタンス数・開始番号)
	//_cmdList->DrawInstanced(6, 1, 0, 0);

	//(インデックスの数・1・0・0・0)
	_cmdList->DrawIndexedInstanced(a * 3, 1, 0, 0, 0);

#pragma endregion
}
