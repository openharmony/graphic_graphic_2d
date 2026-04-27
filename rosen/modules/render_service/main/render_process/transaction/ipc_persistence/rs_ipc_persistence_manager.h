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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_MANAGER_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_MANAGER_H

#include <mutex>

#include "rs_ipc_persistence_def.h"

namespace OHOS {
namespace Rosen {
class RSIpcPersistenceManager {
public:
    RSIpcPersistenceManager() = default;
    ~RSIpcPersistenceManager() noexcept = default;

    IpcPersistenceTypeToDataMap GetReplayData() const;

    // used to register a IPC with calling PID
    void RegisterWithCallingPid(std::shared_ptr<RSIpcPersistenceDataBase> data);
    // used to register a IPC without calling PID
    void RegisterWithoutCallingPid(std::shared_ptr<RSIpcPersistenceDataBase> data);

    // unregister all IPCs of a specific type
    void UnregisterByType(RSIpcPersistenceType type);
    // unregister all IPCs with calling PID
    void UnregisterByCallingPid(pid_t pid);

    // unregister IPC with calling PID of a specific type
    void UnregisterByTypeAndCallingPid(RSIpcPersistenceType type, pid_t pid);
    // unregister IPC without calling PID of a specific type
    void UnregisterWithoutCallingPidByType(RSIpcPersistenceType type);

    static bool Marshalling(Parcel& parcel, const IpcPersistenceTypeToDataMap& typeToDataMap);
    static std::optional<IpcPersistenceTypeToDataMap> Unmarshalling(Parcel& parcel);

private:
    void UnregisterByTypeAndCallingPidLocked(RSIpcPersistenceType type, pid_t pid);
    void UnregisterWithoutCallingPidByTypeLocked(RSIpcPersistenceType type);

    mutable std::mutex mutex_;
    IpcPersistenceTypeToDataMap replayData_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_MANAGER_H
