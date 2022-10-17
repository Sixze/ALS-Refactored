#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Templates/SubclassOf.h"
#include "AlsAnimNotify_CameraShake.generated.h"

class UCameraShakeBase;

UCLASS(DisplayName = "Als Camera Shake Animation Notify")
class ALSCAMERA_API UAlsAnimNotify_CameraShake : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "x"))
	float CameraShakeScale{1.0f};

public:
	UAlsAnimNotify_CameraShake();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;
};
