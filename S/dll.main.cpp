#include <Windows.h>
#include "Util.hpp"
#include <iostream>
#include "Structs.hpp"
#include "Unreal.hpp"
#include "minhook/MinHook.h"
#include "Hooks.hpp"
#include "Offsets.hpp"
#include "Globals.hpp"
#include "Functions.hpp"

#pragma comment(lib, "minhook/libMinHook.x64.lib")

DWORD WINAPI ObjectDumpThread(LPVOID)
{
    return 0;
}

DWORD WINAPI UpdateThread(LPVOID)
{
    while (1)
    {
        if (GetAsyncKeyState(VK_F1) && 0x01)
        {
            Functions::SwitchLevel(L"Apollo_Terrain?Game=/Script/Engine.GameModeBase");
            //Functions::InitCheatManager();
            //Functions::DestroyAll("FortHLODSMActor");
            //Functions::SetupCharacterParts();
        }

        if (GetAsyncKeyState(VK_F2) && 0x01)
        {
            Globals::Pawn = FindObject("PlayerPawn_Generic_C /Game/Athena/Apollo/Maps/Apollo_Terrain.Apollo_Terrain.PersistentLevel.PlayerPawn_Generic_C_");
            std::cout << Globals::Pawn->GetFullName() << std::endl;

            Globals::SetupGlobals();

            CreateThread(0, 0, MovementHook, 0, 0, 0);
        }

        if (GetAsyncKeyState(VK_F3) && 0x01)
        {
        }

        if (GetAsyncKeyState(VK_F4) && 0x01)
        {
            DumpObjects();
        }
    }
}

