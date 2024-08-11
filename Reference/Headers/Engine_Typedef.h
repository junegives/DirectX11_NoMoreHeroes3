#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;
	typedef		char						_char;

	typedef		wchar_t						_tchar;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;	

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;

	typedef		XMUINT2						_uint2;
	typedef		XMUINT3						_uint3;
	typedef		XMUINT4						_uint4;
	
	/* �����͸� �����ϱ����� ���� .*/
	typedef		XMFLOAT2					_float2;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT4					_float4;

	/* ������ �ϱ����� ����. */
	typedef		XMVECTOR					_vector;
	typedef		FXMVECTOR					_fvector;
	typedef		GXMVECTOR					_gvector;
	typedef		HXMVECTOR					_hvector;
	typedef		CXMVECTOR					_cvector;

	/* �����͸� �����ϱ����� ���.*/
	typedef		XMFLOAT4X4					_float4x4;

	/* ������ �ϱ����� ���. */
	typedef		XMMATRIX					_matrix;
	typedef		FXMMATRIX					_fmatrix;
	typedef		CXMMATRIX					_cmatrix;

}

#endif // Engine_Typedef_h__
