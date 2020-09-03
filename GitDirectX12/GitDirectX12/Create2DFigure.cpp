#include "Create2DFigure.h"

#define PI 3.14159265359

using namespace std;

//���_��
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
	//��{�I�Ƀp�C�v���C���͍ŏ��ɍ��!
	InitGraphicsPipeLine();
	CreateVertBuff();
	CreateConstBuff();
	CreateIndexBuff();
}

void Create2DFigure::CreateVertBuff()
{

	//���_�f�[�^
	XMFLOAT3 vertices[a + 1];

	//n�p�`�̕\��
	for (int i = 0; i < a; i++)
	{
		vertices[i].x = 0.5f * sinf(XM_2PI / a * i) *
			window_height / window_width;
		vertices[i].y = 0.5f * cosf(XM_2PI / a * i);
		vertices[i].z = 0.0f;
	}
	vertices[a] = { 0,0,0 };

#pragma region ���_�o�b�t�@�̊m��

	//GPU�ւ̃f�[�^�]���p
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

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

void Create2DFigure::CreateConstBuff()
{
#pragma region �萔�o�b�t�@�̃f�B�X�N���v�^�q�[�v���쐬

	//�ݒ�\����
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};

	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//�V�F�[�_�[���猩����悤�ɂ���
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//�o�b�t�@�[�̐���1
	descHeapDesc.NumDescriptors = 1;

	//����
	result = _dev->CreateDescriptorHeap(
		&descHeapDesc,
		IID_PPV_ARGS(&basicDescHeap)
	);

#pragma endregion

#pragma region �萔�o�b�t�@�̍쐬

	//�q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES cbHeapprop{};

	//GPU�ւ̓]���p
	cbHeapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC cbresDesc{};

	cbresDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//256�o�C�g�A���C�������g
	cbresDesc.Width = (sizeof(ConstBufferData) + 0xff)& ~0xff;
	cbresDesc.Height = 1;
	cbresDesc.DepthOrArraySize = 1;
	cbresDesc.MipLevels = 1;
	cbresDesc.SampleDesc.Count = 1;
	cbresDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPU���\�[�X�̐���
	ID3D12Resource* constBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&cbHeapprop,                      //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbresDesc,                       //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);

#pragma endregion

#pragma region �萔�o�b�t�@�̃f�[�^�]��(cpp���ł̐F�ύX)

	//�}�b�s���O(�R�s�[)
	result = constBuff->Map(0, nullptr, (void**)&cbData);

	//�����瑤�ŐF���w��
	cbData->color = XMFLOAT4(0, 1, 0, 0.2f);

	//�}�b�v����
	constBuff->Unmap(0, nullptr);

#pragma endregion

#pragma region �萔�o�b�t�@�[�r���[�̐���

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};

	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;

	//����
	_dev->CreateConstantBufferView(
		&cbvDesc,
		basicDescHeap->GetCPUDescriptorHandleForHeapStart()
	);

	/*�萔�o�b�t�@�[�r���[��Create���Ȃ��Ƃ����Ȃ�*/

#pragma endregion

}

void Create2DFigure::CreateIndexBuff()
{
	unsigned short indices[a * 3];

	//n�p�`�̕\��
	for (int i = 0; i < a; i++)
	{
		indices[i * 3 + 0] = i;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = a;
	}
	indices[_countof(indices) - 1] = 0;

#pragma region �C���f�N�X�o�b�t�@�̊m��

	ID3D12Resource* indexBuff = nullptr;

	//�C���f�b�N�X�̏������镪�̃T�C�Y�̃��������m��
	resdesc.Width = sizeof(indices);

	//GPU���\�[�X�̐���
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

#pragma endregion

#pragma region �C���f�N�X�o�b�t�@�̃f�[�^�]��

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	//�S�ẴC���f�b�N�X�ɑ΂���
	for (int i = 0; i < _countof(indices); i++)
	{
		//�R�s�[���s��
		indexMap[i] = indices[i];
	}

	//�I��
	indexBuff->Unmap(0, nullptr);

#pragma endregion

#pragma region �C���f�b�N�X�o�b�t�@�[�r���[�̍쐬

	//�o�b�t�@�[�̉��z�A�h���X�̎擾
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	//��ʂ�ݒ�
	ibView.Format = DXGI_FORMAT_R16_UINT;
	//�S���ŉ��o�C�g���𒲂ׂ�
	ibView.SizeInBytes = sizeof(indices);

	//short��16�r�b�g�Ȃ̂�R16_UINT�ɂȂ���

#pragma endregion

}

