#pragma once

#include <string>
#include <Windows.h>
#include <locale>
#include "enums.hpp"

struct UObject;

template <class T>
struct TArray
{
	friend struct FString;

public:

	T* Data;
	int32_t Count;
	int32_t Max;

	TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	int Num() const
	{
		return Count;
	};

	T& operator[](int i)
	{
		return Data[i];
	};

	const T& operator[](int i) const
	{
		return Data[i];
	};

	bool IsValidIndex(int i) const
	{
		return i < Num();
	}

	int Add(T* NewItem)
	{
		Count = Count + 1;
		Max = Max + 1;
		Data = static_cast<T**>(malloc(Count * sizeof(T*)));
		Data[Count - 1] = NewItem;
		return Count;
	}
};

struct FString : private TArray<wchar_t>
{
	FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	}

	bool IsValid() const
	{
		return Data != nullptr;
	}

	const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

template <class TEnum>
class TEnumAsByte
{
public:
	TEnumAsByte()
	{
	}

	TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit TEnumAsByte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit TEnumAsByte(uint8_t _value)
		: value(_value)
	{
	}

	operator TEnum() const
	{
		return static_cast<TEnum>(value);
	}

	TEnum GetValue() const
	{
		return static_cast<TEnum>(value);
	}

private:
	uint8_t value;
};

struct FName;

inline void (*FNameToString)(FName* _this, FString& out);
inline void* (*FreeMemory)(void*);

struct FName
{
	uint32_t ComparisonIndex;
	uint32_t DisplayIndex;

	FName() = default;

	explicit FName(int64_t name)
	{
		DisplayIndex = (name & 0xFFFFFFFF00000000LL) >> 32;
		ComparisonIndex = (name & 0xFFFFFFFFLL);
	};

	auto ToString()
	{
		FString temp;
		FNameToString(this, temp);

		std::wstring ret(temp.c_str());
		FreeMemory((void*)temp.c_str());

		return ret;
	}
};

struct UClass;

struct UObject
{
	void** vftable;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	FName Name;
	UObject* Outer;

	void ProcessEvent(void* Function, void* Params)
	{
		auto Vftable = *reinterpret_cast<void***>(this);
		auto ProcessEventFN = static_cast<void(*)(void*, void*, void*)>(Vftable[0x44]);
		ProcessEventFN(this, Function, Params);
	}

	bool IsA(UClass* cmp) const
	{
		if (Class == cmp)
			return true;
		return false;
	}

	std::string GetName()
	{
		auto name = Name.ToString();
		return std::string(name.begin(), name.end());
	}

	std::string GetFullName()
	{
		std::string temp;

		for (auto outer = Outer; outer; outer = outer->Outer)
		{
			temp = outer->GetName() + "." + temp;
		}

		temp = reinterpret_cast<UObject*>(Class)->GetName() + " " + temp + this->GetName();
		return temp;
	}

	FName GetFName() const
	{
		return *reinterpret_cast<const FName*>(this + 0x18);
	}
};

struct FUObjectItem
{
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
	DWORD SerialNumber2;
};

struct PreFUObjectItem
{
	FUObjectItem* FUObject[10];
};

struct FUObjectArray
{
	PreFUObjectItem* ObjectArray;
	BYTE unknown1[8];
	int32_t MaxElements;
	int32_t NumElements;

	void NumChunks(int* start, int* end) const
	{
		int cStart = 0, cEnd = 0;

		if (!cEnd)
		{
			while (true)
			{
				if (ObjectArray->FUObject[cStart] == nullptr)
				{
					cStart++;
				}
				else
				{
					break;
				}
			}

			cEnd = cStart;
			while (true)
			{
				if (ObjectArray->FUObject[cEnd] == nullptr)
				{
					break;
				}
				cEnd++;
			}
		}

		*start = cStart;
		*end = cEnd;
	}

	UObject* GetByIndex(int32_t index) const
	{
		int cStart = 0, cEnd = 0;
		int chunkIndex, chunkSize = 0xFFFF, chunkPos;
		FUObjectItem* Object;

		NumChunks(&cStart, &cEnd);

		chunkIndex = index / chunkSize;
		if (chunkSize * chunkIndex != 0 &&
			chunkSize * chunkIndex == index)
		{
			chunkIndex--;
		}

		chunkPos = cStart + chunkIndex;
		if (chunkPos < cEnd)
		{
			Object = ObjectArray->FUObject[chunkPos] + (index - chunkSize * chunkIndex);
			if (!Object) { return nullptr; }

			return Object->Object;
		}

		return nullptr;
	}
};

struct FField
{
	void* vtable;
	void* Class;
	void* Owner;
	void* padding;
	FField* Next;
	FName NamePrivate;
	DWORD FlagsPrivate;

	std::string GetName()
	{
		auto name = NamePrivate.ToString();
		return std::string(name.begin(), name.end());
	}

	std::string GetTypeName()
	{
		auto name = (*static_cast<FName*>(Class)).ToString();
		return std::string(name.begin(), name.end());
	}

