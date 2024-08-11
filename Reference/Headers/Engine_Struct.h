#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct
	{
		HWND			hWnd;
		unsigned int	iWinSizeX, iWinSizeY;
		bool			isWindowed;
	} GRAPHIC_DESC;

	typedef struct
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE			eType;
		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
	} LIGHT_DESC;

	typedef struct
	{
		// ShaderResourceView�� �����ϴ� ��� SRV�� �ε�, ���ε� �� �ؽ�ó ���� ����� �̹� �����ϴ� CTexture ���
		shared_ptr<class CTexture> pMtrlTextures[AI_TEXTURE_TYPE_MAX];
	} MESH_MATERIAL_DESC;

	typedef struct
	{
		unsigned short _0, _1, _2;
	} TRIINDICES16;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[1];
	} VTXPOS;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[2];
	}VTXPOINT;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int				iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT2	vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	} VTXNORTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[2];
	}VTXCUBE;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT2	vTexcoord;
		XMFLOAT3	vTangent;

		static const unsigned int iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[4];
	} VTXMESH;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* �� ������ � ����(�ִ� 4��)�� ���¸� �޾Ƽ� ��ȯ�Ǿ���ϴ°�?! */
		// 
		/* iIndex = ������ �޽ð� ����ϴ� ������ �ε��� */
		XMUINT4			vBlendIndices;

		/* ������ ���� ���¸� �� �ۼ�Ʈ�� �̿�����?!(����ġ 0 ~ 1) */
		/* 1 = x + y + z + w */
		XMFLOAT4		vBlendWeights;


		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[6];
	} VTXANIMMESH;

	typedef struct
	{
		XMFLOAT4	vRight, vUp, vLook, vTranslation;
		XMFLOAT4	vDir;
		XMFLOAT4	vColor;
	}VTXINSTANCE;

	typedef struct ENGINE_DLL
	{
		static const unsigned int	iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[8];
	}VTX_RECT_INSTANCE;

	typedef struct ENGINE_DLL
	{
		static const unsigned int	iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[8];
	}VTX_POINT_INSTANCE;

	// �Ľ̿� ����ü

	typedef enum
	{
		NONE,
		DIFFUSE,
		SPECULAR,
		AMBIENT,
		EMISSIVE,
		HEIGHT,
		NORMALS,
		SHININESS,
		OPACITY,
		DISPLACEMENT,
		LIGHTMAP,
		REFLECTION,
		BASE_COLOR,
		NORMAL_CAMERA,
		EMISSION_COLOR,
		METALNESS,
		DIFFUSE_ROUGHNESS,
		AMBIENT_OCCLUSION,
		SHEEN,
		CLEARCOAT,
		TRANSMISSION,
		UNKNOWN,
	} TEXTYPE;
#define TEXTYPE_MAX UNKNOWN

	typedef struct
	{
		TEXTYPE				TexType;		// �ؽ�ó Ÿ��
		string				strTexPath;		// �ؽ�ó ���
	} psMATERIALTEX;

	typedef struct
	{
		string				strMaterialName;
		psMATERIALTEX		MaterialTextures[TEXTYPE_MAX];	// ���׸����� �ؽ�ó��
	} psMATERIAL;

	typedef struct
	{
		_uint				iVertexId;			// ���� ������ ��ġ�� ������ �ε���
		_float				fWeight;			// �ϳ��� ������ ���� ���� ���� ����ġ
	} psWEIGHT;

	typedef struct
	{
		_float				fTime;
		_float3				vScale;
		_float3				vPosition;
		_float4				vRotation;
	} psKEYFRAME;

	typedef struct
	{
		string				strChannelName;

		_uint				iNumKeyFrame;
		psKEYFRAME*			KeyFrames;

	} psCHANNEL;

	typedef struct
	{
		string				strAnimName;
		_float				fDuration;
		_float				fTickPerSecond;

		_uint				iNumChannels;
		psCHANNEL*			Channels;
	} psANIMATION;

	typedef struct
	{
		_int				iParentBoneIndex;	// �θ�� �ε���
		_int				iBoneIndex;			// ���λ� �ε���
		string				strBoneName;		// �� �̸�
		_float4x4			TransformMatrix;	// ��ȯ ���
	} psBONE;

	typedef struct tagMESH_STATIC
	{
		string				strMeshName;		// �޽� �̸�
		_uint				iMaterialIndex;		// ���׸��� �ε���
		_uint				iNumVertices;		// ���� ����
		_uint				iNumFaces;			// �ﰢ�� ����

		VTXMESH*			Vertices;			// �޽��� ������ ���� �迭
		_uint*				vIndices;			// ������ �ε��� �迭
	} psMESH_STATIC;

	typedef struct
	{
		string				strMeshName;		// �޽� �̸�
		_uint				iMaterialIndex;		// ���׸��� �ε���
		_uint				iNumVertices;		// ���� ����
		_uint				iNumFaces;			// �ﰢ�� ����

		VTXANIMMESH*		Vertices;			// �޽��� ������ ���� �迭
		_uint*				vIndices;			// ������ �ε��� �迭

		_uint				iNumBones;			// �� ����
		_uint*				iBoneIndices;		// �� �ε��� �迭
		_float4x4*			OffsetMatrices;		// ������ ��� �迭
	} psMESH_DYNAMIC;

	typedef struct
	{
		_bool				isStatic;
		_uint				iNumMeshes;			// �޽� ����
		_uint				iNumMaterials;		// ���׸��� ����
		_uint				iNumAnimations;		// �ִϸ��̼� ����

		psBONE*				Bones;				// �� �迭
	} psMODEL;

	/*typedef struct
	{
		string				strParticleTag;

		_float3				vPosition;
		_float3				vScale;
		_float3				vRotation;

		_int				iTextureNum;
		string				strTexturePath;

	};*/

}


#endif // Engine_Struct_h__
