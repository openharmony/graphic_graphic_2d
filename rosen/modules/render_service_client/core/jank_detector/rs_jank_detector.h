/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_JANK_DETECTOR_RS_JANK_DETECTOR_H
#define RENDER_SERVICE_CLIENT_CORE_JANK_DETECTOR_RS_JANK_DETECTOR_H

#include <mutex>
#include <vector>

namespace OHOS {
namespace Rosen {
class RSJankDetector final {
public:
    RSJankDetector() = default;
    ~RSJankDetector() {}

    uint64_t GetSysTimeNs();
    void SetRefreshPeriod(uint64_t refreshPeriod);
    uint64_t GetRefreshPeriod() const;
    void UpdateUiDrawFrameMsg(uint64_t startTimeStamp, uint64_t endTimeStamp, const std::string& abilityName);
    void CalculateSkippedFrame(uint64_t renderStartTimeStamp, uint64_t renderEndTimeStamp);

private:
    struct UiDrawFrameMsg {
        uint64_t startTimeStamp = 0;
        uint64_t endTimeStamp = 0;
        std::string abilityName;
    };

    const int JANK_SKIPPED_THRESHOLD = 5;
    const uint64_t NO_DRAW_THRESHOLD = 5000000000; // 5s
    uint64_t refreshPeriod_ = 16666667;
    std::vector<UiDrawFrameMsg> uiDrawFrames_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_JANK_DETECTOR_RS_JANK_DETECTOR_H
