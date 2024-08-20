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

#include "pipeline/rs_pointer_window_manager.h"

namespace OHOS {
namespace Rosen {

void RSPointerWindowManager::UpdatePointerDirtyToGlobalDirty(std::shared_ptr<RSSurfaceRenderNode>& pointWindow,
    std::shared_ptr<RSDisplayRenderNode>& curDisplayNode)
{
    if (pointWindow == nullptr || curDisplayNode == nullptr) {
        return;
    }
    auto dirtyManager = pointWindow->GetDirtyManager();
    if (dirtyManager && !pointWindow->IsHardwareForcedDisabled()) {
        if (!pointWindow->GetIsLastFrameHwcEnabled()) {
            RectI lastFrameSurfacePos = curDisplayNode->GetLastFrameSurfacePos(pointWindow->GetId());
            curDisplayNode->GetDirtyManager()->MergeDirtyRect(lastFrameSurfacePos);
        }
        auto pointerWindowDirtyRegion = dirtyManager->GetCurrentFrameDirtyRegion();
        if (!pointerWindowDirtyRegion.IsEmpty()) {
            curDisplayNode->GetDirtyManager()->MergeHwcDirtyRect(pointerWindowDirtyRegion);
            dirtyManager->SetCurrentFrameDirtyRect(RectI());
            isNeedForceCommitByPointer_ = true;
        } else {
            isNeedForceCommitByPointer_ = false;
        }
    }
}
} // namespace Rosen
} // namespace OHOS