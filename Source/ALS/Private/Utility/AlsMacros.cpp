#include "Utility/AlsMacros.h"

#include "CoreGlobals.h"
#include "Async/UniqueLock.h"
#include "Async/WordMutex.h"
#include "Containers/StaticArray.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Templates/Function.h"

#if DO_ENSURE && !USING_CODE_ANALYSIS

bool UE_COLD UE_DEBUG_SECTION AlsEnsure::Execute(std::atomic<bool>& bExecuted, const FAlsEnsureInfo& EnsureInfo, const TCHAR* Format, ...)
{
	static const auto* EnsureAlwaysEnabledConsoleVariable{
		IConsoleManager::Get().FindConsoleVariable(TEXT("core.EnsureAlwaysEnabled"))
	};
	check(EnsureAlwaysEnabledConsoleVariable != nullptr)

	if ((bExecuted.load(std::memory_order_relaxed) &&
	     (!EnsureInfo.bEnsureAlways || !EnsureAlwaysEnabledConsoleVariable->GetBool())) ||
	    !FPlatformMisc::IsEnsureAllowed())
	{
		return false;
	}

	if (bExecuted.exchange(true, std::memory_order_release) && !EnsureInfo.bEnsureAlways)
	{
		return false;
	}

	static UE::FWordMutex FormatMutex;
	static constexpr auto MessageSize{65535};
	static TStaticArray<TCHAR, MessageSize> Message;

	// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
	UE::TUniqueLock Lock{FormatMutex};

	GET_TYPED_VARARGS(TCHAR, Message.GetData(), MessageSize, FormattedMessageSize - 1, Format, Format);

	if (UNLIKELY(GetEnsureHandler() && GetEnsureHandler()({EnsureInfo.Expression, Message.GetData()})))
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
			UE_LOG(LogOutputDevice, Error, TEXT("Ensure failed: %hs, File: %hs, Line: %d."),
			       EnsureInfo.Expression, EnsureInfo.FilePath, EnsureInfo.LineNumber);

			UE_LOG(LogOutputDevice, Error, TEXT("%s"), Message.GetData());
		}
		else
		{
			UE_LOG(LogOutputDevice, Error, TEXT("Ensure failed: %hs, File: %hs, Line: %d."),
			       EnsureInfo.Expression, EnsureInfo.FilePath, EnsureInfo.LineNumber);

			UE_LOG(LogOutputDevice, Warning, TEXT("%s"), Message.GetData());
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
