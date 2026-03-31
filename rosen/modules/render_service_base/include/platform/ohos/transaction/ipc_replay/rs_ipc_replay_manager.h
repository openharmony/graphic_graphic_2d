/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_IPC_REPLAY_RS_IPC_REPLAY_MANAGER_H
#define RENDER_SERVICE_IPC_REPLAY_RS_IPC_REPLAY_MANAGER_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "platform/ohos/transaction/ipc_replay/rs_ipc_replay_def.h"

namespace OHOS {
namespace Rosen {
class RSIpcReplayManager {
public:
    RSIpcReplayManager() = default;
    ~RSIpcReplayManager() noexcept = default;

    IpcReplayTypeToDataMap GetReplayData() const;

    void Register(pid_t pid, std::shared_ptr<RSIpcReplayDataBase> data);
    void UnregisterByType(RSIpcReplayType type);
    void UnregisterByPid(pid_t pid);

    static bool Marshalling(Parcel& parcel, const IpcReplayTypeToDataMap& typeToDataMap);
    static std::optional<IpcReplayTypeToDataMap> Unmarshalling(Parcel& parcel);

private:
    mutable std::mutex mutex_;
    IpcReplayTypeToDataMap replayData_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_IPC_REPLAY_RS_IPC_REPLAY_MANAGER_H
