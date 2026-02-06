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
#include "feature/hyper_graphic_manager/hgm_rp_energy.h"

#include "common/rs_optional_trace.h"
namespace OHOS {

namespace Rosen {
void HgmRPEnergy::SyncEnergyInfoToRP(const EnergyInfo& energyInfo)
{
    energyInfo_ = energyInfo;
}

void HgmRPEnergy::SetComponentDefaultFps(FrameRateRange& rsRange, pid_t pid)
{
    if (energyInfo_.componentName != SWIPER_DRAG_SCENE || pid != energyInfo_.componentPid || !rsRange.IsZero()) {
        return;
    }
    if (rsRange.componentScene_ != ComponentScene::SWIPER_FLING && !(rsRange.type_ & SWIPER_DRAG_FRAME_RATE_TYPE)) {
        return;
    }
    RS_TRACE_NAME_FMT("%s pid: %d, frameRate: %d", __func__, pid, energyInfo_.componentDefaultFps);
    rsRange.min_ = 0;
    rsRange.max_ = energyInfo_.componentDefaultFps;
    rsRange.preferred_ = energyInfo_.componentDefaultFps;
}
} // namespace Rosen
} // namespace OHOS