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
#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_SURFACE_RENDER_NODE_H

#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSSurfaceRenderNodeUtils {
public:
    /**
     * @brief Query whether the damage region of hardware enabled node (hwcNode) intersects with the given rect.
     *
     * @note Return false if the node is not hardware enabled type or the buffer has not been comsumed.
     */
    static bool IntersectHwcDamageWith(const RSSurfaceRenderNode& hwcNode, const RectI& rect);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_SURFACE_RENDER_NODE_H
