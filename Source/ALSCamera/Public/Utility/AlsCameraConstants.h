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
	static const FName& CameraOffsetXCurve()
	{
		static const FName Name{TEXT("CameraOffsetX")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& CameraOffsetYCurve()
	{
		static const FName Name{TEXT("CameraOffsetY")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& CameraOffsetZCurve()
	{
		static const FName Name{TEXT("CameraOffsetZ")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& PivotOffsetXCurve()
	{
		static const FName Name{TEXT("PivotOffsetX")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& PivotOffsetYCurve()
	{
		static const FName Name{TEXT("PivotOffsetY")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& PivotOffsetZCurve()
	{
		static const FName Name{TEXT("PivotOffsetZ")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& LocationLagXCurve()
	{
		static const FName Name{TEXT("LocationLagX")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& LocationLagYCurve()
	{
		static const FName Name{TEXT("LocationLagY")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& LocationLagZCurve()
	{
		static const FName Name{TEXT("LocationLagZ")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& RotationLagCurve()
	{
		static const FName Name{TEXT("RotationLag")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& FirstPersonOverrideCurve()
	{
		static const FName Name{TEXT("FirstPersonOverride")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants|Animation Curves")
	static const FName& TraceOverrideCurve()
	{
		static const FName Name{TEXT("TraceOverride")};
		return Name;
	}

	// Debug

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera Constants")
	static const FName& CameraCurvesDisplayName()
	{
		static const FName Name{TEXT("ALS.CameraCurves")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& CameraShapesDisplayName()
	{
		static const FName Name{TEXT("ALS.CameraShapes")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& CameraTracesDisplayName()
	{
		static const FName Name{TEXT("ALS.CameraTraces")};
		return Name;
	}
};
