#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsCameraConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class ALSCAMERA_API UAlsCameraConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& CameraOffsetXCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& CameraOffsetYCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& CameraOffsetZCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& PivotOffsetXCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& PivotOffsetYCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& PivotOffsetZCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& LocationLagXCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& LocationLagYCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& LocationLagZCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& RotationLagCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& FirstPersonOverrideCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& TraceOverrideCurve();

	// Debug

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants")
	static const FName& CameraCurvesDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& CameraShapesDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& CameraTracesDisplayName();
};

inline const FName& UAlsCameraConstants::CameraOffsetXCurve()
{
	static const FName Name{TEXT("CameraOffsetX")};
	return Name;
}

inline const FName& UAlsCameraConstants::CameraOffsetYCurve()
{
	static const FName Name{TEXT("CameraOffsetY")};
	return Name;
}

inline const FName& UAlsCameraConstants::CameraOffsetZCurve()
{
	static const FName Name{TEXT("CameraOffsetZ")};
	return Name;
}

inline const FName& UAlsCameraConstants::PivotOffsetXCurve()
{
	static const FName Name{TEXT("PivotOffsetX")};
	return Name;
}

inline const FName& UAlsCameraConstants::PivotOffsetYCurve()
{
	static const FName Name{TEXT("PivotOffsetY")};
	return Name;
}

inline const FName& UAlsCameraConstants::PivotOffsetZCurve()
{
	static const FName Name{TEXT("PivotOffsetZ")};
	return Name;
}

inline const FName& UAlsCameraConstants::LocationLagXCurve()
{
	static const FName Name{TEXT("LocationLagX")};
	return Name;
}

inline const FName& UAlsCameraConstants::LocationLagYCurve()
{
	static const FName Name{TEXT("LocationLagY")};
	return Name;
}

inline const FName& UAlsCameraConstants::LocationLagZCurve()
{
	static const FName Name{TEXT("LocationLagZ")};
	return Name;
}

inline const FName& UAlsCameraConstants::RotationLagCurve()
{
	static const FName Name{TEXT("RotationLag")};
	return Name;
}

inline const FName& UAlsCameraConstants::FirstPersonOverrideCurve()
{
	static const FName Name{TEXT("FirstPersonOverride")};
	return Name;
}

inline const FName& UAlsCameraConstants::TraceOverrideCurve()
{
	static const FName Name{TEXT("TraceOverride")};
	return Name;
}

inline const FName& UAlsCameraConstants::CameraCurvesDisplayName()
{
	static const FName Name{TEXT("ALS.CameraCurves")};
	return Name;
}

inline const FName& UAlsCameraConstants::CameraShapesDisplayName()
{
	static const FName Name{TEXT("ALS.CameraShapes")};
	return Name;
}

inline const FName& UAlsCameraConstants::CameraTracesDisplayName()
{
	static const FName Name{TEXT("ALS.CameraTraces")};
	return Name;
}
