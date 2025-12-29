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

#ifndef HGM_INFO_PARCEL_H
#define HGM_INFO_PARCEL_H

#include <unordered_map>
#include <unordered_set>

#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"
#include "iremote_object.h"
#include "message_parcel.h"
#include "pipeline/rs_context.h"
#include "transaction/rp_hgm_config_data.h"

namespace OHOS {
namespace Rosen {
enum HgmDataChangeType {
    ADAPTIVE_VSYNC = 0,
    HGM_CONFIG_DATA,
    MAX_CHANGE_TYPE
};

using HgmDataChangeTypes = std::bitset<HgmDataChangeType::MAX_CHANGE_TYPE>;

struct HgmServiceToProcessInfo : public Parcelable {
    HgmServiceToProcessInfo() = default;
    ~HgmServiceToProcessInfo() noexcept = default;

    bool Marshalling(Parcel& data) const override;
    [[nodiscard]] static HgmServiceToProcessInfo* Unmarshalling(Parcel& data);

    HgmDataChangeTypes hgmDataChangeTypes;

    uint32_t pendingScreenRefreshRate = 0;
    uint64_t pendingConstraintRelativeTime = 0;

    bool ltpoEnabled = false;
    bool isDelayMode = false;
    int32_t pipelineOffsetPulseNum = 0;
    std::shared_ptr<RPHgmConfigData> rpHgmConfigData = nullptr;

    bool isAdaptive = false;
    std::string gameNodeName;

    bool isPowerIdle = false;
};

struct HgmProcessToServiceInfo : public Parcelable {
    HgmProcessToServiceInfo() = default;
    ~HgmProcessToServiceInfo() noexcept = default;

    bool Marshalling(Parcel& data) const override;
    [[nodiscard]] static HgmProcessToServiceInfo* Unmarshalling(Parcel& data);

    bool MarshallingFrameRateLinker(MessageParcel* message) const;
    bool MarshallingSurfaceData(MessageParcel* message) const;
    bool MarshallingEnergyData(MessageParcel* message) const;
    bool MarshallingVRateData(MessageParcel* message) const;
    bool UnmarshallingFrameRateLinker(MessageParcel* message);
    bool UnmarshallingSurfaceData(MessageParcel* message);
    bool UnmarshallingEnergyData(MessageParcel* message);
    bool UnmarshallingVRateData(MessageParcel* message);

    bool isGameNodeOnTree = false;
    std::unordered_set<FrameRateLinkerId> frameRateLinkerDestroyIds;
    FrameRateLinkerUpdateInfoMap frameRateLinkerUpdateInfoMap;
    FrameRateRange rsCurrRange;
    std::vector<std::tuple<std::string, pid_t>> surfaceData;
    std::unordered_map<std::string, pid_t> uiFrameworkDirtyNodeNameMap;
    EnergyCommonDataMap energyCommonData;
    std::unordered_map<NodeId, int> vRateMap;
    bool isNeedRefreshVRate = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_INFO_PARCEL_H