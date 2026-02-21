#include "AlsModule.h"

#include "Modules/ModuleManager.h"
#include "Utility/AlsLog.h"

#if ALLOW_CONSOLE
#include "Engine/Console.h"
#endif

#if WITH_EDITOR
#include "MessageLogModule.h"
#endif

IMPLEMENT_MODULE(FAlsModule, ALS)

#define LOCTEXT_NAMESPACE "ALSModule"

void FAlsModule::StartupModule()
{
	IModuleInterface::StartupModule();

#if ALLOW_CONSOLE
	UConsole::RegisterConsoleAutoCompleteEntries.AddRaw(this, &FAlsModule::Console_OnRegisterAutoCompleteEntries);
#endif

#if WITH_EDITOR
	auto& MessageLog{FModuleManager::LoadModuleChecked<FMessageLogModule>(FName{TEXTVIEW("MessageLog")})};

	FMessageLogInitializationOptions MessageLogOptions;
	MessageLogOptions.bShowFilters = true;
	MessageLogOptions.bAllowClear = true;
	MessageLogOptions.bDiscardDuplicates = true;

	MessageLog.RegisterLogListing(AlsLog::MessageLogName, LOCTEXT("MessageLogLabel", "ALS"), MessageLogOptions);
#endif
}

void FAlsModule::ShutdownModule()
{
#if ALLOW_CONSOLE
	UConsole::RegisterConsoleAutoCompleteEntries.RemoveAll(this);
#endif

	IModuleInterface::ShutdownModule();
}

#if ALLOW_CONSOLE
// ReSharper disable once CppMemberFunctionMayBeStatic
void FAlsModule::Console_OnRegisterAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands)
{
	const auto CommandColor{GetDefault<UConsoleSettings>()->AutoCompleteCommandColor};

	auto* Command{&AutoCompleteCommands.AddDefaulted_GetRef()};
	Command->Command = FString{TEXTVIEW("Stat Als")};
	Command->Desc = FString{TEXTVIEW("Displays ALS performance statistics.")};
	Command->Color = CommandColor;

	Command = &AutoCompleteCommands.AddDefaulted_GetRef();
	Command->Command = FString{TEXTVIEW("ShowDebug Als.Curves")};
	Command->Desc = FString{TEXTVIEW("Displays animation curves.")};
	Command->Color = CommandColor;

	Command = &AutoCompleteCommands.AddDefaulted_GetRef();
	Command->Command = FString{TEXTVIEW("ShowDebug Als.State")};
	Command->Desc = FString{TEXTVIEW("Displays character state.")};
	Command->Color = CommandColor;

	Command = &AutoCompleteCommands.AddDefaulted_GetRef();
	Command->Command = FString{TEXTVIEW("ShowDebug Als.Shapes")};
	Command->Desc = FString{TEXTVIEW("Displays debug shapes.")};
	Command->Color = CommandColor;

	Command = &AutoCompleteCommands.AddDefaulted_GetRef();
	Command->Command = FString{TEXTVIEW("ShowDebug Als.Traces")};
	Command->Desc = FString{TEXTVIEW("Displays animation traces.")};
	Command->Color = CommandColor;

	Command = &AutoCompleteCommands.AddDefaulted_GetRef();
	Command->Command = FString{TEXTVIEW("ShowDebug Als.Mantling")};
	Command->Desc = FString{TEXTVIEW("Displays mantling traces.")};
	Command->Color = CommandColor;
}
#endif

#undef LOCTEXT_NAMESPACE
