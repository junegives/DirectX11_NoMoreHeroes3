
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

// Geometry Shader�� ���� �Լ���, ���� �ϳ��� ���� ó���� �����Ѵ�.
[maxvertexcount(6)]	// ��µǴ� ������ �ִ� ������ �����Ѵ�. (�� �������� �� 6���� ������ �����Ѵ�.)
void GS_MAIN_BILLBOARD(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
	GS_OUT		Out[4];

	// �������� ���������� ���͸� ����Ѵ�.
	float3		vLook = (g_vCamPosition - In[0].vPosition).xyz;
	// ī�޶��� ������ ������ ���ϰ�, ����ȭ�� �� ������ ���� ũ�⸦ ���Ͽ� ���� ũ��� �����.
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	// ī�޶��� ���� ������ ���ϰ�, ����ȭ�� �� ������ ���� ũ�⸦ ���Ͽ� ���� ũ��� �����.
	float3		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

	// ��-���� ���
	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	// ������ ��ġ�� ����Ѵ�.
	// �������� ���������� ���Ϳ� �����ʰ� ���� ���͸� ���ϰų� ���� ���Ѵ�.
	// �� ��, ��-���� ����� ���Ͽ� ������ ���� ��ġ�� ����Ѵ�.
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

	// �簢���� �� �������� �߰��ϰ� RestartStrip()�� ȣ���Ͽ� ���ο� ��Ʈ��(Strip)�� �����Ѵ�.
	// �̷��� �ϸ� �� ���� �ﰢ���� �ϳ��� ��Ʈ������ ���յǾ� ���簢���� �����ȴ�.
	DataStream.Append(Out[0]);
	DataStream.Append(Out[1]);
	DataStream.Append(Out[2]);
	DataStream.RestartStrip();

	DataStream.Append(Out[0]);
	DataStream.Append(Out[2]);
	DataStream.Append(Out[3]);
}

// Geometry Shader�� ���� �Լ���, ���� �ϳ��� ���� ó���� �����Ѵ�.
[maxvertexcount(6)]	// ��µǴ� ������ �ִ� ������ �����Ѵ�. (�� �������� �� 6���� ������ �����Ѵ�.)
void GS_MAIN_NOBILLBOARD(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
	GS_OUT		Out[4];

	// �������� ���������� ���͸� ����Ѵ�.
	float3		vLook = (g_vCamPosition - In[0].vPosition).xyz;
	// ī�޶��� ������ ������ ���ϰ�, ����ȭ�� �� ������ ���� ũ�⸦ ���Ͽ� ���� ũ��� �����.
	float3		vRight = In[0].vPSize.x * 0.5f;
	// ī�޶��� ���� ������ ���ϰ�, ����ȭ�� �� ������ ���� ũ�⸦ ���Ͽ� ���� ũ��� �����.
	float3		vUp = In[0].vPSize.y * 0.5f;

	float4		vDir = In[0].vDir;

	// ��-���� ���
	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	// ������ ��ġ�� ����Ѵ�.
	// �������� ���������� ���Ϳ� �����ʰ� ���� ���͸� ���ϰų� ���� ���Ѵ�.
	// �� ��, ��-���� ����� ���Ͽ� ������ ���� ��ġ�� ����Ѵ�.
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

	// �簢���� �� �������� �߰��ϰ� RestartStrip()�� ȣ���Ͽ� ���ο� ��Ʈ��(Strip)�� �����Ѵ�.
	// �̷��� �ϸ� �� ���� �ﰢ���� �ϳ��� ��Ʈ������ ���յǾ� ���簢���� �����ȴ�.
	DataStream.Append(Out[0]);
	DataStream.Append(Out[1]);
	DataStream.Append(Out[2]);
	DataStream.RestartStrip();

	DataStream.Append(Out[0]);
	DataStream.Append(Out[2]);
	DataStream.Append(Out[3]);
}

/* ���� ����(TriangleList)�� ���̰� �Ǹ�. */
/* w�����⿬���� �����Ѵ�.(��������) */
/* ����Ʈ��ȯ.(��������ǥ�� ��ȯ�Ѵ�) */
/* �����Ͷ�����.( �ȼ��� �����Ѵ�. )*/

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

