/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FRAME_RATE_REPORT_H
#define OHOS_FRAME_RATE_REPORT_H

#include <unordered_map>
#include <cstdint>

namespace FRAME_TRACE {

class FrameRateReport {
public:
    virtual ~FrameRateReport() = default;
    FrameRateReport(const FrameRateReport& fm) = delete;
    FrameRateReport& operator=(const FrameRateReport& fm) = delete;
    FrameRateReport(FrameRateReport&& fm) = delete;
    FrameRateReport& operator=(FrameRateReport&& fm) = delete;
    static inline FrameRateReport& GetInstance()
    {
        if (instance_ != nullptr) {
            return *instance_;
        }
        instance_ = new FrameRateReport();
        return *instance_;
    }
    bool SendFrameRates(const std::unordered_map<int, uint32_t>& rates);
    void SendFrameRatesToRss(const std::unordered_map<int, uint32_t> rates);
private:
    FrameRateReport() = default;
    static FrameRateReport* instance_;
};
} // namespace FRAME_TRACE

#endif // OHOS_FRAME_RATE_REPORT_H