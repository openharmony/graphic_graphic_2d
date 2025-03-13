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

#ifndef RS_UNMARSHAL_TASK_MANAGER_H
#define RS_UNMARSHAL_TASK_MANAGER_H
#include <memory>
#include <optional>
#include <string>

namespace OHOS::Rosen {
enum class UnmarshalTaskState : uint8_t {
    UNINVOKED,
    RUNNING,
    COMPLETED,
};

struct UnmarshalTaskInfo {
    UnmarshalTaskState state = UnmarshalTaskState::UNINVOKED;
    std::string name;
    uint64_t invokeTimestamp = 0;
    uint64_t completeTimestamp = 0;
    uint64_t uid = 0;
    std::string Dump() const;
    uint64_t GetTaskDuration() const;
};

class RSUnmarshalRunningTaskManager;
class RSUnmarshalCompletedTaskManager;

class RSUnmarshalTaskManager {
public:
    static RSUnmarshalTaskManager& Instance();

    uint64_t BeginTask(std::string name);
    void EndTask(uint64_t uid);
    void Clear();

    std::optional<UnmarshalTaskInfo> GetLongestTask() const;
    std::optional<UnmarshalTaskInfo> GetRunningLongestTask() const;
    std::optional<UnmarshalTaskInfo> GetCompletedLongestTask() const;

    std::string Dump() const;

private:
    RSUnmarshalTaskManager();
    ~RSUnmarshalTaskManager() noexcept = default;

    RSUnmarshalTaskManager(const RSUnmarshalTaskManager&) = delete;
    RSUnmarshalTaskManager(RSUnmarshalTaskManager&&) = delete;
    RSUnmarshalTaskManager& operator=(const RSUnmarshalTaskManager&) = delete;
    RSUnmarshalTaskManager& operator=(RSUnmarshalTaskManager&&) = delete;

    uint64_t GetUid() const;

    std::unique_ptr<RSUnmarshalRunningTaskManager> runningTaskMgr_;
    std::unique_ptr<RSUnmarshalCompletedTaskManager> completedTaskMgr_;
};

namespace UnmarshalTaskUtil {
std::string UnmarshalTaskStateToString(UnmarshalTaskState state);
uint64_t GetTimestamp();
} // namespace UnmarshalTaskUtil
} // namespace OHOS::Rosen
#endif