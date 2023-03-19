#pragma once

#include "Modules/ModuleManager.h"

class ALS_API FALSModule : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
};