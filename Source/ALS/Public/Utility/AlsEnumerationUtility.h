#pragma once

template <typename Enumeration>
static int32 GetEnumValueIndex(const Enumeration Value)
{
	return StaticEnum<Enumeration>()->GetIndexByValue(static_cast<int64>(Value));
}

template <typename Enumeration>
static FString GetEnumValueString(const Enumeration Value)
{
	return StaticEnum<Enumeration>()->GetNameStringByValue(static_cast<int64>(Value));
}
