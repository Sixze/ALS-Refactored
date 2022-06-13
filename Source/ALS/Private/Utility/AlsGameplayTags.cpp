#include "Utility/AlsGameplayTags.h"

namespace AlsViewModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(FirstPerson, TEXT("Als.ViewMode.FirstPerson"))
	UE_DEFINE_GAMEPLAY_TAG(ThirdPerson, TEXT("Als.ViewMode.ThirdPerson"))
}

namespace AlsLocomotionModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Grounded, TEXT("Als.LocomotionMode.Grounded"))
	UE_DEFINE_GAMEPLAY_TAG(InAir, TEXT("Als.LocomotionMode.InAir"))
}

namespace AlsRotationModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(VelocityDirection, TEXT("Als.RotationMode.VelocityDirection"))
	UE_DEFINE_GAMEPLAY_TAG(LookingDirection, TEXT("Als.RotationMode.LookingDirection"))
	UE_DEFINE_GAMEPLAY_TAG(Aiming, TEXT("Als.RotationMode.Aiming"))
}

namespace AlsStanceTags
{
	UE_DEFINE_GAMEPLAY_TAG(Standing, TEXT("Als.Stance.Standing"))
	UE_DEFINE_GAMEPLAY_TAG(Crouching, TEXT("Als.Stance.Crouching"))
}

namespace AlsGaitTags
{
	UE_DEFINE_GAMEPLAY_TAG(Walking, TEXT("Als.Gait.Walking"))
	UE_DEFINE_GAMEPLAY_TAG(Running, TEXT("Als.Gait.Running"))
	UE_DEFINE_GAMEPLAY_TAG(Sprinting, TEXT("Als.Gait.Sprinting"))
}

namespace AlsOverlayModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Default, TEXT("Als.OverlayMode.Default"))
	UE_DEFINE_GAMEPLAY_TAG(Masculine, TEXT("Als.OverlayMode.Masculine"))
	UE_DEFINE_GAMEPLAY_TAG(Feminine, TEXT("Als.OverlayMode.Feminine"))
	UE_DEFINE_GAMEPLAY_TAG(Injured, TEXT("Als.OverlayMode.Injured"))
	UE_DEFINE_GAMEPLAY_TAG(HandsTied, TEXT("Als.OverlayMode.HandsTied"))
	UE_DEFINE_GAMEPLAY_TAG(M4, TEXT("Als.OverlayMode.M4"))
	UE_DEFINE_GAMEPLAY_TAG(PistolOneHanded, TEXT("Als.OverlayMode.PistolOneHanded"))
	UE_DEFINE_GAMEPLAY_TAG(PistolTwoHanded, TEXT("Als.OverlayMode.PistolTwoHanded"))
	UE_DEFINE_GAMEPLAY_TAG(Bow, TEXT("Als.OverlayMode.Bow"))
	UE_DEFINE_GAMEPLAY_TAG(Torch, TEXT("Als.OverlayMode.Torch"))
	UE_DEFINE_GAMEPLAY_TAG(Binoculars, TEXT("Als.OverlayMode.Binoculars"))
	UE_DEFINE_GAMEPLAY_TAG(Box, TEXT("Als.OverlayMode.Box"))
	UE_DEFINE_GAMEPLAY_TAG(Barrel, TEXT("Als.OverlayMode.Barrel"))
}

namespace AlsLocomotionActionTags
{
	UE_DEFINE_GAMEPLAY_TAG(Mantling, TEXT("Als.LocomotionAction.Mantling"))
	UE_DEFINE_GAMEPLAY_TAG(Ragdolling, TEXT("Als.LocomotionAction.Ragdolling"))
	UE_DEFINE_GAMEPLAY_TAG(GettingUp, TEXT("Als.LocomotionAction.GettingUp"))
	UE_DEFINE_GAMEPLAY_TAG(Rolling, TEXT("Als.LocomotionAction.Rolling"))
}

namespace AlsGroundedEntryModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(FromRoll, TEXT("Als.GroundedEntryMode.FromRoll"))
}
