#include "Create2DFigure.h"

using namespace std;

//���_�f�[�^(3���_���̍��W)
XMFLOAT3 vertices[] =
{
	{-0.5f, -0.5f, 0.0f},//����
	{-0.5f, +0.5f, 0.0f},//����
	{+0.5f, -0.5f, 0.0f},//�E��
};

//�G���[�̓��e���܂Ƃ߂āA�����Ɏ���ł���悤�ɂ���
/*����2-1������ăN���X�����g�p�Ƃ����̂ł����A
�V�F�[�_�[�t�@�C���̓ǂݍ��݂̕����ŁAPSBlob��null����
�����Ă��܂��܂��B����1�̎��_����N���X�����Ă����̂�
���s�m�F���Ă���N���X�����Ă���킯�ł͂���܂���B
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
	//��{�I�Ƀp�C�v���C���͍ŏ��ɍ��!
	InitGraphicsPipeLine();
	CreateVertBuff();
}

void Create2DFigure::CreateVertBuff()
{
#pragma region ���_�o�b�t�@�̊m��

	//���_�q�[�v�ݒ�\����
	D3D12_HEAP_PROPERTIES heapprop{};//HeapProperties
	//GPU�ւ̃f�[�^�]���p
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//���\�[�X�̐ݒ�\����
	D3D12_RESOURCE_DESC resdesc{};

	//�o�b�N�o�b�t�@�[���g���̂�Buffer���w�肷��
	resdesc.Dimension =
		D3D12_RESOURCE_DIMENSION_BUFFER;
	//���_��񂪓��镪�̃T�C�Y���m�ۂ���
	resdesc.Width = sizeof(vertices);
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout =
		D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPU���\�[�X�̐���
	ID3D12Resource* vertBuff = nullptr;

	//���_�o�b�t�@�̐���
	result = _dev->CreateCommittedResource(
		&heapprop,           //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resdesc,            //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	/*���_�o�b�t�@�[�́AGPU�ɒ��_�����󂯎�邽�߂̃��\�[�X(�̈�)*/

#pragma endregion

#pragma region ���_���̃R�s�[

	//GPU��̃o�b�t�@�ɑΉ����ĉ��z���������擾
	XMFLOAT3* _vertMap = nullptr;

	//�R�s�[�J�n
	//�o�b�t�@�[�ɒ��_�����R�s�[����
	result = vertBuff->Map(0, nullptr, (void**)&_vertMap);

	//�S���_�̃f�[�^�ɑ΂��čs��
	for (int i = 0; i < _countof(vertices); i++)
	{
		//���W�̃R�s�[
		_vertMap[i] = vertices[i];
	}

	//�R�s�[�I��
	vertBuff->Unmap(0, nullptr);

#pragma endregion

#pragma region ���_�o�b�t�@�[�r���[�̍쐬

	//�o�b�t�@�[�̉��z�A�h���X�̎擾
	vbview.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//�S���ŉ��o�C�g���𒲂ׂ�
	vbview.SizeInBytes = sizeof(vertices);
	//1���_������̃o�C�g���𒲂ׂ�
	vbview.StrideInBytes = sizeof(vertices[0]);

	/*���_�o�b�t�@�[�r���[�́A�f�[�^�̑傫����m�点�����*/

#pragma endregion
}

HRESULT Create2DFigure::ReadShaderFile()
{
#pragma region �V�F�[�_�[�t�@�C���̓ǂݍ��݂ƃR���p�C��

	//���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicVertexShader.hlsl",                      //�V�F�[�_�[���w��
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,              //�C���N���[�h���\�ɂ���
		"VSmain",                                       //�G���g���[�|�C���g�̎w��
		"vs_5_0",                                       //�V�F�[�_�[���f���̎w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�w��
		0,
		&vsBlob,
		&errorBlob
	);

	//�s�N�Z���V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicPixelShader.hlsl",                      //�V�F�[�_�[���w��
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,              //�C���N���[�h���\�ɂ���
		"PSmain",                                       //�G���g���[�|�C���g�̎w��
		"ps_5_0",                                       //�V�F�[�_�[���f���̎w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�w��
		0,
		&psBlob,
		&errorBlob
	);

	//���V�F�[�_�[�̓ǂݍ��݂Ɏ��s�����ꍇ�ɓ��e��\������
	if (FAILED(result))
	{
		//errorBlob�̒�����A�G���[�̓��e��string�^�ɃR�s�[����
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";

		//�G���[�̓��e���o�̓E�B���h�E�ɕ\������
		OutputDebugStringA(error.c_str());
		exit(1);
	}

#pragma endregion

	return S_OK;
}

void Create2DFigure::InitGraphicsPipeLine()
{

#pragma region ���_���C�A�E�g�ݒ�

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

#pragma region �O���t�B�N�X�p�C�v���C���̍쐬

	//�O���t�B�N�X�p�C�v���C���̐ݒ�\����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//�V�F�[�_�[�̃Z�b�g--------------------------------------
	//���_�V�F�[�_�[���p�C�v���C���ɐݒ�
	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
	//�s�N�Z���V�F�[�_�[���p�C�v���C���ɐݒ�
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();
	//--------------------------------------------------------

	//�T���v���}�X�N�ƃ��X�^���C�Y�X�e�[�g�̐ݒ�--------------
	//�f�t�H���g�̐ݒ�
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//�w�ʃJ�����O���s��Ȃ�
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//�|���S������h��Ԃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//�[�x�N���b�s���O��L���ɂ���
	gpipeline.RasterizerState.DepthClipEnable = true;
	//--------------------------------------------------------

	//�u�����h�X�e�[�g�̐ݒ�----------------------------------
	//RGBA �S�Ẵ`�����l����`��
	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	//--------------------------------------------------------

	//���_���C�A�E�g�̐ݒ�------------------------------------
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	//--------------------------------------------------------

	//�}�`�̌`��̐ݒ�
	gpipeline.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//�`��Ώۂ̐�
	gpipeline.NumRenderTargets = 1;

	//0�`1�ɐ��K�����ꂽRGBA
	gpipeline.RTVFormats[0] =
		DXGI_FORMAT_R8G8B8A8_UNORM;

	//1�s�N�Z���ɂ�1��̃T���v�����O
	gpipeline.SampleDesc.Count = 1;


#pragma region ���[�g�V�O�l�`���̐���

	//���[�g�V�O�l�`���̐ݒ�\����
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};

	//���_���̑��݂�`����
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//�I�u�W�F�N�g���m�ۂ���
	ID3DBlob* rootSigBlob = nullptr;

	//�o�C�i���R�[�h(�\�[�X�R�[�h���@�B��ɕϊ��������)�̍쐬
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,            //���[�g�V�O�l�`���̐ݒ�
		D3D_ROOT_SIGNATURE_VERSION_1_0,//���[�g�V�O�l�`���̃o�[�W����
		&rootSigBlob, 				   //�V�F�[�_�[�̎��Ɠ���
		&errorBlob);				   //�G���[�̎��͂����Ƀ��b�Z�[�W������

	//���[�g�V�O�l�`���̐���
	result = _dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature)
	);

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = rootsignature;

#pragma endregion

	//�O���t�B�N�X�p�C�v���C���̐���
	result = _dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&pipelinestate)
	);

#pragma endregion
}

void Create2DFigure::Draw2D()
{
#pragma region �`��R�}���h

	//�p�C�v���C���X�e�[�g�̐ݒ�
	_cmdList->SetPipelineState(pipelinestate);

	//�r���[�|�[�g�̐ݒ�----------------------------------
	D3D12_VIEWPORT viewport{};
	viewport.Width = window_width;
	viewport.Height = window_height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	_cmdList->RSSetViewports(1, &viewport);
	//----------------------------------------------------

	//�V�U�[��`�̐ݒ�------------------------------------
	D3D12_RECT scissorret{};
	scissorret.left = 0;
	scissorret.right = scissorret.left + window_width;
	scissorret.top = 0;
	scissorret.bottom = scissorret.top + window_height;
	_cmdList->RSSetScissorRects(1, &scissorret);
	//----------------------------------------------------

	//���[�g�V�O�l�`���̐ݒ�
	_cmdList->SetGraphicsRootSignature(rootsignature);

	//�v���~�e�B�u�`��̐ݒ�
	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���_�o�b�t�@�̐ݒ�
	_cmdList->IASetVertexBuffers(0, 1, &vbview);

	//�`��
	_cmdList->DrawInstanced(3, 1, 0, 0);

#pragma endregion
}
