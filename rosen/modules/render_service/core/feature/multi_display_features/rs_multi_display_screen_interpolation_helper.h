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

#ifndef RS_MULTI_DISPLAY_SCREEN_INTERPOLATION_HELPER_H
#define RS_MULTI_DISPLAY_SCREEN_INTERPOLATION_HELPER_H

#include <optional>
#include <vector>

#include "hdi_display_type.h"

namespace OHOS::Rosen {
class RSMultiDisplayScreenInterpolationHelper {
public:
    RSMultiDisplayScreenInterpolationHelper(uint32_t width, uint32_t height);
    ~RSMultiDisplayScreenInterpolationHelper() = default;
    RSMultiDisplayScreenInterpolationHelper(const RSMultiDisplayScreenInterpolationHelper&) = delete;
    RSMultiDisplayScreenInterpolationHelper& operator=(const RSMultiDisplayScreenInterpolationHelper&) = delete;
    RSMultiDisplayScreenInterpolationHelper(RSMultiDisplayScreenInterpolationHelper&&) = delete;
    RSMultiDisplayScreenInterpolationHelper& operator=(RSMultiDisplayScreenInterpolationHelper&&) = delete;

    void BackupAndModifyDisplayInfo(GraphicDisplayCapability& capability,
        std::vector<GraphicDisplayModeInfo>& supportedModes);
    std::optional<GraphicDisplayModeInfo> GetOriginModeInfo(int32_t modeId) const;
    const GraphicDisplayCapability& GetOriginCapability() const;

private:
    uint32_t realWidth_;
    uint32_t realHeight_;
    std::vector<GraphicDisplayModeInfo> originSupportedModes_;
    GraphicDisplayCapability originCapability_;
};
} // namespace OHOS::Rosen
#endif // RS_MULTI_DISPLAY_SCREEN_INTERPOLATION_HELPER_H
