
#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;	// ����, ��, ���� ��ȯ ���
texture2D	g_DiffuseTexture;	// 2D �ؽ�ó
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
	float4 vPosition	: SV_POSITION;		// ��ȯ�� ������ ��ǥ (��ũ��)
	float2 vTexcoord	: TEXCOORD0;		// ��ȯ�� �ؽ�ó ��ǥ
	float4 vNormal		: NORMAL;			// ������ �븻 ����
	float4 vWorldPos	: TEXCOORD1;		// ������ ���� ��ǥ
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
	Out.vWorldPos	= mul(vector(In.vPosition, 1.f), g_WorldMatrix);	// ���� ���� ���� ��ǥ ���ϱ�
	Out.vNormal		= normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));		// ���� ���� ���� �븻 ���� ���ϱ�
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4 vPosition	: SV_POSITION;	// ���� ���̴����� ���� ��ũ�� ��ǥ
	float2 vTexcoord	: TEXCOORD0;	// ���� ���̴����� ���� �ؽ�ó ��ǥ
	float4 vNormal		: NORMAL;		// �ȼ��� �븻 ����
	float4 vWorldPos	: TEXCOORD1;	// �ȼ��� ���� ��ǥ
	float4 vProjPos		: TEXCOORD2;
};

struct PS_OUT
{
	vector vDiffuse	: SV_TARGET0;	// ȭ�鿡 �׷��� �ȼ��� ���� �Ӽ�
	vector vNormal	: SV_TARGET1;
	vector vDepth	: SV_TARGET2;
};

struct PS_DISTORTION_OUT
{
	vector vColor : SV_TARGET0;
};

/*
���� ���̴����� �����ϰ� ���� ������ �� �Ͱ� �ȼ� ���̴����� ������ �Ͱ� ũ�� ���̰� ���� ������ ���� ������ �ϴ°� ȿ������.
Specular�� Normal�� �����̳ʰ� �������ִ� ���(�ؽ�ó�� �����ϴ� ���)���� �ȼ� ���̴����� �����ؾ���.

��(phong) ���̵� : �ȼ� �ܿ��� ����
�׶���(Gouraud) ���̵� : ���� �ܿ��� ����
*/

// ���⼺ ���� �ȼ� ���̴�
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// ��ü�� Diffuse (��ü�� ���� ǥ��)
	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(g_LinearSampler, In.vTexcoord);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse = vMtrlDiffuse;
	// �븻 ���͸� (-1, 1) �������� (0, 1) ������ ��ȯ
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}


PS_OUT PS_MAIN_COLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// �븻 ���͸� (-1, 1) �������� (0, 1) ������ ��ȯ
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

	// �븻 ���͸� (-1, 1) �������� (0, 1) ������ ��ȯ
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

	// �븻 ���͸� (-1, 1) �������� (0, 1) ������ ��ȯ
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

technique11	DefaultTechnique	// �׷��Ƚ� ��� ����
{
	// �н� ����
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}

	pass Color
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLOR();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}

	pass Trail
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COLOR();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}

	pass LeopardonRing
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LEOPARDONRING();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}

	pass TriggerPillar
	{
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TRIGGERPILAR();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}

	pass DistortionMesh
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None_ZTestAndWrite, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISTORTION();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}
}