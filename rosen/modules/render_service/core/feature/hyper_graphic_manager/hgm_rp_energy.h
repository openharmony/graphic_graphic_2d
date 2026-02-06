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

#ifndef HGM_RP_ENERGY_H
#define HGM_RP_ENERGY_H
#include <string>

#include "animation/rs_frame_rate_range.h"
#include "transaction/rs_hgm_config_data.h"

namespace OHOS {
namespace Rosen {
class HgmRPEnergy {
public:
    void SyncEnergyInfoToRP(const EnergyInfo& energyInfo);
    void SetComponentDefaultFps(FrameRateRange& rsRange, pid_t pid);

private:
    EnergyInfo energyInfo_ = {};
};
} // namespace Rosen
} // namespace OHOS
#endif