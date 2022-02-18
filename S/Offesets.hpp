#pragma once

#include <Windows.h>

namespace Offsets
{
	namespace Engine
	{
		inline __int64 GameViewport = 0x780;
		inline __int64 ConsoleClass = 0x0F8;
	}

	namespace GameViewportClient
	{
		inline __int64 GameInstance = 0x80;
		inline __int64 World = 0x78;
		inline __int64 ViewportConsole = 0x40;
	}

	namespace GameInstance
	{
		inline __int64 LocalPlayers = 0x38;
	}

	namespace LocalPlayer
	{
		inline __int64 PlayerController = 0x30;
	}

	namespace PlayerController
	{
		inline __int64 Pawn = 0x250;
		inline __int64 CheatManager = 0x338;
		inline __int64 PlayerState = 0x228;
		inline __int64 AcknowledgedPawn = 0x2A0;
		inline __int64 PlayerInput = 0x348;
	}

	namespace World
	{
		inline __int64 PersistentLevel = 0x30;
		inline __int64 GameState = 0x128;
		inline __int64 AuthorityGameMode = 0x120;
		inline __int64 OwningGameInstance = 0x188;
	}

	namespace PlayerState
	{
		inline __int64 CharacterData = 0x4F8;
	}

	namespace CharacterData
	{
		inline __int64 Parts = 0x08;
	}

	namespace Pawn
	{
		inline __int64 PlayerState = 0x240;
	}

	namespace Inventory
	{
		inline __int64 Inventory = 0x230;
	}

	namespace FortItemList
	{
		inline __int64 ReplicatedEntries = 0x108;
		inline __int64 ItemInstances = 0x168;
	}

	namespace FortItemEntry
	{
		inline __int64 Count = 0xC;
		inline __int64 ItemGuid = 0x68;
		inline __int64 ItemDefinition = 0x18;
		inline __int64 LoadedAmmo = 0x2C;
	}

	namespace FortWorldItem
	{
		inline __int64 ItemEntry = 0xE0;
	}

	namespace FortHero
	{
		inline __int64 CharacterParts = 0x238;
	}

	namespace FortPawn
	{
		inline __int64 CurrentMovementStyle = 0x563;
	}

	namespace FortPlayerState
	{
		inline __int64 CurrentHealth = 0xD34;
		inline __int64 MaxHealth = 0xD38;
		inline __int64 CurrentSheild = 0xD3C;
		inline __int64 MaxSheild = 0xD40;
	}

	namespace Character
	{
		inline __int64 Mesh = 0x280;
	}
}
