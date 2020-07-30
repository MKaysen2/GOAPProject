#include "WorldProperty.h"

namespace FPlannerWSOperations
{
	bool CheckEqual(const uint8& LHS, const uint8& RHS)
	{
		return LHS == RHS;
	}

	bool CheckGreater(const uint8& LHS, const uint8& RHS)
	{
		return LHS > RHS;
	}

	bool CheckLess(const uint8& LHS, const uint8& RHS)
	{
		return LHS < RHS;
	}

	bool CheckGreaterOrEqual(const uint8& LHS, const uint8& RHS)
	{
		return LHS >= RHS;
	}

	bool CheckLessOrEqual(const uint8& LHS, const uint8& RHS)
	{
		return LHS <= RHS;
	}

	void OpSet(uint8& LHS, const uint8& RHS)
	{
		LHS = RHS;
	}

	void OpInc(uint8& LHS, const uint8& RHS)
	{
		LHS += RHS;
	}

	void OpDec(uint8& LHS, const uint8& RHS)
	{
		LHS -= RHS;
	}

	void OpNone(uint8& LHS, const uint8& RHS)
	{
		return;
	}
	bool OpValidInc(const uint8& LHS, const uint8& RHS)
	{
		uint32 Sum = LHS + RHS;
		return Sum < 255;
	}

	bool OpValidDec(const uint8& LHS, const uint8& RHS)
	{
		return LHS >= RHS;
	}

	bool OpValidTrue(const uint8& LHS, const uint8& RHS)
	{
		return true;
	}

	TArray<FOperationFunctionPtr> OpFuncs;
	TArray<FOpCheckFunctionPtr> OpValidFuncs;

	TArray<FOperationFunctionPtr> InvOpFuncs;
	TArray<FOpCheckFunctionPtr> InvOpValidFuncs;

	TArray<FConditionFunctionPtr> CondFuncs;

	struct FWSOperationsSetup
	{
		FWSOperationsSetup()
		{
			uint8 NumOps = uint8(ESymbolOp::MAX);
			OpFuncs.Reserve(NumOps);
			OpValidFuncs.Reserve(NumOps);
			InvOpFuncs.Reserve(NumOps);
			InvOpValidFuncs.Reserve(NumOps);

			ensure(OpFuncs.Add(&OpSet) == int32(ESymbolOp::Set));
			ensure(OpValidFuncs.Add(&OpValidTrue) == int32(ESymbolOp::Set));
			ensure(InvOpFuncs.Add(&OpNone) == int32(ESymbolOp::Set));
			ensure(InvOpValidFuncs.Add(&CheckEqual) == int32(ESymbolOp::Set));

			ensure(OpFuncs.Add(&OpInc) == int32(ESymbolOp::Inc));
			ensure(OpValidFuncs.Add(&OpValidInc) == int32(ESymbolOp::Inc));
			ensure(InvOpFuncs.Add(&OpDec) == int32(ESymbolOp::Inc));
			ensure(InvOpValidFuncs.Add(&OpValidDec) == int32(ESymbolOp::Inc));

			ensure(OpFuncs.Add(&OpDec) == int32(ESymbolOp::Dec));
			ensure(OpValidFuncs.Add(&OpValidDec) == int32(ESymbolOp::Dec));
			ensure(InvOpFuncs.Add(&OpInc) == int32(ESymbolOp::Dec));
			ensure(InvOpValidFuncs.Add(&OpValidInc) == int32(ESymbolOp::Dec));
		}
	};
	const FWSOperationsSetup Setup;
}

bool FAISymEffect::Apply(uint8* Values) const
{
	uint8 ValueRHS = (IsRHSAbsolute()) ? Value : Values[(uint8)KeyRHS];

	uint8 KeyLHS = uint8(Key);
	uint8 OpIdx = uint8(Op);
	bool bIsValid = (*(FPlannerWSOperations::OpValidFuncs[OpIdx]))(Values[KeyLHS], ValueRHS);
	if (!bIsValid)
	{
		return false;
	}
	(*(FPlannerWSOperations::OpFuncs[OpIdx]))(Values[KeyLHS], ValueRHS);
	return true;
}

bool FAISymEffect::Revert(uint8* Values) const
{
	uint8 ValueRHS = (IsRHSAbsolute()) ? Value : Values[(uint8)KeyRHS];

	uint8 KeyLHS = uint8(Key);
	uint8 OpIdx = uint8(Op);
	bool bIsValid = (*(FPlannerWSOperations::InvOpValidFuncs[OpIdx]))(Values[KeyLHS], ValueRHS);
	if (!bIsValid)
	{
		return false;
	}
	(*(FPlannerWSOperations::InvOpFuncs[OpIdx]))(Values[KeyLHS], ValueRHS);
	return true;
}
