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

#ifndef SKIA_MASK_FILTER_H
#define SKIA_MASK_FILTER_H

#include "include/core/SkMaskFilter.h"

#include "impl_interface/mask_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaMaskFilter : public MaskFilterImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaMaskFilter() noexcept;
    ~SkiaMaskFilter() {};
    AdapterType GetType() const override { return AdapterType::SKIA_ADAPTER; }

    void InitWithBlur(BlurType t, scalar radius) override;
    sk_sp<SkMaskFilter> GetMaskFilter() const;
private:
    sk_sp<SkMaskFilter> filter_;
};
}
}
}
#endif
