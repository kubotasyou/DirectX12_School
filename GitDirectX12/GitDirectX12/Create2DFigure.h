#pragma once

#include <vector>
#include <d3d12.h>
//���w���C�u�����̃C���N���[�h
#include <DirectXMath.h>
using namespace DirectX;
//D3D�R���p�C���̃C���N���[�h
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")

//�萔�o�b�t�@�[�p�f�[�^�\����
struct ConstBufferData
{
	XMFLOAT4 color;//�F(RGBA)
};

class Create2DFigure
{
public:
	//�R���X�g���N�^
	Create2DFigure(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	//�f�X�g���N�^
	~Create2DFigure();

	//������
	void Initialize();

	//���_�o�b�t�@�̍쐬
	void CreateVertBuff();

	//�萔�o�b�t�@�̍쐬
	void CreateConstBuff();

	//�C���f�b�N�X�o�b�t�@�̍쐬
	void CreateIndexBuff();

	//���p�`�����
	void CreateFigure(float Radius);

	//�V�F�[�_�[�t�@�C���̓ǂݍ���
	HRESULT ReadShaderFile();

	//�O���t�B�b�N�p�C�v���C���̏�����
	void InitGraphicsPipeLine();

	//�`��R�}���h
	void Draw2D();

private:

	HRESULT result;
	ID3D12Device* _dev = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;
	ID3DBlob* vsBlob = nullptr;   //���_�V�F�[�_�[�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr;   //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr;//�G���[�I�u�W�F�N�g
	//�O���t�B�N�X�p�C�v���C���̐���
	ID3D12PipelineState* pipelinestate = nullptr;
	//���[�g�V�O�l�`���̐錾
	ID3D12RootSignature* rootsignature;
	//���_�o�b�t�@�[�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbview{};
	//���_�E�C���f�b�N�X�̃��\�[�X�̐ݒ�\����
	D3D12_RESOURCE_DESC resdesc{};
	//���_�E�C���f�b�N�X�̃q�[�v�ݒ�\����
	D3D12_HEAP_PROPERTIES heapprop{};//HeapProperties
	//�C���f�b�N�X�o�b�t�@�[�r���[�̍쐬
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//�萔�o�b�t�@�ɑ���f�[�^
	ConstBufferData* cbData = nullptr;
	//�萔�o�b�t�@�[�p�̃f�B�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* basicDescHeap = nullptr;


	//�E�B���h�E�T�C�Y
	const int window_width = 1280;
	const int window_height = 720;
};

