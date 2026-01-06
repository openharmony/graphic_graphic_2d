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
#include <unordered_set>

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/rs_screen_property.h"

namespace OHOS {
namespace Rosen {
class RSSpecialLayerUtils {
public:
    static std::unordered_set<uint64_t> GetAllBlackList(const RSRenderNodeMap& nodeMap);
    static std::unordered_set<uint64_t> GetAllWhiteList(const RSRenderNodeMap& nodeMap);

    static std::unordered_set<NodeId> GetMergeBlackList(const RSScreenProperty& screenProperty);

    static void DumpScreenSpecialLayer(const std::string& funcName,
       SpecialLayerType type, ScreenId screenId, const std::unordered_set<NodeId>& nodeIds);

    static void UpdateScreenSpecialLayer(
        const RSScreenProperty& newProperty = {}, const RSScreenProperty& oldProperty = {});

    static void DealWithSpecialLayer(
        RSSurfaceRenderNode& node, RSLogicalDisplayRenderNode& displayNode, bool needCalcScreenSpecialLayer);
    
    static void UpdateScreenSpecialLayersRecord(
        RSSurfaceRenderNode& node, RSLogicalDisplayRenderNode& displayNode, bool needCalcScreenSpecialLayer);

    static void UpdateSpecialLayersRecord(RSSurfaceRenderNode& node, RSLogicalDisplayRenderNode& displayNode);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_FEATURE_RS_SPECIAL_LAYER_UTILS_H