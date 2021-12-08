#include "Node/AlsAnimNode_GameplayTagsBlend.h"

int32 FAlsAnimNode_GameplayTagsBlend::GetActiveChildIndex()
{
	return ActiveTag.IsValid()
		       ? Tags.Find(ActiveTag) + 1
		       : 0;
}

#if WITH_EDITOR
void FAlsAnimNode_GameplayTagsBlend::RefreshPoses()
{
	const auto Difference{BlendPose.Num() - Tags.Num() - 1};
	if (Difference == 0)
	{
		return;
	}

	if (Difference > 0)
	{
		for (auto i{Difference}; i > 0; i--)
		{
			RemovePose(BlendPose.Num() - 1);
		}
	}
	else
	{
		for (auto i{Difference}; i < 0; i++)
		{
			AddPose();
		}
	}
}
#endif
