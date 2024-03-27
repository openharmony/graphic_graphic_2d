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

#include "effect/color_space.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
ColorSpace::ColorSpace(ColorSpaceType t) noexcept : ColorSpace()
{
    type_ = t;
    switch (type_) {
        case ColorSpace::ColorSpaceType::SRGB:
            impl_->InitWithSRGB();
            break;
        case ColorSpace::ColorSpaceType::SRGB_LINEAR:
            impl_->InitWithSRGBLinear();
            break;
        default:
            break;
    }
}

ColorSpace::ColorSpace(std::shared_ptr<ColorSpaceImpl> impl) noexcept
    : type_(ColorSpace::ColorSpaceType::NO_TYPE), impl_(impl)
{}

ColorSpace::ColorSpace(ColorSpaceType t, const Image& image) noexcept : ColorSpace()
{
    type_ = t;
    impl_->InitWithImage(image);
}

ColorSpace::ColorSpace(ColorSpaceType t,
                       const CMSTransferFuncType& func, const CMSMatrixType& matrix) noexcept : ColorSpace()
{
    type_ = t;
    impl_->InitWithRGB(func, matrix);
}

ColorSpace::ColorSpace(ColorSpaceType t,
                       const CMSTransferFunction& func, const CMSMatrix3x3& matrix) noexcept : ColorSpace()
{
    type_ = t;
    impl_->InitWithCustomRGB(func, matrix);
}

ColorSpace::ColorSpace() noexcept
    : type_(ColorSpace::ColorSpaceType::NO_TYPE), impl_(ImplFactory::CreateColorSpaceImpl())
{}

ColorSpace::ColorSpaceType ColorSpace::GetType() const
{
    return type_;
}

std::shared_ptr<ColorSpace> ColorSpace::CreateFromImpl(std::shared_ptr<ColorSpaceImpl> impl)
{
    return std::make_shared<ColorSpace>(impl);
}

std::shared_ptr<ColorSpace> ColorSpace::CreateSRGB()
{
    return std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB);
}

std::shared_ptr<ColorSpace> ColorSpace::CreateSRGBLinear()
{
    return std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::SRGB_LINEAR);
}

std::shared_ptr<ColorSpace> ColorSpace::CreateRefImage(const Image& image)
{
    return std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::REF_IMAGE, image);
}

std::shared_ptr<ColorSpace> ColorSpace::CreateRGB(const CMSTransferFuncType& func, const CMSMatrixType& matrix)
{
    return std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::RGB, func, matrix);
}

std::shared_ptr<ColorSpace> ColorSpace::CreateCustomRGB(const CMSTransferFunction& func, const CMSMatrix3x3& matrix)
{
    return std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::RGB, func, matrix);
}

sk_sp<SkColorSpace> ColorSpace::GetSkColorSpace() const
{
    return impl_->GetSkColorSpace();
}

std::shared_ptr<Data> ColorSpace::Serialize() const
{
    return impl_->Serialize();
}

bool ColorSpace::Deserialize(std::shared_ptr<Data> data)
{
    return impl_->Deserialize(data);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS