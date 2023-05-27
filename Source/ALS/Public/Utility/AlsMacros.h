#pragma once

#include "Misc/AssertionMacros.h"

#define ALS_STRINGIFY_IMPLEMENTATION(Value) #Value

#define ALS_STRINGIFY(Value) ALS_STRINGIFY_IMPLEMENTATION(Value)

#define ALS_GET_TYPE_STRING(Type) \
	((void) sizeof UEAsserts_Private::GetMemberNameCheckedJunk(static_cast<Type*>(nullptr)), TEXTVIEW(#Type))

// A lightweight version of the ensure() macro that doesn't generate a C++ call stack and doesn't send a
// crash report, because it doesn't happen instantly and causes the editor to freeze, which can be annoying.

#if DO_ENSURE && !USING_CODE_ANALYSIS

namespace AlsEnsure
{
	ALS_API bool UE_DEBUG_SECTION VARARGS Execute(bool& bExecuted, bool bEnsureAlways, const ANSICHAR* Expression,
	                                              const TCHAR* StaticMessage, const TCHAR* Format, ...);
}

#define ALS_ENSURE_IMPLEMENTATION(Capture, bEnsureAlways, Expression, Format, ...) \
	(LIKELY(Expression) || [Capture]() UE_DEBUG_SECTION \
	{ \
		static constexpr auto StaticMessage{TEXT("Ensure failed: " #Expression ", File: " __FILE__ ", Line: " ALS_STRINGIFY(__LINE__) ".")}; \
		static auto bExecuted{false}; \
		\
		if (AlsEnsure::Execute(bExecuted, bEnsureAlways, #Expression, StaticMessage, Format, ##__VA_ARGS__)) \
		{ \
			PLATFORM_BREAK(); \
		} \
		\
		return false; \
	}())

#define ALS_ENSURE(Expression) ALS_ENSURE_IMPLEMENTATION( , false, Expression, TEXT(""))
#define ALS_ENSURE_MESSAGE(Expression, Format, ...) ALS_ENSURE_IMPLEMENTATION(&, false, Expression, Format, ##__VA_ARGS__)
#define ALS_ENSURE_ALWAYS(Expression) ALS_ENSURE_IMPLEMENTATION( , true, Expression, TEXT(""))
#define ALS_ENSURE_ALWAYS_MESSAGE(Expression, Format, ...) ALS_ENSURE_IMPLEMENTATION(&, true, Expression, Format, ##__VA_ARGS__)

#else

#define ALS_ENSURE(Expression) (Expression)
#define ALS_ENSURE_MESSAGE(Expression, Format, ...) (Expression)
#define ALS_ENSURE_ALWAYS(Expression) (Expression)
#define ALS_ENSURE_ALWAYS_MESSAGE(Expression, Format, ...) (Expression)

#endif
