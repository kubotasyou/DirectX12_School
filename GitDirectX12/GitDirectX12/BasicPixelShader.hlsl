//ピクセルシェーダー

cbuffer cbuff0 : register(b0)
{
	float4 color;//
};

float4 PSmain(float4 pos : SV_POSITION) : SV_TARGET
{
	return color;
}