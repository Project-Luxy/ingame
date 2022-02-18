#pragma once

#include "Unreal.hpp"
#include "Globals.hpp"
#include "Structs.hpp"
#include "Offsets.hpp"
#include "enums.hpp"

namespace Functions
{
	static void InitConsole() {
		SpawnObjectParams params;

		params.ObjectClass = FindObject("Class /Script/FortniteGame.FortConsole");
		params.Outer = Globals::GameViewport;

		auto GameplayStatics = FindObject("Default__GameplayStatics");
		static auto fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");
		ProcessEvent(GameplayStatics, fn, &params);

		UObject** ViewportConsole = reinterpret_cast<UObject**>(__int64(Globals::GameViewport) + Offsets::GameViewportClient::ViewportConsole);
		*ViewportConsole = params.ReturnValue;
	}

	static void InitCheatManager() {
		/*SpawnObjectParams params;
		params.ObjectClass = FindObject("Class /Script/Engine.CheatManager");
		params.Outer = Globals::PlayerController;
		auto GameplayStatics = FindObject("Default__GameplayStatics");
		static auto fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");
		ProcessEvent(GameplayStatics, fn, &params);
		UObject** CheatManager = reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + Offsets::PlayerController::CheatManager);
		*CheatManager = params.ReturnValue;*/
		Globals::CheatMananger = *reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + Offsets::PlayerController::CheatManager);
	}

	static inline void SwitchLevel(FString URL)
	{
		static auto fn = FindObject("Function /Script/Engine.PlayerController.SwitchLevel");
		ProcessEvent(Globals::PlayerController, fn, &URL);
	}

	static void Possess(UObject* Inpawn) {
		auto PossessFunc = FindObject("Function /Script/Engine.Controller.Possess");
		struct
		{
			class UObject* InPawn;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
		} possesparams;
		possesparams.InPawn = Inpawn;
		ProcessEvent(Globals::PlayerController, PossessFunc, &possesparams);
	}

	static inline UObject* SpawnActor(UObject* ActorClass, FVector loc) {
		auto spawnfunction1 = FindObject("Function /Script/Engine.GameplayStatics.BeginDeferredActorSpawnFromClass");
		auto spawnfunction2 = FindObject("Function /Script/Engine.GameplayStatics.FinishSpawningActor");

		UGameplayStatics_BeginDeferredActorSpawnFromClass_Params bdasfc;
		UGameplayStatics_FinishSpawningActor_Params fsap;

		bdasfc.ActorClass = (UClass*)ActorClass;
		bdasfc.CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		bdasfc.SpawnTransform.Rotation = FQuat{ 0,0,0,0 };
		bdasfc.SpawnTransform.Scale3D = FVector{ 1,1,1 };
		bdasfc.SpawnTransform.Translation = loc;
		bdasfc.WorldContextObject = Globals::World;
		bdasfc.Owner = nullptr;

		auto GameplayStatics = FindObject("Default__GameplayStatics");

		std::cout << GameplayStatics->GetFullName();
		std::cout << spawnfunction1->GetFullName();

		ProcessEvent(GameplayStatics, spawnfunction1, &bdasfc);

		fsap.Actor = (UObject*)bdasfc.ReturnValue;
		fsap.SpawnTransform = bdasfc.SpawnTransform;

		ProcessEvent(GameplayStatics, spawnfunction2, &fsap);

		return (UObject*)fsap.ReturnValue;
	}

	static inline void DestroyAll(const char* ClassToDestroy)
	{
		UObject* LocatedClass = FindObject(ClassToDestroy);
		ProcessEvent(Globals::CheatMananger, FindObject("Function /Script/Engine.CheatManager.DestroyAll"), &LocatedClass);
	}

	static inline void StartMatch()
	{
		static auto fn = FindObject("Function /Script/Engine.GameMode.StartMatch");
		ProcessEvent(Globals::GameMode, fn, nullptr);
	}

	static inline void ServerReadyToStartMatch()
	{
		static auto fn = FindObject("Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch");
		ProcessEvent(Globals::PlayerController, fn, nullptr);
	}

	static inline void SetMaxHealth()
	{
		auto PlayerState = *reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + Offsets::Pawn::PlayerState);
		auto CurrentHealth = reinterpret_cast<float*>(__int64(PlayerState) + Offsets::FortPlayerState::CurrentHealth);
		auto MaxHealth = reinterpret_cast<float*>(__int64(PlayerState) + Offsets::FortPlayerState::MaxHealth);
		auto CurrentSheild = reinterpret_cast<float*>(__int64(PlayerState) + Offsets::FortPlayerState::CurrentSheild);
		auto MaxSheild = reinterpret_cast<float*>(__int64(PlayerState) + Offsets::FortPlayerState::MaxSheild);
		*CurrentHealth = 100;
		*MaxHealth = 100;
		*CurrentSheild = 100;
		*MaxSheild = 100;
	}

	static inline void AddMovementInput(UObject* Pawn, FVector Loc, float ScaleValue, bool bForce)
	{
		struct Params
		{
			struct FVector WorldDirection;
			float ScaleValue;
			bool bForce;
		};
		Params params;
		params.WorldDirection = Loc;
		params.ScaleValue = ScaleValue;
		params.bForce = bForce;

		static auto fn = FindObject("Function /Script/Engine.Pawn.AddMovementInput");
		ProcessEvent(Pawn, fn, &params);
	}

	static inline FVector GetActorRightVector(UObject* Actor)
	{
		struct Params
		{
			struct FVector ReturnValue;
		};
		Params params;

		static auto fn = FindObject("Function /Script/Engine.Actor.GetActorRightVector");
		ProcessEvent(Actor, fn, &params);

		return params.ReturnValue;
	}

	static inline FVector GetActorForwardVector(UObject* Actor)
	{
		struct Params
		{
			struct FVector ReturnValue;
		};
		Params params;

		static auto fn = FindObject("Function /Script/Engine.Actor.GetActorForwardVector");
		ProcessEvent(Actor, fn, &params);

		return params.ReturnValue;
	}

	static inline bool K2_SetActorLocation(UObject* Actor, FVector Location)
	{
		struct Params
		{
			struct FVector NewLocation;
			bool bSweep;
			struct FHitResult SweepHitResult;
			bool bTeleport;
			bool ReturnValue;
		};
		Params params;
		params.NewLocation = Location;
		params.bSweep = true;
		params.SweepHitResult = FHitResult();
		params.bTeleport = true;

		static auto fn = FindObject("Function /Script/Engine.Actor.K2_SetActorLocation");
		ProcessEvent(Actor, fn, &params);

		return params.ReturnValue;
	}

	static inline bool bIsJumpProvidingForce(UObject* Pawn)
	{
		struct Params
		{
			bool ReturnValue;
		};
		Params params;

		static auto fn = FindObject("Function /Script/Engine.Character.IsJumpProvidingForce");
		ProcessEvent(Pawn, fn, &params);

		return params.ReturnValue;
	}

	static inline void Jump(UObject* Pawn)
	{
		static auto fn = FindObject("Function /Script/Engine.Character.Jump");
		ProcessEvent(Pawn, fn, nullptr);
	}

	static void ServerChoosePart(UObject* Target, TEnumAsByte<EFortCustomPartType> Type, UObject* ChosenCharacterPart)
	{
		static UObject* ServerChoosePart = FindObject("Function /Script/FortniteGame.FortPlayerPawn.ServerChoosePart");

		struct
		{
			TEnumAsByte<EFortCustomPartType> Type;
			UObject* ChosenCharacterPart;
		} Params;

		Params.Type = Type;
		Params.ChosenCharacterPart = ChosenCharacterPart;

		ProcessEvent(Target, ServerChoosePart, &Params);
	}

	static void SetOwner(UObject* TargetActor, UObject* NewOwner)
	{
		static UObject* SetOwner = FindObject("Function /Script/Engine.Actor.SetOwner");
		ProcessEvent(TargetActor, SetOwner, &NewOwner);
	}

	struct QuickbarSlot
	{
		TArray<struct FGuid> Items;
		bool bEnabled;
		char Unk00[0x7];
	};

	struct InventoryPointer
	{
		UObject* Inventory;
	};

	struct QuickBarPointer
	{
		UObject* QuickBar;
	};

	UObject* QuickBars;
	UObject* Inventory;

	static void SetupInventory(UObject* Controller)
	{
		QuickBars = (UObject*)(SpawnActor(FindObject("Class /Script/FortniteGame.FortQuickBars"), FVector{ 0,0,38923 }));
		reinterpret_cast<QuickBarPointer*>(Controller)->QuickBar = QuickBars;
		SetOwner(QuickBars, Controller);


	}
}