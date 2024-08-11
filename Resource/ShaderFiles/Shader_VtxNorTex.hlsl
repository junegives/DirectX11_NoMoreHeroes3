
//float2, float3, float4 == vector
//float3x3, float4x4 == matrix, float1x4



/* �������� : ������Ʈ ���̺�*/
/* ���� ä��� : �ʱ�ȭ, Ŭ���̾�Ʈ���� ���̴� ���������� ���� �����Ѵ�. */
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector		g_vLightDiffuse;	// ���� ����
vector		g_vLightAmbient;	// ���� �ݻ� ����
vector		g_vLightSpecular;	// ���� ���ݻ�(���̶���Ʈ) ����

texture2D	g_DiffuseTexture[2];
texture2D	g_MaskTexture;

vector		g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);	// ��ü�� �ݻ� ����
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);	// ��ü�� ���ݻ�(���̶���Ʈ) ����

vector		g_vLightDir;		// ���⼺ ������ ����

vector		g_vLightPos;
float		g_fLightRange;	// ���� ��� �Ÿ�

vector		g_vCamPosition;			// ���ݻ� ������ ����ϱ� ���� ī�޶� ��ġ


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
/* 1. ������ ��ȯ(���庯ȯ, �亯ȯ, ������ȯ) �� �����Ѵ�. */
/* 2. ������ ������ �����Ѵ�. */
/* 3. ������ ���޹ް� ����� ������ �����Ѵ�. */

// /* ����*/ VS_MAIN(float3 vPosition : POSITION, float2 vTexcoord : TEXCOORD0)
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

/* ���� ����(TriangleList)�� ���̰� �Ǹ�. */
/* w�����⿬���� �����Ѵ�.(��������) */
/* ����Ʈ��ȯ.(��������ǥ�� ��ȯ�Ѵ�) */
/* �����Ͷ�����.( �ȼ��� �����Ѵ�. )*/

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
/* 1. �ȼ��� ���� �����Ѵ�. */

/*
���� ���̴����� �����ϰ� ���� ������ �� �Ͱ� �ȼ� ���̴����� ������ �Ͱ� ũ�� ���̰� ���� ������ ���� ������ �ϴ°� ȿ������.
Specular�� Normal�� �����̳ʰ� �������ִ� ���(�ؽ�ó�� �����ϴ� ���)���� �ȼ� ���̴����� �����ؾ���.

��(phong) ���̵� : �ȼ� �ܿ��� ����
�׶���(Gouraud) ���̵� : ���� �ܿ��� ����
*/

// �� ���� �ȼ� ���̴�
PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Diffuse
	vector		vMtrlDiffuse = g_DiffuseTexture[0].Sample(g_LinearSampler, In.vTexcoord * 30.f);

	vector		vLightDir = In.vWorldPos - g_vLightPos;

	// Attenuation
	float		fDistance = length(vLightDir);		// �ȼ��� ���� ������ �Ÿ�
	float		fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);	// �Ÿ��� ������� 0~1�� ������ ���谪 ���

	// Ambient
	float		fShade = max(dot(normalize(vLightDir) * -1.f, normalize(In.vNormal)), 0.f);	// ���� ���� (�� ���Ϳ� �븻 ������ ������ 90~270)
	vector		vAmbient = g_vLightAmbient * g_vMtrlAmbient;							// �ֺ� ��ü�� �ݻ��ϴ� ��

	// Specular
	vector		vReflect = reflect(normalize(vLightDir), normalize(In.vNormal));		// �ݻ� ����
	vector		vLook = In.vWorldPos - g_vCamPosition;									// �ü� ����
	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);	// Specular ���� (���̶���Ʈ ����)
	vector		vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;			// Specular

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse * min(fShade + vAmbient, 1.f) + vSpecular) * fAtt;

	return Out;
}

// ���⼺ ���� �ȼ� ���̴�
PS_OUT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Diffuse
	vector		vSourDiffuse = g_DiffuseTexture[0].Sample(g_LinearSampler, In.vTexcoord * 30.f);
	vector		vDestDiffuse = g_DiffuseTexture[1].Sample(g_LinearSampler, In.vTexcoord * 30.f);
	vector		vMask = g_MaskTexture.Sample(g_LinearSampler, In.vTexcoord);

	// mask texture�� ��� �κп� DestDiffuse ���, ������ �κп� SourDiffuse ���
	vector		vMtrlDiffuse = vDestDiffuse * vMask + vSourDiffuse * (1.f - vMask);

	// Ambient
	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);	// ���� ���� (�� ���Ϳ� �븻 ������ ������ 90~270)
	vector		vAmbient = g_vLightAmbient * g_vMtrlAmbient;									// �ֺ� ��ü�� �ݻ��ϴ� ��

	// Specular
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));		// �ݻ� ����
	vector		vLook = In.vWorldPos - g_vCamPosition;									// �ü� ����
	float		fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 30.f);	// Specular ���� (���̶���Ʈ ����)
	vector		vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;			// Specular

	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min(fShade + vAmbient, 1.f) + vSpecular;

	return Out;
}

/* EffectFramework */

technique11	DefaultTechnique
{
	// �н� ���� 1
	// ������
	pass Lighting_Point
	{
		/* RenderState����. */

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	// �н� ���� 2
	// ���⼺ ����
	pass Lighting_Directional
	{
		/* RenderState����. */

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}
}


