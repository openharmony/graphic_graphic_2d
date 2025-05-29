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
#include "gfx/dump/rs_dump_manager.h"

#include <vector>
#include <string>
#include <future>

#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS::Rosen {
// Define a static constant to represent the fixed length of HELPINFO_CMD
static const size_t HELPINFO_CMD_FIXED_LENGTH = 30;

RSDumpManager::RSDumpManager()
{
    // Register the help information function to the function mapping table
    RSDumpFunc helpInfoFunc = [this](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                                                std::string &dumpString) -> void {
        DumpHelpInfo(dumpString);
    };
    Register({ RSDumpID::HELP_INFO, helpInfoFunc });
}

// Get the instance of RSDumpManager
RSDumpManager& RSDumpManager::GetInstance()
{
    // Define a static RSDumpManager object to ensure only one instance is created
    static RSDumpManager instance;
    return instance;
}

// Register a module to RSDumpManager
void RSDumpManager::Register(RSDumpHander rsDumpHander)
{
    RSDumpID rsDumpId = rsDumpHander.rsDumpId;
    if (rsDumpHanderMap_.find(rsDumpId) != rsDumpHanderMap_.end()) {
        // If the module ID already exists, output a warning message
        RS_LOGW("RSDumpManager::Register, rsDumpId %{public}d is already exist.", static_cast<int>(rsDumpId));
    } else {
        rsDumpHanderMap_[rsDumpId] = rsDumpHander;
    }
}

// Register multiple Dump points to RSDumpManager
void RSDumpManager::Register(const std::vector<RSDumpHander> &rsDumpHanders)
{
    if (rsDumpHanders.empty()) {
        return;
    }
    for (const auto &rsDumpHander : rsDumpHanders) {
        Register(rsDumpHander);
    }
}

// Unregister a Dump point
void RSDumpManager::UnRegister(RSDumpID rsDumpId)
{
    auto it = rsDumpHanderMap_.find(rsDumpId);
    if (it != rsDumpHanderMap_.end()) {
        rsDumpHanderMap_.erase(it);
    } else {
        // If the Dump point does not exist, output a warning message
        RS_LOGW("RSDumpManager::UnRegister, rsDumpId %{public}d is not exist.", static_cast<int>(rsDumpId));
    }
}

// Execute a command
void RSDumpManager::CmdExec(std::unordered_set<std::u16string>& argSets, std::string &out)
{
    if (!argSets.empty()) {
        MatchAndExecuteCommand(argSets, out);
    } else {
        // If the parameters are empty, output an error message
        RS_LOGW("RSDumpManager::CmdExec, args is empty.");
        DumpHelpInfo(out);
    }
}

// Match and execute a command
void RSDumpManager::MatchAndExecuteCommand(std::unordered_set<std::u16string> &argSets, std::string &out)
{
    for (const std::u16string &cmd : argSets) {
        auto it = cmdMap_.find(cmd);
        if (it != cmdMap_.end()) {
            std::string cmd_str = std::string(cmd.begin(), cmd.end());
            RS_LOGI("RSDumpManager::cmdExec, cmd is '%{public}s'.", cmd_str.c_str());

            RSDumpCmd entry = it->second;
            for (RSDumpID rsDumpId : entry.rsDumpIds) {
                DoDump(rsDumpId, cmd, argSets, out);
            }
            break;
        }
    }
    if (out.empty()) {
        // If the command is not found, output help information
        DumpHelpInfo(out);
    }
}

// Execute a specific dump operation
void RSDumpManager::DoDump(RSDumpID rsDumpId, const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                           std::string &out)
{
    RS_TRACE_NAME_FMT("RSDumpManager::DoDump rsDumpId [%d]", rsDumpId);
    RS_LOGI("RSDumpManager::DoDump, rsDumpId is '%{public}d'.", static_cast<uint8_t>(rsDumpId));
    auto it = rsDumpHanderMap_.find(rsDumpId);
    if (it != rsDumpHanderMap_.end()) {
        RSDumpHander hander = it->second;
        if (!hander.tag.empty() && hander.tag.size() > 0) {
            out.append("\n\n-- ").append(hander.tag).append("\n");
        }
        hander.func(cmd, argSets, out);
    } else {
        // If the hander is not registered, output an error message
        RS_LOGE("RSDumpManager::DoDump, rsDumpId %{public}d not registered.", static_cast<int>(rsDumpId));
    }
}

// Generate help information
void RSDumpManager::DumpHelpInfo(std::string &out)
{
    out.append("------ Graphic2D--RenderSerice ------\n")
        .append("Usage:\n");
    for (const auto &entry : cmdMap_) {
        std::u16string cmd = entry.first;
        if (excludeCmds_.find(cmd) != excludeCmds_.end()) {
            continue;
        }
        std::string cmd_str = std::string(cmd.begin(), cmd.end());
        out.append(cmd_str)
            .append(HELPINFO_CMD_FIXED_LENGTH - cmd_str.length(), ' ')
            .append("|")
            .append(entry.second.helpInfo)
            .append("\n");
    }
}
}  // namespace OHOS::Rosen