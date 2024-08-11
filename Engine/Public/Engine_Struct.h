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
		// ShaderResourceView를 관리하는 대신 SRV를 로딩, 바인딩 등 텍스처 관리 기능이 이미 존재하는 CTexture 사용
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

		/* 이 정점은 어떤 뼈들(최대 4개)의 상태를 받아서 변환되어야하는가?! */
		// 
		/* iIndex = 각각의 메시가 사용하는 뼈들의 인덱스 */
		XMUINT4			vBlendIndices;

		/* 각각의 뼈의 상태를 몇 퍼센트나 이용할지?!(가중치 0 ~ 1) */
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

	// 파싱용 구조체

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
		TEXTYPE				TexType;		// 텍스처 타입
		string				strTexPath;		// 텍스처 경로
	} psMATERIALTEX;

	typedef struct
	{
		string				strMaterialName;
		psMATERIALTEX		MaterialTextures[TEXTYPE_MAX];	// 머테리얼의 텍스처들
	} psMATERIAL;

	typedef struct
	{
		_uint				iVertexId;			// 뼈가 영향을 미치는 정점의 인덱스
		_float				fWeight;			// 하나의 정점에 대한 뼈의 영향 가중치
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
		_int				iParentBoneIndex;	// 부모뼈 인덱스
		_int				iBoneIndex;			// 본인뼈 인덱스
		string				strBoneName;		// 뼈 이름
		_float4x4			TransformMatrix;	// 변환 행렬
	} psBONE;

	typedef struct tagMESH_STATIC
	{
		string				strMeshName;		// 메쉬 이름
		_uint				iMaterialIndex;		// 머테리얼 인덱스
		_uint				iNumVertices;		// 정점 개수
		_uint				iNumFaces;			// 삼각형 개수

		VTXMESH*			Vertices;			// 메쉬가 가지는 정점 배열
		_uint*				vIndices;			// 정점의 인덱스 배열
	} psMESH_STATIC;

	typedef struct
	{
		string				strMeshName;		// 메쉬 이름
		_uint				iMaterialIndex;		// 머테리얼 인덱스
		_uint				iNumVertices;		// 정점 개수
		_uint				iNumFaces;			// 삼각형 개수

		VTXANIMMESH*		Vertices;			// 메쉬가 가지는 정점 배열
		_uint*				vIndices;			// 정점의 인덱스 배열

		_uint				iNumBones;			// 뼈 개수
		_uint*				iBoneIndices;		// 뼈 인덱스 배열
		_float4x4*			OffsetMatrices;		// 오프셋 행렬 배열
	} psMESH_DYNAMIC;

	typedef struct
	{
		_bool				isStatic;
		_uint				iNumMeshes;			// 메쉬 개수
		_uint				iNumMaterials;		// 머테리얼 개수
		_uint				iNumAnimations;		// 애니메이션 개수

		psBONE*				Bones;				// 뼈 배열
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
