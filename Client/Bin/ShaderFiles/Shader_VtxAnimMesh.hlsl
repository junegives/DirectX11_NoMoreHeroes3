
#include "Shader_Defines.hlsli"

/* �� �޽ÿ��� ������ �ִ� ������ CombinedTransformationMatrix */
matrix		g_BoneMatrices[256];
// �� �޽ð� �̿��ϴ� ���� ������ 256���� �Ѿ�� ������. (512���� �÷����� ��¦�� �ٿ�����) (�� 2�� n���� �ʿ�� ����)


matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;	// ����, ��, ���� ��ȯ ���
texture2D	g_DiffuseTexture;	// 2D �ؽ�ó
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
	float4 vPosition	: SV_POSITION;		// ��ȯ�� ������ ��ǥ (��ũ��)
	float4 vNormal		: NORMAL;			// ������ �븻 ����
	float2 vTexcoord	: TEXCOORD0;		// ��ȯ�� �ؽ�ó ��ǥ
	float4 vWorldPos	: TEXCOORD1;		// ������ ���� ��ǥ
	float4 vProjPos		: TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);	// World * View
	matWVP = mul(matWV, g_ProjMatrix);			// World * View * Projection

	// �������� ��Ű�� ó��(���� �Ǻθ� ����, �����鿡 ���� ���¸� ���ؼ� ���� ���󰡵���)
	// -> ���̴����� ó���Ͽ����Ƿ� GPU���� ���ư�
	// -> �ϵ���� ��Ű��

	// g_BoneMatrices : ��� ���� ��ȯ ����� ��� �迭
	// In.vBlendIndices : ���� ������ ������ �޴� �ִ� 4���� ���� �ε���
	// In.VBlendWeights : ���� ������ ���� �ش� ���� ����ġ

	// ���� ���� ��� �� 1���� ���� In.vBlendWeights.w�� ���� �������ش�(x + y + z + w = 1���� �̿�)
	// In.vBlendIndices.w�� 0�̿��� ���� (�� 1���� �ε��� 0�̴ϱ�)
	// �����ǿ� ������� �������� �ʰ� �׵� ����� ����������
	float	fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix	BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector	vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector	vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);	// ���� ���� ���� ��ǥ ���ϱ�
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));		// ���� ���� ���� �븻 ���� ���ϱ�
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4 vPosition	: SV_POSITION;	// ���� ���̴����� ���� ��ũ�� ��ǥ
	float4 vNormal		: NORMAL;		// �ȼ��� �븻 ����
	float2 vTexcoord	: TEXCOORD0;	// ���� ���̴����� ���� �ؽ�ó ��ǥ
	float4 vWorldPos	: TEXCOORD1;	// �ȼ��� ���� ��ǥ
	float4 vProjPos		: TEXCOORD2;
};

struct PS_OUT
{
	vector vDiffuse : SV_TARGET0;
	vector vNormal : SV_TARGET1;
	vector vDepth : SV_TARGET2;
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

technique11	DefaultTechnique	// �׷��Ƚ� ��� ����
{
	// �н� ����
	pass Default
	{
		SetRasterizerState(RS_None_Cull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}

	// �н� ����
	pass AfterImage
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_AFTER();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}

	pass LeopardonCube
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LEOPARDON_CUBE();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}

	pass LeopardonCubeMetal
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LEOPARDON_CUBE_METAL();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}
}