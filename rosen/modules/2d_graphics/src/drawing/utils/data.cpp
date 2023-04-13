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

#include "utils/data.h"

#include "impl_factory.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Data::Data() noexcept : impl_(ImplFactory::CreateDataImpl()) {}

bool Data::BuildFromMalloc(const void* data, size_t length)
{
    return (impl_ == nullptr) ? false : impl_->BuildFromMalloc(data, length);
}

bool Data::BuildWithCopy(const void* data, size_t length)
{
    return (impl_ == nullptr) ? false : impl_->BuildWithCopy(data, length);
}

bool Data::BuildWithoutCopy(const void* data, size_t length)
{
    return (impl_ == nullptr) ? false : impl_->BuildWithoutCopy(data, length);
}

bool Data::BuildUninitialized(size_t length)
{
    return (impl_ == nullptr) ? false : impl_->BuildUninitialized(length);
}

size_t Data::GetSize() const
{
    return (impl_ == nullptr) ? 0 : impl_->GetSize();
}

const void* Data::GetData() const
{
    return (impl_ == nullptr) ? nullptr : impl_->GetData();
}

void* Data::WritableData()
{
    return (impl_ == nullptr) ? nullptr : impl_->WritableData();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
