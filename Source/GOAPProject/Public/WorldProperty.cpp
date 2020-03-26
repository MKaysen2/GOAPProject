#include "WorldProperty.h"

void FWorldProperty::Apply(const FWorldProperty& Other)
{
	DataType = Other.DataType;
	switch (Other.DataType) 
	{
	case Type::kBool:
		Data.bValue = Other.Data.bValue;
		break;

	default:
		
		break;
	}
	
}
