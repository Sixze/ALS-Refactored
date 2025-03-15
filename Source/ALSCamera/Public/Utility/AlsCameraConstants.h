#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsCameraConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class ALSCAMERA_API UAlsCameraConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:
	// Animation Curves

	inline static const FName CameraOffsetXCurve{TEXTVIEW("CameraOffsetX")};
	inline static const FName CameraOffsetYCurve{TEXTVIEW("CameraOffsetY")};
	inline static const FName CameraOffsetZCurve{TEXTVIEW("CameraOffsetZ")};
	inline static const FName FovOffsetCurve{TEXTVIEW("FovOffset")};
	inline static const FName PivotOffsetXCurve{TEXTVIEW("PivotOffsetX")};
	inline static const FName PivotOffsetYCurve{TEXTVIEW("PivotOffsetY")};
	inline static const FName PivotOffsetZCurve{TEXTVIEW("PivotOffsetZ")};
	inline static const FName LocationLagXCurve{TEXTVIEW("LocationLagX")};
	inline static const FName LocationLagYCurve{TEXTVIEW("LocationLagY")};
	inline static const FName LocationLagZCurve{TEXTVIEW("LocationLagZ")};
	inline static const FName RotationLagCurve{TEXTVIEW("RotationLag")};
	inline static const FName FirstPersonOverrideCurve{TEXTVIEW("FirstPersonOverride")};
	inline static const FName TraceOverrideCurve{TEXTVIEW("TraceOverride")};

	// Debug

	inline static const FName CameraCurvesDebugDisplay{TEXTVIEW("ALS.CameraCurves")};
	inline static const FName CameraShapesDebugDisplay{TEXTVIEW("ALS.CameraShapes")};
	inline static const FName CameraTracesDebugDisplay{TEXTVIEW("ALS.CameraTraces")};

public:
	// Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& CameraOffsetXCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& CameraOffsetYCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& CameraOffsetZCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FovOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PivotOffsetXCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PivotOffsetYCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PivotOffsetZCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LocationLagXCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LocationLagYCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LocationLagZCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationLagCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FirstPersonOverrideCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& TraceOverrideCurveName();

	// Debug

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CameraCurvesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CameraShapesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Camera Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CameraTracesDebugDisplayName();
};

inline const FName& UAlsCameraConstants::CameraOffsetXCurveName()
{
	return CameraOffsetXCurve;
}

inline const FName& UAlsCameraConstants::CameraOffsetYCurveName()
{
	return CameraOffsetYCurve;
}

inline const FName& UAlsCameraConstants::CameraOffsetZCurveName()
{
	return CameraOffsetZCurve;
}

inline const FName& UAlsCameraConstants::FovOffsetCurveName()
{
	return FovOffsetCurve;
}

inline const FName& UAlsCameraConstants::PivotOffsetXCurveName()
{
	return PivotOffsetXCurve;
}

inline const FName& UAlsCameraConstants::PivotOffsetYCurveName()
{
	return PivotOffsetYCurve;
}

inline const FName& UAlsCameraConstants::PivotOffsetZCurveName()
{
	return PivotOffsetZCurve;
}

inline const FName& UAlsCameraConstants::LocationLagXCurveName()
{
	return LocationLagXCurve;
}

inline const FName& UAlsCameraConstants::LocationLagYCurveName()
{
	return LocationLagYCurve;
}

inline const FName& UAlsCameraConstants::LocationLagZCurveName()
{
	return LocationLagZCurve;
}

inline const FName& UAlsCameraConstants::RotationLagCurveName()
{
	return RotationLagCurve;
}

inline const FName& UAlsCameraConstants::FirstPersonOverrideCurveName()
{
	return FirstPersonOverrideCurve;
}

inline const FName& UAlsCameraConstants::TraceOverrideCurveName()
{
	return TraceOverrideCurve;
}

inline const FName& UAlsCameraConstants::CameraCurvesDebugDisplayName()
{
	return CameraCurvesDebugDisplay;
}

inline const FName& UAlsCameraConstants::CameraShapesDebugDisplayName()
{
	return CameraShapesDebugDisplay;
}

inline const FName& UAlsCameraConstants::CameraTracesDebugDisplayName()
{
	return CameraTracesDebugDisplay;
}
