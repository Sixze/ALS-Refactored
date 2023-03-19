#pragma once

#include "Logging/LogMacros.h"

namespace AlsLog
{
	inline static const FName MessageLogName{TEXTVIEW("Als")};
}

ALS_API DECLARE_LOG_CATEGORY_EXTERN(LogAls, Log, All)
