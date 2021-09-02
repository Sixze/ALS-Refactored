#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"

#include "AlsAnimNotify_SetGroundedEntryMode.generated.h"

enum class EAlsGroundedEntryMode : uint8;

UCLASS(Meta = (DisplayName = "Als Set Grounded Entry Mode Animation Notify"))
class ALS_API UAlsAnimNotify_SetGroundedEntryMode : public UAnimNotify
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	EAlsGroundedEntryMode Mode;

public:
	UAlsAnimNotify_SetGroundedEntryMode();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;
};
