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

#include "hgm_info_parcel.h"

#include "platform/common/rs_log.h"

#undef LOG_TAG
#define LOG_TAG "HgmInfoParcel"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_DATA_SIZE = 512;
}
bool HgmServiceToProcessInfo::Marshalling(Parcel& data) const
{
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    if (!message->WriteUint32(pendingScreenRefreshRate) || !message->WriteUint64(pendingConstraintRelativeTime)) {
        return false;
    }

    bool adaptiveVsyncChange = hgmDataChangeTypes.test(HgmDataChangeType::ADAPTIVE_VSYNC);
    if (!message->WriteBool(adaptiveVsyncChange)) {
        return false;
    }
    if (adaptiveVsyncChange) {
        if (!message->WriteBool(isAdaptive) || !message->WriteString(gameNodeName)) {
            return false;
        }
    }

    bool rpHgmConfigDataChange = hgmDataChangeTypes.test(HgmDataChangeType::HGM_CONFIG_DATA);
    if (!message->WriteBool(rpHgmConfigDataChange)) {
        return false;
    }
    if (rpHgmConfigDataChange) {
        if (!message->WriteBool(ltpoEnabled) || !message->WriteBool(isDelayMode) ||
            !message->WriteInt32(pipelineOffsetPulseNum)) {
            return false;
        }
        if (!message->WriteParcelable(rpHgmConfigData.get())){
            return false;
        }
    }

    if (!message->WriteBool(isPowerIdle)) {
        return false;
    }
    return true;
}

HgmServiceToProcessInfo* HgmServiceToProcessInfo::Unmarshalling(Parcel& data)
{
    auto result = new HgmServiceToProcessInfo();
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    if (!message->ReadUint32(result->pendingScreenRefreshRate) ||
        !message->ReadUint64(result->pendingConstraintRelativeTime)) {
        return nullptr;
    }

    bool adaptiveVsyncChange = false;
    if (!message->ReadBool(adaptiveVsyncChange)) {
        return nullptr;
    }
    if (adaptiveVsyncChange) {
        result->hgmDataChangeTypes.set(HgmDataChangeType::ADAPTIVE_VSYNC);
        if (!message->ReadBool(result->isAdaptive) || !message->ReadString(result->gameNodeName)) {
            return nullptr;
        }
    }

    bool rpHgmConfigDataChange = false;
    if (!message->ReadBool(rpHgmConfigDataChange)) {
        return nullptr;
    }
    if (rpHgmConfigDataChange) {
        result->hgmDataChangeTypes.set(HgmDataChangeType::HGM_CONFIG_DATA);
        if (!message->ReadBool(result->ltpoEnabled) || !message->ReadBool(result->isDelayMode) ||
            !message->ReadInt32(result->pipelineOffsetPulseNum)) {
            return nullptr;
        }
        result->rpHgmConfigData = std::shared_ptr<RPHgmConfigData>(message->ReadParcelable<RPHgmConfigData>());
    }

    if (!message->ReadBool(result->isPowerIdle)) {
        return nullptr;
    }
    return result;
}

bool HgmProcessToServiceInfo::Marshalling(Parcel& data) const
{
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    if (!message->WriteBool(isGameNodeOnTree)) {
        return false;
    }
    if (!MarshallingFrameRateLinker(message)) {
        return false;
    }
    if (!MarshallingSurfaceData(message)) {
        return false;
    }
    if (!MarshallingEnergyData(message)) {
        return false;
    }
    if (!MarshallingVRateMap(message)) {
        return false;
    }
    return true;
}

HgmProcessToServiceInfo* HgmProcessToServiceInfo::Unmarshalling(Parcel& data)
{
    auto result = new HgmProcessToServiceInfo();
    MessageParcel* message = static_cast<MessageParcel*>(&data);
    result->isGameNodeOnTree = message->ReadBool();
    if (!result->UnmarshallingFrameRateLinker(message)) {
        return nullptr;
    }
    if (!result->UnmarshallingSurfaceData(message)) {
        return nullptr;
    }
    if (!result->UnmarshallingEnergyData(message)) {
        return nullptr;
    }
    if (!result->UnmarshallingVRateMap(message)) {
        return nullptr;
    }
    return result;
}

