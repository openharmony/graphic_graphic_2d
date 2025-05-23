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

#ifndef SAFUZZ_RANDOM_RS_FILTER_H
#define SAFUZZ_RANDOM_RS_FILTER_H

#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
class RandomRSFilter {
public:
    static std::shared_ptr<RSFilter> GetRandomRSFilter();

private:
    static std::shared_ptr<RSFilter> GetRandomFilter();
    static std::shared_ptr<RSFilter> GetRandomBlurFilter();
    static std::shared_ptr<RSFilter> GetRandomMaterialFilter();
    static std::shared_ptr<RSFilter> GetRandomLightUpEffectFilter();
    static std::shared_ptr<RSFilter> GetRandomAIBarFilter();
    static std::shared_ptr<RSFilter> GetRandomLinearGradientBlurShaderFilter();
    static std::shared_ptr<RSFilter> GetRandomForegroundEffectFilter();
    static std::shared_ptr<RSFilter> GetRandomMotionBlurFilter();
    static std::shared_ptr<RSFilter> GetRandomSpherizeEffectFilter();
    static std::shared_ptr<RSFilter> GetRandomHDRUIBrightnessFilter();
    static std::shared_ptr<RSFilter> GetRandomColorfulShadowFilter();
    static std::shared_ptr<RSFilter> GetRandomAttractionEffectFilter();
    static std::shared_ptr<RSFilter> GetRandomMagnifierShaderFilter();
    static std::shared_ptr<RSFilter> GetRandomWaterRippleShaderFilter();
    static std::shared_ptr<RSFilter> GetRandomFlyOutShaderFilter();
    static std::shared_ptr<RSFilter> GetRandomDistortionFilter();
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_RS_FILTER_H
