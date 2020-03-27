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
		return rhs.Data.bValue == this->Data.bValue;
	case Type::kObj:
		return rhs.Data.objValue == this->Data.objValue;
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
		return rhs.Data.bValue == this->Data.bValue;
	case Type::kObj:
		return rhs.Data.objValue == this->Data.objValue;
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
		Data.bValue = Other.Data.bValue;
		break;
	case Type::kObj:
		Data.objValue = Other.Data.objValue;
		break;
	case Type::kVariable: //I think this only happens when initializing actions/goals
		Data.kValue = Other.Data.kValue;
		break;

	default:
		//reset
		DataType = Type::kBool;
		Data.bValue = false;
		break;
	}
	
}

FString FWorldProperty::ToString() const
{
	FString KeyString = GETENUMSTRING("EWorldKey", key);
	FString TypeString;
	FString DataString;
	switch (DataType)
	{
	case Type::kBool:
		TypeString = TEXT("bool");
		DataString = (Data.bValue) ? TEXT("True") : TEXT("False");
		break;
	case Type::kVariable:
		TypeString = TEXT("Var");
		DataString = GETENUMSTRING("EWorldKey", Data.kValue);
		break;
	case Type::kObj:
		TypeString = TEXT("Obj");
		DataString = (IsValid(Data.objValue)) ? Data.objValue->GetName() : TEXT("<Nil>");
		break;
	default:
		TypeString = TEXT("ERR");
		DataString = TEXT("ERR");
		break;
	}

	return FString::Printf(TEXT("< %d | %.5s | %.5s > : %d"), (uint8)key, *TypeString, *DataString, bUnsatisfied);
}