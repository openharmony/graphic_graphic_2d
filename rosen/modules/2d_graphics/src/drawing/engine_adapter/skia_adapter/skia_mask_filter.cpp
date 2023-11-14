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
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
#include "skia_mask_filter.h"

#include "effect/mask_filter.h"
#include "utils/data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaMaskFilter::SkiaMaskFilter() noexcept : filter_(nullptr) {}

void SkiaMaskFilter::InitWithBlur(BlurType t, scalar sigma)
{
    filter_ = SkMaskFilter::MakeBlur(static_cast<SkBlurStyle>(t), sigma);
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
#ifdef ROSEN_OHOS
    if (filter_ == nullptr) {
        LOGE("SkiaMaskFilter::Serialize, filter_ is nullptr!");
        return nullptr;
    }

    SkBinaryWriteBuffer writer;
    writer.writeFlattenable(filter_.get());
    size_t length = writer.bytesWritten();
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildUninitialized(length);
    writer.writeToMemory(data->WritableData());
    return data;
#else
    return nullptr;
#endif
}

bool SkiaMaskFilter::Deserialize(std::shared_ptr<Data> data)
{
#ifdef ROSEN_OHOS
    if (data == nullptr) {
        LOGE("SkiaMaskFilter::Deserialize, data is invalid!");
        return false;
    }

    SkReadBuffer reader(data->GetData(), data->GetSize());
    filter_ = reader.readMaskFilter();
    return filter_ != nullptr;
#else
    return false;
#endif
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS