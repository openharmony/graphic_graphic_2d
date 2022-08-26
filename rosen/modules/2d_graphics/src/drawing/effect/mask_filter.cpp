/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "effect/mask_filter.h"

#include "impl_factory.h"

#include "impl_interface/mask_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
MaskFilter::MaskFilter(FilterType t, BlurType blurType, scalar sigma) noexcept : MaskFilter()
{
    type_ = t;
    impl_->InitWithBlur(blurType, sigma);
}

MaskFilter::MaskFilter() noexcept : type_(MaskFilter::FilterType::NO_TYPE), impl_(ImplFactory::CreateMaskFilterImpl())
{}

MaskFilter::FilterType MaskFilter::GetType() const
{
    return type_;
}

std::shared_ptr<MaskFilter> MaskFilter::CreateBlurMaskFilter(BlurType blurType, scalar sigma)
{
    return std::make_shared<MaskFilter>(MaskFilter::FilterType::BLUR, blurType, sigma);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS