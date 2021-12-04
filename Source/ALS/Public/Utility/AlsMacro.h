#pragma once

#define GET_TYPE_STRING(TypeName) \
	((void)sizeof(UE4Asserts_Private::GetMemberNameCheckedJunk((TypeName*)0)), TEXT(#TypeName))

