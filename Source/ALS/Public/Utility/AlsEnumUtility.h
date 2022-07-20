#pragma once

#include "Containers/UnrealString.h"
#include "UObject/ReflectedTypeAccessors.h"

namespace AlsEnumUtility
{
	template <typename EnumType>
	constexpr int32 GetIndexByValue(const EnumType Value)
	{
		return StaticEnum<EnumType>()->GetIndexByValue(static_cast<int64>(Value));
	}

	template <typename EnumType>
	constexpr FString GetNameStringByValue(const EnumType Value)
	{
		return StaticEnum<EnumType>()->GetNameStringByValue(static_cast<int64>(Value));
	}
}
