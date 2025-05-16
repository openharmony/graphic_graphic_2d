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

#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"
#include "platform/common/rs_log.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
const std::chrono::steady_clock::duration NATIVE_VSYNC_FALLBACK_INTERVAL = 
    std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::milliseconds(200));
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

RSRenderFrameRateLinker::RSRenderFrameRateLinker(FrameRateLinkerId id, ObserverType observer)
    : id_(id), observer_(observer)
{
    Notify();
}

RSRenderFrameRateLinker::RSRenderFrameRateLinker(ObserverType observer)
    : RSRenderFrameRateLinker(GenerateId(), observer) {}

RSRenderFrameRateLinker::RSRenderFrameRateLinker(FrameRateLinkerId id) : RSRenderFrameRateLinker(id, nullptr) {}

RSRenderFrameRateLinker::RSRenderFrameRateLinker() : RSRenderFrameRateLinker(GenerateId(), nullptr) {}

RSRenderFrameRateLinker::RSRenderFrameRateLinker(const RSRenderFrameRateLinker& other)
{
    Copy(std::move(other));
}

RSRenderFrameRateLinker::RSRenderFrameRateLinker(const RSRenderFrameRateLinker&& other)
{
    Copy(std::move(other));
}

RSRenderFrameRateLinker& RSRenderFrameRateLinker::operator=(const RSRenderFrameRateLinker& other)
{
    Copy(std::move(other));
    return *this;
}

RSRenderFrameRateLinker& RSRenderFrameRateLinker::operator=(const RSRenderFrameRateLinker&& other)
{
    Copy(std::move(other));
    return *this;
}

RSRenderFrameRateLinker::~RSRenderFrameRateLinker()
{
    Notify();
}

void RSRenderFrameRateLinker::SetExpectedRange(const FrameRateRange& range)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (expectedRange_.preferred_ != range.preferred_) {
        for (auto& [_, cb] : expectedFpsChangeCallbacks_) {
            if (cb) {
                cb->OnFrameRateLinkerExpectedFpsUpdate(ExtractPid(id_), range.preferred_);
            }
        }
    }

    if (expectedRange_ != range) {
        expectedRange_ = range;
        Notify();
    }
}

const FrameRateRange& RSRenderFrameRateLinker::GetExpectedRange() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return expectedRange_;
}

void RSRenderFrameRateLinker::SetFrameRate(uint32_t rate)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (frameRate_ != rate) {
        frameRate_ = rate;
        Notify();
    }
}

uint32_t RSRenderFrameRateLinker::GetFrameRate() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return frameRate_;
}

void RSRenderFrameRateLinker::SetAnimatorExpectedFrameRate(int32_t animatorExpectedFrameRate)
{
    if (animatorExpectedFrameRate_ != animatorExpectedFrameRate) {
        animatorExpectedFrameRate_ = animatorExpectedFrameRate;
        Notify();
    }
}

void RSRenderFrameRateLinker::SetVsyncName(const std::string& vsyncName)
{
    vsyncName_ = vsyncName;
}

void RSRenderFrameRateLinker::Copy(const RSRenderFrameRateLinker&& other)
{
    id_ = other.id_;
    expectedRange_ = other.expectedRange_;
    frameRate_ = other.frameRate_;
    animatorExpectedFrameRate_ = other.animatorExpectedFrameRate_;
}

void RSRenderFrameRateLinker::Notify()
{
    if (observer_ != nullptr) {
        observer_(*this);
    }
}

void RSRenderFrameRateLinker::RegisterExpectedFpsUpdateCallback(pid_t listener,
    sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback)
{
    if (callback == nullptr) {
        expectedFpsChangeCallbacks_.erase(listener);
        return;
    }

    // if this listener has registered a callback before, replace it.
    expectedFpsChangeCallbacks_[listener] = callback;
    callback->OnFrameRateLinkerExpectedFpsUpdate(ExtractPid(id_), expectedRange_.preferred_);
}

void RSRenderFrameRateLinker::UpdateNativeVSyncTimePoint()
{
    nativeVSyncTimePoint_.store(std::chrono::steady_clock::now());
}

bool RSRenderFrameRateLinker::NativeVSyncIsTimeOut() const
{
    return std::chrono::steady_clock::now() - nativeVSyncTimePoint_.load() > NATIVE_VSYNC_FALLBACK_INTERVAL;
}
} // namespace Rosen
} // namespace OHOS
