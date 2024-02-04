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
#include "utils/data.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
#endif

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

bool SkiaData::BuildWithProc(const void* ptr, size_t length, DataReleaseProc proc, void* ctx)
{
    skData_ = SkData::MakeWithProc(ptr, length, proc, ctx);
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

bool SkiaData::BuildEmpty()
{
    skData_ = SkData::MakeEmpty();
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

std::shared_ptr<Data> SkiaData::MakeFromFileName(const char path[])
{
    sk_sp<SkData> skData = SkData::MakeFromFileName(path);
    if (!skData) {
        LOGD("SkiaData::MakeFromFileName, skData is nullptr!");
        return nullptr;
    }
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->GetImpl<SkiaData>()->SetSkData(skData);
    return data;
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

std::shared_ptr<Data> SkiaData::Serialize() const
{
#ifdef ROSEN_OHOS
    if (skData_ == nullptr) {
        LOGD("SkiaData::Serialize, skData_ is nullptr!");
        return nullptr;
    }

    SkBinaryWriteBuffer writer;
    writer.writeDataAsByteArray(skData_.get());

    size_t length = writer.bytesWritten();
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildUninitialized(length);
    writer.writeToMemory(data->WritableData());
    return data;
#else
    return nullptr;
#endif
}
} // Drawing
} // Rosen
} // OHOS
