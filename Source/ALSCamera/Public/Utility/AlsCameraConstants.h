#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsCameraConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class ALSCAMERA_API UAlsCameraConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:
	// Animation Curves

	inline static FName CameraOffsetXCurve{TEXTVIEW("CameraOffsetX")};
	inline static FName CameraOffsetYCurve{TEXTVIEW("CameraOffsetY")};
	inline static FName CameraOffsetZCurve{TEXTVIEW("CameraOffsetZ")};
	inline static FName FovOffsetCurve{TEXTVIEW("FovOffset")};
	inline static FName PivotOffsetXCurve{TEXTVIEW("PivotOffsetX")};
	inline static FName PivotOffsetYCurve{TEXTVIEW("PivotOffsetY")};
	inline static FName PivotOffsetZCurve{TEXTVIEW("PivotOffsetZ")};
	inline static FName LocationLagXCurve{TEXTVIEW("LocationLagX")};
	inline static FName LocationLagYCurve{TEXTVIEW("LocationLagY")};
	inline static FName LocationLagZCurve{TEXTVIEW("LocationLagZ")};
	inline static FName RotationLagCurve{TEXTVIEW("RotationLag")};
	inline static FName FirstPersonOverrideCurve{TEXTVIEW("FirstPersonOverride")};
	inline static FName TraceOverrideCurve{TEXTVIEW("TraceOverride")};

	// Debug

	inline static FName CameraCurvesDebugDisplay{TEXTVIEW("ALS.CameraCurves")};
	inline static FName CameraShapesDebugDisplay{TEXTVIEW("ALS.CameraShapes")};
	inline static FName CameraTracesDebugDisplay{TEXTVIEW("ALS.CameraTraces")};

public:
	// Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName CameraOffsetXCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName CameraOffsetYCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName CameraOffsetZCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName FovOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName PivotOffsetXCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName PivotOffsetYCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName PivotOffsetZCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LocationLagXCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LocationLagYCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LocationLagZCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName RotationLagCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName FirstPersonOverrideCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName TraceOverrideCurveName();

	// Debug

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static FName CameraCurvesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static FName CameraShapesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static FName CameraTracesDebugDisplayName();
};

inline FName UAlsCameraConstants::CameraOffsetXCurveName()
{
	return CameraOffsetXCurve;
}

inline FName UAlsCameraConstants::CameraOffsetYCurveName()
{
	return CameraOffsetYCurve;
}

inline FName UAlsCameraConstants::CameraOffsetZCurveName()
{
	return CameraOffsetZCurve;
}

inline FName UAlsCameraConstants::FovOffsetCurveName()
{
	return FovOffsetCurve;
}

inline FName UAlsCameraConstants::PivotOffsetXCurveName()
{
	return PivotOffsetXCurve;
}

inline FName UAlsCameraConstants::PivotOffsetYCurveName()
{
	return PivotOffsetYCurve;
}

inline FName UAlsCameraConstants::PivotOffsetZCurveName()
{
	return PivotOffsetZCurve;
}

inline FName UAlsCameraConstants::LocationLagXCurveName()
{
	return LocationLagXCurve;
}

inline FName UAlsCameraConstants::LocationLagYCurveName()
{
	return LocationLagYCurve;
}

inline FName UAlsCameraConstants::LocationLagZCurveName()
{
	return LocationLagZCurve;
}

inline FName UAlsCameraConstants::RotationLagCurveName()
{
	return RotationLagCurve;
}

inline FName UAlsCameraConstants::FirstPersonOverrideCurveName()
{
	return FirstPersonOverrideCurve;
}

inline FName UAlsCameraConstants::TraceOverrideCurveName()
{
	return TraceOverrideCurve;
}

inline FName UAlsCameraConstants::CameraCurvesDebugDisplayName()
{
	return CameraCurvesDebugDisplay;
}

inline FName UAlsCameraConstants::CameraShapesDebugDisplayName()
{
	return CameraShapesDebugDisplay;
}

inline FName UAlsCameraConstants::CameraTracesDebugDisplayName()
{
	return CameraTracesDebugDisplay;
}
