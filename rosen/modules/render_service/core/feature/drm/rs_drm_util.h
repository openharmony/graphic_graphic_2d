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

#ifndef RS_DRM_UTIL_H
#define RS_DRM_UTIL_H

#include "pipeline/rs_processor.h"

namespace OHOS {
namespace Rosen {

class RSDrmUtil {
public:
    RSDrmUtil() = default;
    ~RSDrmUtil() = default;
    using DrawablesVec = std::vector<std::pair<NodeId, DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>>;

    static void ClearDrmNodes();
    static void CollectDrmNodes(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode);
    static void AddDrmCloneCrossNode(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        DrawablesVec& hardwareEnabledDrawables);
    static void DRMCreateLayer(std::shared_ptr<RSProcessor> processor, Drawing::Matrix hwcMatrix);
    static void PreAllocateProtectedBuffer(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        const std::shared_ptr<RSSurfaceHandler>& surfaceHandler);
private:
    inline static std::unordered_map<NodeId, // map<first level node ID, drm surface node>
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>> drmNodes_ = {};
};

} // namespace Rosen
} // namespace OHOS
#endif // RS_DRM_UTIL_H