#pragma once
#include "GameObject.h"

class CDistortion_Mesh : public CGameObject
{
public:
	CDistortion_Mesh();
	virtual ~CDistortion_Mesh() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;





};

