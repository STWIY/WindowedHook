#include <Windows.h>
#include <d3d9.h>
#include <cstdint>

//===============================================================================

struct tContextInitData // Sizeof(0x20)
{
    UINT m_Width;
    UINT m_Height;
    UINT m_Depth;
    UINT m_Unknown0;
    UINT m_Unknown1;
    D3DMULTISAMPLE_TYPE m_MultiSampleType;
    bool m_Windowed;
};

namespace Pure3DHook
{
    typedef BOOL(__thiscall* Fn_CreateD3DRender)(void*, void*);
    Fn_CreateD3DRender g_NewRenderContext;

    BOOL __fastcall NewRenderContext(void* ecx, void* edx, tContextInitData* p_Param)
    {
        p_Param->m_Windowed = true;
        return g_NewRenderContext(ecx, p_Param);
    }
}

//===============================================================================

void CreateVFuncHook(void* p_VFunc, void* p_Hook, void** p_Original)
{
    if (p_Original) {
        *p_Original = *reinterpret_cast<void**>(p_VFunc);
    }

    DWORD _OldProtect;
    if (!VirtualProtect(p_VFunc, sizeof(void*), PAGE_READWRITE, &_OldProtect)) {
        return;
    }

    *reinterpret_cast<void**>(p_VFunc) = p_Hook;
    VirtualProtect(p_VFunc, sizeof(void*), _OldProtect, &_OldProtect);
}

//===============================================================================

int __stdcall DllMain(HMODULE p_Module, DWORD p_Reason, void* p_Reserved)
{
    if (p_Reason == DLL_PROCESS_ATTACH) {
        CreateVFuncHook(reinterpret_cast<void*>(0x767E6C), Pure3DHook::NewRenderContext, (void**)&Pure3DHook::g_NewRenderContext);
    }
    else if (p_Reason == DLL_PROCESS_DETACH) {
        CreateVFuncHook(reinterpret_cast<void*>(0x767E6C), Pure3DHook::g_NewRenderContext, 0);
    }

    return 1;
}
