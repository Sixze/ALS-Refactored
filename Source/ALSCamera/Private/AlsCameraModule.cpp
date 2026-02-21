#include "AlsCameraModule.h"

#include "Modules/ModuleManager.h"

#if ALLOW_CONSOLE
#include "Engine/Console.h"
#endif

IMPLEMENT_MODULE(FAlsCameraModule, ALSCamera)

void FAlsCameraModule::StartupModule()
{
	IModuleInterface::StartupModule();

#if ALLOW_CONSOLE
	UConsole::RegisterConsoleAutoCompleteEntries.AddRaw(this, &FAlsCameraModule::Console_OnRegisterAutoCompleteEntries);
#endif
}

void FAlsCameraModule::ShutdownModule()
{
#if ALLOW_CONSOLE
	UConsole::RegisterConsoleAutoCompleteEntries.RemoveAll(this);
#endif

	IModuleInterface::ShutdownModule();
}

#if ALLOW_CONSOLE
// ReSharper disable once CppMemberFunctionMayBeStatic
void FAlsCameraModule::Console_OnRegisterAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands)
{
	const auto CommandColor{GetDefault<UConsoleSettings>()->AutoCompleteCommandColor};

	auto* Command{&AutoCompleteCommands.AddDefaulted_GetRef()};
	Command->Command = FString{TEXTVIEW("ShowDebug Als.CameraCurves")};
	Command->Desc = FString{TEXTVIEW("Displays camera animation curves.")};
	Command->Color = CommandColor;

	Command = &AutoCompleteCommands.AddDefaulted_GetRef();
	Command->Command = FString{TEXTVIEW("ShowDebug Als.CameraShapes")};
	Command->Desc = FString{TEXTVIEW("Displays camera debug shapes.")};
	Command->Color = CommandColor;

	Command = &AutoCompleteCommands.AddDefaulted_GetRef();
	Command->Command = FString{TEXTVIEW("ShowDebug Als.CameraTraces")};
	Command->Desc = FString{TEXTVIEW("Displays camera traces.")};
	Command->Color = CommandColor;
}
#endif
