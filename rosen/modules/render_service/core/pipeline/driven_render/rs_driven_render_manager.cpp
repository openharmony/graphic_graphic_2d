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

#include "rs_driven_render_manager.h"

#include <parameters.h>
#include "platform/common/rs_log.h"
#include "rs_driven_render_ext.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
static std::unique_ptr<RSDrivenRenderManager> g_drivenRenderManagerInstance =
    std::make_unique<RSDrivenRenderManager>();

RSDrivenRenderManager& RSDrivenRenderManager::GetInstance()
{
    return *g_drivenRenderManagerInstance;
}

void RSDrivenRenderManager::InitInstance()
{
    if (!system::GetBoolParameter("persist.rosen.drivenrender.enabled", false)) {
        RS_LOGD("RSDrivenRenderManager: driven render not enabled.");
        return;
    }
    RSDrivenRenderExt::Instance().OpenDrivenRenderExt();
    RSDrivenRenderManager* instance = RSDrivenRenderExt::Instance().CreateDrivenRenderManager();
    if (!instance) {
        RS_LOGE("RSDrivenRenderManager: init instance failed!");
        return;
    }
    g_drivenRenderManagerInstance.reset(instance);
    g_drivenRenderManagerInstance->drivenRenderEnabled_ = true;
}

bool RSDrivenRenderManager::GetDrivenRenderEnabled() const
{
    return drivenRenderEnabled_ && system::GetBoolParameter("rosen.debug.drivenrender.enabled", true);
}

const DrivenUniRenderMode& RSDrivenRenderManager::GetUniDrivenRenderMode() const
{
    return uniRenderMode_;
}

float RSDrivenRenderManager::GetUniRenderGlobalZOrder() const
{
    return uniRenderGlobalZOrder_;
}

void RSDrivenRenderManager::ClipHoleForDrivenNode(RSPaintFilterCanvas& canvas, const RSCanvasRenderNode& node) const
{
    auto contentSurfaceNode = RSDrivenRenderManager::GetInstance().GetContentSurfaceNode();
    if (contentSurfaceNode->GetDrivenCanvasNode() == nullptr ||
        contentSurfaceNode->GetDrivenCanvasNode()->GetId() != node.GetId()) {
        return;
    }
    auto& property = node.GetRenderProperties();
    Vector4f clipHoleRect = property.GetFrame();
    static int pixel = -1; // clip hole rect should large than content bounds
    auto x = std::ceil(clipHoleRect.x_ + pixel); // x decrease 1 pixel
    auto y = std::ceil(clipHoleRect.y_ + pixel); // y decrease 1 pixel
    auto width = std::floor(clipHoleRect.z_ - (2 * pixel)); // width increase 2 pixels
    auto height = std::floor(clipHoleRect.w_ - (2 * pixel)); // height increase 2 pixels
    RRect absClipRRect = RRect({x, y, width, height}, property.GetCornerRadius());

    // clip hole
    canvas.save();
    canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    canvas.clear(SK_ColorTRANSPARENT);
    canvas.restore();
}
} // namespace Rosen
} // namespace OHOS
