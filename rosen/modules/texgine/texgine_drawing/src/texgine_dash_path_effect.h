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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_DASH_PATH_EFFECT_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_DASH_PATH_EFFECT_H

#include <memory>

#include "texgine_path_effect.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineDashPathEffect {
public:
    /*
     * @brief Make a Dash path
     * @param intervals[] The array containing an even number of entries (>=2), This array will be
     *                    copied in Make, and can be disposed of freely after.
     *         count      The number of elements in the intervals array
     *         phase      The offset to intervals
     */
    static std::shared_ptr<TexginePathEffect> Make(const float intervals[], int count, float phase);
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_DASH_PATH_EFFECT_H
