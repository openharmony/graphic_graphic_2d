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

#ifndef COLOR_SPACE_IMPL_H
#define COLOR_SPACE_IMPL_H

#include "include/core/SkColorSpace.h"

#include "base_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;
enum class CMSTransferFuncType;
enum class CMSMatrixType;
struct CMSTransferFunction;
struct CMSMatrix3x3;
class Image;
class ColorSpaceImpl : public BaseImpl {
public:
    ColorSpaceImpl() noexcept {}
    ~ColorSpaceImpl() override {}

    virtual void InitWithSRGB() = 0;
    virtual void InitWithSRGBLinear() = 0;
    virtual void InitWithImage(const Image& image) = 0;
    virtual void InitWithRGB(const CMSTransferFuncType& func, const CMSMatrixType& matrix) = 0;
    virtual void InitWithCustomRGB(const CMSTransferFunction& func, const CMSMatrix3x3& matrix) = 0;
    virtual sk_sp<SkColorSpace> GetSkColorSpace() const = 0;
    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif