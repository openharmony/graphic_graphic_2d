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

#include "feature/hyper_graphic_manager/rs_render_frame_rate_linker_map.h"
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"

#undef LOG_TAG
#define LOG_TAG "graphic_2d_hgm"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_FRAME_RATE_LINKER_SIZE = 2048;
}

bool RSRenderFrameRateLinkerMap::RegisterFrameRateLinker(const std::shared_ptr<RSRenderFrameRateLinker>& linkerPtr)
{
    if (frameRateLinkerMap_.size() >= MAX_FRAME_RATE_LINKER_SIZE) {
        ROSEN_LOGE("%{public}s: fail, linker max size %{public}" PRIu32 ", please check frame rate.",
            __func__, MAX_FRAME_RATE_LINKER_SIZE);
        return false;
    }
    auto id = linkerPtr->GetId();
    if (frameRateLinkerMap_.find(id) != frameRateLinkerMap_.end()) {
        return false;
    }
    frameRateLinkerMap_.emplace(id, linkerPtr);
    return true;
}

void RSRenderFrameRateLinkerMap::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    ROSEN_LOGI("%{public}s: RSRenderFrameRateLinkerMap id: %{public}" PRIu64, __func__, id);
    frameRateLinkerMap_.erase(id);
}

void RSRenderFrameRateLinkerMap::FilterFrameRateLinkerByPid(pid_t pid)
{
    ROSEN_LOGD("%{public}s: removing all linker belong to pid %{public}d", __func__, static_cast<int32_t>(pid));
    EraseIf(frameRateLinkerMap_, [pid](const auto& pair) -> bool { return ExtractPid(pair.first) == pid; });
}

std::shared_ptr<RSRenderFrameRateLinker> RSRenderFrameRateLinkerMap::GetFrameRateLinker(FrameRateLinkerId id)
{
    if (auto iter = frameRateLinkerMap_.find(id);
        iter != frameRateLinkerMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

const std::unordered_map<FrameRateLinkerId, std::shared_ptr<RSRenderFrameRateLinker>>&
    RSRenderFrameRateLinkerMap::Get() const
{
    return frameRateLinkerMap_;
}

bool RSRenderFrameRateLinkerMap::RegisterFrameRateLinkerExpectedFpsUpdateCallback(pid_t listenerPid,
    int32_t dstPid, sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback)
{
    bool success = false;
    for (auto& [id, linker] : frameRateLinkerMap_) {
        if (ExtractPid(id) == dstPid && linker != nullptr) {
            linker->RegisterExpectedFpsUpdateCallback(listenerPid, callback);
            success = true;
        }
    }
    if (!success) {
        ROSEN_LOGE("%{public}s: failed: cannot register callback to any linker by dstPid=%{public}d",
            __func__, dstPid);
    }
    return success;
}

void RSRenderFrameRateLinkerMap::UnRegisterExpectedFpsUpdateCallbackByListener(pid_t listenerPid)
{
    for (auto& [_, linker] : frameRateLinkerMap_) {
        if (linker != nullptr) {
            linker->RegisterExpectedFpsUpdateCallback(listenerPid, nullptr);
        }
    }
}

void RSRenderFrameRateLinkerMap::UnregisterFrameRateLinker(const std::unordered_set<FrameRateLinkerId>& unregisterIds)
{
    for (const auto destoryId : unregisterIds) {
        ROSEN_LOGI("%{public}s: id: %{public}" PRIu64, __func__, destoryId);
        frameRateLinkerMap_.erase(destoryId);
    }
}

void RSRenderFrameRateLinkerMap::UpdateFrameRateLinkers(const FrameRateLinkerUpdateInfoMap& updateInfoMap)
{
    for (const auto& [linkerId, updateInfo] : updateInfoMap) {
        if (auto linker = GetFrameRateLinker(linkerId)) {
            linker->SetExpectedRange(updateInfo.range);
            linker->SetAnimatorExpectedFrameRate(updateInfo.animatorExpectedFrameRate);
        }
    }
}
} // namespace Rosen
} // namespace OHOS