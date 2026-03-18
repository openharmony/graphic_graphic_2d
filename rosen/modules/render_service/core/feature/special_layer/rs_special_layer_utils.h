/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Rosen {
enum class DisplaySpecialLayerState {
    NO_SPECIAL_LAYER = 0,
    HAS_SPECIAL_LAYER = 1,
    CAPTURE_WINDOW = 2,
};

enum class DrawType {
    NONE,       // Normal drawing without special handling
    DRAW_WHITE, // Draw white mask (e.g. security layers in snapshot for single surface)
    DRAW_BLACK, // Draw black mask (e.g. security layers in security display)
    SKIP_DRAW   // Skip drawing without any rendering operation
};

class RSSpecialLayerUtils {
public:
    // Check if virtual screen region intersects with special layer
    static void CheckSpecialLayerIntersectMirrorDisplay(const RSLogicalDisplayRenderNode& mirrorNode,
        RSLogicalDisplayRenderNode& sourceNode);
    static DisplaySpecialLayerState GetSpecialLayerStateInVisibleRect(
        RSLogicalDisplayRenderParams* displayParams, RSScreenRenderParams* screenParams);
    static DisplaySpecialLayerState GetSpecialLayerStateInSubTree(
        RSLogicalDisplayRenderParams& displayParams, RSScreenRenderParams* screenParams);
    static void DumpScreenSpecialLayer(const std::string& funcName,
        SpecialLayerType type, ScreenId screenId, const std::unordered_set<NodeId>& nodeIds);
    static bool NeedProcessSecLayerInDisplay(bool enableVisibleRect, RSScreenRenderParams& mirrorScreenParam,
        RSLogicalDisplayRenderParams& mirrorParam, RSLogicalDisplayRenderParams& sourceParam);
    static bool HasMirrorDisplay(const RSRenderNodeMap& nodeMap);
    static std::unordered_set<uint64_t> GetAllBlackList(const RSRenderNodeMap& nodeMap);
    static std::unordered_set<uint64_t> GetAllWhiteList(const RSRenderNodeMap& nodeMap);
    static std::unordered_set<NodeId> GetMergeBlackList(const RSScreenProperty& screenProperty);
    static void UpdateInfoWithGlobalBlackList(const RSRenderNodeMap& nodeMap);
    static void UpdateScreenSpecialLayer(const RSScreenProperty& screenProperty);
    static void UpdateScreenSpecialLayer(const RSScreenProperty& screenProperty, ScreenPropertyType type);
    static void DealWithSpecialLayer(
        RSSurfaceRenderNode& node, RSLogicalDisplayRenderNode& displayNode, bool needCalcScreenSpecialLayer);
    static DrawType GetDrawTypeInSecurityDisplay(
        const RSSurfaceRenderParams& surfaceParams, const RSRenderThreadParams& uniParams);
    static DrawType GetDrawTypeInSnapshot(const RSSurfaceRenderParams& surfaceParams);
private:
    static bool CheckCurrentTypeIntersectVisibleRect(const std::unordered_set<NodeId>& nodeIds,
        uint32_t currentType, const RectI& visibleRect);
    static void UpdateScreenSpecialLayersRecord(
        RSSurfaceRenderNode& node, RSLogicalDisplayRenderNode& displayNode, bool needCalcScreenSpecialLayer);
    static void UpdateSpecialLayersRecord(RSSurfaceRenderNode& node, RSLogicalDisplayRenderNode& displayNode);
    static void NotifyScreenSpecialLayerChange();
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_FEATURE_RS_SPECIAL_LAYER_UTILS_H