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

#include "params/rs_root_render_params.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSRootRenderParams::RSRootRenderParams(NodeId id) : RSRenderParams(id) {}

void RSRootRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetRootRenderParam = static_cast<RSRootRenderParams*>(target.get());
    if (targetRootRenderParam == nullptr) {
        RS_LOGE("RSRootRenderParams::OnSync targetCanvasDrawingParam is null");
        return;
    }
    targetRootRenderParam->culledNodes_ = std::move(culledNodes_);
    targetRootRenderParam->isOcclusionCullingOn_ = isOcclusionCullingOn_;

    RSRenderParams::OnSync(target);
}
} // namespace OHOS::Rosen
