#include "WorldProperty.h"

bool FWorldProperty::Equals(const FWorldProperty& rhs)
{
	if (rhs.DataType != this->DataType)
	{
		return false;
	}
	switch (DataType)
	{
	case Type::kBool:
		return rhs.nValue == this->nValue;
	case Type::kVariable:
		return true;
	default:
		return false;
	}
}

bool FWorldProperty::Equals(const FWorldProperty& rhs) const
{
	if (rhs.DataType != this->DataType)
	{
		return false;
	}
	switch (DataType)
	{
	case Type::kBool:
		return rhs.nValue == this->nValue;
	case Type::kVariable:
		return true;
	default:
		return false;
	}

}

void FWorldProperty::Apply(const FWorldProperty& Other)
{
	DataType = Other.DataType;
	switch (Other.DataType) 
	{
	case Type::kBool:
		nValue = Other.nValue;
		break;
	case Type::kVariable: //I think this only happens when initializing actions/goals
		nValue = Other.nValue;
		break;

	default:
		//reset
		DataType = Type::kBool;
		nValue = false;
		break;
	}
	
}

FString FWorldProperty::ToString() const
{
	FString KeyString = GETENUMSTRING("EWorldKey", Key);
	FString TypeString;
	FString DataString;
	switch (DataType)
	{
	case Type::kBool:
		TypeString = TEXT("bool");
		DataString = (nValue) ? TEXT("True") : TEXT("False");
		break;
	case Type::kVariable:
		TypeString = TEXT("Var");
		DataString = GETENUMSTRING("EWorldKey", (EWorldKey)nValue);
		break;
	default:
		TypeString = TEXT("ERR");
		DataString = TEXT("ERR");
		break;
	}

	return FString::Printf(TEXT("< %d | %.5s | %.5s > : %d"), (uint8)Key, *TypeString, *DataString, bUnsatisfied);
}