#include "Utility/AlsMacros.h"

#include "CoreGlobals.h"
#include "Logging/LogMacros.h"

#if DO_ENSURE && !USING_CODE_ANALYSIS

bool UE_DEBUG_SECTION AlsEnsure::Execute(bool& bExecuted, const bool bEnsureAlways, const ANSICHAR* Expression,
                                         const TCHAR* StaticMessage, const TCHAR* Format, ...)
{
	if ((bExecuted && !bEnsureAlways) || !FPlatformMisc::IsEnsureAllowed())
	{
		return false;
	}

	bExecuted = true;

	static constexpr auto FormattedMessageSize{4096};
	TCHAR FormattedMessage[FormattedMessageSize];

	GET_VARARGS(FormattedMessage, FormattedMessageSize, FormattedMessageSize - 1, Format, Format);

	if (UNLIKELY(GetEnsureHandler() && GetEnsureHandler()({Expression, FormattedMessage})))
	{
		return false;
	}

	UE_LOG(LogOutputDevice, Warning, TEXT("%s"), StaticMessage);
	UE_LOG(LogOutputDevice, Warning, TEXT("%s"), FormattedMessage);

	PrintScriptCallstack();

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
