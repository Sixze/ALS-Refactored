#pragma once

#define GET_TYPE_STRING(TypeName) \
	((void) sizeof UE4Asserts_Private::GetMemberNameCheckedJunk(static_cast<TypeName*>(nullptr)), TEXT(#TypeName))