bool HgmProcessToServiceInfo::MarshallingFrameRateLinker(MessageParcel* message) const
{
    if (auto size = frameRateLinkerDestroyIds.size(); size > MAX_DATA_SIZE) {
        RS_LOGE("HgmProcessToServiceInfo Marshalling frameRateLinkerDestroyIds Failed size:%{public}zu", size);
        if (!message->WriteInt32(0)) {
            return false;
        }
    } else {
        if (!message->WriteInt32(size)) {
            return false;
        }
        for (const auto destroyId : frameRateLinkerDestroyIds) {
            if (!message->WriteUint64(destroyId)) {
                return false;
            }
        }
    }
    if (auto size = frameRateLinkerUpdateInfoMap.size(); size > MAX_DATA_SIZE) {
        RS_LOGE("HgmProcessToServiceInfo Marshalling frameRateLinkerUpdateInfoMap Failed size:%{public}zu", size);
        if (!message->WriteInt32(0)) {
            return false;
        }
    } else {
        if (!message->WriteInt32(size)) {
            return false;
        }
        for (const auto& [linkerId, updateInfo] : frameRateLinkerUpdateInfoMap) {
            if (!message->WriteUint64(linkerId) || !message->WriteInt32(updateInfo.range.min_) ||
                !message->WriteInt32(updateInfo.range.max_) || !message->WriteInt32(updateInfo.range.preferred_) ||
                !message->WriteUint64(updateInfo.range.type_) ||
                !message->WriteInt32(updateInfo.range.componentScene_) ||
                !message->WriteInt32(updateInfo.animatorExpectedFrameRate)) {
                return false;
            }
        }
    }
    if (!message->WriteInt32(rsCurrRange.min_) || !message->WriteInt32(rsCurrRange.max_) ||
        !message->WriteInt32(rsCurrRange.preferred_) || !message->WriteUint64(rsCurrRange.type_) ||
        !message->WriteInt32(rsCurrRange.componentScene_)) {
        return false;
    }
    return true;
}

bool HgmProcessToServiceInfo::MarshallingSurfaceData(MessageParcel* message) const
{
    if (auto size = surfaceData.size(); size > MAX_DATA_SIZE) {
        RS_LOGE("HgmProcessToServiceInfo Marshalling surfaceData Failed size:%{public}zu", size);
        if (!message->WriteInt32(0)) {
            return false;
        }
    } else {
        if (!message->WriteInt32(size)) {
            return false;
        }
        for (const auto& [surfaceName, nodePid] : surfaceData) {
            if (!message->WriteString(surfaceName) || !message->WriteInt32(nodePid)) {
                return false;
            }
        }
    }

    if (auto size = uiFrameworkDirtyNodeNameMap.size(); size > MAX_DATA_SIZE) {
        RS_LOGE("HgmProcessToServiceInfo Marshalling uiFrameworkDirtyNodeNameMap Failed size:%{public}zu", size);
        if (!message->WriteInt32(0)) {
            return false;
        }
    } else {
        if (!message->WriteInt32(size)) {
            return false;
        }
        for (const auto& [nodeName, nodePid] : uiFrameworkDirtyNodeNameMap) {
            if (!message->WriteString(nodeName) || !message->WriteInt32(nodePid)) {
                return false;
            }
        }
    }
    return true;
}

bool HgmProcessToServiceInfo::MarshallingEnergyData(MessageParcel* message) const
{
    uint32_t eventSize = static_cast<uint32_t>(energyCommonData.size());
    if (!message->WriteUint32(eventSize)) {
        return false;
    }
    for (const auto& eventData : energyCommonData) {
        int32_t eventCode = static_cast<int32_t>(eventData.first);
        if (!message->WriteInt32(eventCode)) {
            return false;
        }
        uint32_t dataSize = static_cast<uint32_t>(eventData.second.size());
        if (!message->WriteUint32(dataSize)) {
            return false;
        }
        for (const auto& commonData : eventData.second) {
            if (!message->WriteString(commonData.first)) {
                return false;
            }
            if (!message->WriteString(commonData.second)) {
                return false;
            }
        }
    }
    return true;
}

bool HgmProcessToServiceInfo::MarshallingVRateMap(MessageParcel* message) const
{
    if (!message->WriteInt32(vRateMap.size())) {
        return false;
    }
    for (const auto& [nodeID, rate] : vRateMap) {
        if (!message->WriteUint64(nodeID) || !message->WriteInt32(rate)) {
            return false;
        }
    }
    return true;
}

