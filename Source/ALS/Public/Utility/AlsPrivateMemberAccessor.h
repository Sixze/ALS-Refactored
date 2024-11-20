#pragma once

#include <type_traits>

namespace AlsPrivateMemberAccessor
{
	// Global pointer to a field or method.
	template <typename AccessorName>
	typename AccessorName::MemberType TMemberPointer;

	// Struct that is used to initialize TPointer.
	template <typename AccessorName, typename AccessorName::MemberType MemberPointer>
	struct TPointerInitializer
	{
		TPointerInitializer()
		{
			TMemberPointer<AccessorName> = MemberPointer;
		}

		static TPointerInitializer Instance;
	};

	// Declaration of a TPointerInitializer instance.
	template <typename AccessorName, typename AccessorName::MemberType MemberPointer>
	TPointerInitializer<AccessorName, MemberPointer> TPointerInitializer<AccessorName, MemberPointer>::Instance;

	// Function that helps access a field or call a method.
	template <typename AccessorName, typename ThisType, typename... ArgumentsType>
	decltype(auto) Access(ThisType&& This, ArgumentsType&&... Arguments)
	{
		if constexpr (std::is_pointer_v<ThisType>)
		{
			if constexpr (sizeof...(Arguments) > 0)
			{
				return (This->*TMemberPointer<AccessorName>)(Forward<ArgumentsType>(Arguments)...);
			}
			else
			{
				return This->*TMemberPointer<AccessorName>;
			}
		}
		else
		{
			if constexpr (sizeof...(Arguments) > 0)
			{
				return (This.*TMemberPointer<AccessorName>)(Forward<ArgumentsType>(Arguments)...);
			}
			else
			{
				return This.*TMemberPointer<AccessorName>;
			}
		}
	}
}

// Alternative to UE_DEFINE_PRIVATE_MEMBER_PTR() that works with overloaded functions.
#define ALS_DEFINE_PRIVATE_MEMBER_ACCESSOR(AccessorName, MemberPointer, ...) \
	struct AccessorName \
	{ \
		using MemberType = __VA_ARGS__; \
		\
		template <typename ThisType, typename... ArgumentsType> \
		static decltype(auto) Access(ThisType&& This, ArgumentsType&&... Arguments) \
		{ \
			return AlsPrivateMemberAccessor::Access<AccessorName>(Forward<ThisType>(This), Forward<ArgumentsType>(Arguments)...); \
		} \
	}; \
	\
	template struct AlsPrivateMemberAccessor::TPointerInitializer<AccessorName, MemberPointer>; \
