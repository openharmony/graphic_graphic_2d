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

#include "platform/ohos/transaction/ipc_replay/rs_ipc_replay_manager.h"

#include "platform/ohos/transaction/ipc_replay/rs_ipc_replay_data.h"

#undef LOG_TAG
#define LOG_TAG "RSIpcReplayManager"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_MAP_SIZE = 100;
constexpr uint32_t MAX_VECTOR_SIZE = 200;
} // namespace

IpcReplayTypeToDataMap RSIpcReplayManager::GetReplayData() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return replayData_;
}

void RSIpcReplayManager::Register(pid_t pid, std::shared_ptr<RSIpcReplayDataBase> data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    replayData_[data->GetType()].emplace_back(data);
}

void RSIpcReplayManager::UnregisterByType(RSIpcReplayType type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    replayData_.erase(type);
}

void RSIpcReplayManager::UnregisterByPid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto mapIter = replayData_.begin(); mapIter != replayData_.end();) {
        auto& dataVec = mapIter->second;
        for (auto dataVecIter = dataVec.begin(); dataVecIter != dataVec.end();) {
            if ((*dataVecIter)->GetPid() == pid) {
                dataVecIter = dataVec.erase(dataVecIter);
            } else {
                dataVecIter++;
            }
        }
        if (dataVec.empty()) {
            mapIter = replayData_.erase(mapIter);
        } else {
            mapIter++;
        }
    }
}

bool RSIpcReplayManager::Marshalling(Parcel& parcel, const IpcReplayTypeToDataMap& typeToDataMap)
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

std::optional<IpcReplayTypeToDataMap> RSIpcReplayManager::Unmarshalling(Parcel& parcel)
{
    uint32_t mapSize;
    if (!parcel.ReadUint32(mapSize) || mapSize > MAX_MAP_SIZE) {
        return std::nullopt;
    }
    IpcReplayTypeToDataMap result;
    for (uint32_t i = 0; i < mapSize; i++) {
        uint32_t type;
        if (!parcel.ReadUint32(type)) {
            return std::nullopt;
        }
        auto ipcReplayType = static_cast<RSIpcReplayType>(type);
        uint32_t dataVecSize;
        if (!parcel.ReadUint32(dataVecSize) || dataVecSize > MAX_VECTOR_SIZE) {
            return std::nullopt;
        }
        for (uint32_t j = 0; j < dataVecSize; j++) {
            std::shared_ptr<RSIpcReplayDataBase> data;
            switch (ipcReplayType) {
                case RSIpcReplayType::SET_WATERMARK:
                    data = std::shared_ptr<SetWatermarkReplayData>(SetWatermarkReplayData::Unmarshalling(parcel));
                    break;
                default:
                    break;
            }
            if (data) {
                result[ipcReplayType].emplace_back(data);
            } else {
                return std::nullopt;
            }
        }
    }
    return result;
}
} // namespace Rosen
} // namespace OHOS
