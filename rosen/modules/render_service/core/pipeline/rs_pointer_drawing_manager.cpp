/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "pipeline/rs_pointer_drawing_manager.h"

#include "params/rs_render_params.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_util.h"

namespace OHOS {
namespace Rosen {
RSPointerDrawingManager& RSPointerDrawingManager::Instance()
{
    static RSPointerDrawingManager instance;
    return instance;
}

void RSPointerDrawingManager::UpdatePointerInfo()
{
    int64_t rsNodeId = 0;
    BoundParam boundTemp = {0.0f, 0.0f, 0.0f, 0.0f};
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!BoundHasUpdateCompareChange(true, false)) {
            return;
        }
        rsNodeId = GetRsNodeId();
        boundTemp = GetBound();
    }

    if (rsNodeId <= 0) {
        return;
    }

    // prepare
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(rsNodeId);
    if (node == nullptr) {
        return;
    }
    node->GetMutableRenderProperties().SetBounds({boundTemp.x, boundTemp.y, boundTemp.z, boundTemp.w});
    node->SetDirty();
    node->GetMutableRenderProperties().OnApplyModifiers();
    node->OnApplyModifiers();
    node->AddToPendingSyncList();

    // 1.Set TotalMatrix
    auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode != nullptr) {
        RSUniRenderUtil::UpdateHwcNodeProperty(surfaceNode);
    }

    // 2.update (layerInfo.matrix = TotalMatrix)
    if (surfaceNode->GetScreenId() != -1) {
        auto screenManager = CreateOrGetScreenManager();
        if (!screenManager) {
            RS_LOGE("RSPointerDrawingManager::UpdatePointerInfo screenManager is null!");
            return;
        }
        auto screenInfo = screenManager->QueryScreenInfo(surfaceNode->GetScreenId());
        auto transform = RSUniRenderUtil::GetLayerTransform(*surfaceNode, screenInfo);
        surfaceNode->UpdateHwcNodeLayerInfo(transform, isPointerEnableHwc_);
    }
}

} // namespace Rosen
} // namespace OHOS
