// 셰이더 헤더파일 hlsli 
// 셰이더 파일들에 중복되는 것 포함
// Render State 설정

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

// 셰이더에서는 옵션의 언더바 마지막것만 써주면 됨
// 렌더 스테이트 한 번 세팅하고 다시 안되돌리면 계속 적용됨
// 옵션 안채우면 디폴트

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


