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

#include "rs_ipc_persistence_manager.h"

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "RSIpcPersistenceManager"

namespace OHOS {
namespace Rosen {

RSIpcPersistenceManager::FactoryRegistry& RSIpcPersistenceManager::GetFactoryRegistry()
{
    static FactoryRegistry* reg = new FactoryRegistry();
    return *reg;
}

void RSIpcPersistenceManager::RegisterFactory(RSIServiceToRenderConnectionInterfaceCode typeId, TransferFactory factory)
{
    auto& reg = GetFactoryRegistry();
    std::lock_guard<std::mutex> lock(reg.mutex);
    bool isNewTypeId = reg.map.find(typeId) == reg.map.end();
    reg.map[typeId] = factory;
    if (reg.typeMutexes.find(typeId) == reg.typeMutexes.end()) {
        reg.typeMutexes.emplace(typeId, std::make_shared<std::mutex>());
    }
    if (isNewTypeId) {
        auto typeIds = std::make_shared<std::vector<RSIServiceToRenderConnectionInterfaceCode>>(*reg.typeIds);
        typeIds->push_back(typeId);
        reg.typeIds = std::move(typeIds);
    }
}

std::shared_ptr<const std::vector<RSIServiceToRenderConnectionInterfaceCode>>
RSIpcPersistenceManager::GetRegisteredTypeIds()
{
    auto& reg = GetFactoryRegistry();
    std::lock_guard<std::mutex> lock(reg.mutex);
    return reg.typeIds;
}

std::shared_ptr<std::mutex> RSIpcPersistenceManager::GetTypeMutex(
    RSIServiceToRenderConnectionInterfaceCode typeId)
{
    auto& reg = GetFactoryRegistry();
    std::lock_guard<std::mutex> lock(reg.mutex);
    auto it = reg.typeMutexes.find(typeId);
    if (it == reg.typeMutexes.end()) {
        it = reg.typeMutexes.emplace(typeId, std::make_shared<std::mutex>()).first;
    }
    return it->second;
}

std::shared_ptr<RSIpcTransferBase> RSIpcPersistenceManager::CreateTransferByTypeId(
    RSIServiceToRenderConnectionInterfaceCode typeId, Parcel& parcel, int32_t& errCode, uint32_t maxEntries)
{
    TransferFactory factory = nullptr;
    {
        auto& reg = GetFactoryRegistry();
        std::lock_guard<std::mutex> lock(reg.mutex);
        auto it = reg.map.find(typeId);
        if (it == reg.map.end()) {
            RS_LOGE("%{public}s: typeId %{public}u not registered", __func__, static_cast<uint32_t>(typeId));
            errCode = ERR_INVALID_DATA;
            return nullptr;
        }
        factory = it->second;
    }
    return factory(parcel, maxEntries, errCode);
}

IpcPersistenceMap RSIpcPersistenceManager::GetPersistenceMap() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return persistedData_;
}

bool RSIpcPersistenceManager::Marshalling(Parcel& parcel, const IpcPersistenceMap& map)
{
    if (map.size() > Detail::MAX_PERSIST_MAP_SIZE) {
        RS_LOGE("%{public}s: map size %{public}zu exceeds max %{public}u",
            __func__, map.size(), Detail::MAX_PERSIST_MAP_SIZE);
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(map.size()))) {
        RS_LOGE("%{public}s: WriteUint32 size failed", __func__);
        return false;
    }
    for (const auto& [typeId, transfer] : map) {
        if (!parcel.WriteUint32(static_cast<uint32_t>(typeId))) {
            RS_LOGE("%{public}s: WriteUint32 typeId failed", __func__);
            return false;
        }
        if (!transfer) {
            RS_LOGE("%{public}s: transfer is nullptr", __func__);
            return false;
        }
        if (!transfer->ProxyMarshalling(parcel)) {
            RS_LOGE("%{public}s: Transfer ProxyMarshalling failed", __func__);
            return false;
        }
    }
    return true;
}

std::optional<IpcPersistenceMap> RSIpcPersistenceManager::Unmarshalling(Parcel& parcel)
{
    uint32_t typeCount = 0;
    if (!parcel.ReadUint32(typeCount) || typeCount > Detail::MAX_PERSIST_MAP_SIZE) {
        RS_LOGE("%{public}s: typeCount %{public}u exceeds max %{public}u",
                __func__, typeCount, Detail::MAX_PERSIST_MAP_SIZE);
        return std::nullopt;
    }

    IpcPersistenceMap dataMap;
    for (uint32_t i = 0; i < typeCount; ++i) {
        uint32_t typeIdVal;
        if (!parcel.ReadUint32(typeIdVal)) {
            RS_LOGE("%{public}s: ReadUint32 typeId failed", __func__);
            return std::nullopt;
        }
        auto typeId = static_cast<RSIServiceToRenderConnectionInterfaceCode>(typeIdVal);
        if (dataMap.find(typeId) != dataMap.end()) {
            RS_LOGE("%{public}s: duplicate typeId %{public}u in replay data", __func__, typeIdVal);
            return std::nullopt;
        }
        int32_t deserErr = ERR_NULL_OBJECT;
        auto transfer = RSIpcPersistenceManager::CreateTransferByTypeId(typeId, parcel, deserErr);
        if (!transfer) {
            RS_LOGE("%{public}s: CreateTransferByTypeId failed for typeId %{public}u, err=%{public}d",
                __func__, typeIdVal, deserErr);
            return std::nullopt;
        }
        dataMap[typeId] = std::move(transfer);
    }
    return dataMap;
}

void RSIpcPersistenceManager::PersistTransfer(const std::shared_ptr<RSIpcTransferBase>& transfer)
{
    if (!transfer || !transfer->IsPersistent()) {
        return;
    }
    auto typeMutex = GetTypeMutex(transfer->GetPersistLockTypeId());
    std::lock_guard<std::mutex> typeLock(*typeMutex);
    transfer->Persist(persistedData_, mutex_);
}

void RSIpcPersistenceManager::ClearPid(pid_t pid)
{
    auto typeIds = GetRegisteredTypeIds();
    for (const auto& typeId : *typeIds) {
        auto typeMutex = GetTypeMutex(typeId);
        std::lock_guard<std::mutex> typeLock(*typeMutex);
        std::shared_ptr<RSIpcTransferBase> target;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = persistedData_.find(typeId);
            if (it == persistedData_.end()) {
                continue;
            }
            target = it->second;
        }
        if (target) {
            target->ClearPid(pid);
        }
    }
}

} // namespace Rosen
} // namespace OHOS
