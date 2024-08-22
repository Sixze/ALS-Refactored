#include "Utility/AlsMacros.h"

#include "CoreGlobals.h"
#include "Async/UniqueLock.h"
#include "Async/WordMutex.h"
#include "Containers/StaticArray.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Templates/Function.h"

#if DO_ENSURE && !USING_CODE_ANALYSIS

bool UE_DEBUG_SECTION AlsEnsure::Execute(std::atomic<bool>& bExecuted, const bool bEnsureAlways,
                                         const ANSICHAR* Expression, const TCHAR* StaticMessage, const TCHAR* Format, ...)
{
	static const auto* EnsureAlwaysEnabledConsoleVariable{
		IConsoleManager::Get().FindConsoleVariable(TEXT("core.EnsureAlwaysEnabled"))
	};
	check(EnsureAlwaysEnabledConsoleVariable != nullptr)

	if ((bExecuted.load(std::memory_order_relaxed) &&
	     (!bEnsureAlways || !EnsureAlwaysEnabledConsoleVariable->GetBool())) ||
	    !FPlatformMisc::IsEnsureAllowed())
	{
		return false;
	}

	if (bExecuted.exchange(true, std::memory_order_release) && !bEnsureAlways)
	{
		return false;
	}

	static UE::FWordMutex FormatMutex;
	static constexpr auto FormattedMessageSize{65535};
	static TStaticArray<TCHAR, FormattedMessageSize> FormattedMessage;

	// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
	UE::TUniqueLock Lock{FormatMutex};

	GET_TYPED_VARARGS(TCHAR, FormattedMessage.GetData(), FormattedMessageSize, FormattedMessageSize - 1, Format, Format);

	if (UNLIKELY(GetEnsureHandler() && GetEnsureHandler()({Expression, FormattedMessage.GetData()})))
	{
		return false;
	}

	if (FPlatformTime::GetSecondsPerCycle() != 0.0)
	{
		static const auto* EnsuresAreErrorsConsoleVariable{
			IConsoleManager::Get().FindConsoleVariable(TEXT("core.EnsuresAreErrors"))
		};
		check(EnsuresAreErrorsConsoleVariable != nullptr)

#if !NO_LOGGING
		if (EnsuresAreErrorsConsoleVariable->GetBool())
		{
			UE_LOG(LogOutputDevice, Error, TEXT("%s"), StaticMessage);
			UE_LOG(LogOutputDevice, Error, TEXT("%s"), FormattedMessage.GetData());
		}
		else
		{
			UE_LOG(LogOutputDevice, Warning, TEXT("%s"), StaticMessage);
			UE_LOG(LogOutputDevice, Warning, TEXT("%s"), FormattedMessage.GetData());
		}
#endif

		PrintScriptCallstack();
	}

	if (!FPlatformMisc::IsDebuggerPresent())
	{
		FPlatformMisc::PromptForRemoteDebugging(true);
		return false;
	}

#if UE_BUILD_SHIPPING
	return true;
#else
	return !GIgnoreDebugger;
#endif
}

#endif
