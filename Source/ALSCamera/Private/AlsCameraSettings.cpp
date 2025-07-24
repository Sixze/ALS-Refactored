#include "AlsCameraSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsCameraSettings)

#if WITH_EDITORONLY_DATA
void UAlsCameraSettings::Serialize(FArchive& Archive)
{
	UObject::Serialize(Archive); // NOLINT(bugprone-parent-virtual-call)

	if (Archive.IsLoading())
	{
		PostProcess.OnAfterLoad();
	}
}
#endif
