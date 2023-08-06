// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mem.h"
#include "proc.h"

// Created with ReClass.NET 1.2 by KN4CK3R

struct Vector3 { float x, y, z; };

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}


class weapon
{
public:
    union
    {
        DEFINE_MEMBER_N(int*, ammoptr, 0x0014);
    };
};

class ent
{
public:
    union
    {
        DEFINE_MEMBER_N(Vector3, poshead, 0x0004);
        DEFINE_MEMBER_N(Vector3, pos, 0x0034);
        DEFINE_MEMBER_N(Vector3, angle, 0x0040);
        DEFINE_MEMBER_N(int32_t, health, 0x00F8);
        DEFINE_MEMBER_N(int32_t, armor, 0x00FC);
        DEFINE_MEMBER_N(weapon*, curweapon, 0x0374);
    };
};

DWORD WINAPI HackThread(HMODULE hModule)
{
    // create console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Hi\n";

    // get module base
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    bool bHealth = false, bAmmo = false, bRecoil = false, bArmor = false;

    // hack loop
    while (true)
    {
        // key input
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }

        if (GetAsyncKeyState(VK_F1) & 1)
        {
            bHealth = !bHealth;
        }

        if (GetAsyncKeyState(VK_F2) & 1)
        {
            bAmmo = !bAmmo;
        }

        if (GetAsyncKeyState(VK_F3) & 1)
        {
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
            }
            else
            {
                mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10);
            }
        }

        if (GetAsyncKeyState(VK_F4) & 1)
        {
            bArmor = !bArmor;
        }

        // continus write/freeze
        ent* localPlayer = *(ent**)(moduleBase + 0x10f4f4);

        if (localPlayer)
        {
            if (bHealth)
            {
                localPlayer->health = 1337;
            }

            if (bArmor)
            {
                localPlayer->armor = 1337;
            }

            if (bAmmo)
            {
                *localPlayer->curweapon->ammoptr = 1337;
            }
        }
        Sleep(5);
    }

    // cleanup & eject
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

