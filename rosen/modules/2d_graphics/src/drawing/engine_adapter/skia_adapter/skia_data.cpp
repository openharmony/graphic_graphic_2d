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

#include "skia_data.h"

#include "include/core/SkImageInfo.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaData::SkiaData() noexcept : skData_(nullptr) {}

bool SkiaData::BuildFromMalloc(const void* data, size_t length)
{
    skData_ = SkData::MakeFromMalloc(data, length);
    return skData_ != nullptr;
}

bool SkiaData::BuildWithCopy(const void* data, size_t length)
{
    skData_ = SkData::MakeWithCopy(data, length);
    return skData_ != nullptr;
}

bool SkiaData::BuildWithoutCopy(const void* data, size_t length)
{
    skData_ = SkData::MakeWithoutCopy(data, length);
    return skData_ != nullptr;
}

bool SkiaData::BuildUninitialized(size_t length)
{
    skData_ = SkData::MakeUninitialized(length);
    return skData_ != nullptr;
}

size_t SkiaData::GetSize() const
{
    return (skData_ == nullptr) ? 0 : skData_->size();
}

const void* SkiaData::GetData() const
{
    return (skData_ == nullptr) ? nullptr : skData_->data();
}

void* SkiaData::WritableData()
{
    return (skData_ == nullptr) ? nullptr : skData_->writable_data();
}

sk_sp<SkData> SkiaData::GetSkData() const
{
    return skData_;
}

void SkiaData::SetSkData(const sk_sp<SkData>& data)
{
    skData_ = data;
}
} // Drawing
} // Rosen
} // OHOS
