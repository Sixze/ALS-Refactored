#include "ALSModule.h"

#if WITH_EDITOR
#include "MessageLogModule.h"
#endif

#include "Utility/AlsLog.h"

IMPLEMENT_MODULE(FALSModule, ALS)

#define LOCTEXT_NAMESPACE "ALSModule"

void FALSModule::StartupModule()
{
	FDefaultModuleImpl::StartupModule();

#if WITH_EDITOR
	auto& MessageLog{FModuleManager::LoadModuleChecked<FMessageLogModule>(FName{TEXTVIEW("MessageLog")})};

	FMessageLogInitializationOptions Options;
	Options.bShowFilters = true;
	Options.bAllowClear = true;
	Options.bDiscardDuplicates = true;

	MessageLog.RegisterLogListing(AlsLog::MessageLogName, LOCTEXT("MessageLogLabel", "ALS"), Options);
#endif
}

void FALSModule::ShutdownModule()
{
	FDefaultModuleImpl::ShutdownModule();
}

#undef LOCTEXT_NAMESPACE
