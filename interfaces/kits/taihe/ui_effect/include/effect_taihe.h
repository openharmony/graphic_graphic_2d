/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef TAIHE_INCLUDE_UIEFFECT_EFFECT_TAIHE_H
#define TAIHE_INCLUDE_UIEFFECT_EFFECT_TAIHE_H

#include "effect/include/visual_effect.h"
#include "ohos.graphics.uiEffect.uiEffect.BrightnessBlender.proj.1.hpp"
#include "ohos.graphics.uiEffect.uiEffect.VisualEffect.proj.1.hpp"
#include "ohos.graphics.uiEffect.uiEffect.proj.hpp"
#include "ohos.graphics.uiEffect.uiEffect.impl.hpp"
#include "stdexcept"

namespace ANI::UIEffect {
using namespace taihe;
using namespace ohos::graphics::uiEffect::uiEffect;

class VisualEffectImpl {
public:
    VisualEffectImpl();
    explicit VisualEffectImpl(std::shared_ptr<OHOS::Rosen::VisualEffect> visualEffect);
    ~VisualEffectImpl();

    VisualEffect BackgroundColorBlender(BrightnessBlender const& brightnessBlender);
    VisualEffect ColorGradient(array_view<Color> colors, array_view<uintptr_t> positions, array_view<double> strengths,
        optional_view<Mask> alphaMask);
    VisualEffect BorderLight(uintptr_t lightPosition, uintptr_t lightColor, double lightIntensity, double borderWidth);

private:
    bool IsVisualEffectValid() const;

    std::shared_ptr<OHOS::Rosen::VisualEffect> nativeVisualEffect_;
};
} // namespace ANI::UIEffect

#endif