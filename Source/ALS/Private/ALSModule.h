#pragma once

#include "Modules/ModuleManager.h"

struct FAutoCompleteCommand;

class ALS_API FALSModule : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:
#if ALLOW_CONSOLE
	void Console_OnRegisterAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands);
#endif
};
