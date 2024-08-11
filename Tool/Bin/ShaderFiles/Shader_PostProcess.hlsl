#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix		g_ViewMatrixInv, g_ProjMatrixInv;

float		g_fTexW = 1280.0f;
float		g_fTexH = 720.0f;
float		g_fGlowPower = 2.2f;

static const float fWeight[13] = {
	0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1,
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

float g_fDistortionStrength = 1.f;

Texture2D	g_Texture;
Texture2D	g_BackBufferTexture;
Texture2D	g_BlendTexture;
Texture2D	g_EffectTexture;
Texture2D	g_BlurTexture;
Texture2D	g_DistortionTexture;

vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector		g_vCamPosition;

Texture2D	g_NormalTexture;
Texture2D	g_DiffuseTexture;
Texture2D	g_ShadeTexture;
Texture2D	g_DepthTexture;
Texture2D	g_SpecularTexture;

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


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4		vBackBufferCopy = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);
	
	float4		vBlendTexture = g_BlendTexture.Sample(g_LinearSampler, In.vTexcoord);

	Out.vColor = pow(pow(abs(vBackBufferCopy), g_fGlowPower) + pow(abs(vBlendTexture), g_fGlowPower), 1.f / g_fGlowPower);

	return Out;
}

float4	Blur_X(float2 vTexcoord)
{
	float4		vOut = (float4)0;

	float2		vUV = (float2)0;
	float		fTotal = 0.f;

	for (int i = -6; i < 7; ++i)
	{
		vUV = vTexcoord + float2(1.f / g_fTexW * i, 0);
		vOut += fWeight[6 + i] * g_BlurTexture.Sample(g_LinearSampler, vUV);
		fTotal += fWeight[6 + i];
	}

	vOut /= fTotal;

	return vOut;
}

float4	Blur_Y(float2 vTexcoord)
{
	float4		vOut = (float4)0;

	float2		vUV = (float2)0;
	float		fTotal = 0.f;

	for (int i = -6; i < 7; ++i)
	{
		vUV = vTexcoord + float2(0, 1.f / (g_fTexH / 2.f) * i);
		//vUV = vTexcoord + float2(0, 1.f / (g_fTexH * i));
		vOut += fWeight[6 + i] * g_BlurTexture.Sample(g_LinearSampler, vUV);
		fTotal += fWeight[6 + i];
	}

	vOut /= fTotal;

	return vOut;
}

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector vBlurTexture = g_BlurTexture.Sample(g_ClampSampler, In.vTexcoord);
	Out.vColor = Blur_X(In.vTexcoord);

	return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = Blur_Y(In.vTexcoord);

	return Out;
}

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4		vDistortionTexture = g_DistortionTexture.Sample(g_LinearSampler, In.vTexcoord);

	// 디스토션이 적용되는 픽셀
	if (vDistortionTexture.a > 0.f)
	{
		float2		vNoisedUVs = In.vTexcoord + vDistortionTexture.r;

		In.vTexcoord = lerp(In.vTexcoord, vNoisedUVs, g_fDistortionStrength / 100);

		Out.vColor = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);
	}

	else
		Out.vColor = g_BackBufferTexture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
}

technique11	DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Blur_X
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
	}
	pass Blur_Y
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
	}
	pass Distortion
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}
}
