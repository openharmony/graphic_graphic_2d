/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ROSEN_COMPOSER_JANK_STATS_H
#define ROSEN_COMPOSER_JANK_STATS_H

#include <cstdint>
#include <deque>
#include "nocopyable.h"

namespace OHOS {
namespace Rosen {
constexpr int FRAME_RATE_WINDOW_SIZE = 9;
constexpr int FRAME_WINDOW_MIDDLE_NUM = 4;

class RSComposerJankStats {
public:
    static RSComposerJankStats& GetInstance();
    void CalculateJankInfo(int64_t timestamp);
private:
    RSComposerJankStats() = default;
    ~RSComposerJankStats() = default;
    DISALLOW_COPY_AND_MOVE(RSComposerJankStats);
    int GetRate(int i);
    void CheckRefreshRate();
    bool IsAllEqual();
    bool IsAllLessThan60Hz();
    bool IsDtrictlyIncreasing();
    bool IsDtrictlyDecreasing();
    std::deque<int> refreshRates_;
    int64_t lastTimestamp_ = 0;
}
}
}