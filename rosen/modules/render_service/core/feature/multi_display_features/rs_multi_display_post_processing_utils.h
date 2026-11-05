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

#ifndef RS_MULTI_DISPLAY_POST_PROCESSING_UTILS_H
#define RS_MULTI_DISPLAY_POST_PROCESSING_UTILS_H

#include <memory>
#include <optional>
#include <string>

#include "rs_multi_display_feature_common.h"
#include "surface_type.h"

namespace OHOS::Rosen {
class RSPaintFilterCanvas;
struct ComposerScreenInfo;

struct RSMultiDisplayInterpolationFilterParams {
    bool filterOn = false;
    float paramA = 0.0f;
    float paramN = 0.0f;
    float realWidth = 0.0f;
    float realHeight = 0.0f;
    float targetWidth = 0.0f;
    float targetHeight = 0.0f;
};

class RSMultiDisplayPostProcessingUtils {
public:
    RSMultiDisplayPostProcessingUtils() = delete;
    ~RSMultiDisplayPostProcessingUtils() = delete;
    static std::optional<InterpolationParams> GetActiveInterpolationParams(
        ScreenId screenId, uint32_t width, uint32_t height);
    static bool AdjustBufferConfigForInterpolation(uint32_t phyWidth,
        uint32_t phyHeight, BufferRequestConfig& bufferConfig);
    static bool Prepare(RSPaintFilterCanvas& canvas, RSMultiDisplayInterpolationFilterParams& filterParams,
        const ComposerScreenInfo& composerScreenInfo);
    static void Process(RSPaintFilterCanvas& canvas,
        RSMultiDisplayInterpolationFilterParams& filterParams, const bool needPostProcessing);
private:
    static bool CheckFilterParamsIsValid(
        const RSMultiDisplayInterpolationFilterParams& filterParams, const ComposerScreenInfo& composerScreenInfo);
};
} // namespace OHOS::Rosen
#endif // RS_MULTI_DISPLAY_POST_PROCESSING_UTILS_H
