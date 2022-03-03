#pragma once

#define GET_TYPE_STRING(TypeName) \
	((void) sizeof UEAsserts_Private::GetMemberNameCheckedJunk(static_cast<TypeName*>(nullptr)), TEXT(#TypeName))
