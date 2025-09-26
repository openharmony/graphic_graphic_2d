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

#ifndef TAIHE_INCLUDE_UI_EFFECT_TAIHE_UTILS_H
#define TAIHE_INCLUDE_UI_EFFECT_TAIHE_UTILS_H

#include <hilog/log.h>

#include "common/rs_vector4.h"
#include "effect/include/brightness_blender.h"
#include "effect/shader_effect.h"
#include "ohos.graphics.uiEffect.BrightnessBlender.proj.1.hpp"
#include "ohos.graphics.uiEffect.TileMode.proj.1.hpp"
#include "ohos.graphics.uiEffect.proj.hpp"

namespace ANI::UIEffect {

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001405

#undef LOG_TAG
#define LOG_TAG "UiEffect_TaiHe"

#define UIEFFECT_LOG_D(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
#define UIEFFECT_LOG_I(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define UIEFFECT_LOG_E(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)

using namespace taihe;
using namespace ohos::graphics::uiEffect;
namespace Drawing = OHOS::Rosen::Drawing;

bool IsSystemApp();
Drawing::TileMode ConvertTileModeFromTaiheTileMode(TileMode tileMode);
bool ConvertVector4fFromTaiheArray(OHOS::Rosen::Vector4f& vector4f, taihe::array_view<double> stretchSizes);
bool ParseBrightnessBlender(OHOS::Rosen::BrightnessBlender& blender, const BrightnessBlender& brightnessBlender);

} // namespace ANI::UIEffect

#endif