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

#include "effect_taihe.h"

#include "effect/include/background_color_effect_para.h"
#include "effect/include/brightness_blender.h"
#include "effect/include/visual_effect.h"
#include "ohos.graphics.uiEffect.BrightnessBlender.proj.1.hpp"
#include "ohos.graphics.uiEffect.VisualEffect.proj.1.hpp"
#include "ui_effect_taihe_utils.h"

using namespace ANI::UIEffect;

namespace ANI::UIEffect {
VisualEffectImpl::VisualEffectImpl()
{
    nativeVisualEffect_ = std::make_shared<OHOS::Rosen::VisualEffect>();
}

VisualEffectImpl::VisualEffectImpl(std::shared_ptr<OHOS::Rosen::VisualEffect> visualEffect)
{
    nativeVisualEffect_ = visualEffect;
}

VisualEffectImpl::~VisualEffectImpl()
{
    nativeVisualEffect_ = nullptr;
}

VisualEffect VisualEffectImpl::backgroundColorBlender(BrightnessBlender const& brightnessBlender)
{
    if (!IsSystemApp()) {
        UIEFFECT_LOG_E("call backgroundColorBlender failed, is not system app");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    if (!IsVisualEffectValid()) {
        UIEFFECT_LOG_E("VisualEffectImpl::backgroundColorBlender failed, visual effect is invalid");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    auto backgroundColorEffectPara = std::make_shared<OHOS::Rosen::BackgroundColorEffectPara>();
    auto blender = std::make_shared<OHOS::Rosen::BrightnessBlender>();
    if (!ParseBrightnessBlender(*blender, brightnessBlender)) {
        UIEFFECT_LOG_E("VisualEffectImpl::backgroundColorBlender parse brightnessBlender failed");
        return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
    }

    backgroundColorEffectPara->SetBlender(blender);

    nativeVisualEffect_->AddPara(backgroundColorEffectPara);
    return make_holder<VisualEffectImpl, VisualEffect>(nativeVisualEffect_);
}

bool VisualEffectImpl::IsVisualEffectValid() const
{
    return nativeVisualEffect_ != nullptr;
}

VisualEffect createEffect()
{
    return make_holder<VisualEffectImpl, VisualEffect>();
}

} // namespace ANI::UIEffect

// NOLINTBEGIN
TH_EXPORT_CPP_API_createEffect(createEffect);
// NOLINTEND