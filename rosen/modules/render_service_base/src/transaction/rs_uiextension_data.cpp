/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "transaction/rs_uiextension_data.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
bool RSUIExtensionData::MarshallingRectInfo(const SecRectInfo& rectInfo, Parcel& parcel)
{
    // Write coordinates(4 int), scale(2 float), anchor position(2float).
    bool marshallingSuccess = true;
    marshallingSuccess &= parcel.WriteInt32(rectInfo.relativeCoords.GetLeft());
    marshallingSuccess &= parcel.WriteInt32(rectInfo.relativeCoords.GetTop());
    marshallingSuccess &= parcel.WriteInt32(rectInfo.relativeCoords.GetWidth());
    marshallingSuccess &= parcel.WriteInt32(rectInfo.relativeCoords.GetHeight());
    marshallingSuccess &= parcel.WriteFloat(rectInfo.scale[0]);
    marshallingSuccess &= parcel.WriteFloat(rectInfo.scale[1]);
    marshallingSuccess &= parcel.WriteFloat(rectInfo.anchor[0]);
    marshallingSuccess &= parcel.WriteFloat(rectInfo.anchor[1]);
    return marshallingSuccess;
}

void RSUIExtensionData::UnmarshallingRectInfo(SecRectInfo& rectInfo, Parcel& parcel)
{
    // Read coordinates(4 int), scale(2 float), anchor position(2float).
    rectInfo.relativeCoords.SetAll(parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadInt32());
    rectInfo.scale[0] = parcel.ReadFloat();
    rectInfo.scale[1] = parcel.ReadFloat();
    rectInfo.anchor[0] = parcel.ReadFloat();
    rectInfo.anchor[1] = parcel.ReadFloat();
}


bool RSUIExtensionData::Marshalling(Parcel& parcel) const
{
    bool marshallingSuccess = true;
    marshallingSuccess &= parcel.WriteUint32(secData_.size());
    for (const auto& data : secData_) {
        marshallingSuccess &= parcel.WriteUint64(data.first);     // hostNodeId
        marshallingSuccess &= parcel.WriteUint32(static_cast<uint32_t>(data.second.size()));
        for (const auto& secSurfaceInfo : data.second) {
            marshallingSuccess &= MarshallingRectInfo(secSurfaceInfo.uiExtensionRectInfo, parcel);
            marshallingSuccess &= parcel.WriteInt32(secSurfaceInfo.hostPid);
            marshallingSuccess &= parcel.WriteInt32(secSurfaceInfo.uiExtensionPid);
            marshallingSuccess &= parcel.WriteUint64(secSurfaceInfo.hostNodeId);
            marshallingSuccess &= parcel.WriteUint64(secSurfaceInfo.uiExtensionNodeId);

            marshallingSuccess &= parcel.WriteUint32(static_cast<uint32_t>(secSurfaceInfo.upperNodes.size()));
            for (const auto& rectInfo : secSurfaceInfo.upperNodes) {
                marshallingSuccess &= MarshallingRectInfo(rectInfo, parcel);
            }
        }
    }
    if (!marshallingSuccess) {
        RS_LOGE("RSUIExtensionData::Marshalling failed");
    }
    return marshallingSuccess;
}

RSUIExtensionData* RSUIExtensionData::Unmarshalling(Parcel& parcel)
{
    auto uiExtensionData = new (std::nothrow) RSUIExtensionData();
    if (!uiExtensionData) {
        return nullptr;
    }
    auto mapSize = parcel.ReadUint32();
    if (mapSize > uiExtensionData->secData_.max_size()) {
        RS_LOGE("RSUIExtensionData Unmarshalling failed, map size overflow.");
        delete uiExtensionData;
        return nullptr;
    }
    for (uint32_t hostIndex = 0; hostIndex < mapSize; ++hostIndex) {
        uint64_t hostNodeId = parcel.ReadUint64();
        uiExtensionData->secData_.insert(std::make_pair(hostNodeId, std::vector<SecSurfaceInfo>()));
        uint32_t uiExtensionNodesCount = parcel.ReadUint32();
        if (uiExtensionNodesCount > uiExtensionData->secData_[hostNodeId].max_size()) {
            RS_LOGE("RSUIExtensionData Unmarshalling failed, vector size overflow.");
            delete uiExtensionData;
            return nullptr;
        }
        for (uint32_t uiExtensionIndex = 0; uiExtensionIndex < uiExtensionNodesCount; ++uiExtensionIndex) {
            SecSurfaceInfo secSurfaceInfo;
            UnmarshallingRectInfo(secSurfaceInfo.uiExtensionRectInfo, parcel);
            secSurfaceInfo.hostPid = parcel.ReadInt32();
            secSurfaceInfo.uiExtensionPid = parcel.ReadInt32();
            secSurfaceInfo.hostNodeId = parcel.ReadUint64();
            secSurfaceInfo.uiExtensionNodeId = parcel.ReadUint64();
            uint32_t upperNodesCount = parcel.ReadUint32();
            if (upperNodesCount > secSurfaceInfo.upperNodes.max_size()) {
                RS_LOGE("RSUIExtensionData Unmarshalling failed, upperNodes size overflow.");
                delete uiExtensionData;
                return nullptr;
            }
            for (uint32_t upperNodesIndex = 0; upperNodesIndex < upperNodesCount; ++upperNodesIndex) {
                SecRectInfo upperNodeInfo;
                UnmarshallingRectInfo(upperNodeInfo, parcel);
                secSurfaceInfo.upperNodes.emplace_back(upperNodeInfo);
            }
            uiExtensionData->secData_[hostNodeId].emplace_back(secSurfaceInfo);
        }
    }
    return uiExtensionData;
}
} // namespace Rosen
} // namespace OHOS
