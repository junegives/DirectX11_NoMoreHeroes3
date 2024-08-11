#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

#define NO_COPY(CLASSNAME)										\
		private:												\
		CLASSNAME(const CLASSNAME&);							\
		CLASSNAME& operator = (const CLASSNAME&);				

#define DECLARE_SINGLETON(CLASSNAME)							\
		NO_COPY(CLASSNAME)										\
		private:												\
		static CLASSNAME*	m_pInstance;						\
		public:													\
		static CLASSNAME*	GetInstance( void );				\
		static unsigned int DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {						\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned int CLASSNAME::DestroyInstance( void ) {		\
			unsigned int iRefCnt = 0;							\
			if(nullptr != m_pInstance)	{						\
				iRefCnt = m_pInstance->Release();				\
				if(0 == iRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return iRefCnt;										\
		}

#define DECLARE_SINGLETON_SMART(CLASSNAME)							\
		NO_COPY(CLASSNAME)										\
		private:												\
		static shared_ptr<CLASSNAME>	m_pInstance;						\
		public:													\
		static shared_ptr<CLASSNAME>	GetInstance( void );				\
		static unsigned int DestroyInstance( void );			

#define IMPLEMENT_SINGLETON_SMART(CLASSNAME)							\
		shared_ptr<CLASSNAME>	CLASSNAME::m_pInstance = nullptr;			\
		shared_ptr<CLASSNAME>	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {						\
				m_pInstance = make_shared<CLASSNAME>();					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned int CLASSNAME::DestroyInstance( void ) {		\
			m_pInstance = nullptr;						\
			return 0;											\
		}


#endif // Engine_Macro_h__
