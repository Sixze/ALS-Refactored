#pragma once

template <typename Enumeration>
static constexpr int32 GetEnumValueIndex(const Enumeration Value)
{
	return StaticEnum<Enumeration>()->GetIndexByValue(static_cast<int64>(Value));
}

template <typename Enumeration>
static constexpr FString GetEnumValueString(const Enumeration Value)
{
	return StaticEnum<Enumeration>()->GetNameStringByValue(static_cast<int64>(Value));
}
