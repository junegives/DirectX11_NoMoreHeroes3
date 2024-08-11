#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix		g_ViewMatrixInv, g_ProjMatrixInv;

float		g_fTexW = 1280.0f;
float		g_fTexH = 720.0f;

static const float fTotal = 6.2108;

static const float fWeight[13] = {
	0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1,
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

Texture2D	g_Texture;
Texture2D	g_EffectTexture;
Texture2D	g_BlurTexture;

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
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
	vector vShade : SV_TARGET0;
	vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(g_PointSampler, In.vTexcoord);
	vector		vDepthDesc = g_DepthTexture.Sample(g_PointSampler, In.vTexcoord);

	/* 0 ~ 1 -> -1 ~ 1 */
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse *
		saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) +
			(g_vLightAmbient * g_vMtrlAmbient));

	vector		vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));

	/* vWorldPos : 현재 렌더링 상태는 디퍼드로 수행한다. 디퍼드 화면에 대한 셰이딩 */
	/* Pixel셰이더에 입력된 픽셀이 어떤 객체의 픽셀인지 알 방법 없다. */
	vector		vWorldPos;

	/* 투영스페이스 상의 위치다. */
	/* 0 -> -1 */
	/* 1 -> 1 */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;

	/* 0 -> 1 */
	/* 1 -> -1 */
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;

	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;


	float		fViewZ = vDepthDesc.y * 1000.f;
	/* 투영행렬까지만 곱한 위치. */
	vWorldPos = vWorldPos * fViewZ;

	/* 뷰스페이스 상의 위치를 구한다. */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구한다. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f);

	return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);
	if (0.0f == vDiffuse.a)
		discard;

	vector		vShade = g_ShadeTexture.Sample(g_LinearSampler, In.vTexcoord);
	vector		vSpecular = g_SpecularTexture.Sample(g_LinearSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade + vSpecular;
	//Out.vColor = vDiffuse * vShade;

	return Out;
}

float4	Blur_X(float2 vTexcoord)
{
	float4		vOut = (float4)0;

	float2		vUV = (float2)0;

	for (int i = -6; i < 7; ++i)
	{
		vUV = vTexcoord + float2(1.f / g_fTexW * i, 0);
		vOut += fWeight[6 + i] * g_EffectTexture.Sample(g_ClampSampler, vUV);
	}

	vOut /= fTotal;

	return vOut;
}

float4	Blur_Y(float2 vTexcoord)
{
	float4		vOut = (float4)0;

	float2		vUV = (float2)0;

	for (int i = -6; i < 7; ++i)
	{
		vUV = vTexcoord + float2(0, 1.f / (g_fTexH / 2.f) * i);
		vOut += fWeight[6 + i] * g_EffectTexture.Sample(g_ClampSampler, vUV);
	}

	vOut /= fTotal;

	return vOut;
}

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = Blur_X(In.vTexcoord);

	return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = Blur_Y(In.vTexcoord);

	return Out;
}

technique11	DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Deferred_Final
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
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
}
