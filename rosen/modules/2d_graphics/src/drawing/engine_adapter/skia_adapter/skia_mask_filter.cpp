/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "src/core/SkMaskFilterBase.h"
#include "skia_mask_filter.h"
#include "skia_helper.h"
#include "effect/mask_filter.h"
#include "utils/data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaMaskFilter::SkiaMaskFilter() noexcept : filter_(nullptr) {}

void SkiaMaskFilter::InitWithBlur(BlurType t, scalar sigma, bool respectCTM)
{
    filter_ = SkMaskFilter::MakeBlur(static_cast<SkBlurStyle>(t), sigma, respectCTM);
}

sk_sp<SkMaskFilter> SkiaMaskFilter::GetMaskFilter() const
{
    return filter_;
}

void SkiaMaskFilter::SetSkMaskFilter(const sk_sp<SkMaskFilter>& filter)
{
    filter_ = filter;
}

std::shared_ptr<Data> SkiaMaskFilter::Serialize() const
{
    if (filter_ == nullptr) {
        return nullptr;
    }

    return SkiaHelper::FlattenableSerialize(filter_.get());
}

bool SkiaMaskFilter::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        LOGD("SkiaMaskFilter::Deserialize, data is invalid!");
        return false;
    }

    filter_ = SkiaHelper::FlattenableDeserialize<SkMaskFilterBase>(data);
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS