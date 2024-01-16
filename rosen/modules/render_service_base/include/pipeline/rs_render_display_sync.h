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
#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_DISPLAY_SYNC_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_DISPLAY_SYNC_H

#include <refbase.h>

#include "animation/rs_frame_rate_range.h"
#include "animation/rs_render_animation.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderDisplaySync : public std::enable_shared_from_this<RSRenderDisplaySync> {
public:
    explicit RSRenderDisplaySync(NodeId id);
    explicit RSRenderDisplaySync(std::weak_ptr<RSRenderAnimation> renderAnimation);
    ~RSRenderDisplaySync() = default;

    uint64_t GetId() const;
    bool OnFrameSkip(uint64_t timestamp, int64_t period, bool isDisplaySyncEnabled);
    void SetExpectedFrameRateRange(const FrameRateRange& range);
    const FrameRateRange& GetExpectedFrameRange() const;
    void SetAnimateResult(std::tuple<bool, bool, bool>& result);
    std::tuple<bool, bool, bool> GetAnimateResult() const;
private:
    int32_t CalcSkipRateCount(int32_t frameRate);
    int32_t GetNearestFrameRate(int32_t num, const std::vector<int32_t>& rates);

    NodeId id_ = 0;
    uint64_t timestamp_ = 0;
    int64_t currentPeriod_ = 0;
    int64_t currentFrameRate_ = 0;
    int64_t referenceCount_ = 0;
    int64_t skipRateCount_ = 1;

    bool isSkipCountUpdate_ = false;
    std::tuple<bool, bool, bool> animateResult_;
    FrameRateRange expectedFrameRateRange_;
    std::weak_ptr<RSRenderAnimation> renderAnimation_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_RENDER_DISPLAY_SYNC_H
