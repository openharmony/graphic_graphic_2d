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

#include <include/core/SkMaskFilter.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
enum TexgineBlurStyle : int {
    kNormal_SkBlurStyle,
    kSolid_SkBlurStyle,
    kOuter_SkBlurStyle,
    kInner_SkBlurStyle,

    kLastEnum_SkBlurStyle = kInner_SkBlurStyle,
};

class TexgineMaskFilter {
public:
    sk_sp<SkMaskFilter> GetMaskFilter();
    void SetMaskFilter(sk_sp<SkMaskFilter> filter);
    static std::shared_ptr<TexgineMaskFilter> MakeBlur(TexgineBlurStyle style, float sigma,
                                        bool respectCTM = true);

private:
    sk_sp<SkMaskFilter> filter_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_MASK_FILTER_H
