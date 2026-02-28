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

#ifndef OHOS_ROSEN_ANI_IMAGE_FILTER_H
#define OHOS_ROSEN_ANI_IMAGE_FILTER_H

#include "ani_drawing_utils.h"
#include "effect/image_filter.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniImageFilter final {
public:
    explicit AniImageFilter(std::shared_ptr<ImageFilter> imageFilter) : imageFilter_(imageFilter) {}
    ~AniImageFilter();

    static ani_status AniInit(ani_env *env);

    static ani_object CreateFromColorFilter(
        ani_env* env, ani_object obj, ani_object aniColorFilterObj, ani_object aniImageFilterObj);

    static ani_object CreateBlurImageFilter(ani_env* env, ani_object obj,
        ani_double sigmaX, ani_double sigmaY, ani_enum_item aniTileMode, ani_object aniImageFilterObj);

    static ani_object CreateOffsetImageFilter(ani_env* env, ani_object obj, ani_double dx, ani_double dy,
        ani_object aniInputImageFilterobj);

    static ani_object createComposeImageFilter(ani_env* env, ani_object obj, ani_object cOuterImageFilterobj,
        ani_object cInnerImageFilterobj);

    static ani_object createBlendImageFilter(ani_env* env, ani_object obj, ani_enum_item blendModeobj,
        ani_object backgroundImageFilteObj, ani_object foregroundImageFilterobj);

    static ani_object CreateFromShaderEffect(ani_env* env, ani_object obj, ani_object aniShaderEffectobj);

    static ani_object CreateFromImage(ani_env* env, ani_object obj, ani_object pixelmapObj,
        ani_object srcRectobj, ani_object dstRectobj);

    DRAWING_API std::shared_ptr<ImageFilter> GetImageFilter();
private:
    std::shared_ptr<ImageFilter> imageFilter_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_IMAGE_FILTER_H
