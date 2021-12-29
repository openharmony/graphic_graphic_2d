/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_manager.h"

#include <string>

#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"

#ifdef ROSEN_OHOS
#include <surface.h>
#include "pipeline/rs_recording_canvas.h"
#include "platform/drawing/rs_platform_canvas.h"
#endif

namespace OHOS {
namespace Rosen {
void RSRenderManager::SetPlatformSurface(OHOS::Surface* surface)
{
#ifdef ROSEN_OHOS
    if (surface == nullptr) {
        ROSEN_LOGE("RSRenderManager::SetPlatformSurface receive null surface");
        return;
    }
    platformCanvas_ = std::make_shared<PlatformCanvas>();
    platformCanvas_->SetSurface(surface);
#endif
}

void RSRenderManager::SetSurfaceSize(int width, int height)
{
#ifdef ROSEN_OHOS
    ROSEN_LOGI("RSRenderManager::SetSurfaceSize [%d %d]", width, height);
    dirtyManager_.SetSurfaceSize(width, height);
    if (platformCanvas_) {
        platformCanvas_->SetSurfaceSize(width, height);
    }
#endif
}

void RSRenderManager::Animate(int64_t timestamp)
{
    hasRunningAnimation_ = false;
    for (const auto& it : RSRenderNodeMap::Instance().renderNodeMap_) {
        if (auto renderNode = RSBaseRenderNode::ReinterpretCast<RSPropertyRenderNode>(it.second)) {
            hasRunningAnimation_ |= renderNode->Animate(timestamp);
        }
    }

    if (hasRunningAnimation_) {
        RSRenderThread::Instance().RequestNextVSync();
    }
}

bool RSRenderManager::HasRunningAnimation() const
{
    return hasRunningAnimation_;
}

void RSRenderManager::UpdateNodes()
{
    auto root = RSBaseRenderNode::ReinterpretCast<RSPropertyRenderNode>(root_.lock());
    if (root != nullptr) {
        root->Update(dirtyManager_, nullptr, false);
    }
    dirtyManager_.IntersectDirtyRect(GetSurfaceRegion());
}

bool RSRenderManager::IsDirty() const
{
    return dirtyManager_.IsDirty();
}

void RSRenderManager::SetRoot(RSBaseRenderNode::SharedPtr& root)
{
    root_ = root;
}

RectI RSRenderManager::GetDirtyRegion() const
{
    return dirtyManager_.GetDirtyRegion();
}

RectI RSRenderManager::GetSurfaceRegion() const
{
    return RectI(0, 0, dirtyManager_.GetSurfaceWidth(), dirtyManager_.GetSurfaceHeight());
}

void RSRenderManager::Clear()
{
    dirtyManager_.Clear();
}

std::string RSRenderManager::DumpRenderTree() const
{
    std::string outString;
    auto root = root_.lock();
    if (root != nullptr) {
        root->DumpTree(outString);
    }
    return outString;
}
} // namespace Rosen
} // namespace OHOS
