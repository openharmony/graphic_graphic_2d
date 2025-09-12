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
#ifndef RS_SURFACE_WATERMARK_H
#define RS_SURFACE_WATERMARK_H
#include <cstdint>
#include "pixel_map.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "screen_manager/rs_screen_manager.h"
#include "render/rs_pixel_map_util.h"
namespace OHOS {
namespace Rosen {

class RSSurfaceWatermarkHelper {
public:
    RSSurfaceWatermarkHelper() = default;
    ~RSSurfaceWatermarkHelper() = default;
    uint32_t SetSurfaceWatermark(pid_t pid, const std::string& name, std::shared_ptr<Media::PixelMap> pixelMap,
        const std::vector<NodeId>& nodeIdList, SurfaceWatermarkType watermarkType,
        RSContext& mainContext, bool isSystemCalling = false);
    void ClearSurfaceWatermarkForNodes(pid_t pid, const std::string& name,
        const std::vector<NodeId>& nodeIdList, RSContext& mainContext, bool isSystemCalling = false);
    void ClearSurfaceWatermark(pid_t pid,
        const std::string& name, RSContext& mainContext, bool isSystemCalling = false, bool isDeathMonitor = false);
    void ClearSurfaceWatermark(pid_t pid, RSContext& mainContext);
    inline std::unordered_map<std::string, std::pair<std::shared_ptr<Drawing::Image>,
        pid_t>> &GetSurfaceWatermarks()
    {
        return surfaceWatermarks_;
    }
private:
    template<typename Container>
    void TraverseAndOperateNodeList(const pid_t& pid, const std::string& name,
        const Container& nodeIdList, RSContext& mainContext,
        std::function<uint32_t(const std::string& name, std::shared_ptr<RSSurfaceRenderNode>)> func,
        const bool& isSystemCalling);
    inline void ClearWatermarkNameMapNodeId(const std::string& name)
    {
        watermarkNameMapNodeId_.erase(name);
    }
    inline void RemoveNodeIdForWatermarkNameMapNodeId(const std::string& name, NodeId id)
    {
        auto iter = watermarkNameMapNodeId_.find(name);
        if (iter == watermarkNameMapNodeId_.end()) {
            return;
        }
        iter->second.first.erase(id);
    }

    inline bool CheckClearWatermarkPermission(const pid_t& pid, const std::string& name, bool isSystemCalling)
    {
        auto iter = surfaceWatermarks_.find(name);
        if (iter == surfaceWatermarks_.end()) {
            return false;
        }
        if (!isSystemCalling && iter->second.second != pid) {
            return false;
        }
        return true;
    }

    inline SurfaceWatermarkType GetWatermarkType(const std::string& name)
    {
        auto iter = watermarkNameMapNodeId_.find(name);
        if (iter == watermarkNameMapNodeId_.end()) {
            return INVALID_WATER_MARK;
        }
        return iter->second.second;
    }
    ScreenInfo GetScreenInfo(const std::shared_ptr<RSSurfaceRenderNode>& node, RSContext& mainContext);
    void AddWatermarkNameMapNodeId(std::string name, NodeId id, SurfaceWatermarkType watermarkType);
    std::unordered_map<std::string, std::pair<std::shared_ptr<Drawing::Image>, pid_t>> surfaceWatermarks_ = {};
    // Quickly clear watermark when the process is destroyed.
    std::unordered_map<std::string, std::pair<std::unordered_set<NodeId>,
        SurfaceWatermarkType>> watermarkNameMapNodeId_ = {};
    uint32_t globalErrorCode_ = 0;
    uint32_t registerSurfaceWatermarkCount_ = 0;
    bool hasNodeOperationSuccess_ = false;
};
} // Rosen
} // OHOS

#endif