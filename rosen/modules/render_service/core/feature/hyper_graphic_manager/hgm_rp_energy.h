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

#include "hgm_energy_consumption_policy.h"

#include "transaction/rp_hgm_config_data.h"
namespace OHOS::Rosen {
class HgmRPEnergy {
public:
    HgmRPEnergy();
    ~HgmRPEnergy();

    void SetTouchState(bool isIdle);

    void MoveEnergyCommonDataTo(EnergyCommonDataMap& commonData);

    void AddEnergyCommonData(EnergyEvent event, const std::string& key, const std::string& value);

    void StatisticAnimationTime(uint64_t timestamp);

    void HgmConfigUpdateCallback(std::shared_ptr<RPHgmConfigData> configData);

private:
    void GetComponentFps(FrameRateRange& range);

    std::mutex mutex_;
    bool isTouchIdle_ = false;
    EnergyCommonDataMap energyCommonData_;
    std::unordered_map<std::string, int32_t> componentPowerConfig_;
};
}; // namespace OHOS::Rosen
