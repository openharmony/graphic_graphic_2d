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

#include <atomic>
#include <chrono>
#include <map>
#include <shared_mutex>

#include "rs_unmarshal_task_manager.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
uint64_t UnmarshalTaskInfo::GetTaskDuration() const
{
    if (state == UnmarshalTaskState::COMPLETED) {
        return completeTimestamp - invokeTimestamp;
    } else if (state == UnmarshalTaskState::RUNNING) {
        return UnmarshalTaskUtil::GetTimestamp() - invokeTimestamp;
    }
    return 0;
}

std::string UnmarshalTaskInfo::Dump() const
{
    std::string dumpStr = "{";
    dumpStr += "state: ";
    dumpStr += UnmarshalTaskUtil::UnmarshalTaskStateToString(state);
    dumpStr += ", name: ";
    dumpStr += name;
    dumpStr += ", timestamp: [";
    dumpStr += std::to_string(invokeTimestamp);
    dumpStr += ", ";
    dumpStr += std::to_string(completeTimestamp);
    dumpStr += "], uid: ";
    dumpStr += std::to_string(uid);
    dumpStr += "}";
    return dumpStr;
}

class RSUnmarshalRunningTaskManager {
public:
    void Clear()
    {
        std::unique_lock<std::shared_mutex> lock { runningTaskMutex_ };
        runningTasks_.clear();
    }

    void Push(UnmarshalTaskInfo info)
    {
        auto uid = info.uid;
        std::unique_lock<std::shared_mutex> lock { runningTaskMutex_ };
        runningTasks_.insert_or_assign(uid, std::move(info));
    }

    std::optional<UnmarshalTaskInfo> Pop(uint64_t uid)
    {
        std::unique_lock<std::shared_mutex> lock { runningTaskMutex_ };
        if (auto node = runningTasks_.extract(uid)) {
            return node.mapped();
        }
        return std::nullopt;
    }

    std::optional<UnmarshalTaskInfo> GetLongestTask() const
    {
        std::shared_lock<std::shared_mutex> lock { runningTaskMutex_ };
        if (runningTasks_.empty()) {
            return std::nullopt;
        }
        std::optional<UnmarshalTaskInfo> ret;
        uint64_t maxDuration = 0;
        for (const auto& [_, info] : runningTasks_) {
            auto duration = info.GetTaskDuration();
            if (duration > maxDuration) {
                maxDuration = duration;
                ret = info;
            }
        }
        return ret;
    }

    std::string Dump() const
    {
        std::string dumpStr = "RSUnmarshalRunningTaskManager Dump : [";
        {
            std::shared_lock<std::shared_mutex> lock { runningTaskMutex_ };
            for (const auto& [_, info] : runningTasks_) {
                dumpStr += info.Dump();
                dumpStr += ",";
            }
        }
        if (!dumpStr.empty()) {
            dumpStr.pop_back();
        }
        dumpStr += "]";
        return dumpStr;
    }

private:
    mutable std::shared_mutex runningTaskMutex_;
    std::map<uint64_t, UnmarshalTaskInfo> runningTasks_;
};

class RSUnmarshalCompletedTaskManager {
public:
    void Clear()
    {
        std::unique_lock<std::shared_mutex> lock { completedTaskMutex_ };
        taskRankingTable_.clear();
    }

    void Push(UnmarshalTaskInfo info)
    {
        auto uid = info.uid;
        auto duration = info.GetTaskDuration();
        std::unique_lock<std::shared_mutex> lock { completedTaskMutex_ };
        taskRankingTable_.insert_or_assign({ duration, uid }, std::move(info));
    }

    std::optional<UnmarshalTaskInfo> Pop()
    {
        std::unique_lock<std::shared_mutex> lock { completedTaskMutex_ };
        if (taskRankingTable_.empty()) {
            return std::nullopt;
        }
        return taskRankingTable_.extract(taskRankingTable_.cbegin()).mapped();
    }

    std::optional<UnmarshalTaskInfo> GetLongestTask() const
    {
        std::shared_lock<std::shared_mutex> lock { completedTaskMutex_ };
        if (taskRankingTable_.empty()) {
            return std::nullopt;
        }
        return taskRankingTable_.cbegin()->second;
    }