bool HgmProcessToServiceInfo::UnmarshallingFrameRateLinker(MessageParcel* message)
{
    int32_t destroySize;
    if (!message->ReadInt32(destroySize)) {
        return false;
    }
    if (static_cast<uint32_t>(destroySize) > MAX_DATA_SIZE) {
        RS_LOGE("HgmProcessToServiceInfo Unmarshalling frameRateLinkerDestroyIds Failed, size:%{public}d",
            destroySize);
        return false;
    }

    for (int i = 0; i < destroySize; i++) {
        FrameRateLinkerId destroyLinkerId;
        if (!message->ReadUint64(destroyLinkerId)) {
            return false;
        }
        frameRateLinkerDestroyIds.insert(destroyLinkerId);
    }

    int32_t updateInfoSize;
    if (!message->ReadInt32(updateInfoSize)) {
        return false;
    }
    if (static_cast<uint32_t>(updateInfoSize) > MAX_DATA_SIZE) {
        RS_LOGE("HgmProcessToServiceInfo Unmarshalling frameRateLinkerUpdateInfoMap Failed, size:%{public}d",
            updateInfoSize);
        return false;
    }
    for (int i = 0; i < updateInfoSize; i++) {
        FrameRateLinkerId linkerId;
        int32_t minFps;
        int32_t maxFps;
        int32_t preferred;
        uint64_t rangeType;
        int32_t componentScene;
        int32_t animatorExpectedFrameRate;
        if (!message->ReadUint64(linkerId) || !message->ReadInt32(minFps) ||
            !message->ReadInt32(maxFps) || !message->ReadInt32(preferred) ||
            !message->ReadUint64(rangeType) || !message->ReadInt32(componentScene) ||
            !message->ReadInt32(animatorExpectedFrameRate)) {
            return false;
        }
        FrameRateRange range{minFps, maxFps, preferred, rangeType, ComponentScene(componentScene)};
        FrameRateLinkerUpdateInfo updateInfo{range, animatorExpectedFrameRate};
        frameRateLinkerUpdateInfoMap.insert_or_assign(linkerId, updateInfo);
    }

    int32_t minFps;
    int32_t maxFps;
    int32_t preferred;
    uint64_t rangeType;
    int32_t componentScene;
    if (!message->ReadInt32(minFps) || !message->ReadInt32(maxFps) ||
        !message->ReadInt32(preferred) || !message->ReadUint64(rangeType) ||
        !message->ReadInt32(componentScene)) {
        return false;
    }
    rsCurrRange.Set(minFps, maxFps, preferred, rangeType, ComponentScene(componentScene));
    return true;
}

bool HgmProcessToServiceInfo::UnmarshallingSurfaceData(MessageParcel* message)
{
    int32_t surfaceDataSize;
    if (!message->ReadInt32(surfaceDataSize)) {
        return false;
    }
    if (static_cast<uint32_t>(surfaceDataSize) > MAX_DATA_SIZE) {
        RS_LOGE("HgmProcessToServiceInfo Unmarshalling surfaceData Failed, size:%{public}d",
            surfaceDataSize);
        return false;
    }
    for (int i = 0; i < surfaceDataSize; i++) {
        std::string surfaceName;
        pid_t nodePid;
        if (!message->ReadString(surfaceName) || !message->ReadInt32(nodePid)) {
            return false;
        }
        surfaceData.emplace_back(std::tuple<std::string, pid_t>({surfaceName, nodePid}));
    }

    int32_t uiFrameworkDirtyNodeNameMapSize;
    if (!message->ReadInt32(uiFrameworkDirtyNodeNameMapSize)) {
        return false;
    }
    if (static_cast<uint32_t>(uiFrameworkDirtyNodeNameMapSize) > MAX_DATA_SIZE) {
        RS_LOGE("HgmProcessToServiceInfo Unmarshalling uIFrameworkDirtyNodeNameMap Failed, size:%{public}d",
            uiFrameworkDirtyNodeNameMapSize);
        return false;
    }
    for (int i = 0; i < uiFrameworkDirtyNodeNameMapSize; i++) {
        std::string nodeName;
        pid_t nodePid;
        if (!message->ReadString(nodeName) || !message->ReadInt32(nodePid)) {
            return false;
        }
        uiFrameworkDirtyNodeNameMap.insert_or_assign(nodeName, nodePid);
    }
    return true;
}

bool HgmProcessToServiceInfo::UnmarshallingEnergyData(MessageParcel* message)
{
    uint32_t eventSize;
    if (!message->ReadUint32(eventSize)) {
        return false;
    }
    for (uint32_t i = 0; i < eventSize; i++) {
        int32_t eventCode;
        uint32_t dataSize;
        if (!message->ReadInt32(eventCode) || !message->ReadUint32(dataSize)) {
            return false;
        }
        std::unordered_map<std::string, std::string> energyData;
        for (uint32_t j = 0; j < dataSize; j++) {
            std::string key;
            std::string value;
            if (!message->ReadString(key) || !message->ReadString(value)) {
                return false;
            }
            energyData.emplace(key, value);
        }
        EnergyEvent event = static_cast<EnergyEvent>(eventCode);
        energyCommonData.emplace(event, energyData);
    }
    return true;
}

bool HgmProcessToServiceInfo::UnmarshallingVRateMap(MessageParcel* message)
{
    int32_t vRateMapSize;
    if (!message->ReadInt32(vRateMapSize)) {
        return false;
    }
    for (int i = 0; i < vRateMapSize; i++) {
        NodeId nodeID;
        int32_t rate;
        if (!message->ReadUint64(nodeID) || !message->ReadInt32(rate)) {
            return false;
        }
        vRateMap.insert(std::make_pair(nodeID, rate));
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS