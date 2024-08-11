
#include "Shader_Defines.hlsli"

//float2, float3, float4 == vector
//float3x3, float4x4 == matrix, float1x4



/* �������� : ������Ʈ ���̺�*/
/* ���� ä��� : �ʱ�ȭ, Ŭ���̾�Ʈ���� ���̴� ���������� ���� �����Ѵ�. */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;

//float		g_fData = 10;
//vector		g_vData = vector(1.f, 1.f, 1.f, 1.f);
//vector		g_vData = (vector)0.0f;


/* Vertex Shader */
/* 1. ������ ��ȯ(���庯ȯ, �亯ȯ, ������ȯ) �� �����Ѵ�. */
/* 2. ������ ������ �����Ѵ�. */
/* 3. ������ ���޹ް� ����� ������ �����Ѵ�. */

// /* ����*/ VS_MAIN(float3 vPosition : POSITION, float2 vTexcoord : TEXCOORD0)
struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

/* ���� ����(TriangleList)�� ���̰� �Ǹ�. */
/* w�����⿬���� �����Ѵ�.(��������) */
/* ����Ʈ��ȯ.(��������ǥ�� ��ȯ�Ѵ�) */
/* �����Ͷ�����.( �ȼ��� �����Ѵ�. )*/

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

/* Pixel Shader */
/* 1. �ȼ��� ���� �����Ѵ�. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

	if (Out.vColor.a <= 0.3)
		discard;

	//Out.vColor.gb = Out.vColor.r;

	return Out;
}

/* EffectFramework */

technique11	DefaultTechnique
{
	pass DefaultPass
	{
		/* RenderState����. */
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}


