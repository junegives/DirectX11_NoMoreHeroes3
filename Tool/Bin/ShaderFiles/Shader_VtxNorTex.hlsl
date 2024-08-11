
//float2, float3, float4 == vector
//float3x3, float4x4 == matrix, float1x4



/* 전역변수 : 컨스턴트 테이블*/
/* 값을 채운다 : 초기화, 클라이언트에서 쉐이더 전역변수에 값을 전달한다. */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector		g_vLightDiffuse;	// 빛의 색상
vector		g_vLightAmbient;	// 빛의 반사 색상
vector		g_vLightSpecular;	// 빛의 정반사(하이라이트) 색상

texture2D	g_DiffuseTexture[2];
texture2D	g_MaskTexture;

vector		g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);	// 물체의 반사 색상
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);	// 물체의 정반사(하이라이트) 색상

vector		g_vLightDir;		// 방향성 광원의 방향

vector		g_vLightPos;
float		g_fLightRange;	// 빛이 닿는 거리

vector		g_vCamPosition;			// 정반사 각도를 계산하기 위한 카메라 위치


sampler		g_LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

//float		g_fData = 10;
//vector		g_vData = vector(1.f, 1.f, 1.f, 1.f);
//vector		g_vData = (vector)0.0f;


/* Vertex Shader */
/* 1. 정점의 변환(월드변환, 뷰변환, 투영변환) 을 수행한다. */
/* 2. 정점의 구성을 변경한다. */
/* 3. 정점을 전달받고 결과로 정점을 리턴한다. */

// /* 정점*/ VS_MAIN(float3 vPosition : POSITION, float2 vTexcoord : TEXCOORD0)
struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal	= mul(vector(In.vNormal, 0.f), g_WorldMatrix);

	return Out;
}

/* 정점 세개(TriangleList)가 모이게 되면. */
/* w나누기연산을 수행한다.(원근투영) */
/* 뷰포트변환.(윈도우좌표로 변환한다) */
/* 래스터라이즈.( 픽셀을 생성한다. )*/

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

/* Pixel Shader */
/* 1. 픽셀의 색을 결정한다. */

/*
정점 셰이더에서 연산하고 선형 보간을 한 것과 픽셀 셰이더에서 연산한 것과 크게 차이가 없기 때문에 정점 연산을 하는게 효율적임.
Specular나 Normal을 디자이너가 제공해주는 경우(텍스처가 존재하는 경우)에는 픽셀 셰이더에서 연산해야함.

퐁(phong) 셰이드 : 픽셀 단에서 연산
그라우드(Gouraud) 셰이드 : 정점 단에서 연산
*/

// 점 광원 픽셀 셰이더
PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Diffuse
	vector		vMtrlDiffuse = g_DiffuseTexture[0].Sample(g_LinearSampler, In.vTexcoord * 30.f);

	vector		vLightDir = In.vWorldPos - g_vLightPos;

	// Attenuation
	float		fDistance = length(vLightDir);		// 픽셀과 광원 사이의 거리
	float		fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);	// 거리를 기반으로 0~1의 값으로 감쇠값 계산

	// Ambient
	float		fShade = max(dot(normalize(vLightDir) * -1.f, normalize(In.vNormal)), 0.f);	// 빛의 세기 (빛 벡터와 노말 벡터의 각도가 90~270)
	vector		vAmbient = g_vLightAmbient * g_vMtrlAmbient;							// 주변 물체가 반사하는 빛

	// Specular
	vector		vReflect = reflect(normalize(vLightDir), normalize(In.vNormal));		// 반사 벡터
	vector		vLook = In.vWorldPos - g_vCamPosition;									// 시선 벡터
	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);	// Specular 세기 (하이라이트 세기)
	vector		vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;			// Specular

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse * min(fShade + vAmbient, 1.f) + vSpecular) * fAtt;

	return Out;
}

// 방향성 광원 픽셀 셰이더
PS_OUT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Diffuse
	vector		vSourDiffuse = g_DiffuseTexture[0].Sample(g_LinearSampler, In.vTexcoord * 30.f);
	vector		vDestDiffuse = g_DiffuseTexture[1].Sample(g_LinearSampler, In.vTexcoord * 30.f);
	vector		vMask = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);

	// mask texture의 흰색 부분에 DestDiffuse 출력, 검은색 부분에 SourDiffuse 출력
	vector		vMtrlDiffuse = vDestDiffuse * vMask + vSourDiffuse * (1.f - vMask);

	// Ambient
	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);	// 빛의 세기 (빛 벡터와 노말 벡터의 각도가 90~270)
	vector		vAmbient = g_vLightAmbient * g_vMtrlAmbient;									// 주변 물체가 반사하는 빛

	// Specular
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));		// 반사 벡터
	vector		vLook = In.vWorldPos - g_vCamPosition;									// 시선 벡터
	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);	// Specular 세기 (하이라이트 세기)
	vector		vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;			// Specular

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min(fShade + vAmbient, 1.f) + vSpecular;

	return Out;
}

/* EffectFramework */

technique11	DefaultTechnique
{
	// 패스 정의 1
	// 점광원
	pass Lighting_Point
	{
		/* RenderState설정. */

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	// 패스 정의 2
	// 방향성 광원
	pass Lighting_Directional
	{
		/* RenderState설정. */

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}
}