DWORD WINAPI MainThread(LPVOID)
{
    Util::InitConsole();

    std::cout << "[Sodium]: Finding Patterns!\n";

    GObjectsAddr = (uintptr_t)Util::FindPattern("\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x0C\xC8\x48\x8D\x04\xD1\xEB\x06", "xxx????xxxxxxxxxx");
    if (!GObjectsAddr)
    {
        std::cout << "[Sodium]: Failed to find GObjects address!\n";
        return 0;
    }
    auto GObjectsOffset = *reinterpret_cast<uint32_t*>(GObjectsAddr + 3);
    auto GObjectsAdded = GObjectsAddr + 7 + GObjectsOffset;
    GObjectsAddr = GObjectsAdded;

    GObjects = decltype(GObjects)(GObjectsAdded);
    //std::cout << "GObjects: " << GObjects << std::endl; //Testing
    std::cout << "[Sodium]: GObjects Done!\n";

    FreeMemoryAddr = (uintptr_t)Util::FindPattern("\x48\x85\xC9\x0F\x84\x00\x00\x00\x00\x53\x48\x83\xEC\x20", "xxxxx????xxxxx");
    if (!FreeMemoryAddr)
    {
        std::cout << "[Sodium]: Failed to find FreeMemory address!\n";
        return 0;
    }

    FreeMemory = decltype(FreeMemory)(FreeMemoryAddr);

    //std::cout << "FreeMemory: " << FreeMemory << std::endl;
    std::cout << "[Sodium]: FreeMemory Done!\n";

    FNameToStringAddr = (uintptr_t)Util::FindPattern("\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x55\x57\x41\x56\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x45\x33\xF6\x48\x8B\xF2\x44\x39\x71\x04\x0F\x85\x00\x00\x00\x00\x8B\x19\x0F\xB7\xFB\xE8\x00\x00\x00\x00\x8B\xCB\x48\x8D\x54\x24\x00\x48\xC1\xE9\x10\x8D\x1C\x3F\x48\x03\x5C\xC8\x00\x48\x8B\xCB\xF6\x03\x01\x0F\x85\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x0F\xB7\x1B\xC1\xEB\x06\x4C\x89\x36\x4C\x89\x76\x08\x85\xDB\x74\x48\x44\x38\x74\x24\x00\x74\x41\x8D\x53\x01\x48\x8B\xCE\xE8\x00\x00\x00\x00\x8B\x4E\x08\xFF\xC1\x03\xCB\x89\x4E\x08\x3B\x4E\x0C\x7E\x08\x48\x8B\xCE\xE8\x00\x00\x00\x00", "xxxx?xxxx?xxxxxxxx????xxx????xxx????xxxxxx????xxxxxxxxxxxx????xxxxxx????xxxxxx?xxxxxxxxxxx?xxxxxxxx????x????xxxxxxxxxxxxxxxxxxxxx?xxxxxxxxx????xxxxxxxxxxxxxxxxxxx????");
    if (!FNameToStringAddr)
    {
        std::cout << "[Sodium]: Failed to find FNameToString address!\n";
        return 0;
    }

    FNameToString = decltype(FNameToString)(FNameToStringAddr);
    //std::cout << "FNameToString: " << FNameToString << std::endl; //Testing
    std::cout << "[Sodium]: FNameToString Done!\n";

    FortEngine = FindObject("FortEngine_");
    auto FortEngineVftable = *reinterpret_cast<void***>(FortEngine);

    auto ProcessEventAddr = (uintptr_t)FortEngineVftable[0x44];
    if (!ProcessEventAddr)
    {
        std::cout << "[Sodium]: Failed to find ProcessEvent address!\n";
        return 0;
    }

    ProcessEvent = decltype(ProcessEvent)(ProcessEventAddr);

    //std::cout << "ProcessEvent: " << ProcessEvent << std::endl; //Testing

    if (MH_Initialize() != MH_OK)
    {
        std::cout << "[Sodium]: Failed to setup minhook!\n";
        return 0;
    }
    SetupAntiClose();

    MH_CreateHook(reinterpret_cast<void*>(ProcessEventAddr), ProcessEventHook, reinterpret_cast<void**>(&ProcessEvent));
    MH_EnableHook(reinterpret_cast<void*>(ProcessEventAddr));

    std::cout << "[Sodium]: ProcessEvent Done!\n";

    auto SpawnActorAddr = (uintptr_t)Util::FindPattern("\x48\x8B\xC4\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x70\xA8\x0F\x29\x78\x98\x44\x0F\x29\x40\x00\x44\x0F\x29\x88\x00\x00\x00\x00\x44\x0F\x29\x90\x00\x00\x00\x00\x44\x0F\x29\x98\x00\x00\x00\x00\x44\x0F\x29\xA0\x00\x00\x00\x00\x44\x0F\x29\xA8\x00\x00\x00\x00\x44\x0F\x29\xB0\x00\x00\x00\x00\x44\x0F\x29\xB8\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\x50\x45\x33\xED", "xxxxxxxxxxxxxxxxxx????xxx????xxxxxxxxxxxx?xxxx????xxxx????xxxx????xxxx????xxxx????xxxx????xxxx????xxx????xxxxxxxxxx");
    if (!SpawnActorAddr)
    {
        std::cout << "[Sodium]: Failed to find SpawnActor address!\n";
        return 0;
    }

    SpawnActorLong = decltype(SpawnActorLong)(SpawnActorAddr);

    MH_CreateHook(reinterpret_cast<void*>(SpawnActorAddr), SpawnActorHook, reinterpret_cast<void**>(&SpawnActorLong));
    MH_EnableHook(reinterpret_cast<void*>(SpawnActorAddr));

    std::cout << "[Sodium]: SpawnActor Done!\n";

    auto AthenaGameModeCrashAddr = Util::FindPattern("\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x20\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\xDA\x48\x8B\xF9\x48\x85\xC0\x0F\x84\x00\x00\x00\x00", "xxxx?xxxxxxxx????xxxxxxxxxxx????");
    if (!AthenaGameModeCrashAddr)
    {
        std::cout << "[Sodium]: Failed to find AthenaGameModeCrash address\n";
        return 0;
    }

    //AthenaGameModeCrash = decltype(AthenaGameModeCrash)(AthenaGameModeCrashAddr);

    //MH_CreateHook((void*)AthenaGameModeCrashAddr, AthenaGameModeCrashHook, (void**)(&AthenaGameModeCrash));
    //MH_EnableHook((void*)AthenaGameModeCrashAddr);

    std::cout << "[Sodium]: AthenaGameModeCrash Done!\n";

    Globals::SetupGlobals();
    Functions::InitConsole();

    //Functions::InitCheatManager();
    //Functions::SwitchLevel(L"Apollo_Terrain?Game=/Script/Engine.GameModeBase");

    CreateThread(0, 0, UpdateThread, 0, 0, 0);

    std::cout << "[Sodium]: Setup!\n";

    return 0;
}

BOOL APIENTRY DllMain(HMODULE mod,DWORD reason,LPVOID res)
{
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, MainThread, mod, 0, 0);

    return TRUE;
}
