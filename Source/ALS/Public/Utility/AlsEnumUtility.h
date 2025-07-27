#pragma once

#include "Containers/UnrealString.h"
#include "UObject/ReflectedTypeAccessors.h"

namespace AlsEnumUtility
{
	template <class EnumType>
	concept CIsUEnumClass = static_cast<bool>(TIsUEnumClass<EnumType>::Value);

	template <CIsUEnumClass EnumType>
	int32 GetIndexByValue(const EnumType Value)
	{
		return StaticEnum<EnumType>()->GetIndexByValue(static_cast<int64>(Value));
	}

	template <CIsUEnumClass EnumType>
	FString GetNameStringByValue(const EnumType Value)
	{
		return StaticEnum<EnumType>()->GetNameStringByValue(static_cast<int64>(Value));
	}
}
