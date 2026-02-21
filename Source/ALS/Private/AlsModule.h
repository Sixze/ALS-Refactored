#pragma once

#include "Containers/Array.h"
#include "Modules/ModuleInterface.h"

struct FAutoCompleteCommand;

class ALS_API FAlsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:
#if ALLOW_CONSOLE
	void Console_OnRegisterAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands);
#endif
};
