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

#include "rs_ipc_persistence_data.h"

#undef LOG_TAG
#define LOG_TAG "RSIpcPersistenceManager"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_MAP_SIZE = 100;
constexpr uint32_t MAX_VECTOR_SIZE = 200;
} // namespace

IpcPersistenceTypeToDataMap RSIpcPersistenceManager::GetReplayData() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return replayData_;
}

void RSIpcPersistenceManager::RegisterWithCallingPid(std::shared_ptr<RSIpcPersistenceDataBase> data)
{
    if (data->GetCallingPid() == IPC_PERSISTENCE_DEFAULT_PID) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    UnregisterByTypeAndCallingPidLocked(data->GetType(), data->GetCallingPid());
    replayData_[data->GetType()].emplace_back(data);
}

void RSIpcPersistenceManager::RegisterWithoutCallingPid(std::shared_ptr<RSIpcPersistenceDataBase> data)
{
    if (data->GetCallingPid() != IPC_PERSISTENCE_DEFAULT_PID) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    UnregisterWithoutCallingPidByTypeLocked(data->GetType());
    replayData_[data->GetType()].emplace_back(data);
}

void RSIpcPersistenceManager::UnregisterByType(RSIpcPersistenceType type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    replayData_.erase(type);
}

void RSIpcPersistenceManager::UnregisterByCallingPid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto mapIter = replayData_.begin(); mapIter != replayData_.end();) {
        auto& dataVec = mapIter->second;
        dataVec.erase(std::remove_if(
            dataVec.begin(), dataVec.end(), [pid](const auto& data) { return data->GetCallingPid() == pid; }));
        if (dataVec.empty()) {
            mapIter = replayData_.erase(mapIter);
        } else {
            mapIter++;
        }
    }
}

void RSIpcPersistenceManager::UnregisterWithoutCallingPidByTypeLocked(RSIpcPersistenceType type)
{
    auto mapIter = replayData_.find(type);
    if (mapIter == replayData_.end()) {
        return;
    }
    auto& dataVec = mapIter->second;
    dataVec.erase(std::remove_if(dataVec.begin(), dataVec.end(),
        [](const auto& data) { return data->GetCallingPid() == IPC_PERSISTENCE_DEFAULT_PID; }));
    if (dataVec.empty()) {
        replayData_.erase(mapIter);
    }
}

void RSIpcPersistenceManager::UnregisterWithoutCallingPidByType(RSIpcPersistenceType type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    UnregisterWithoutCallingPidByTypeLocked(type);
}

void RSIpcPersistenceManager::UnregisterByTypeAndCallingPidLocked(RSIpcPersistenceType type, pid_t pid)
{
    auto mapIter = replayData_.find(type);
    if (mapIter == replayData_.end()) {
        return;
    }
    auto& dataVec = mapIter->second;
    dataVec.erase(std::remove_if(
        dataVec.begin(), dataVec.end(), [pid](const auto& data) { return data->GetCallingPid() == pid; }));
    if (dataVec.empty()) {
        replayData_.erase(mapIter);
    }
}

void RSIpcPersistenceManager::UnregisterByTypeAndCallingPid(RSIpcPersistenceType type, pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    UnregisterByTypeAndCallingPidLocked(type, pid);
}

bool RSIpcPersistenceManager::Marshalling(Parcel& parcel, const IpcPersistenceTypeToDataMap& typeToDataMap)
{
    if (!parcel.WriteUint32(typeToDataMap.size())) {
        return false;
    }
    for (const auto& [type, dataVec] : typeToDataMap) {
        if (!parcel.WriteUint32(static_cast<uint32_t>(type))) {
            return false;
        }
        if (!parcel.WriteUint32(dataVec.size())) {
            return false;
        }
        for (const auto& data : dataVec) {
            if (!data->Marshalling(parcel)) {
                return false;
            }
        }
    }
    return true;
}

std::optional<IpcPersistenceTypeToDataMap> RSIpcPersistenceManager::Unmarshalling(Parcel& parcel)
{
    uint32_t mapSize;
    if (!parcel.ReadUint32(mapSize) || mapSize > MAX_MAP_SIZE) {
        return std::nullopt;
    }
    IpcPersistenceTypeToDataMap result;
    for (uint32_t i = 0; i < mapSize; i++) {
        uint32_t type;
        if (!parcel.ReadUint32(type)) {
            return std::nullopt;
        }
        auto ipcPersistenceType = static_cast<RSIpcPersistenceType>(type);
        uint32_t dataVecSize;
        if (!parcel.ReadUint32(dataVecSize) || dataVecSize > MAX_VECTOR_SIZE) {
            return std::nullopt;
        }
        for (uint32_t j = 0; j < dataVecSize; j++) {
            std::shared_ptr<RSIpcPersistenceDataBase> data;
            switch (ipcPersistenceType) {
                case RSIpcPersistenceType::SET_WATERMARK:
                    data = std::shared_ptr<SetWatermarkPersistenceData>(
                        SetWatermarkPersistenceData::Unmarshalling(parcel));
                    break;
                case RSIpcPersistenceType::SHOW_WATERMARK:
                    data = std::shared_ptr<ShowWatermarkPersistenceData>(
                        ShowWatermarkPersistenceData::Unmarshalling(parcel));
                    break;
                case RSIpcPersistenceType::ON_HWC_EVENT:
                    data = std::shared_ptr<OnHwcEventPersistenceData>(OnHwcEventPersistenceData::Unmarshalling(parcel));
                    break;
                case RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED:
                    data = std::shared_ptr<SetBehindWindowFilterEnabledPersistenceData>(
                        SetBehindWindowFilterEnabledPersistenceData::Unmarshalling(parcel));
                    break;
                case RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED:
                    data = std::shared_ptr<SetShowRefreshRateEnabledPersistenceData>(
                        SetShowRefreshRateEnabledPersistenceData::Unmarshalling(parcel));
                    break;
                case RSIpcPersistenceType::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK:
                    data = std::shared_ptr<SelfDrawingNodeRectChangeCallbackPersistenceData>(
                        SelfDrawingNodeRectChangeCallbackPersistenceData::Unmarshalling(parcel));
                    break;
                default:
                    break;
            }
            if (data) {
                result[ipcPersistenceType].emplace_back(data);
            } else {
                return std::nullopt;
            }
        }
    }
    return result;
}
} // namespace Rosen
} // namespace OHOS
