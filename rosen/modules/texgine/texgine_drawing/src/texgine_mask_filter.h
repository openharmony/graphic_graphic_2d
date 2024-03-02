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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_MASK_FILTER_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_MASK_FILTER_H

#include <memory>

#include "drawing.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineMaskFilter {
public:
    enum TexgineBlurStyle : int {
        K_NORMAL_SK_BLUR_STYLE, // Inner and outer blur
        K_SOLID_SK_BLUR_STYLE,  // solid inner and blur outer
        K_OUTER_SK_BLUR_STYLE,  // only blur outer
        K_INNER_SK_BLUR_STYLE,  // only blur inner
        K_LAST_ENUM_SK_BLUR_STYLE = K_INNER_SK_BLUR_STYLE,
    };
    std::shared_ptr<RSMaskFilter> GetMaskFilter() const;

    /*
     * @brief Sets SkMaskFilter that user want to TexgineMaskFilter
     */
    void SetMaskFilter(const std::shared_ptr<RSMaskFilter> filter);

    /*
     * @brief Create a blur maskfilter
     * @param style The TexgineBlurStyle to use
     * @param sigma The standard deviation of the Gaussian blur to apply. Must be greater than 0
     * @param respectCTM If true, the blurry Sigma is modified by CTM
     */
    static std::shared_ptr<TexgineMaskFilter> MakeBlur(TexgineBlurStyle style,
        float sigma, bool respectCTM = true);

private:
    std::shared_ptr<RSMaskFilter> filter_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_MASK_FILTER_H
