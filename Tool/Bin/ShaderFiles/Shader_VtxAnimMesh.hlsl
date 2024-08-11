
/* �� �޽ÿ��� ������ �ִ� ������ CombinedTransformationMatrix */
matrix		g_BoneMatrices[256];
// �� �޽ð� �̿��ϴ� ���� ������ 256���� �Ѿ�� ������. (512���� �÷����� ��¦�� �ٿ�����) (�� 2�� n���� �ʿ�� ����)


matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;	// ����, ��, ���� ��ȯ ���
vector		g_vLightDiffuse;	// ���� ����
vector		g_vLightAmbient;	// ���� �ݻ� ����
vector		g_vLightSpecular;	// ���� ���ݻ�(���̶���Ʈ) ����

texture2D	g_DiffuseTexture;	// 2D �ؽ�ó
vector		g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);		// ��ü�� �ݻ� ����
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);		// ��ü�� ���ݻ�(���̶���Ʈ) ����

vector		g_vLightDir;	// ���⼺ ������ ����

vector		g_vCamPosition;		// ���ݻ� ������ ����ϱ� ���� ī�޶� ��ġ

sampler		g_LinearSampler = sampler_state
{
	// Filter = MIN_MAG_MIP_POINT;
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler		g_PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

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
	float2 vTexcoord	: TEXCOORD0;		// ��ȯ�� �ؽ�ó ��ǥ
	float4 vNormal		: NORMAL;			// ������ �븻 ����
	float4 vWorldPos	: TEXCOORD1;		// ������ ���� ��ǥ
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

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);	// ���� ���� ���� ��ǥ ���ϱ�
	Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);		// ���� ���� ���� �븻 ���� ���ϱ�

	return Out;
}

struct PS_IN
{
	float4 vPosition	: SV_POSITION;	// ���� ���̴����� ���� ��ũ�� ��ǥ
	float2 vTexcoord	: TEXCOORD0;	// ���� ���̴����� ���� �ؽ�ó ��ǥ
	float4 vNormal		: NORMAL;		// �ȼ��� �븻 ����
	float4 vWorldPos	: TEXCOORD1;	// �ȼ��� ���� ��ǥ
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;	// ȭ�鿡 �׷��� �ȼ��� ���� �Ӽ�
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
}

technique11	DefaultTechnique	// �׷��Ƚ� ��� ����
{
	// �н� ����
	pass Default
	{
		VertexShader = compile vs_5_0 VS_MAIN();	// ���� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : ���� ���̴� ���� �Լ�)
		PixelShader = compile ps_5_0 PS_MAIN();		// �ȼ� ���̴� ���� (vs_5_0 : ���̴� ����) (VS_MAIN : �ȼ� ���̴� ���� �Լ�)
	}
}