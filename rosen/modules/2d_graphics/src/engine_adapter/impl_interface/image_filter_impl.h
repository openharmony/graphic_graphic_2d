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

#ifndef IMAGE_FILTER_IMPL_H
#define IMAGE_FILTER_IMPL_H

#include <memory>

#include "base_impl.h"

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ImageFilter;
class ColorFilter;
class ImageFilterImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    ImageFilterImpl() noexcept {}
    virtual ~ImageFilterImpl() {}
    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }

    virtual void InitWithBlur(scalar sigmaX, scalar sigmaY, const ImageFilter& f) = 0;
    virtual void InitWithColor(const ColorFilter& colorFilter, const ImageFilter& f) = 0;
    virtual void InitWithOffset(scalar dx, scalar dy, const ImageFilter& f) = 0;
    virtual void InitWithArithmetic(scalar k1, scalar k2, scalar k3, scalar k4, bool enforcePMColor,
        const ImageFilter& f1, const ImageFilter& f2) = 0;
    virtual void InitWithCompose(const ImageFilter& f1, const ImageFilter& f2) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif