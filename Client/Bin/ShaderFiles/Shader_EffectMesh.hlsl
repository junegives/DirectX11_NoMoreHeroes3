
#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;	// 월드, 뷰, 투영 변환 행렬
texture2D	g_DiffuseTexture;	// 2D 텍스처
texture2D	g_MaskTexture;

float		g_fAlpha = 1.f;

float		g_fUVx = 0.f;
float		g_fUVy = 0.f;

float2		g_vUVPivot = { 0.f, 0.f };
float4		g_vFrontColor;
float4		g_vBackColor;

float4		g_vColor;

float		g_fDistortionTimer;
float		g_fDistortionSpeed;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float3 vTangent : TANGENT;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;		// 변환된 정점의 좌표 (스크린)
	float2 vTexcoord	: TEXCOORD0;		// 변환된 텍스처 좌표
	float4 vNormal		: NORMAL;			// 정점의 노말 벡터
	float4 vWorldPos	: TEXCOORD1;		// 정점의 월드 좌표
	float4 vProjPos		: TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);	// World * View
	matWVP = mul(matWV, g_ProjMatrix);			// World * View * Projection

	Out.vPosition	= mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord	= In.vTexcoord;
	Out.vWorldPos	= mul(vector(In.vPosition, 1.f), g_WorldMatrix);	// 월드 공간 상의 좌표 구하기
	Out.vNormal		= normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));		// 월드 공간 상의 노말 벡터 구하기
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4 vPosition	: SV_POSITION;	// 정점 쉐이더에서 계산된 스크린 좌표
	float2 vTexcoord	: TEXCOORD0;	// 정점 쉐이더에서 계산된 텍스처 좌표
	float4 vNormal		: NORMAL;		// 픽셀의 노말 벡터
	float4 vWorldPos	: TEXCOORD1;	// 픽셀의 월드 좌표
	float4 vProjPos		: TEXCOORD2;
};

struct PS_OUT
{
	vector vDiffuse	: SV_TARGET0;	// 화면에 그려질 픽셀의 색상 속성
	vector vNormal	: SV_TARGET1;
	vector vDepth	: SV_TARGET2;
};

struct PS_DISTORTION_OUT
{
	vector vColor : SV_TARGET0;
};

/*
정점 셰이더에서 연산하고 선형 보간을 한 것과 픽셀 셰이더에서 연산한 것과 크게 차이가 없기 때문에 정점 연산을 하는게 효율적임.
Specular나 Normal을 디자이너가 제공해주는 경우(텍스처가 존재하는 경우)에는 픽셀 셰이더에서 연산해야함.

퐁(phong) 셰이드 : 픽셀 단에서 연산
그라우드(Gouraud) 셰이드 : 정점 단에서 연산
*/

// 방향성 광원 픽셀 셰이더
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// 물체의 Diffuse (물체의 색상 표현)
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse = vMtrlDiffuse;
	// 노말 벡터를 (-1, 1) 범위에서 (0, 1) 범위로 변환
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}


PS_OUT PS_MAIN_COLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// 노말 벡터를 (-1, 1) 범위에서 (0, 1) 범위로 변환
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	Out.vDiffuse = g_vColor;

	if (Out.vDiffuse.a < 0.2f)
		discard;

	return Out;
}


PS_OUT PS_LEOPARDONRING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// 노말 벡터를 (-1, 1) 범위에서 (0, 1) 범위로 변환
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	In.vTexcoord.x += g_fUVx;
	In.vTexcoord.y += g_fUVy;

	vector		vMtrlMask = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (vMtrlMask.r < 0.1)
		Out.vDiffuse = vector(0.741, 0.878, 1.f, 1.f);

	else if (vMtrlMask.r > 0.15)
		Out.vDiffuse = vector(1.f, 0.52f, 0.76f, 1.f);

	else
		Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);

	Out.vDiffuse.a = g_fAlpha;

	return Out;
}

PS_OUT PS_TRIGGERPILAR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// 노말 벡터를 (-1, 1) 범위에서 (0, 1) 범위로 변환
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	In.vTexcoord.x += g_vUVPivot.x;
	In.vTexcoord.y += g_vUVPivot.y;

	vector		vMtrlMask = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
	if (vMtrlMask.r <= 0.1f)
		discard;

	Out.vDiffuse.rgb = g_vBackColor.rgb * (1 - vMtrlMask.r) + g_vFrontColor.rgb * vMtrlMask.r;
	Out.vDiffuse.a = vMtrlMask.a * g_fAlpha;

	return Out;
}

PS_DISTORTION_OUT PS_DISTORTION(PS_IN In)
{
	PS_DISTORTION_OUT		Out = (PS_DISTORTION_OUT)0;

	vector		vMaskTexture = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord + g_fDistortionTimer * g_fDistortionSpeed);

	Out.vColor = vMaskTexture;
	Out.vColor.a = 1.f;

	return Out;
}

technique11	DefaultTechnique	// 그래픽스 기법 정의
{
	// 패스 정의
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}

	pass Color
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLOR();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}

	pass Trail
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLOR();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}

	pass LeopardonRing
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LEOPARDONRING();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}

	pass TriggerPillar
	{
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TRIGGERPILAR();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}

	pass DistortionMesh
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}
}