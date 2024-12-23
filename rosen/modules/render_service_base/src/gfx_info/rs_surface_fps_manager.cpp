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

#include <memory>

#include "gfx_info/rs_surface_fps_manager.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSSurfaceFpsManager& RSSurfaceFpsManager::GetInstance()
{
    static RSSurfaceFpsManager instance;
    return instance;
}

bool RSSurfaceFpsManager::Register(NodeId id, std::string name)
{
    if (surfaceFpsMap_.find(id) != surfaceFpsMap_.end()) {
        RS_LOGE("RSSurfaceFpsManager::Register node:%{public}" PRIu64 "already existed.", id);
        return false;
    }
    surfaceFpsMap_[id] = std::make_shared<RSSurfaceFps>(name);
    return true;
}
bool RSSurfaceFpsManager::Unregister(NodeId id)
{
    if (surfaceFpsMap_.find(id) == surfaceFpsMap_.end()) {
        RS_LOGE("RSSurfaceFpsManager::Unregister node:%{public}" PRIu64 "not exist.", id);
        return false;
    }
    surfaceFpsMap_.erase(id);
    return true;
}
std::shared_ptr<RSSurfaceFps> RSSurfaceFpsManager::GetSurfaceFps(NodeId id) const
{
    auto iter = surfaceFpsMap_.find(id);
    if (iter == surfaceFpsMap_.end()) {
        return nullptr;
    }
    return iter->second;
}
std::shared_ptr<RSSurfaceFps> RSSurfaceFpsManager::GetSurfaceFps(std::string name) const
{
    for (auto [id, surfaceFps] : surfaceFpsMap_) {
        if (surfaceFps->GetName() == name) {
            return surfaceFps;
        }
    }
    return nullptr;
}
bool RSSurfaceFpsManager::RecordPresentTime(NodeId id, uint64_t timestamp, int32_t seqNum) const
{
    const auto& surfaceFps = GetSurfaceFps(id);
    if (surfaceFps == nullptr) {
        RS_LOGE("RSSurfaceFpsManager::RecordPresentTime surfaceFps is nullptr.");
        return false;
    }
    surfaceFps->RecordPresentTime(timestamp, seqNum);
    return true;
}
void RSSurfaceFpsManager::Dump(std::string& result, std::string name) const
{
    const auto& surfaceFps = GetSurfaceFps(name);
    if (surfaceFps == nullptr) {
        return ;
    }
    result += "\n surface [" + name + "]:\n";
    surfaceFps->Dump(result);
}
void RSSurfaceFpsManager::ClearDump(std::string& result, std::string name) const
{
    const auto& surfaceFps = GetSurfaceFps(name);
    if (surfaceFps == nullptr) {
        return ;
    }
    result += "\n The fps info of surface [" + name + "] is cleared.\n";
    surfaceFps->ClearDump();
}
}