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

#include <cstdint>
#include <limits>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

#include <parcel.h>
#include "ipc_types.h"

#include "rs_ipc_persistence_def.h"

namespace OHOS {
namespace Rosen {

class RSIpcPersistenceManager {
public:
    using TransferFactory = std::shared_ptr<RSIpcTransferBase>(*)(Parcel&, uint32_t, int32_t&);

    RSIpcPersistenceManager() = default;
    ~RSIpcPersistenceManager() noexcept = default;

    RSIpcPersistenceManager(const RSIpcPersistenceManager&) = delete;
    RSIpcPersistenceManager& operator=(const RSIpcPersistenceManager&) = delete;
    RSIpcPersistenceManager(RSIpcPersistenceManager&&) = delete;
    RSIpcPersistenceManager& operator=(RSIpcPersistenceManager&&) = delete;

    static void RegisterFactory(RSIServiceToRenderConnectionInterfaceCode typeId, TransferFactory factory);
    static std::shared_ptr<const std::vector<RSIServiceToRenderConnectionInterfaceCode>> GetRegisteredTypeIds();
    static std::shared_ptr<std::mutex> GetTypeMutex(RSIServiceToRenderConnectionInterfaceCode typeId);
    static std::shared_ptr<RSIpcTransferBase> CreateTransferByTypeId(
        RSIServiceToRenderConnectionInterfaceCode typeId, Parcel& parcel,
        int32_t& errCode, uint32_t maxEntries = std::numeric_limits<uint32_t>::max());
    void PersistTransfer(const std::shared_ptr<RSIpcTransferBase>& transfer);
    void ClearPid(pid_t pid);

    IpcPersistenceMap GetPersistenceMap() const;
    static bool Marshalling(Parcel& parcel, const IpcPersistenceMap& map);
    [[nodiscard]] static std::optional<IpcPersistenceMap> Unmarshalling(Parcel& parcel);

private:
    using FactoryMap = std::unordered_map<RSIServiceToRenderConnectionInterfaceCode, TransferFactory>;
    using TypeMutexMap = std::unordered_map<RSIServiceToRenderConnectionInterfaceCode, std::shared_ptr<std::mutex>>;
    struct FactoryRegistry {
        std::mutex mutex;
        FactoryMap map;
        TypeMutexMap typeMutexes;
        std::shared_ptr<const std::vector<RSIServiceToRenderConnectionInterfaceCode>> typeIds =
            std::make_shared<std::vector<RSIServiceToRenderConnectionInterfaceCode>>();
    };
    static FactoryRegistry& GetFactoryRegistry();

    mutable std::mutex mutex_;
    IpcPersistenceMap persistedData_;
};

template<RSIServiceToRenderConnectionInterfaceCode TypeId, typename TransferClass>
class RSIpcPersistentTransferRegister {
public:
    RSIpcPersistentTransferRegister()
    {
        RSIpcPersistenceManager::RegisterFactory(
            TypeId,
            [](Parcel& parcel, uint32_t maxEntries, int32_t& errCode) -> std::shared_ptr<RSIpcTransferBase> {
                return TransferClass::StubUnmarshalling(parcel, maxEntries, errCode);
            });
    }
};

template<typename TransferClass>
const RSIpcPersistentTransferRegister<TransferClass::TypeId, TransferClass>
    TransferRegistrationChecker<TransferClass>::registrar;
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_MANAGER_H
