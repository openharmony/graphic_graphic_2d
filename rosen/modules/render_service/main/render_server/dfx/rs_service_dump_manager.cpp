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
#include "rs_service_dump_manager.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS::Rosen {
constexpr size_t PROCESS_DUMP_TREE_TIMEOUT = 2500;

bool RSServiceDumpManager::IsServiceDumpCmd(const std::u16string& cmd)
{
    auto it = std::find(serviceDumpList.begin(), serviceDumpList.end(), cmd);
    return it != serviceDumpList.end();
}

bool RSServiceDumpManager::IsProcessDumpCmd(const std::u16string& cmd)
{
    auto it = std::find(processDumpList.begin(), processDumpList.end(), cmd);
    return it != processDumpList.end();
}

void RSServiceDumpManager::InitProcessDumpTask(int32_t processCount)
{
    std::unique_lock<std::mutex> lock(collectDumpMutex_);
    processCount_ = processCount;
    completionCount_ = 0;
    dumpDataList_.clear();
}

void RSServiceDumpManager::WaitForDump(std::string& dumpString)
{
    std::unique_lock<std::mutex> lock(processDumpMutex_);
    {
        RS_TRACE_NAME_FMT("RSServiceDumpManager::WaitForDump");
        processDumpCondVar_.wait_for(lock, std::chrono::milliseconds(PROCESS_DUMP_TREE_TIMEOUT), [this] () {
            return IsDumpCompleted();
        });
    }

    dumpString += "\n-----RSProcessDump-----";
    RS_TRACE_NAME_FMT("RSServiceDumpManager::WaitForDump Finish wait");
    {
        std::unique_lock<std::mutex> lock(collectDumpMutex_);
        for (const auto& dumpData : dumpDataList_) {
            if (dumpData != "") {
                dumpString += "\n";
                dumpString += dumpData;
            }
        }
    }
    RS_LOGI("RSServiceDumpManager::WaitForDump Finish wait");
}

bool RSServiceDumpManager::IsDumpCompleted()
{
    return completionCount_ == processCount_;
}

void RSServiceDumpManager::CollectDump(std::string& dumpString)
{
    std::unique_lock<std::mutex> lock(collectDumpMutex_);
    dumpDataList_.push_back(dumpString);
    completionCount_ += 1;
    processDumpCondVar_.notify_all();
}

void RSServiceDumpManager::DoDump(const std::vector<std::u16string>& args, std::string& dumpString,
    const sptr<RSRenderProcessManager> processManager)
{
    std::unordered_set<std::u16string> serviceArgSets;
    std::unordered_set<std::u16string> processArgSets;
    for (decltype(args.size()) index = 0; index < args.size(); ++index) {
        bool isCmdArg = false;
        if (IsServiceDumpCmd(args[index])) {
            serviceArgSets.insert(args[index]);
            isCmdArg = true;
        }
        if (IsProcessDumpCmd(args[index])) {
            processArgSets.insert(args[index]);
            isCmdArg = true;
        }

        if (!isCmdArg) {
            serviceArgSets.insert(args[index]);
            processArgSets.insert(args[index]);
        }
    }

    if (!serviceArgSets.empty() || processArgSets.empty()) {
        std::string cmdStr;
        for (const std::u16string &cmd : serviceArgSets) {
            cmdStr += std::string(cmd.begin(), cmd.end()) + " ";
        }

        RS_TRACE_NAME("RSServiceDumpManager::DoDump args is [ " + cmdStr + " ]");
        RSDumpManager::CmdExec(serviceArgSets, dumpString);
    }
    if (!processManager) {
        return;
    }
    auto serviceToRenderConns = processManager->GetServiceToRenderConns();
    if (!processArgSets.empty()) {
        if (serviceToRenderConns.size() == 0) {
            RS_LOGE("%{public}s serviceToRenderConns is empty", __func__);
            return;
        }
        if (rsDumpCallbackDirector_ == nullptr) {
            rsDumpCallbackDirector_ = new RSDumpCallbackDirector(this);
        }
        InitProcessDumpTask(static_cast<int32_t>(serviceToRenderConns.size()));
        for (auto conn : serviceToRenderConns) {
            conn->DoDump(processArgSets, rsDumpCallbackDirector_);
        }
        WaitForDump(dumpString);
    }
}
}