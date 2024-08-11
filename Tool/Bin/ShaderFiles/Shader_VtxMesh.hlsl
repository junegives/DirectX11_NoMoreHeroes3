
#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;	// 월드, 뷰, 투영 변환 행렬
texture2D	g_DiffuseTexture;	// 2D 텍스처

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

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);	// 월드 공간 상의 좌표 구하기
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));		// 월드 공간 상의 노말 벡터 구하기
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

	/*
	// 빛의 세기
	// 빛의 벡터와 노말 벡터의 각도가 90도~270도일 때만 빛을 받아야한다.
	// 계산하기 쉽게 빛의 벡터를 180도 꺾어서 계산하면 값이 0이상일 때 코사인 그래프와 같다.
	// 위의 사실을 이용해서 코사인을 통해서 빛의 세기를 계산한다.
	// 빛의 벡터를 180도 꺾고, 빛의 벡터, 노말 벡터를 노멀라이징한 뒤 두 벡터를 내적하면 코사인 결과를 알 수 있다.
	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	// 주변 물체가 반사하는 빛
	// 빛의 반사 색상과 세기 * 물체의 반사 색상과 세기
	vector		vAmbient = g_vLightAmbient * g_vMtrlAmbient;

	// 반사 벡터
	// 1. 빛 벡터와 길이가 1인 법선 벡터 내적
	// 2. 물체 위치에서 구한 정사영 벡터 2번 더하기
	// 3. 빛 벡터의 끝 지점에서 2번의 최종 위치ㅣ까지 이은 벡터 구하기
	// 4. 3번의 벡터를 물체 위치로 옮기기
	// * 2번에서 벡터를 1번만 더해서 진행하면 슬라이딩 벡터가 나온다. (ex, 몬스터가 벽에 막혔을 때 이동 경로 등)
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	// 시선 벡터
	vector		vLook = In.vWorldPos - g_vCamPosition;

	// Specular 세기(하이라이트 세기)
	// 30 거듭제곱 하여 광택 부분 강조
	// cos 함수를 제곱하면 그래프가 바뀐다. (볼록하던 부분이 오목하게)
	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);

	// Specular(정반사 == 하이라이트)
	// 계산이 끝난 물체의 색상 + (LightSpec * MtrlSpec * (0~1 : 스페큘러 세기))
	vector		vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min(fShade + vAmbient, 1.f) + vSpecular;

	//Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
	*/
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
}