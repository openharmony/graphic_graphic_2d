/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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


#ifndef VSYNC_VSYNC_CONTROLLER_H
#define VSYNC_VSYNC_CONTROLLER_H

#include <cstdint>
#include <refbase.h>
#include <mutex>
#include "vsync_generator.h"
#include "graphic_common.h"

namespace OHOS {
namespace Rosen {
class VSyncController : public VSyncGenerator::Callback {
public:
    class Callback {
    public:
        virtual void OnVSyncEvent(int64_t now, int64_t period, uint32_t refreshRate, VSyncMode vsyncMode) = 0;
        virtual ~Callback() = default;
        /* std::pair<id, refresh rate> */
        virtual void OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates) = 0;
    };

    VSyncController(const sptr<VSyncGenerator> &geng, int64_t offset);
    ~VSyncController();

    // nocopyable
    VSyncController(const VSyncController &) = delete;
    VSyncController &operator=(const VSyncController &) = delete;

    VsyncError SetEnable(bool enable, bool& isGeneratorEnable);
    VsyncError SetCallback(Callback* cb);
    VsyncError SetPhaseOffset(int64_t offset);

private:

    void OnVSyncEvent(int64_t now, int64_t period, uint32_t refreshRate, VSyncMode vsyncMode);
    void OnPhaseOffsetChanged(int64_t phaseOffset);
    /* std::pair<id, refresh rate> */
    void OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates);
    wptr<VSyncGenerator> generator_;
    std::mutex callbackMutex_;
    Callback* callback_;

    std::mutex offsetMutex_;
    int64_t phaseOffset_;
    bool enabled_;
};
} // namespace Rosen
} // namespace OHOS

#endif
