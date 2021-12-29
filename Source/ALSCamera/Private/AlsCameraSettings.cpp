#include "AlsCameraSettings.h"

#if WITH_EDITORONLY_DATA
void UAlsCameraSettings::Serialize(const FStructuredArchive::FRecord Record)
{
	const auto& Archive{Record.GetUnderlyingArchive()};

	Super::Serialize(Record);

	if (Archive.IsLoading())
	{
		PostProcess.OnAfterLoad();
	}
}
#endif
