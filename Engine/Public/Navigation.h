#pragma once
#include "Component.h"

/* �׺���̼��� �����ϴ� Cell�� �����Ѵ�. */
/* �� �׺���̼� ������Ʈ�� �̿��ϴ� ��ü�� � Cell �ȿ� �־��°�?! */

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_int	iStartCellIndex;
	} NAVI_DESC;

public:
	CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	HRESULT		Initialize_Prototype(const wstring& strNavigationDataFilePath);
	HRESULT		Initialize(void* pArg) override;
	void		Tick(_fmatrix TerrainWorldMatrix);
	_bool		isMove(_fvector vPosition);

public:
	_float		Compute_Height(_vector vPosition);
	_bool		Compute_CurrentIdx(_vector vPosition);

public:
	_vector		CalcSlideVec(_vector vLook);

	void		GetCellCenterPos(_vector* vCenterPos);
	void		GetCellCenterPos(_uint iCellIdx, _vector* vCenterPos);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	_int								m_iCurrentIndex = { -1 };
	vector<shared_ptr<class CCell>>		m_Cells;
	static _float4x4					m_WorldMatrix; // �׺���̼� ��ü�� ���� ���� ����� ��� �ν��Ͻ��� �����ϰ� �����ϱ� ���ؼ� static ����


#ifdef _DEBUG
private:
	shared_ptr<class CShader>			m_pShader = { nullptr };
#endif

private:
	HRESULT	Make_Neighbors();

public:
	static shared_ptr<CNavigation> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>, const wstring& strNavigationDataFilePath);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;
};

END