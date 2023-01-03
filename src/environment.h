// 32/64 Bit, Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define FREEMAP_ENVIRONMENT64
#else
#define FREEMAP_ENVIRONMENT32
#endif
#endif

// 32/64 Bit, Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define FREEMAP_ENVIRONMENT64
#else
#define FREEMAP_ENVIRONMENT32
#endif
#endif