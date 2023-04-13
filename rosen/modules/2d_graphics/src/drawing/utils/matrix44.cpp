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

#include "utils/matrix44.h"

#include "impl_factory.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Matrix44::Matrix44() : impl_(ImplFactory::CreateMatrix44Impl()) {}

void Matrix44::Translate(scalar dx, scalar dy, scalar dz)
{
    if (impl_ != nullptr) {
        impl_->Translate(dx, dy, dz);
    }
}

void Matrix44::Scale(scalar sx, scalar sy, scalar sz)
{
    if (impl_ != nullptr) {
        impl_->Scale(sx, sy, sz);
    }
}

Matrix44 Matrix44::operator*(const Matrix44& other)
{
    if (impl_ != nullptr) {
        impl_->Multiply(*this, other);
    }

    return *this;
}

Matrix44::operator Matrix() const
{
    return (impl_ == nullptr) ? *this : impl_->ConvertToMatrix();
}

void Matrix44::SetMatrix44(const Buffer& buffer)
{
    if (impl_ != nullptr) {
        impl_->SetMatrix44(buffer);
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