	std::string GetFullName()
	{
		std::string temp;

		for (auto outer = Next; outer; outer = outer->Next)
		{
			temp = outer->GetName() + "." + temp;
		}

		temp = static_cast<UObject*>(Class)->GetName() + " " + temp + this->GetName();
		return temp;
	}
};

struct FProperty : FField
{
	int32_t ArrayDim;
	int32_t ElementSize;
	DWORD PropertyFlags;
	uint16_t RepIndex;
	TEnumAsByte<DWORD> BlueprintReplicationCondition;
	int32_t Offset_Internal;
	FName RepNotifyFunc;
	FProperty* PropertyLinkNext;
	FProperty* NextRef;
	FProperty* DestructorLinkNext;
	FProperty* PostConstructLinkNext;

	bool IsValid() const
	{
		return this != nullptr;
	}
};

struct UField : UObject
{
	UField* Next;
	void* padding_01;
	void* padding_02;
};

struct UStruct : UField
{
	UStruct* SuperStruct;
	UField* Children;
	FField* ChildProperties;
	int32_t PropertiesSize;
	int32_t MinAlignment;
	TArray<uint8_t> Script;
	FProperty* PropertyLink;
	FProperty* RefLink;
	FProperty* DestructorLink;
	FProperty* PostConstructLink;
};

struct UClass : UStruct
{
};

struct FVector
{
	float X;
	float Y;
	float Z;

	FVector()
		: X(0), Y(0), Z(0)
	{
	}

	FVector(float x, float y, float z)
		: X(x),
		Y(y),
		Z(z)
	{
	}

	FVector operator-(FVector v)
	{
		return FVector(X - v.X, Y - v.Y, Z - v.Z);
	}

	FVector operator+(FVector v)
	{
		return FVector(X + v.X, Y + v.Y, Z + v.Z);
	}

	float Distance(FVector v)
	{
		return ((X - v.X) * (X - v.X) +
			(Y - v.Y) * (Y - v.Y) +
			(Z - v.Z) * (Z - v.Z));
	}
};

struct FRotator
{
	float Pitch;
	float Yaw;
	float Roll;

	FRotator()
		: Pitch(0),
		Yaw(0),
		Roll(0)
	{
	}

	FRotator(float pitch, float yaw, float roll)
		: Pitch(pitch),
		Yaw(yaw),
		Roll(roll)
	{
	}
};

struct FQuat
{
	float W, X, Y, Z;

	FQuat()
		: W(0),
		X(0),
		Y(0),
		Z(0)
	{
	}

	FQuat(float w, float x, float y, float z)
		: W(w),
		X(x),
		Y(y),
		Z(z)
	{
	}
};

struct FTransform
{
	FQuat Rotation;
	FVector Translation;
	char UnknownData_1C[0x4];
	FVector Scale3D;
	char UnknownData_2C[0x4];
};

struct FLinearColor
{
	float R;
	float G;
	float B;
	float A;

	FLinearColor()
		: R(0), G(0), B(0), A(0)
	{
	}

	FLinearColor(float r, float g, float b, float a)
		: R(r),
		G(g),
		B(b),
		A(a)
	{
	}
	// Enum Engine.ESpawnActorCollisionHandlingMethod
	enum class ESpawnActorCollisionHandlingMethod : uint8_t
	{
		Undefined = 0,
		AlwaysSpawn = 1,
		AdjustIfPossibleButAlwaysSpawn = 2,
		AdjustIfPossibleButDontSpawnIfColliding = 3,
		DontSpawnIfColliding = 4,
		ESpawnActorCollisionHandlingMethod_MAX = 5
	};
};

struct UGameplayStatics_BeginDeferredActorSpawnFromClass_Params
{
	class UObject* WorldContextObject;                                       // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
	class UClass* ActorClass;                                               // (Parm, ZeroConstructor, IsPlainOldData)
	struct FTransform                                  SpawnTransform;                                           // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
	ESpawnActorCollisionHandlingMethod                 CollisionHandlingOverride;                                // (Parm, ZeroConstructor, IsPlainOldData)
	class AActor* Owner;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
	class AActor* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

struct UGameplayStatics_FinishSpawningActor_Params
{
	class UObject* Actor;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
	struct FTransform                                  SpawnTransform;                                           // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
	class AActor* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

struct FActorSpawnParameters
{
	unsigned char Unk00[0x40];
};

struct SpawnObjectParams
{
	UObject* ObjectClass;
	UObject* Outer;
	UObject* ReturnValue;
};

struct FGuid
{
	int A;
	int B;
	int C;
	int D;
};

struct FHitResult
{
};

enum class EFortMovementStyle : uint8_t
{
	Running = 0,
	Walking = 1,
	Charging = 2,
	Sprinting = 3,
	PersonalVehicle = 4,
	Flying = 5,
	Tethered = 6,
	EFortMovementStyle_MAX = 7
};

enum class EFortCustomPartType : uint8_t
{
	Head = 0,
	Body = 1,
	Hat = 2,
	Backpack = 3,
	Charm = 4,
	Face = 5,
	NumTypes = 6,
	EFortCustomPartType_MAX = 7
};

struct USkinnedMeshComponent_SetSkeletalMesh_Params
{
	UObject* NewMesh;
	bool bReinitPose;
};