    std::string Dump() const
    {
        std::string dumpStr = "RSUnmarshalCompletedTaskManager Dump: [";
        {
            std::shared_lock<std::shared_mutex> lock { completedTaskMutex_ };
            for (const auto& [_, info] : taskRankingTable_) {
                dumpStr += info.Dump();
                dumpStr += ",";
            }
        }
        if (!dumpStr.empty()) {
            dumpStr.pop_back();
        }
        dumpStr += "]";
        return dumpStr;
    }

private:
    mutable std::shared_mutex completedTaskMutex_;
    std::map<std::pair<uint64_t, uint64_t>, UnmarshalTaskInfo, std::greater<>> taskRankingTable_;
};

RSUnmarshalTaskManager::RSUnmarshalTaskManager() : runningTaskMgr_(std::make_unique<RSUnmarshalRunningTaskManager>()),
    completedTaskMgr_(std::make_unique<RSUnmarshalCompletedTaskManager>())
{
}

RSUnmarshalTaskManager& RSUnmarshalTaskManager::Instance()
{
    static RSUnmarshalTaskManager mgr;
    return mgr;
}

uint64_t RSUnmarshalTaskManager::BeginTask(std::string name)
{
    auto uid = GetUid();
    runningTaskMgr_->Push({
        .state = UnmarshalTaskState::RUNNING,
        .name = std::move(name),
        .invokeTimestamp = UnmarshalTaskUtil::GetTimestamp(),
        .uid = uid,
    });
    return uid;
}

void RSUnmarshalTaskManager::EndTask(uint64_t uid)
{
    if (auto taskInfo = runningTaskMgr_->Pop(uid)) {
        auto& info = taskInfo.value();
        info.state = UnmarshalTaskState::COMPLETED;
        info.completeTimestamp = UnmarshalTaskUtil::GetTimestamp();
        completedTaskMgr_->Push(std::move(info));
    } else {
        RS_LOGE("RSUnmarshalTaskManager EndTask Invalid Uid %{public}" PRIu64, uid);
    }
}

void RSUnmarshalTaskManager::Clear()
{
    completedTaskMgr_->Clear();
}

std::optional<UnmarshalTaskInfo> RSUnmarshalTaskManager::GetLongestTask() const
{
    if (auto completedTask = GetCompletedLongestTask()) {
        if (auto runningTask = GetRunningLongestTask()) {
            return (completedTask.value().GetTaskDuration() < runningTask.value().GetTaskDuration()) ?
                runningTask : completedTask;
        }
        return completedTask;
    }
    return GetRunningLongestTask();
}

std::optional<UnmarshalTaskInfo> RSUnmarshalTaskManager::GetRunningLongestTask() const
{
    return runningTaskMgr_->GetLongestTask();
}

std::optional<UnmarshalTaskInfo> RSUnmarshalTaskManager::GetCompletedLongestTask() const
{
    return completedTaskMgr_->GetLongestTask();
}

uint64_t RSUnmarshalTaskManager::GetUid() const
{
    static std::atomic_uint64_t uid;
    return uid.fetch_add(1, std::memory_order_relaxed);
}

std::string RSUnmarshalTaskManager::Dump() const
{
    std::string dumpStr = "RunningTasks: ";
    dumpStr += runningTaskMgr_->Dump();
    dumpStr += "|| completedTasks: ";
    dumpStr += completedTaskMgr_->Dump();
    return dumpStr;
}

namespace UnmarshalTaskUtil {
std::string UnmarshalTaskStateToString(UnmarshalTaskState state)
{
    switch (state) {
        case UnmarshalTaskState::COMPLETED : {
            return "COMPLETED";
        }
        case UnmarshalTaskState::RUNNING : {
            return "RUNNING";
        }
        case UnmarshalTaskState::UNINVOKED : {
            return "UNINVOKED";
        }
        default : {
            return "UNDEFSTATE";
        }
    }
}

uint64_t GetTimestamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}
} // namespace UnmarshalTaskUtil
} // namespace OHOS::Rosen