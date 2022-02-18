#pragma once

#include "Structs.hpp"
#include "Unreal.hpp"
#include "Offsets.hpp"

namespace Globals
{
    UObject* GameViewport;
    UObject* GameInstance;
    TArray<UObject*> LocalPlayers;
    UObject* LocalPlayer;
    UObject* PlayerController;
    UObject* CheatMananger;
    UObject* World;
    UObject* GameMode;
    UObject* GameState;
    UObject* Pawn;
    UObject* PlayerState;
    UObject* FortInventory;
    UObject* QuickBars;

	inline static void SetupGlobals()
	{
        GameViewport = *reinterpret_cast<UObject**>(__int64(FortEngine) + Offsets::Engine::GameViewport);
        GameInstance = *reinterpret_cast<UObject**>(__int64(GameViewport) + Offsets::GameViewportClient::GameInstance);
        LocalPlayers = *reinterpret_cast<TArray<UObject*>*>(__int64(GameInstance) + Offsets::GameInstance::LocalPlayers);
        LocalPlayer = LocalPlayers[0];
        PlayerController = *reinterpret_cast<UObject**>(__int64(LocalPlayer) + Offsets::LocalPlayer::PlayerController);
        World = *reinterpret_cast<UObject**>(__int64(GameViewport) + Offsets::GameViewportClient::World);
        GameMode = *reinterpret_cast<UObject**>(__int64(World) + Offsets::World::AuthorityGameMode);
        GameState = *reinterpret_cast<UObject**>(__int64(World) + Offsets::World::GameState);
	}
}
