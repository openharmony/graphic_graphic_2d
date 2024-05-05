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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PATH_1D_PATH_EFFECT_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PATH_1D_PATH_EFFECT_H

#include <memory>

#include "drawing.h"

#include "texgine_path.h"
#include "texgine_path_effect.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexginePath1DPathEffect {
public:
    enum Style {
        K_TRANSLATE_STYLE,
        K_ROTATE_STYLE,
        K_MORPH_STYLE,
        K_LASTENUM_STYLE = K_MORPH_STYLE,
    };

    /*
     * @brief Dash by copying the specified path
     * @param path The path to copying
     * @param advance Space between path instances
     * @param style Path transformation style between each point
     */
    static std::shared_ptr<TexginePathEffect> Make(const TexginePath& path, float advance, float phase, Style style);
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PATH_1D_PATH_EFFECT_H
