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

#ifndef RENDER_SERVICE_BASE_GFX_FPS_INFO_RS_SURFACE_FPS_MANAGER_H
#define RENDER_SERVICE_BASE_GFX_FPS_INFO_RS_SURFACE_FPS_MANAGER_H

#include <shared_mutex>

#include "rs_surface_fps.h"
#include "common/rs_common_def.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSSurfaceFpsManager {
public:
    static RSSurfaceFpsManager &GetInstance();
    bool RegisterSurfaceFps(NodeId id, const std::string& name);
    bool UnregisterSurfaceFps(NodeId id);
    bool RecordPresentTime(NodeId id, uint64_t timestamp, uint32_t seqNum);
    void Dump(std::string& result, NodeId id);
    void ClearDump(std::string& result, NodeId id);
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceFps>> GetSurfaceFpsMap() const;
private:
    RSSurfaceFpsManager() = default;
    ~RSSurfaceFpsManager() = default;
    RSSurfaceFpsManager(const RSSurfaceFpsManager&) = delete;
    RSSurfaceFpsManager(const RSSurfaceFpsManager&&) = delete;
    RSSurfaceFpsManager& operator=(const RSSurfaceFpsManager&) = delete;
    RSSurfaceFpsManager& operator=(const RSSurfaceFpsManager&&) = delete;

    std::shared_ptr<RSSurfaceFps> GetSurfaceFps(NodeId id);
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceFps>> surfaceFpsMap_;
    std::shared_mutex smtx;
};
}
#endif