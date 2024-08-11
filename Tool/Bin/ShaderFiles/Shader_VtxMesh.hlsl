
#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;	// ����, ��, ���� ��ȯ ���
texture2D	g_DiffuseTexture;	// 2D �ؽ�ó

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

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);	// ���� ���� ���� ��ǥ ���ϱ�
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));		// ���� ���� ���� �븻 ���� ���ϱ�
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

	/*
	// ���� ����
	// ���� ���Ϳ� �븻 ������ ������ 90��~270���� ���� ���� �޾ƾ��Ѵ�.
	// ����ϱ� ���� ���� ���͸� 180�� ��� ����ϸ� ���� 0�̻��� �� �ڻ��� �׷����� ����.
	// ���� ����� �̿��ؼ� �ڻ����� ���ؼ� ���� ���⸦ ����Ѵ�.
	// ���� ���͸� 180�� ����, ���� ����, �븻 ���͸� ��ֶ���¡�� �� �� ���͸� �����ϸ� �ڻ��� ����� �� �� �ִ�.
	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	// �ֺ� ��ü�� �ݻ��ϴ� ��
	// ���� �ݻ� ����� ���� * ��ü�� �ݻ� ����� ����
	vector		vAmbient = g_vLightAmbient * g_vMtrlAmbient;

	// �ݻ� ����
	// 1. �� ���Ϳ� ���̰� 1�� ���� ���� ����
	// 2. ��ü ��ġ���� ���� ���翵 ���� 2�� ���ϱ�
	// 3. �� ������ �� �������� 2���� ���� ��ġ�ӱ��� ���� ���� ���ϱ�
	// 4. 3���� ���͸� ��ü ��ġ�� �ű��
	// * 2������ ���͸� 1���� ���ؼ� �����ϸ� �����̵� ���Ͱ� ���´�. (ex, ���Ͱ� ���� ������ �� �̵� ��� ��)
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	// �ü� ����
	vector		vLook = In.vWorldPos - g_vCamPosition;

	// Specular ����(���̶���Ʈ ����)
	// 30 �ŵ����� �Ͽ� ���� �κ� ����
	// cos �Լ��� �����ϸ� �׷����� �ٲ��. (�����ϴ� �κ��� �����ϰ�)
	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);

	// Specular(���ݻ� == ���̶���Ʈ)
	// ����� ���� ��ü�� ���� + (LightSpec * MtrlSpec * (0~1 : ����ŧ�� ����))
	vector		vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min(fShade + vAmbient, 1.f) + vSpecular;

	//Out.vColor = g_Texture.Sample(g_LinearSampler, In.vTexcoord);

	return Out;
	*/
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
}