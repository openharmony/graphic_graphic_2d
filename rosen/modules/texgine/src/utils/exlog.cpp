/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "texgine/utils/exlog.h"

#include <chrono>
#include <ctime>
#include <iomanip>

namespace OHOS {
namespace TextEngine {
void ExTime(Logger &algnlogger, enum Logger::LOG_PHASE phase)
{
    if (phase == Logger::LOG_PHASE::BEGIN) {
        auto timer = time(nullptr);
        auto now = localtime(&timer);
        int64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() + (8 * 1000LL * 60 * 60);
        algnlogger << std::setw(4) << std::setfill('0') << (now->tm_year + 1900)
            << "/" << std::setw(2) << std::setfill('0') << (now->tm_mon + 1)
            << "/" << std::setw(2) << std::setfill('0') << (now->tm_mday)
            << " " << std::setw(2) << std::setfill('0') << (nowMs / (1000LL * 60 * 60) % 24)
            << ":" << std::setw(2) << std::setfill('0') << (nowMs / (1000LL * 60) % 60)
            << ":" << std::setw(2) << std::setfill('0') << (nowMs / (1000LL) % 60)
            << "." << std::setw(3) << std::setfill('0') << (nowMs % 1000);
    }

    if (algnlogger.GetLevel() != Logger::LOG_LEVEL::DEBUG) {
        Logger::Stdout(algnlogger, phase);
    }
}
} // namespace TextEngine
} // namespace OHOS
