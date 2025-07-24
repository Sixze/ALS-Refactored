#include "Nodes/AlsRigUnit_ChainLength.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnit_ChainLength)

namespace AlsChainLengthRigUnit
{
	static float CalculateChainLength(FRigTransformElement* AncestorElement, FRigTransformElement* DescendantElement,
	                                  const URigHierarchy* Hierarchy, const ERigTransformType::Type TransformType,
	                                  TBitArray<>& VisitedElements)
	{
		// Based on URigHierarchy::IsDependentOn().

		if (AncestorElement == nullptr || DescendantElement == nullptr)
		{
			return -1.0f;
		}

		if (DescendantElement == AncestorElement)
		{
			return 0.0f;
		}

		const auto DescendantElementIndex{DescendantElement->GetIndex()};

		if (!VisitedElements.IsValidIndex(DescendantElementIndex))
		{
			return -1.0f;
		}

		if (VisitedElements[DescendantElementIndex])
		{
			return -1.0f;
		}

		float ChainLength{-1.0};
		FRigTransformElement* ParentElement{nullptr};

		const auto* SingleParentElement{Cast<FRigSingleParentElement>(DescendantElement)};
		if (SingleParentElement != nullptr)
		{
			ParentElement = SingleParentElement->ParentElement;
			ChainLength = CalculateChainLength(AncestorElement, ParentElement, Hierarchy, TransformType, VisitedElements);
		}
		else
		{
			const auto* MultiParentElement{Cast<FRigMultiParentElement>(DescendantElement)};
			if (MultiParentElement != nullptr)
			{
				for (const auto& ParentConstraint : MultiParentElement->ParentConstraints)
				{
					ParentElement = ParentConstraint.ParentElement;
					ChainLength = CalculateChainLength(AncestorElement, ParentElement, Hierarchy, TransformType, VisitedElements);

					if (ChainLength >= 0.0f)
					{
						break;
					}
				}
			}
		}

		if (ChainLength < 0.0f)
		{
			return -1.0f;
		}

		return UE_REAL_TO_FLOAT(ChainLength + FVector::Distance(
			Hierarchy->GetTransform(DescendantElement, TransformType).GetLocation(),
			Hierarchy->GetTransform(ParentElement, TransformType).GetLocation()));
	}
}

FAlsRigUnit_ChainLength_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	const auto* Hierarchy{ExecuteContext.Hierarchy};

	if (!IsValid(Hierarchy) ||
	    !CachedAncestorItem.UpdateCache(AncestorItem, Hierarchy) ||
	    !CachedDescendantItem.UpdateCache(DescendantItem, Hierarchy))
	{
		return;
	}

	auto* AncestorTransformElement{const_cast<FRigTransformElement*>(Cast<FRigTransformElement>(CachedAncestorItem.GetElement()))};
	auto* DescendantTransformElement{const_cast<FRigTransformElement*>(Cast<FRigTransformElement>(CachedDescendantItem.GetElement()))};
	const auto TransformType{bInitial ? ERigTransformType::InitialGlobal : ERigTransformType::CurrentGlobal};
	TBitArray VisitedElements{false, Hierarchy->Num()};

	Length = AlsChainLengthRigUnit::CalculateChainLength(AncestorTransformElement, DescendantTransformElement,
	                                                     Hierarchy, TransformType, VisitedElements);

	if (Length < 0.0f)
	{
		Length = FMath::Max(0.0f, AlsChainLengthRigUnit::CalculateChainLength(DescendantTransformElement, AncestorTransformElement, // NOLINT(readability-suspicious-call-argument)
		                                                                      Hierarchy, TransformType, VisitedElements));
	}
}
