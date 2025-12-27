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

#ifndef OHOS_ROSEN_ANI_SHADOW_LAYER_H
#define OHOS_ROSEN_ANI_SHADOW_LAYER_H

#include "ani_drawing_utils.h"
#include "effect/blur_draw_looper.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniShadowLayer final {
public:
    explicit AniShadowLayer(std::shared_ptr<BlurDrawLooper> blurDrawLooper = nullptr)
        : blurDrawLooper_(blurDrawLooper) {}
    ~AniShadowLayer();

    static ani_status AniInit(ani_env *env);

    static ani_object Create(
        ani_env* env, ani_object obj, ani_double blurRadius, ani_double x, ani_double y, ani_object color);
    static ani_object CreateWithColor(
        ani_env* env, ani_object obj, ani_double blurRadius, ani_double x, ani_double y, ani_object color);

    std::shared_ptr<BlurDrawLooper> GetBlurDrawLooper();
private:
    static ani_object CreateLooper(ani_env* env, const std::shared_ptr<BlurDrawLooper> blurDrawLooper);
    std::shared_ptr<BlurDrawLooper> blurDrawLooper_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_SHADOW_LAYER_H