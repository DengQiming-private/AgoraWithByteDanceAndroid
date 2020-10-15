#pragma once
#include "IAgoraLog.h"

namespace agora{
    namespace rtc{
        class IExtensionFacility {
        public:
            virtual ~IExtensionFacility() = default;
            virtual void fireEvent(const char* key, const char* json_value) = 0;
            virtual void log(commons::LOG_LEVEL level, const char* message) = 0;
        };
    }
}