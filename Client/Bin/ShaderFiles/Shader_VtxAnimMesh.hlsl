
#include "Shader_Defines.hlsli"

/* 이 메시에게 영향을 주는 뼈들의 CombinedTransformationMatrix */
matrix		g_BoneMatrices[256];
// 각 메시가 이용하는 뼈의 개수가 256개를 넘어가면 터진다. (512개로 늘려보고 살짝식 줄여보기) (꼭 2의 n승일 필요는 없음)


matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;	// 월드, 뷰, 투영 변환 행렬
texture2D	g_DiffuseTexture;	// 2D 텍스처
texture2D	g_MaskTexture;
texture2D	g_OutlineMaskTexture;

float		g_fAlpha = 1.f;
float3		g_vColor = { 1.f, 1.f, 1.f };

float3		g_vColor0 = { 1.f, 1.f, 1.f };
float3		g_vColor1 = { 1.f, 1.f, 1.f };

float		g_fMaskAlpha0 = 0.f;
float		g_fMaskAlpha1 = 1.f;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float3 vTangent : TANGENT;

	uint4  vBlendIndices : BLENDINDEX;
	float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;		// 변환된 정점의 좌표 (스크린)
	float4 vNormal		: NORMAL;			// 정점의 노말 벡터
	float2 vTexcoord	: TEXCOORD0;		// 변환된 텍스처 좌표
	float4 vWorldPos	: TEXCOORD1;		// 정점의 월드 좌표
	float4 vProjPos		: TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);	// World * View
	matWVP = mul(matWV, g_ProjMatrix);			// World * View * Projection

	// 정점들의 스키닝 처리(뼈에 피부를 입힘, 정점들에 뼈의 상태를 곱해서 뼈를 따라가도록)
	// -> 셰이더에서 처리하였으므로 GPU에서 돌아감
	// -> 하드웨어 스키닝

	// g_BoneMatrices : 모든 뼈의 변환 행렬이 담긴 배열
	// In.vBlendIndices : 현재 정점이 영향을 받는 최대 4개의 뼈의 인덱스
	// In.VBlendWeights : 현재 정점에 대한 해당 뼈의 가중치

	// 본이 없는 경우 본 1개로 조작 In.vBlendWeights.w를 직접 조작해준다(x + y + z + w = 1임을 이용)
	// In.vBlendIndices.w는 0이여서 괜찮 (본 1개면 인덱스 0이니까)
	// 포지션에 영행렬이 곱해지지 않고 항등 행렬이 곱해지도록
	float	fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix	BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector	vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector	vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);	// 월드 공간 상의 좌표 구하기
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));		// 월드 공간 상의 노말 벡터 구하기
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4 vPosition	: SV_POSITION;	// 정점 쉐이더에서 계산된 스크린 좌표
	float4 vNormal		: NORMAL;		// 픽셀의 노말 벡터
	float2 vTexcoord	: TEXCOORD0;	// 정점 쉐이더에서 계산된 텍스처 좌표
	float4 vWorldPos	: TEXCOORD1;	// 픽셀의 월드 좌표
	float4 vProjPos		: TEXCOORD2;
};

struct PS_OUT
{
	vector vDiffuse : SV_TARGET0;
	vector vNormal : SV_TARGET1;
	vector vDepth : SV_TARGET2;
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

	/* -1.0 ~ 1.f -> 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_AFTER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = vector(g_vColor, g_fAlpha);

	/* -1.0 ~ 1.f -> 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_LEOPARDON_CUBE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlMask = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
	vector		vMtrlOutMask = g_OutlineMaskTexture.Sample(g_LinearSampler, In.vTexcoord);

	/*if (vMtrlOutMask.a >= 0.5f)
	{
		Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);
		Out.vDiffuse.a = vMtrlOutMask.a;
	}*/


		if (vMtrlMask.r <= g_fMaskAlpha0)
		{
			Out.vDiffuse = vector(g_vColor0, 1.f);

			if (vMtrlMask.r > g_fMaskAlpha0 - 0.1f)
			{
				Out.vDiffuse.a = (vMtrlMask.r - g_fMaskAlpha0) * 10.f;
			}
		}

		else if (vMtrlMask.r >= g_fMaskAlpha1)
		{
			Out.vDiffuse = vector(g_vColor1, 1.f);

			if (vMtrlMask.r < g_fMaskAlpha1 + 0.1f)
			{
				Out.vDiffuse.a = (g_fMaskAlpha1 - vMtrlMask.r) * 10.f;
			}
		}

		else
		{
			discard;
		}
	
	/* -1.0 ~ 1.f -> 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_LEOPARDON_CUBE_METAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlMask = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);
	vector		vMtrlOutMask = g_OutlineMaskTexture.Sample(g_LinearSampler, In.vTexcoord);

	/*if (vMtrlOutMask.a >= 0.5f)
	{
		Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);
		Out.vDiffuse.a = vMtrlOutMask.a;
	}*/


	if (vMtrlMask.r <= g_fMaskAlpha0)
	{
		Out.vDiffuse = vector(g_vColor0, 1.f);
	}

	else if (vMtrlMask.r >= g_fMaskAlpha1)
	{
		Out.vDiffuse = vector(g_vColor1, 1.f);
	}

	/* -1.0 ~ 1.f -> 0 ~ 1 */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}

technique11	DefaultTechnique	// 그래픽스 기법 정의
{
	// 패스 정의
	pass Default
	{
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}

	// 패스 정의
	pass AfterImage
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_AFTER();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}

	pass LeopardonCube
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LEOPARDON_CUBE();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}

	pass LeopardonCubeMetal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// 정점 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 정점 셰이더 메인 함수)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LEOPARDON_CUBE_METAL();		// 픽셀 셰이더 설정 (vs_5_0 : 셰이더 버전) (VS_MAIN : 픽셀 셰이더 메인 함수)
	}
}