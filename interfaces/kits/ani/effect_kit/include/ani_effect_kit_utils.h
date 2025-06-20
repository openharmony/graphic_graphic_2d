/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ANI_EFFECT_KIT_UTILS_H
#define OHOS_ANI_EFFECT_KIT_UTILS_H

#include <ani.h>

#include "ani_filter.h"
#include "ani_color_picker.h"

namespace OHOS {
namespace Rosen {
class AniEffectKitUtils {
public:
    static ani_object CreateAniObject(ani_env* env, const char* className, const char* methodSig, ani_long object);
    static ani_object CreateAniUndefined(ani_env* env);
    static AniFilter* GetFilterFromEnv([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object obj);
    //新增
    static AniColorPicker* GetColorPickerFromEnv([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object obj);
};

} // namespace Rosen
} // namespace OHOS
#endif //OHOS_ANI_EFFECT_KIT_UTILS_H