void Create2DFigure::CreateFigure(float Radius)
{
	//���_�z��̐錾
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

	//n�p�`�̕\��
	for (int i = 0; i < a; i++)
	{
		vert[i].x = Radius * sinf(XM_2PI / a * i) * 
			window_height /  window_width;
		vert[i].y = Radius * cosf(XM_2PI / a * i);
		vert[i].z = 0.0f;
	}

	//�C���f�b�N�X�z��
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

	//n�p�`�̕\��
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
	//���C���[�t���[���\�����s��
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//�[�x�N���b�s���O��L���ɂ���
	gpipeline.RasterizerState.DepthClipEnable = true;

	//���X�^���C�U�Ƃ́A���_�̃s�N�Z����
	//--------------------------------------------------------

	//�f�B�X�N���v�^�e�[�u���̏�����--------------------------
	//�f�B�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descTblRange{};

	//�萔�͈��
	descTblRange.NumDescriptors = 1;
	//��ʂ͒萔���w��(ConstBufferView)
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//0�Ԃ���J�n
	descTblRange.BaseShaderRegister = 0;
	//�W���ݒ�
	descTblRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootparam = {};

	//��ʂ̐ݒ�(DiscriptorTable)
	rootparam.ParameterType = 
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

	//�f�B�X�N���v�^�����W���w��
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;

	//�f�B�X�N���v�^�����W�̐�
	rootparam.DescriptorTable.NumDescriptorRanges = 1;

	//�S�ẴV�F�[�_�[���猩����悤�ɂ���
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//--------------------------------------------------------

	//�u�����h�X�e�[�g�̐ݒ�----------------------------------
	////RGBA �S�Ẵ`�����l����`��
	//gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask =
	//	D3D12_COLOR_WRITE_ENABLE_ALL;

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�(�S����8����炵��)
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
	//�W���ݒ�
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//���ʂ̐ݒ�(�ǂ̃u�����h���g���ɂ������Ă��K�{)
	blendDesc.BlendEnable = true;//�u�����h������L���ɂ���
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//���Z����
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;  //�\�[�X�̒l��100%�g��
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;//�f�X�g�̒l��0%�g��

#pragma region �u�����h�̂��낢��
	//���Z����
	//blendDesc.BlendOp = D3D12_BLEND_OP_ADD;//���Z
	//blendDesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l��100%�g��
	//blendDesc.DestBlend = D3D12_BLEND_ONE;//�f�X�g�̒l��100%�g��
	//�\�[�X(�}�`�̐F)�ƃf�X�g(�w�i�̐F)��100%�̒l�����Z����
	//�f�X�g     �\�[�X    ����
	//(0,0,1) + (1,0,0) = (1,0,1)

	//���Z����
	//blendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//�f�X�g����\�[�X�����Z
	//blendDesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l��100%�g��
	//blendDesc.DestBlend = D3D12_BLEND_ONE;//�f�X�g�̒l��100%�g��
	//�f�X�g(�w�i�̐F)��100%����\�[�X(�}�`�̐F)��100%�̒l�����Z����
	//�f�X�g     �\�[�X    ����
	//(1,1,1) - (1,0,0) = (0,1,1)

	////�F���]
	//blendDesc.BlendOp = D3D12_BLEND_OP_ADD;         //���Z
	//blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f - �f�X�g�̃J���[�̒l
	//blendDesc.DestBlend = D3D12_BLEND_ZERO;         //�g�p���Ȃ�
	////�f�X�g�̃J���[�𔽓]�����l(1����������l)���g�p����
	////�\�[�X     �f�X�g    ����
	////(1,1,1) - (1,0,0) = (0,1,1)�ɂȂ�炵��

	//����������(�A���t�@�u�����f�B���O)
	blendDesc.BlendOp = D3D12_BLEND_OP_ADD;         //���Z
	blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;     //�\�[�X�̃��l
	blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f - �\�[�X�̃��l
	//�\�[�X�̃��l�̊����ɉ����āA�\�[�X�ƃf�X�g�̃J���[��������
	//        �f�X�g      �\�[�X          ����
	//��) (0,0,1)*0.2 + (1,1,1)*0.8 = (0.8,0.8,1)
#pragma endregion

	//�u�����h�X�e�[�g�̓o�^
	gpipeline.BlendState.RenderTarget[0] = blendDesc;
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

	//�f�B�X�N���v�^�e�[�u���̏���ǉ�
	rootSignatureDesc.Flags = 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.pParameters = &rootparam;
	//���[�g�p�����[�^�̐�
	rootSignatureDesc.NumParameters = 1;

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
	viewport.Width = window_width;  //����
	viewport.Height = window_height;//�c��
	viewport.TopLeftX = 0;          //����X
	viewport.TopLeftY = 0;          //����Y
	viewport.MinDepth = 0.0f;       //�ŏ��[�x(0)
	viewport.MaxDepth = 1.0f;       //�ő�[�x(1)
	//�R�}���h���X�g�ɒǉ�����
	_cmdList->RSSetViewports(1, &viewport);

	//�r���[�|�[�g�́A�E�B���h�E�̒��ł̕`��͈�
	//----------------------------------------------------

	//�V�U�[��`�̐ݒ�------------------------------------
	D3D12_RECT scissorret{};
	scissorret.left = 0;               //�؂蔲�����W��
	scissorret.right = 
		scissorret.left + window_width;//�؂蔲�����W�E
	scissorret.top = 0;                //�؂蔲�����W��
	scissorret.bottom = 
		scissorret.top + window_height;//�؂蔲�����W��
	//�R�}���h���X�g�ɒǉ�����
	_cmdList->RSSetScissorRects(1, &scissorret);

	//�V�U�[��`�́A�r���[�|�[�g�̒��ł̕`��͈�
	//----------------------------------------------------

	//���[�g�V�O�l�`���̐ݒ�
	_cmdList->SetGraphicsRootSignature(rootsignature);

	//�f�B�X�N���v�^�q�[�v�̐ݒ�------------------------------
	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap };
	_cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//�擪�A�h���X���擾
	_cmdList->SetGraphicsRootDescriptorTable(0,
		basicDescHeap->GetGPUDescriptorHandleForHeapStart());
	//--------------------------------------------------------


#pragma region �v���~�e�B�u�`��̈ꗗ

	////�_�̃��X�g
	//_cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	////���̃��X�g
	//_cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	////���̃X�g���b�v
	//_cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	////�O�p�`�̃��X�g
	//_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	////�O�p�`�̃X�g���b�v
	//_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//STRIP�̓f�[�^�̌������������A�������Ă�B
	//LIST �̓f�[�^�̌����͈������A�Ɨ����Ă���B

#pragma endregion
	//�v���~�e�B�u�`��̐ݒ�
	_cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���_�o�b�t�@�̐ݒ�
	_cmdList->IASetVertexBuffers(0, 1, &vbview);

	//�C���f�b�N�X�o�b�t�@�̃Z�b�g
	_cmdList->IASetIndexBuffer(&ibView);

	//�`��
	//(���_���E�C���X�^���X���E�J�n�ԍ�)
	//_cmdList->DrawInstanced(6, 1, 0, 0);

	//(�C���f�b�N�X�̐��E1�E0�E0�E0)
	_cmdList->DrawIndexedInstanced(a * 3, 1, 0, 0, 0);

#pragma endregion
}
