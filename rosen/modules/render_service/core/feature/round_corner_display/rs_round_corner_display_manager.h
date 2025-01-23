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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_RCD_MANAGER_H
#define RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_RCD_MANAGER_H

#include <mutex>
#include <thread>
#include <unordered_map>

#include "common/rs_common_def.h"
#include "rs_round_corner_display.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {

class RoundCornerDisplayManager {
public:
enum class RCDLayerType : uint32_t {
    INVALID = 0,
    TOP = 1,
    BOTTOM = 2
};

static bool CheckRcdRenderEnable(const ScreenInfo& screenInfo)
{
    return screenInfo.state == ScreenState::HDI_OUTPUT_ENABLE;
}

public:
    using RoundCornerDisplayMap = std::unordered_map<NodeId, std::shared_ptr<RoundCornerDisplay>>;
    using RCDLayerMap = std::unordered_map<std::string, std::pair<NodeId, RCDLayerType>>;
    using RCDLayerInfoVec = std::vector<std::pair<NodeId, RCDLayerType>>;
    RoundCornerDisplayManager();
    virtual ~RoundCornerDisplayManager();

    // add rendertarget nodeId info to map by layername, if layername exist update info
    void AddLayer(const std::string& name, NodeId id, RCDLayerType type);

    // Get rendertarget nodeId info via layername
    std::pair<NodeId, RCDLayerType> GetLayerPair(const std::string& layerName);

    // Check the layer is RCD layer via layername
    bool CheckLayerIsRCD(const std::string& layerName);

    // the soft draw api: draw all rendertarget node input and canvas.
    void DrawRoundCorner(const RCDLayerInfoVec& layerInfos, RSPaintFilterCanvas* canvas);

    // only add once rcd module for screen via nodeId
    void AddRoundCornerDisplay(NodeId id);

    // remove rcd resuorce for screen via nodeId
    void RemoveRCDResource(NodeId id);

    // update displayWidth_ and displayHeight_
    void UpdateDisplayParameter(NodeId id, uint32_t left, uint32_t top, uint32_t width, uint32_t height);

    // update notchStatus_
    void UpdateNotchStatus(NodeId id, int status);

    // update curOrientation_ and lastOrientation_
    void UpdateOrientationStatus(NodeId id, ScreenRotation orientation);

    // update hardwareInfo_.resourceChanged and resourcePreparing
    void UpdateHardwareResourcePrepared(NodeId id, bool prepared);

    // handle rcdDirtyType_ and assign dirty rect
    bool HandleRoundCornerDirtyRect(NodeId id, RectI &dirtyRect, const RCDLayerType type);

    // run rcd hardwareComposer buffer prepare task via rendertarget ID
    void RunHardwareTask(NodeId id, const std::function<void()>& task);

    // get the hardware info via rendertarget ID which rcd hardwareComposer buffer prepare task needed
    rs_rcd::RoundCornerHardware GetHardwareInfo(NodeId id, bool preparing = false);

    // get the rcd enable tag
    bool GetRcdEnable() const;

    // get is notch need update tag for the rendertarget id
    bool IsNotchNeedUpdate(NodeId id, bool notchStatus);

private:
    bool CheckExist(NodeId id);

    // the soft draw top rcd api: draw rendertarget node input and canvas.
    void DrawTopRoundCorner(NodeId id, RSPaintFilterCanvas* canvas);

    // the soft draw bottom rcd api: draw rendertarget node input and canvas.
    void DrawBottomRoundCorner(NodeId id, RSPaintFilterCanvas* canvas);

    // remove rcd module for screen via nodeId
    void RemoveRoundCornerDisplay(NodeId id);

    // remove rcd layer info for screen via nodeId
    void RemoveRCDLayerInfo(NodeId id);

    std::mutex rcdMapMut_;
    RoundCornerDisplayMap rcdMap_; // key, value : rendertargetNodeId, rcd module
    std::mutex rcdLayerMapMut_;
    RCDLayerMap rcdlayerMap_; // key, value : rcdLayerName, rendertargetNodeId
};
} // namespace Rosen
} // namespace OHOS
#endif