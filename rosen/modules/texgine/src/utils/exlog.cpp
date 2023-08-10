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
namespace Rosen {
namespace TextEngine {
void ExTime(Logger &algnlogger, enum Logger::LOG_PHASE phase)
{
    if (phase == Logger::LOG_PHASE::BEGIN) {
        auto timer = time(nullptr);
        auto now = localtime(&timer);
        if (now == nullptr) {
            return;
        }
        int64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() + (8 * 1000LL * 60 * 60);
        int year = static_cast<int>(now->tm_year) + 1900;
        int month = now->tm_mon + 1;
        int day = now->tm_mday;
        int hour = nowMs / (1000LL * 60 * 60) % 24;
        int minute = nowMs / (1000LL * 60) % 60;
        int second = nowMs / (1000LL) % 60;
        int milliseconds = nowMs % 1000;
        int longWidth = 4;
        int middleWidth = 3;
        int minWidth = 2;
        char fill = '0';
        algnlogger << std::setw(longWidth) << std::setfill(fill) << year
            << "/" << std::setw(minWidth) << std::setfill(fill) << month
            << "/" << std::setw(minWidth) << std::setfill(fill) << day
            << " " << std::setw(minWidth) << std::setfill(fill) << hour
            << ":" << std::setw(minWidth) << std::setfill(fill) << minute
            << ":" << std::setw(minWidth) << std::setfill(fill) << second
            << "." << std::setw(middleWidth) << std::setfill(fill) << milliseconds;
    }

    if (algnlogger.GetLevel() != Logger::LOG_LEVEL::DEBUG) {
        Logger::OutputByStdout(algnlogger, phase);
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
