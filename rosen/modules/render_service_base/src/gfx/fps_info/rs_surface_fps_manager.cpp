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

#include "common/rs_common_def.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"

namespace OHOS::Rosen {
RSSurfaceFpsManager& RSSurfaceFpsManager::GetInstance()
{
    static RSSurfaceFpsManager instance;
    return instance;
}

bool RSSurfaceFpsManager::RegisterSurfaceFps(NodeId id, const std::string& name)
{
    std::unique_lock<std::shared_mutex> lock(smtx);
    if (surfaceFpsMap_.find(id) != surfaceFpsMap_.end()) {
        return false;
    }
    surfaceFpsMap_[id] = std::make_shared<RSSurfaceFps>(name);
    return true;
}

bool RSSurfaceFpsManager::UnregisterSurfaceFps(NodeId id)
{
    std::unique_lock<std::shared_mutex> lock(smtx);
    if (surfaceFpsMap_.find(id) == surfaceFpsMap_.end()) {
        return false;
    }
    surfaceFpsMap_.erase(id);
    return true;
}

std::shared_ptr<RSSurfaceFps> RSSurfaceFpsManager::GetSurfaceFps(NodeId id)
{
    std::shared_lock<std::shared_mutex> lock(smtx);
    auto iter = surfaceFpsMap_.find(id);
    if (iter == surfaceFpsMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

std::shared_ptr<RSSurfaceFps> RSSurfaceFpsManager::GetSurfaceFps(const std::string& name)
{
    std::shared_lock<std::shared_mutex> lock(smtx);
    for (auto [id, surfaceFps] : surfaceFpsMap_) {
        if (surfaceFps->GetName() == name) {
            return surfaceFps;
        }
    }
    return nullptr;
}

std::shared_ptr<RSSurfaceFps> RSSurfaceFpsManager::GetSurfaceFpsByPid(pid_t pid)
{
    std::shared_lock<std::shared_mutex> lock(smtx);
    for (auto [id, surfaceFps] : surfaceFpsMap_) {
        if (ExtractPid(id) == pid) {
            return surfaceFps;
        }
    }
    return nullptr;
}

bool RSSurfaceFpsManager::RecordPresentTime(NodeId id, uint64_t timestamp, uint32_t seqNum)
{
    const auto& surfaceFps = GetSurfaceFps(id);
    if (surfaceFps == nullptr) {
        return false;
    }
    return surfaceFps->RecordPresentTime(timestamp, seqNum);
}

void RSSurfaceFpsManager::Dump(std::string& result, const std::string& name)
{
    const auto& surfaceFps = GetSurfaceFps(name);
    if (surfaceFps == nullptr) {
        return ;
    }
    result += " surface [" + name + "]:\n";
    surfaceFps->Dump(result);
}

void RSSurfaceFpsManager::ClearDump(std::string& result, const std::string& name)
{
    const auto& surfaceFps = GetSurfaceFps(name);
    if (surfaceFps == nullptr) {
        return ;
    }
    result += " The fps info of surface [" + name + "] is cleared.\n";
    surfaceFps->ClearDump();
}

void RSSurfaceFpsManager::Dump(std::string& result, NodeId id)
{
    const auto& surfaceFps = GetSurfaceFps(id);
    if (surfaceFps == nullptr) {
        return ;
    }
    result += " surface [" + surfaceFps->GetName() + "]:\n";
    surfaceFps->Dump(result);
}

void RSSurfaceFpsManager::ClearDump(std::string& result, NodeId id)
{
    const auto& surfaceFps = GetSurfaceFps(id);
    if (surfaceFps == nullptr) {
        return ;
    }
    result += " The fps info of surface [" + surfaceFps->GetName() + "] is cleared.\n";
    surfaceFps->ClearDump();
}

void RSSurfaceFpsManager::DumpByPid(std::string& result, pid_t pid)
{
    const auto& surfaceFps = GetSurfaceFpsByPid(pid);
    if (surfaceFps == nullptr) {
        return ;
    }
    result += " surface [" + surfaceFps->GetName() + "]:\n";
    surfaceFps->Dump(result);
}

void RSSurfaceFpsManager::ClearDumpByPid(std::string& result, pid_t pid)
{
    const auto& surfaceFps = GetSurfaceFpsByPid(pid);
    if (surfaceFps == nullptr) {
        return ;
    }
    result += " The fps info of surface [" + surfaceFps->GetName() + "] is cleared.\n";
    surfaceFps->ClearDump();
}

std::unordered_map<NodeId, std::shared_ptr<RSSurfaceFps>> RSSurfaceFpsManager::GetSurfaceFpsMap() const
{
    return surfaceFpsMap_;
}
}