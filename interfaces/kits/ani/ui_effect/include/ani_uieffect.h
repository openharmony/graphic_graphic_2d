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

#ifndef UI_EFFECT_ANI_UIEFFECT_H
#define UI_EFFECT_ANI_UIEFFECT_H

#undef LOG_TAG
#define LOG_TAG "UiEffect_Ani"

#define UIEFFECT_LOG_D(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
#define UIEFFECT_LOG_I(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define UIEFFECT_LOG_E(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)

#include <iostream>

#include "ani.h"
#include "effect/include/background_color_effect_para.h"
#include "effect/include/blender.h"
#include "effect/include/brightness_blender.h"
#include "effect/include/visual_effect.h"
#include "effect/include/visual_effect_para.h"
#include "hilog/log.h"

namespace OHOS {
namespace Rosen {
static const std::string ANI_UIEFFECT_SPACE = "L@ohos/graphics/uiEffect/uiEffect";
static const std::string ANI_UIEFFECT_VISUAL_EFFECT = ANI_UIEFFECT_SPACE + "/VisualEffectInternal;";
static const std::string ANI_UIEFFECT_BRIGHTNESS_BLENDER = ANI_UIEFFECT_SPACE + "/BrightnessBlenderInternal;";
class AniEffect {
public:
    AniEffect();
    ~AniEffect();
    static ani_object CreateEffect(ani_env* env);
    static ani_object CreateBrightnessBlender(ani_env* env, ani_object para);
    static ani_object BackgroundColorBlender(ani_env* env, ani_object obj, ani_object para);

private:
    static ani_object CreateAniObject(ani_env* env, std::string name, const char* signature, ani_long addr);
    static void ParseBrightnessBlender(ani_env* env, ani_object para_obj, std::shared_ptr<BrightnessBlender>& blender);
};
} // namespace Rosen
} // namespace OHOS
#endif