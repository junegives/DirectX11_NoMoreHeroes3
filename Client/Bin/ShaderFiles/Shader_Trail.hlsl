
#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;
Texture2D	g_DiffuseTexture;
Texture2D	g_AlphaTexture;

float4		g_vColor;
float2		g_fUVFlow;
int			g_iCutUV;

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
	Out.vTexcoord = float2(
		In.vTexcoord.x + g_fUVFlow.x,
		In.vTexcoord.y + g_fUVFlow.y);

	return Out;
}

/* 정점 세개(TriangleList)가 모이게 되면. */
/* w나누기연산을 수행한다.(원근투영) */
/* 뷰포트변환.(윈도우좌표로 변환한다) */
/* 래스터라이즈.( 픽셀을 생성한다. )*/

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

	if (0 < g_iCutUV)
	{
		if (In.vTexcoord.x > 1.f && In.vTexcoord.y > 1.f)
			discard;
	}

	Out.vColor = vector(g_vColor);

	if (0 == Out.vColor.a)
		discard;

	return Out;
}

PS_OUT PS_ALPHA_TEXTURE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_iCutUV)
	{
		if (In.vTexcoord.x > 1.f && In.vTexcoord.y > 1.f)
			discard;
	}

	vector vTextureAlpha = g_AlphaTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (vTextureAlpha.r <= 0.001f && vTextureAlpha.g <= 0.001f && vTextureAlpha.b <= 0.001f)
		discard;

	Out.vColor = g_vColor;
	/*Out.vColor.a = vTextureAlpha.r;
	if (0 == Out.vColor.a)
		discard;*/

	return Out;
};


/* EffectFramework */

technique11	DefaultTechnique
{
	pass DefaultPass
	{
		/* RenderState설정. */
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass AlphaPass
	{
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHA_TEXTURE();
	}
}


