
#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture;

float			g_fAlpha = 1.f;
vector			g_ColorBack = vector(1.f, 1.f, 1.f, 1.f);
vector			g_ColorFront = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vCamPosition;
float			g_fLifeTime = 1.f;
float			g_fDuration = 1.f;

float2			g_vUVPivot = { 0.f, 0.f };
float2			g_vUVScale = { 1.f, 1.f };

float4			g_vColor = { 1.f, 1.f, 1.f, 1.f };

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

	Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (0.9 > Out.vColor.r)
		discard;

	vector vMaskColor = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);

	// �����Ǵ� �ð�
	if (g_fLifeTime > g_fDuration - 0.1f)
	{
		if (vMaskColor.r > (g_fDuration - g_fLifeTime) * 10.f)
			discard;
	}

	// ������� �ð�
	else if (g_fLifeTime < 0.1f)
	{
		if (vMaskColor.r < (0.1f - g_fLifeTime) * 10.f)
			discard;
	}

	Out.vColor.a = Out.vColor.r;

	if (Out.vColor.a <= 0.3)
		discard;

	Out.vColor.gb = Out.vColor.r;

	return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
	
	if (Out.vColor.r < 0.1f)
		discard;

	Out.vColor.rgb = g_ColorBack.rgb * (1 - Out.vColor.r) + g_ColorFront.rgb * Out.vColor.r;

	/*if (In.vTexcoord.x < 0.166f) {
		Out.vColor.r = 1.0f;
		Out.vColor.g = In.vTexcoord.x / 0.166f;
		Out.vColor.b = 0.0f;
	}
	else if (In.vTexcoord.x < 0.333f) {
		Out.vColor.r = 1.0f - (In.vTexcoord.x - 0.166f) / 0.166f;
		Out.vColor.g = 1.0f;
		Out.vColor.b = 0.0f;
	}
	else if (In.vTexcoord.x < 0.5f) {
		Out.vColor.r = 0.0f;
		Out.vColor.g = 1.0f;
		Out.vColor.b = (In.vTexcoord.x - 0.333f) / 0.166f;
	}
	else if (In.vTexcoord.x < 0.666f) {
		Out.vColor.r = 0.0f;
		Out.vColor.g = 1.0f - (In.vTexcoord.x - 0.5f) / 0.166f;
		Out.vColor.b = 1.0f;
	}
	else if (In.vTexcoord.x < 0.833f) {
		Out.vColor.r = (In.vTexcoord.x - 0.666f) / 0.166f;
		Out.vColor.g = 0.0f;
		Out.vColor.b = 1.0f;
	}
	else {
		Out.vColor.r = 1.0f;
		Out.vColor.g = 0.0f;
		Out.vColor.b = 1.0f - (In.vTexcoord.x - 0.833f) / 0.166f;
	}*/

	//Out.vColor.a = In.vTexcoord.x;

	Out.vColor.a *= g_fAlpha;
	if (Out.vColor.a <= 0.0f)
		discard;

	return Out;
}

PS_OUT PS_UV_PIVOT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	In.vTexcoord = In.vTexcoord * g_vUVScale + g_vUVPivot;

	Out.vColor = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (Out.vColor.r < 0.5)
		discard;

	Out.vColor *= g_vColor;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

//PS_OUTPUT_SINGLE FX_PaperBurn(VS_OUTPUT_TEX input)
//{
//	PS_OUTPUT_SINGLE	output = (PS_OUTPUT_SINGLE)0;
//	//g_EffectTex1
//	float4	vColor = (float4)0.f;
//
//	if (g_iAnimation2DEnable == 0)
//	{
//		vColor = g_DiffuseTex.Sample(g_PointSmp, input.vUV);
//	}
//	else
//	{
//		if (g_iAnim2DType == AT_ATLAS)
//			vColor = g_DiffuseTex.Sample(g_PointSmp, input.vUV);
//		else
//			vColor = g_ArrayTex.Sample(g_PointSmp, float3(input.vUV, g_iAnim2DFrame));
//	}
//
//	float4 vFX_tex = g_EffectTex1.Sample(g_LinearSmp, input.vUV);
//
//
//	if (vColor.a == 0.f)
//		clip(-1);
//
//	if (vFX_tex.r >= g_vMtrlDif.a)
//		vColor.a = 1;
//	else
//		vColor.a = 0;
//
//	if (vFX_tex.r >= g_vMtrlDif.a - 0.05 && vFX_tex.r <= g_vMtrlDif.a + 0.05)
//		vColor = float4(1, 0, 0, 1); // ��
//	else
//		;
//
//	if (vFX_tex.r >= g_vMtrlDif.a - 0.03 && vFX_tex.r <= g_vMtrlDif.a + 0.03)
//		vColor = float4(1, 1, 0, 1); // ��
//	else
//		;
//
//	if (vFX_tex.r >= g_vMtrlDif.a - 0.025 && vFX_tex.r <= g_vMtrlDif.a + 0.025)
//		vColor = float4(1, 1, 1, 1); // ��
//	else
//		;
//
//	output.vColor = vColor;
//	return output;
//}
//
//PS_OUTPUT_SINGLE FX_Mask(VS_OUTPUT_TEX input)
//{
//	PS_OUTPUT_SINGLE output = (PS_OUTPUT_SINGLE)0;
//	float4 vFX_tex = g_EffectTex1.Sample(g_LinearSmp, input.vUV);
//
//	float4	vColor = (float4)0.f;
//
//	if (g_iAnimation2DEnable == 0)
//	{
//		vColor = g_DiffuseTex.Sample(g_PointSmp, input.vUV);
//	}
//	else
//	{
//		if (g_iAnim2DType == AT_ATLAS)
//			vColor = g_DiffuseTex.Sample(g_PointSmp, input.vUV);
//		else
//			vColor = g_ArrayTex.Sample(g_PointSmp, float3(input.vUV, g_iAnim2DFrame));
//	}
//
//	vColor.a = vFX_tex.g;
//	output.vColor = vColor;
//	return output;
//
//}
//
//PS_OUTPUT_SINGLE FX_Distortion(VS_OUTPUT_TEX input)
//{
//	PS_OUTPUT_SINGLE output = (PS_OUTPUT_SINGLE)0;
//
//	// ������ �ؽ�ó�� r,g,b,a
//	float4 vFX_tex = g_EffectTex1.Sample(g_LinearSmp, input.vUV);
//	// ��ǻ�� �ؽ�ó(������� ���� �ؽ�ó)
//	float4	vColor = (float4)0.f;
//	// ����ġ (������ �ؽ�ó�� r�� (�׷��� ������)�� ��)
//	float fWeight = vFX_tex.r * 0.5f;
//
//	if (g_iAnimation2DEnable == 0)
//	{
//		// ��ǻ�� �ؽ�ó�� UV�� + ����ġ�� ������ ��ǥ�� �ȼ� ���� ����
//		vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV + fWeight);
//	}
//	else
//	{
//		if (g_iAnim2DType == AT_ATLAS)
//			vColor = g_DiffuseTex.Sample(g_LinearSmp, input.vUV + fWeight);
//		else
//			vColor = g_ArrayTex.Sample(g_LinearSmp, float3(input.vUV + fWeight, g_iAnim2DFrame));
//	}
//
//	output.vColor = vColor;
//	return output;
//}

/* EffectFramework */

technique11	DefaultTechnique
{
	pass DefaultPass
	{
		/* RenderState����. */
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass DefaultTrail
	{
		/* RenderState����. */
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TRAIL();
	}

	pass UVTexture
	{
		/* RenderState����. */
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UV_PIVOT();
	}
}
