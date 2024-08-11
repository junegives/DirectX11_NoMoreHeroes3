
#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector		g_vCamPosition = vector(0.f, 0.f, -1.f, 1.f);
texture2D	g_Texture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vPSize : PSIZE;

	row_major float4x4 TransformMatrix : WORLD;
	float4 vDir : TEXCOORD0;

	float4 vColor : COLOR0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
	float4 vDir : TEXCOORD0;
	float4 vColor : COLOR0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	vector		vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = float2(length(In.TransformMatrix._11_12_13) * In.vPSize.x,
		length(In.TransformMatrix._21_22_23) * In.vPSize.y);
	Out.vDir = In.vDir;
	Out.vColor = In.vColor;

	return Out;
}

struct GS_IN
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
	float4 vDir : TEXCOORD0;
	float4 vColor : COLOR0;
};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vColor : COLOR0;
};

// void GS_MAIN(point GS_IN[1], inout LineStream<GS_OUT> DataStream)
// void GS_MAIN(triangle GS_IN[3], inout TriangleStream<GS_OUT> DataStream)
// void GS_MAIN(line GS_IN[2], inout TriangleStream<GS_OUT> DataStream)

// Geometry Shader의 메인 함수로, 정점 하나에 대한 처리를 수행한다.
[maxvertexcount(6)]	// 출력되는 정점의 최대 개수를 지정한다. (각 정점마다 총 6개의 정점을 생성한다.)
void GS_MAIN_BILLBOARD(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
	GS_OUT		Out[4];

	// 시점에서 정점까지의 벡터를 계산한다.
	float3		vLook = (g_vCamPosition - In[0].vPosition).xyz;
	// 카메라의 오른쪽 방향을 구하고, 정규화한 후 정점의 가로 크기를 곱하여 절반 크기로 만든다.
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	// 카메라의 위쪽 방향을 구하고, 정규화한 후 정점의 세로 크기를 곱하여 절반 크기로 만든다.
	float3		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

	// 뷰-투영 행렬
	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	// 정점의 위치를 계산한다.
	// 시점에서 정점까지의 벡터에 오른쪽과 위쪽 벡터를 더하거나 빼서 구한다.
	// 그 후, 뷰-투영 행렬을 곱하여 정점의 최종 위치를 계산한다.
	Out[0].vPosition = In[0].vPosition + float4(vRight, 0.f) + float4(vUp, 0.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vColor = In[0].vColor;

	Out[1].vPosition = In[0].vPosition - float4(vRight, 0.f) + float4(vUp, 0.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);                             
	Out[1].vColor = In[0].vColor;

	Out[2].vPosition = In[0].vPosition - float4(vRight, 0.f) - float4(vUp, 0.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vColor = In[0].vColor;

	Out[3].vPosition = In[0].vPosition + float4(vRight, 0.f) - float4(vUp, 0.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vColor = In[0].vColor;

	// 사각형의 네 꼭짓점을 추가하고 RestartStrip()을 호출하여 새로운 스트립(Strip)을 시작한다.
	// 이렇게 하면 두 개의 삼각형이 하나의 스트립으로 결합되어 직사각형이 형성된다.
	DataStream.Append(Out[0]);
	DataStream.Append(Out[1]);
	DataStream.Append(Out[2]);
	DataStream.RestartStrip();

	DataStream.Append(Out[0]);
	DataStream.Append(Out[2]);
	DataStream.Append(Out[3]);
}

// Geometry Shader의 메인 함수로, 정점 하나에 대한 처리를 수행한다.
[maxvertexcount(6)]	// 출력되는 정점의 최대 개수를 지정한다. (각 정점마다 총 6개의 정점을 생성한다.)
void GS_MAIN_NOBILLBOARD(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
	GS_OUT		Out[4];

	// 시점에서 정점까지의 벡터를 계산한다.
	float3		vLook = (g_vCamPosition - In[0].vPosition).xyz;
	// 카메라의 오른쪽 방향을 구하고, 정규화한 후 정점의 가로 크기를 곱하여 절반 크기로 만든다.
	float3		vRight = In[0].vPSize.x * 0.5f;
	// 카메라의 위쪽 방향을 구하고, 정규화한 후 정점의 세로 크기를 곱하여 절반 크기로 만든다.
	float3		vUp = In[0].vPSize.y * 0.5f;

	float4		vDir = In[0].vDir;

	// 뷰-투영 행렬
	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	// 정점의 위치를 계산한다.
	// 시점에서 정점까지의 벡터에 오른쪽과 위쪽 벡터를 더하거나 빼서 구한다.
	// 그 후, 뷰-투영 행렬을 곱하여 정점의 최종 위치를 계산한다.
	Out[0].vPosition = In[0].vPosition + vDir * 0.2f + float4(0.f, In[0].vPSize.y * 2.f, 0.f, 0.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vColor = In[0].vColor;

	Out[1].vPosition = In[0].vPosition  + float4(0.f, In[0].vPSize.y, 0.f, 0.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vColor = In[0].vColor;

	Out[2].vPosition = In[0].vPosition  - float4(0.f, In[0].vPSize.y, 0.f, 0.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vColor = In[0].vColor;

	Out[3].vPosition = In[0].vPosition + vDir * 0.2f - float4(0.f, In[0].vPSize.y * 2.f, 0.f, 0.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vColor = In[0].vColor;

	/*Out[0].vPosition = In[0].vPosition - float4(In[0].vPSize.x, 0.f, 0.f, 0.f) + float4(0.f, In[0].vPSize.y, 0.f, 0.f);
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vColor = In[0].vColor;

	Out[1].vPosition = In[0].vPosition + float4(In[0].vPSize.x, 0.f, 0.f, 0.f) + float4(0.f, In[0].vPSize.y, 0.f, 0.f);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vColor = In[0].vColor;

	Out[2].vPosition = In[0].vPosition + float4(In[0].vPSize.x, 0.f, 0.f, 0.f) - float4(0.f, In[0].vPSize.y, 0.f, 0.f);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vColor = In[0].vColor;

	Out[3].vPosition = In[0].vPosition - float4(In[0].vPSize.x, 0.f, 0.f, 0.f) - float4(0.f, In[0].vPSize.y, 0.f, 0.f);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vColor = In[0].vColor;*/

	// 사각형의 네 꼭짓점을 추가하고 RestartStrip()을 호출하여 새로운 스트립(Strip)을 시작한다.
	// 이렇게 하면 두 개의 삼각형이 하나의 스트립으로 결합되어 직사각형이 형성된다.
	DataStream.Append(Out[0]);
	DataStream.Append(Out[1]);
	DataStream.Append(Out[2]);
	DataStream.RestartStrip();

	DataStream.Append(Out[0]);
	DataStream.Append(Out[2]);
	DataStream.Append(Out[3]);
}

/* 정점 세개(TriangleList)가 모이게 되면. */
/* w나누기연산을 수행한다.(원근투영) */
/* 뷰포트변환.(윈도우좌표로 변환한다) */
/* 래스터라이즈.( 픽셀을 생성한다. )*/

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float4 vColor : COLOR0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.vColor.a == 0.f)
		discard;

	Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord) * In.vColor;

	if (Out.vColor.a < 0.3f)
		discard;

	Out.vColor = In.vColor;

	return Out;
}

PS_OUT PS_MAIN_COLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.vColor.a <= 0.1f)
		discard;

	Out.vColor = In.vColor;
	Out.vColor.a = In.vColor.a;

	return Out;
}

technique11	DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_BILLBOARD();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass ColorPass
	{
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN_NOBILLBOARD();
		PixelShader = compile ps_5_0 PS_MAIN_COLOR();
	}
}

