#include "Utility/AlsMontageUtility.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Utility/AlsMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsMontageUtility)

FTransform UAlsMontageUtility::ExtractRootTransformFromMontage(const UAnimMontage* Montage, const float Time)
{
	// Based on UMotionWarpingUtilities::ExtractRootTransformFromAnimation().

	if (!ALS_ENSURE(IsValid(Montage)) || !ALS_ENSURE(!Montage->SlotAnimTracks.IsEmpty()))
	{
		return FTransform::Identity;
	}

	const auto* Segment{Montage->SlotAnimTracks[0].AnimTrack.GetSegmentAtTime(Time)};
	if (!ALS_ENSURE(Segment != nullptr))
	{
		return FTransform::Identity;
	}

	const auto* Sequence{Cast<UAnimSequence>(Segment->GetAnimReference())};
	if (!ALS_ENSURE(IsValid(Sequence)))
	{
		return FTransform::Identity;
	}

	const FAnimExtractContext ExtractionContext{static_cast<double>(Segment->ConvertTrackPosToAnimPos(Time))};
	return Sequence->ExtractRootTrackTransform(ExtractionContext, nullptr);
}

FTransform UAlsMontageUtility::ExtractLastRootTransformFromMontage(const UAnimMontage* Montage)
{
	// Based on UMotionWarpingUtilities::ExtractRootTransformFromAnimation().

	if (!ALS_ENSURE(IsValid(Montage)) || !ALS_ENSURE(!Montage->SlotAnimTracks.IsEmpty()) ||
	    !ALS_ENSURE(!Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.IsEmpty()))
	{
		return FTransform::Identity;
	}

	const auto& Segment{Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.Last()};
	const auto* Sequence{Cast<UAnimSequence>(Segment.GetAnimReference())};

	if (!ALS_ENSURE(IsValid(Sequence)))
	{
		return FTransform::Identity;
	}

	const FAnimExtractContext ExtractionContext{static_cast<double>(Segment.GetEndPos())};
	return Sequence->ExtractRootTrackTransform(ExtractionContext, nullptr);
}

void UAlsMontageUtility::StopMontagesWithAnySharedSlots(UAnimInstance* AnimationInstance, const UAnimMontage* ReferenceMontage,
                                                        const float BlendOutDuration)
{
	if (!ALS_ENSURE(IsValid(AnimationInstance)) || !ALS_ENSURE(IsValid(ReferenceMontage)))
	{
		return;
	}

	TSet<FName, DefaultKeyFuncs<FName>, TInlineSetAllocator<8>> SlotNames;
	SlotNames.Reserve(ReferenceMontage->SlotAnimTracks.Num());

	for (const auto& SlotTrack : ReferenceMontage->SlotAnimTracks)
	{
		SlotNames.Emplace(SlotTrack.SlotName);
	}

	for (auto* MontageInstance : AnimationInstance->MontageInstances)
	{
		if (MontageInstance == nullptr || !MontageInstance->IsActive())
		{
			continue;
		}

		const auto* Montage{MontageInstance->Montage.Get()};

		for (const auto& SlotTrack : Montage->SlotAnimTracks)
		{
			if (!SlotNames.Contains(SlotTrack.SlotName))
			{
				continue;
			}

			FMontageBlendSettings BlendOutSettings{Montage->BlendOut};

			if (BlendOutDuration >= 0.0f)
			{
				BlendOutSettings.Blend.BlendTime = BlendOutDuration;
			}

			BlendOutSettings.BlendMode = Montage->BlendModeOut;
			BlendOutSettings.BlendProfile = Montage->BlendProfileOut;

			MontageInstance->Stop(BlendOutSettings);
			break;
		}
	}
}
