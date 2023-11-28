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

#include "texgine_mask_filter.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#ifndef USE_ROSEN_DRAWING
sk_sp<SkMaskFilter> TexgineMaskFilter::GetMaskFilter() const
#else
std::shared_ptr<RSMaskFilter> TexgineMaskFilter::GetMaskFilter() const
#endif
{
    return filter_;
}

#ifndef USE_ROSEN_DRAWING
void TexgineMaskFilter::SetMaskFilter(const sk_sp<SkMaskFilter> filter)
#else
void TexgineMaskFilter::SetMaskFilter(const std::shared_ptr<RSMaskFilter> filter)
#endif
{
    filter_ = filter;
}

std::shared_ptr<TexgineMaskFilter> TexgineMaskFilter::MakeBlur(TexgineBlurStyle style,
    float sigma, bool respectCTM)
{
#ifndef USE_ROSEN_DRAWING
    auto filter = SkMaskFilter::MakeBlur(static_cast<SkBlurStyle>(style), sigma, respectCTM);
#else
    std::shared_ptr<RSMaskFilter> filter = RSMaskFilter::CreateBlurMaskFilter(static_cast<Drawing::BlurType>(style),
        sigma, respectCTM);
#endif
    auto maskFilter = std::make_shared<TexgineMaskFilter>();
    maskFilter->SetMaskFilter(filter);
    return maskFilter;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
