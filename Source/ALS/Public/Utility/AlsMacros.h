#pragma once

#include "Misc/AssertionMacros.h"

#define ALS_GET_TYPE_STRING(Type) \
	((void) sizeof UEAsserts_Private::GetMemberNameCheckedJunk(static_cast<Type*>(nullptr)), TEXTVIEW(#Type))

// A lightweight version of the ensure() macro that doesn't generate a C++ call stack and doesn't send a
// crash report, because it doesn't happen instantly and causes the editor to freeze, which can be annoying.

#if DO_ENSURE && !USING_CODE_ANALYSIS

namespace AlsEnsure
{
	struct FAlsEnsureInfo
	{
	public:
		const ANSICHAR* Expression{nullptr};

		const ANSICHAR* FilePath{nullptr};

		int32 LineNumber{0};

		uint8 bEnsureAlways : 1 {false};

	public:
		constexpr FAlsEnsureInfo(
			const ANSICHAR* InExpression,
			const ANSICHAR* InFilePath,
			const int32 InLineNumber,
			const bool bInEnsureAlways) : Expression(InExpression)
			                              , FilePath(InFilePath)
			                              , LineNumber(InLineNumber)
			                              , bEnsureAlways(bInEnsureAlways) {}
	};

	ALS_API bool UE_COLD UE_DEBUG_SECTION VARARGS
	Execute(std::atomic<bool>& bExecuted, const FAlsEnsureInfo& EnsureInfo, const TCHAR* Format, ...);
}

#define ALS_ENSURE_IMPLEMENTATION(Capture, bEnsureAlways, Expression, Format, ...) \
	(LIKELY(Expression) || [Capture]() UE_COLD UE_DEBUG_SECTION \
	{ \
		static constexpr AlsEnsure::FAlsEnsureInfo EnsureInfo{#Expression, __builtin_FILE(), __builtin_LINE(), bEnsureAlways}; \
		static std::atomic<bool> bExecuted{false}; \
		\
		UE_VALIDATE_FORMAT_STRING(Format, ##__VA_ARGS__); \
		\
		if (AlsEnsure::Execute(bExecuted, EnsureInfo, Format, ##__VA_ARGS__)) \
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
