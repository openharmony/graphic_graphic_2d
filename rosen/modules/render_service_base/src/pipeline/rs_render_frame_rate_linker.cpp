/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_frame_rate_linker.h"

#include "platform/common/rs_log.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
FrameRateLinkerId RSRenderFrameRateLinker::GenerateId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 0;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        ROSEN_LOGE("RSRenderFrameRateLinker Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((FrameRateLinkerId)pid_ << 32) | (currentId);
}

RSRenderFrameRateLinker::RSRenderFrameRateLinker(FrameRateLinkerId id) : id_(id) {}
RSRenderFrameRateLinker::RSRenderFrameRateLinker() : id_(GenerateId()) {}

void RSRenderFrameRateLinker::SetExpectedRange(const FrameRateRange& range)
{
    expectedRange_ = range;
}

const FrameRateRange& RSRenderFrameRateLinker::GetExpectedRange() const
{
    return expectedRange_;
}

void RSRenderFrameRateLinker::SetFrameRate(uint32_t rate)
{
    frameRate_ = rate;
}

uint32_t RSRenderFrameRateLinker::GetFrameRate() const
{
    return frameRate_;
}
} // namespace Rosen
} // namespace OHOS
