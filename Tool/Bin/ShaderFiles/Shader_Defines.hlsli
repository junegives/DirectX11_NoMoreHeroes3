// ���̴� ������� hlsli 
// ���̴� ���ϵ鿡 �ߺ��Ǵ� �� ����
// Render State ����

/* For.Sampler_State */

sampler		g_LinearSampler = sampler_state
{
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

sampler		g_ClampSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

// ���̴������� �ɼ��� ����� �������͸� ���ָ� ��
// ���� ������Ʈ �� �� �����ϰ� �ٽ� �ȵǵ����� ��� �����
// �ɼ� ��ä��� ����Ʈ

/* For.Render_State */

/* Rasterizer State : D3D11_RASTERIZER_DESC */
RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_None_Cull
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = WireFrame;
	FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_None_ZTestAndWrite
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlending
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};


