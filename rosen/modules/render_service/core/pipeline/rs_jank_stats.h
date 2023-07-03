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

#ifndef ROSEN_JANK_STATS_H
#define ROSEN_JANK_STATS_H

#include <cstdint>
#include <vector>

namespace OHOS {
namespace Rosen {
class RSJankStats {
public:
    static RSJankStats& GetInstance();
    void SetStartTime();
    void SetEndTime();
    void SetRSJankStats(int times);
    void ReportJankStats();

private:
    RSJankStats() {};
    ~RSJankStats() {};
    RSJankStats(const RSJankStats&) = delete;
    RSJankStats(const RSJankStats&&) = delete;
    void operator=(const RSJankStats&) = delete;
    void operator=(const RSJankStats&&) = delete;
    constexpr static int JANK_STATS_SIZE = 8;
    bool isfirstSetStart_ = true;
    bool isNeedReport_ = false;
    int64_t startTime_ = 0;
    int64_t endTime_ = 0;
    int64_t lastReportTime_ = 0;
    std::vector<uint16_t> rsJankStats_ = std::vector<uint16_t>(JANK_STATS_SIZE, 0);

    enum JankRangeType : int16_t {
        JANK_FRAME_6_FREQ = 0,
        JANK_FRAME_15_FREQ,
        JANK_FRAME_20_FREQ,
        JANK_FRAME_36_FREQ,
        JANK_FRAME_48_FREQ,
        JANK_FRAME_60_FREQ,
        JANK_FRAME_120_FREQ,
        JANK_FRAME_180_FREQ,
        JANK_FRAME_INVALID,
    };
};

} // namespace Rosen
} // namespace OHOS
#endif