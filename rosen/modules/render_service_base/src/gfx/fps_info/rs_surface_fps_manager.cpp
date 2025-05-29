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
#include <string>
#include <codecvt>
#include <locale>

#include "common/rs_common_def.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"

namespace OHOS::Rosen {
RSSurfaceFpsManager& RSSurfaceFpsManager::GetInstance()
{
    static RSSurfaceFpsManager instance;
    return instance;
}

bool ConvertToLongLongUint(const std::string& str, uint64_t& value, int8_t base = 10)
{
    char* end;
    errno = 0;
    value = std::strtoull(str.c_str(), &end, base);
    if (end == str.c_str()) {
        return false;
    }
    if (errno == ERANGE && value == ULLONG_MAX) {
        return false;
    }
    if (*end != '\0') {
        return false;
    }
    return true;
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

std::shared_ptr<RSSurfaceFps> RSSurfaceFpsManager::GetSurfaceFps(const std::string& name, bool& isUnique)
{
    std::shared_lock<std::shared_mutex> lock(smtx);
    std::shared_ptr<RSSurfaceFps> surfaceFpsPtr = nullptr;
    uint8_t sameNameNumber = 0;
    for (auto [id, surfaceFps] : surfaceFpsMap_) {
        if (surfaceFps->GetName() == name) {
            surfaceFpsPtr = surfaceFps;
            sameNameNumber++;
        }
    }
    isUnique = (sameNameNumber == 1);
    return surfaceFpsPtr;
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
    bool isUnique(false);
    const auto& surfaceFps = GetSurfaceFps(name, isUnique);
    if (surfaceFps == nullptr) {
        return ;
    } else if (!isUnique) {
        result = "There are multiple surfaces with the same name, please use \"fps -id NodeId\" to query.\n";
        return ;
    }
    result += " surface [" + name + "]:\n";
    surfaceFps->Dump(result);
}

void RSSurfaceFpsManager::ClearDump(std::string& result, const std::string& name)
{
    bool isUnique(false);
    const auto& surfaceFps = GetSurfaceFps(name, isUnique);
    if (surfaceFps == nullptr) {
        return ;
    } else if (!isUnique) {
        result = "There are multiple surfaces with the same name, please use \"fps -id NodeId\" to query.\n";
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

void RSSurfaceFpsManager::ProcessParam(
    const std::unordered_set<std::u16string>& argSets, std::string& option, std::string& argStr)
{
    if (argSets.size() == 1) {
        option = PARAM_NAME;
    }
    for (const std::u16string& arg : argSets) {
        std::string str = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}
            .to_bytes(arg);
        if (options.find(str) != options.end()) {
            option = str;
        } else {
            argStr = str;
        }
    }
    return ;
}

bool RSSurfaceFpsManager::IsSurface(const std::string& option, const std::string& argStr)
{
    return option == PARAM_ID || uniRenderArgs.find(argStr) == uniRenderArgs.end();
}

void RSSurfaceFpsManager::DumpSurfaceNodeFps(
    std::string& dumpString, const std::string& option, const std::string& arg)
{
    dumpString += "\n-- The recently fps records info of screens:\n";
    if (option == PARAM_NAME) {
        Dump(dumpString, arg);
    } else if (option == PARAM_ID) {
        NodeId nodeId = 0;
        if (!ConvertToLongLongUint(arg, nodeId)) {
            dumpString = "The input nodeId is invalid, please re-enter";
            return ;
        } else {
            Dump(dumpString, nodeId);
        }
    } else {
        dumpString = "The input option must be \"-name\" or \"-id\", please re-enter";
        return ;
    }
}

void RSSurfaceFpsManager::ClearSurfaceNodeFps(
    std::string& dumpString, const std::string& option, const std::string& arg)
{
    dumpString += "\n-- Clear fps records info of screens:\n";
    if (option == PARAM_NAME) {
        ClearDump(dumpString, arg);
    } else if (option == PARAM_ID) {
        NodeId nodeId = 0;
        if (!ConvertToLongLongUint(arg, nodeId)) {
            dumpString = "The input nodeId is invalid, please re-enter";
            return ;
        } else {
            ClearDump(dumpString, nodeId);
        }
    } else {
        dumpString = "The input option must be \"-name\" or \"-id\", please re-enter";
        return ;
    }
}

std::unordered_map<NodeId, std::shared_ptr<RSSurfaceFps>> RSSurfaceFpsManager::GetSurfaceFpsMap() const
{
    return surfaceFpsMap_;
}
}