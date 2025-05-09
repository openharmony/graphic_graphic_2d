/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <string>

#include "common/rs_common_def.h"
#include "display_engine/rs_vpe_manager.h"
#include "platform/common/rs_log.h"

using namespace OHOS::Media::VideoProcessingEngine;

void VpeVideoCallbackImpl::OnOutputBufferAvailable(uint32_t index, VpeBufferFlag flag)
{
    if (videoFilter_.expired()) {
        RS_LOGE("videoFilter_ is expired");
        return;
    }
    std::shared_ptr<VpeVideo> video = videoFilter_.lock();
    if (video == nullptr) {
        RS_LOGE("video is nullptr");
        return;
    }
    video->ReleaseOutputBuffer(index, true);
}
namespace OHOS {
namespace Rosen {
RSVpeManager& RSVpeManager::GetInstance()
{
    static RSVpeManager instance{};
    return instance;
}

void RSVpeManager::ReleaseVpeVideo(uint64_t nodeId)
{
    std::lock_guard<std::mutex> lock(vpeVideoLock_);
    auto it = allVpeVideo_.find(nodeId);
    if (it == allVpeVideo_.end()) {
        return;
    }
    std::shared_ptr<VpeVideo> vpeVideoImp = it->second;
    if (vpeVideoImp != nullptr) {
        int32_t ret = vpeVideoImp->Stop();
        if (ret != 0) {
            RS_LOGE("vpeVideo stop failed nodeId:%{public}llu, ret:%{public}d", nodeId, ret);
        }
        ret = vpeVideoImp->Release();
        if (ret != 0) {
            RS_LOGE("vpeVideo release failed nodeId:%{public}llu, ret:%{public}d", nodeId, ret);
        }
    }
    allVpeVideo_.erase(nodeId);
    return;
}

std::shared_ptr<VpeVideo> RSVpeManager::GetVpeVideo(uint32_t type, const RSSurfaceRenderNodeConfig& config)
{
    ReleaseVpeVideo(config.id);
    return VpeVideo::Create(type);
}

sptr<Surface> RSVpeManager::GetVpeVideoSurface(uint32_t type, const sptr<Surface>& RSSurface,
    const RSSurfaceRenderNodeConfig& config)
{
    if (config.nodeType != OHOS::Rosen::RSSurfaceNodeType::SELF_DRAWING_NODE) {
        return RSSurface;
    }

    std::shared_ptr<VpeVideo> vpeVideo = GetVpeVideo(type, config);
    if (vpeVideo == nullptr) {
        RS_LOGE("GetVpeVideo failed");
        return RSSurface;
    }

    std::shared_ptr<VpeVideoCallback> cb = std::make_shared<VpeVideoCallbackImpl>(vpeVideo);
    int32_t ret = vpeVideo->RegisterCallback(cb);
    if (ret != 0) {
        RS_LOGE("RegisterCallback failed");
        return RSSurface;
    }

    if (type == VIDEO_TYPE_DETAIL_ENHANCER) {
        Media::Format param{};
        param.PutIntValue(ParameterKey::DETAIL_ENHANCER_QUALITY_LEVEL, DETAIL_ENHANCER_LEVEL_HIGH);
        if (vpeVideo->SetParameter(param) != 0) {
            RS_LOGE("SetParameter failed!");
            return RSSurface;
        }
    }

    ret = vpeVideo->SetOutputSurface(RSSurface);
    if (ret != 0) {
        RS_LOGE("SetOutputSurface failed");
        return RSSurface;
    }

    sptr<Surface> vpeSurface = vpeVideo->GetInputSurface();
    if (vpeSurface == nullptr) {
        RS_LOGE("GetInputSurface failed");
        return RSSurface;
    }

    ret = vpeVideo->Start();
    if (ret != 0) {
        RS_LOGE("start failed");
        return RSSurface;
    }
    uint32_t mapSize = 0;
    {
        std::lock_guard<std::mutex> lock(vpeVideoLock_);
        allVpeVideo_[config.id] = vpeVideo;
        mapSize = allVpeVideo_.size();
    }
    RS_LOGD("type:%{public}u vepSF:%{public}llu RSSF:%{public}llu nodeId:%{public}llu mapSize:%{public}u",
        type, vpeSurface->GetUniqueId(), RSSurface->GetUniqueId(), config.id, mapSize);
    return vpeSurface;
}

sptr<Surface> RSVpeManager::CheckAndGetSurface(const sptr<Surface>& surface, const RSSurfaceRenderNodeConfig& config)
{
    Media::Format parameter{};
    sptr<Surface> vpeSurface = surface;
    std::vector<uint32_t> supportTypes = { VIDEO_TYPE_DETAIL_ENHANCER, VIDEO_TYPE_AIHDR_ENHANCER };
    for (auto& type : supportTypes) {
        if (VpeVideo::IsSupported(type, parameter)) {
            vpeSurface = GetVpeVideoSurface(type, vpeSurface, config);
        }
    }
    return vpeSurface;
}
} // namespace Rosen
} // namespace OHOS