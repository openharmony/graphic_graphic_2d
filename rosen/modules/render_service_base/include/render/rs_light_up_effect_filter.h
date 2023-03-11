/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LIGHT_UP_EFFECT_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LIGHT_UP_EFFECT_FILTER_H

#include "include/core/SkColorFilter.h"
#include "include/effects/SkImageFilters.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
class RSLightUpEffectFilter : public RSSkiaFilter {
public:
    RSLightUpEffectFilter(float lightUpDegree);
    ~RSLightUpEffectFilter() override;
    float GetLightUpDegree();

    std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Multiply(float rhs) override;
    std::shared_ptr<RSFilter> Negate() override;
private:
    float lightUpDegree_ = 0.f;
    sk_sp<SkImageFilter> CreateLightUpEffectFilter(float lightUpDegree);

    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LIGHT_UP_EFFECT_FILTER_H