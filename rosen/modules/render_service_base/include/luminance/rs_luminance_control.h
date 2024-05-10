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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_LUMINANCE_CONTROL_H
#define ROSEN_RENDER_SERVICE_BASE_RS_LUMINANCE_CONTROL_H

#include <cinttypes>
#include <mutex>
#include <unordered_map>

#include "common/rs_macros.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSLuminanceControl {
public:
    RSB_EXPORT static RSLuminanceControl& Get()
    {
        static RSLuminanceControl instance;
        return instance;
    }
    RSLuminanceControl(const RSLuminanceControl&) = delete;
    RSLuminanceControl& operator=(const RSLuminanceControl&) = delete;
    RSLuminanceControl(RSLuminanceControl&&) = delete;
    RSLuminanceControl& operator=(RSLuminanceControl&&) = delete;

    RSB_EXPORT void SetHdrStatus([[maybe_unused]] ScreenId screenId, [[maybe_unused]] bool isHdrOn);
    RSB_EXPORT bool IsHdrOn([[maybe_unused]] ScreenId screenId) const;
    RSB_EXPORT bool IsDimmingOn([[maybe_unused]] ScreenId screenId) const;
    RSB_EXPORT void DimmingIncrease([[maybe_unused]] ScreenId screenId);

    RSB_EXPORT void SetSdrLuminance([[maybe_unused]] ScreenId screenId, [[maybe_unused]] uint32_t level);
    RSB_EXPORT uint32_t GetNewHdrLuminance([[maybe_unused]] ScreenId screenId);
    RSB_EXPORT void SetNowHdrLuminance([[maybe_unused]] ScreenId screenId, [[maybe_unused]] uint32_t level);
    RSB_EXPORT bool IsNeedUpdateLuminance([[maybe_unused]] ScreenId screenId);

    RSB_EXPORT float GetHdrTmoNits([[maybe_unused]] ScreenId screenId, int Mode) const;
    RSB_EXPORT float GetHdrDisplayNits([[maybe_unused]] ScreenId screenId) const;
    RSB_EXPORT double GetHdrBrightnessRatio([[maybe_unused]] ScreenId screenId, int Mode) const;

private:
    RSLuminanceControl() = default;
    ~RSLuminanceControl() = default;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_LUMINANCE_CONTROL_H
