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

#ifndef OHOS_ANI_COLOR_PICKER_H
#define OHOS_ANI_COLOR_PICKER_H

#include <ani.h>
#include "effect_type.h"
#include "color_picker.h"

namespace OHOS {
    namespace Rosen {
        class AniColorPicker {
        public:
            static ani_status Init(ani_env* env);

            static ani_object createColorPicker1(ani_env* env, ani_object pixelmap);
            static ani_object createColorPicker2(ani_env* env, ani_object pixelmap, ani_object region);

            static ani_object GetMainColor(ani_env* env, ani_object obj);
            static ani_object GetMainColorSync(ani_env* env, ani_object obj);
            static ani_object GetLargestProportionColor(ani_env* env, ani_object obj);
            static ani_object GetTopProportionColors(ani_env* env, ani_object obj, ani_double color_count);
            static ani_object GetHighestSaturationColor(ani_env* env, ani_object obj);
            static ani_object GetAverageColor(ani_env* env, ani_object obj);
            static ani_boolean IsBlackOrWhiteOrGrayColor(ani_env* env, ani_object obj, ani_double color_value);

            std::shared_ptr<ColorPicker> nativeColorPicker_ = {nullptr};
            std::shared_ptr<Media::PixelMap> srcPixelMap_ = nullptr;
            double coordinatesBuffer[4];

        private:
            static thread_local std::shared_ptr<ColorPicker> sColorPicker_;
        };
    } // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_COLOR_PICKER_H