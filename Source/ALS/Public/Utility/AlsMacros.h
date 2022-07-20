#pragma once

#include "HAL/Platform.h"

#define ALS_STRINGIFY_IMPLEMENTATION(Value) TEXT(#Value)

#define ALS_STRINGIFY(Value) ALS_STRINGIFY_IMPLEMENTATION(Value)

#define ALS_GET_TYPE_STRING(Type) \
	((void) sizeof UEAsserts_Private::GetMemberNameCheckedJunk(static_cast<Type*>(nullptr)), TEXT(#Type))

#if DO_ENSURE && !USING_CODE_ANALYSIS

#define ALS_ENSURE_IMPLEMENTATION(Expression, bEnsureAlways, Format, Capture, ...) \
	(LIKELY(Expression) || ([Capture]() FORCENOINLINE UE_DEBUG_SECTION \
	{ \
		static auto bExecuted{false}; \
		\
		if ((bEnsureAlways || !bExecuted) && FPlatformMisc::IsEnsureAllowed()) \
		{ \
			bExecuted = true; \
			\
			UE_LOG(LogOutputDevice, Warning, TEXT("Ensure failed: ") TEXT(#Expression) TEXT(", File: ") __FILE__ TEXT(", Line: ") ALS_STRINGIFY(__LINE__)); \
			UE_LOG(LogOutputDevice, Warning, Format, __VA_ARGS__); \
			\
			PrintScriptCallstack(); \
			\
			if (FPlatformMisc::IsDebuggerPresent()) \
			{ \
				PLATFORM_BREAK(); \
			} \
			else \
			{ \
				FPlatformMisc::PromptForRemoteDebugging(true); \
			} \
		} \
	}(), false))

#define ALS_ENSURE(Expression) ALS_ENSURE_IMPLEMENTATION(Expression, false, TEXT(""), )
#define ALS_ENSURE_MESSAGE(Expression, Format, ...) ALS_ENSURE_IMPLEMENTATION(Expression, false, Format, &, __VA_ARGS__)
#define ALS_ENSURE_ALWAYS(Expression) ALS_ENSURE_IMPLEMENTATION(Expression true, TEXT(""), )
#define ALS_ENSURE_ALWAYS_MESSAGE(Expression, Format, ...) ALS_ENSURE_IMPLEMENTATION(Expression true, Format, &, __VA_ARGS__)

#else

#define ALS_ENSURE(Expression) (Expression)
#define ALS_ENSURE_MESSAGE(Expression, Format, ...) (Expression)
#define ALS_ENSURE_ALWAYS(Expression) (Expression)
#define ALS_ENSURE_ALWAYS_MESSAGE(Expression, Format, ...) (Expression)

#endif
