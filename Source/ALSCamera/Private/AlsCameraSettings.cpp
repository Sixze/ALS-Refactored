#include "AlsCameraSettings.h"

#if WITH_EDITORONLY_DATA
void UAlsCameraSettings::Serialize(FArchive& Archive)
{
	UObject::Serialize(Archive);

	if (Archive.IsLoading())
	{
		PostProcess.OnAfterLoad();
	}
}
#endif
