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
#ifndef RENDER_SERVICE_CORE_FEATURE_RS_SPECIAL_LAYER_UTILS_H
#define RENDER_SERVICE_CORE_FEATURE_RS_SPECIAL_LAYER_UTILS_H

#include "common/rs_common_def.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_map.h"

namespace OHOS {
namespace Rosen {
enum class DisplaySpecialLayerState {
    NO_SPECIAL_LAYER = 0,
    HAS_SPECIAL_LAYER = 1,
    CAPTURE_WINDOW = 2,
};

class RSSpecialLayerUtils {
public:
    // Check if virtual screen region intersects with special layer
    static void CheckSpecialLayerIntersectMirrorDisplay(const RSLogicalDisplayRenderNode& mirrorNode,
        RSLogicalDisplayRenderNode& sourceNode, bool enableVisibleRect);

    static DisplaySpecialLayerState GetSpecialLayerStateInVisibleRect(
        RSLogicalDisplayRenderParams* displayParams, RSScreenRenderParams* screenParams);
    static DisplaySpecialLayerState GetSpecialLayerStateInSubTree(
        RSLogicalDisplayRenderParams& displayParams, RSScreenRenderParams* screenParams);
    static void DumpScreenSpecialLayer(const std::string& funcName,
        SpecialLayerType type, ScreenId screenId, const std::unordered_set<NodeId>& nodeIds);
private:
    static bool CheckCurrentTypeIntersectVisibleRect(const std::unordered_set<NodeId>& nodeIds,
        uint32_t currentType, const RectI& visibleRect);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_FEATURE_RS_SPECIAL_LAYER_UTILS_H