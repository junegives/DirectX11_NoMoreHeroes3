
//float2, float3, float4 == vector
//float3x3, float4x4 == matrix, float1x4



/* 전역변수 : 컨스턴트 테이블*/
/* 값을 채운다 : 초기화, 클라이언트에서 쉐이더 전역변수에 값을 전달한다. */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_Texture;

sampler		g_LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
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

/* Pixel Shader */
/* 1. 픽셀의 색을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
}

/* EffectFramework */

technique11	DefaultTechnique
{
	pass DefaultPass
	{
		/* RenderState설정. */

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}


