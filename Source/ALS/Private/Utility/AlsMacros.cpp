#include "Utility/AlsMacros.h"

#include "CoreGlobals.h"
#include "Async/UniqueLock.h"
#include "Async/WordMutex.h"
#include "Containers/StaticArray.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Templates/Function.h"

#if DO_ENSURE && !USING_CODE_ANALYSIS

namespace 
{
    uint8 GEnsureResetState = 1;
    FAutoConsoleCommand CVarResetEnsureState(
        TEXT("AlsEnsure.ResetEnsureState"),
        TEXT("Reset all ensures so they will fire again"),
        FConsoleCommandDelegate::CreateLambda([]()
            {
                ++GEnsureResetState;
            }));
}

namespace AlsEnsure
{
	static bool CanExecute(std::atomic<uint8>& bExecuted, const FAlsEnsureInfo& EnsureInfo)
	{
	    static const auto* EnsureAlwaysEnabledConsoleVariable{
	        IConsoleManager::Get().FindConsoleVariable(TEXT("core.EnsureAlwaysEnabled"))
        };
	    check(EnsureAlwaysEnabledConsoleVariable != nullptr)

	    if ((bExecuted.load(std::memory_order_relaxed) == GEnsureResetState &&
             (!EnsureInfo.bEnsureAlways || !EnsureAlwaysEnabledConsoleVariable->GetBool())) ||
            !FPlatformMisc::IsEnsureAllowed())
	    {
	        return false;
	    }

	    // UE::Assert::Private::CheckEnsureFailed(EnsureInfo.bEnsureAlways, bExecuted);
	    return GEnsureResetState != bExecuted.exchange(GEnsureResetState, std::memory_order_release) || EnsureInfo.bEnsureAlways;
	}

	static bool ExecuteInternal(const FAlsEnsureInfo& EnsureInfo, const TCHAR* Message)
	{
		if (UNLIKELY(GetEnsureHandler() && GetEnsureHandler()({EnsureInfo.Expression, Message})))
		{
			return false;
		}

		if (FPlatformTime::GetSecondsPerCycle() != 0.0f)
		{
			static const auto* EnsuresAreErrorsConsoleVariable{
				IConsoleManager::Get().FindConsoleVariable(TEXT("core.EnsuresAreErrors"))
			};
			check(EnsuresAreErrorsConsoleVariable != nullptr)

#if !NO_LOGGING
			if (EnsuresAreErrorsConsoleVariable->GetBool())
			{
				UE_LOG(LogOutputDevice, Error, TEXT("Ensure failed: %hs, File: %hs, Line: %d."),
				       EnsureInfo.Expression, EnsureInfo.FilePath, EnsureInfo.LineNumber)

				UE_LOG(LogOutputDevice, Error, TEXT("%s"), Message)
			}
			else
			{
				UE_LOG(LogOutputDevice, Error, TEXT("Ensure failed: %hs, File: %hs, Line: %d."),
				       EnsureInfo.Expression, EnsureInfo.FilePath, EnsureInfo.LineNumber)

				UE_LOG(LogOutputDevice, Warning, TEXT("%s"), Message)
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

	bool UE_COLD UE_DEBUG_SECTION Execute(std::atomic<uint8>& bExecuted, const FAlsEnsureInfo& EnsureInfo)
	{
		return CanExecute(bExecuted, EnsureInfo) && ExecuteInternal(EnsureInfo, TEXT(""));
	}

	bool UE_COLD UE_DEBUG_SECTION ExecuteFormat(std::atomic<uint8>& bExecuted, const FAlsEnsureInfo& EnsureInfo,
	                                            const TCHAR* Format, ...)
	{
		if (!CanExecute(bExecuted, EnsureInfo))
		{
			return false;
		}

		static UE::FWordMutex FormatMutex;
		static constexpr auto MessageSize{65535};
		static TStaticArray<TCHAR, MessageSize> Message;

		// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
		UE::TUniqueLock Lock{FormatMutex};

		GET_TYPED_VARARGS(TCHAR, Message.GetData(), MessageSize, FormattedMessageSize - 1, Format, Format);

		return ExecuteInternal(EnsureInfo, Message.GetData());
	}
}

#endif
