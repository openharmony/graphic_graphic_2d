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

#include "skia_picture.h"

#include "skia_data.h"
#include "skia_serial_procs.h"
#include "utils/data.h"
#include "utils/log.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPicture::SkiaPicture() noexcept : skiaPicture_(nullptr) {}

const sk_sp<SkPicture> SkiaPicture::GetPicture() const
{
    return skiaPicture_;
}

std::shared_ptr<Data> SkiaPicture::Serialize() const
{
    if (skiaPicture_ == nullptr) {
        LOGD("SkiaPicture::Serialize, SkPicture is nullptr!");
        return nullptr;
    }

    auto skData = skiaPicture_->serialize();
    auto data = std::make_shared<Data>();
    data->GetImpl<SkiaData>()->SetSkData(skData);

    return data;
}

bool SkiaPicture::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        return false;
    }

    skiaPicture_ = SkPicture::MakeFromData(data->GetData(), data->GetSize());
    return skiaPicture_ != nullptr;
}

void SkiaPicture::SetSkPicture(const sk_sp<SkPicture>& skPicture)
{
    skiaPicture_ = skPicture;
}

int SkiaPicture::ApproximateOpCount(bool nested)
{
    if (skiaPicture_ == nullptr) {
        return -1;
    }
    return skiaPicture_->approximateOpCount(nested);
}

std::shared_ptr<Data> SkiaPicture::Serialize(SerialProcs* proc)
{
    if (proc == nullptr) {
        return nullptr;
    }
    auto skiaSerialProcs = proc->GetImpl<SkiaSerialProcs>();
    if (skiaSerialProcs == nullptr) {
        return nullptr;
    }
    auto skProc = skiaSerialProcs->GetSkSerialProcs();
    if (proc->HasTypefaceProc()) {
        skProc->fTypefaceProc = [](SkTypeface* tf, void* ctx) {
            return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
        };
    }
    auto data = std::make_shared<Data>();
    auto skData = skiaPicture_->serialize(skProc);
    auto skiaData = data->GetImpl<SkiaData>();
    if (skiaData == nullptr) {
        return nullptr;
    }
    skiaData->SetSkData(skData);
    return data;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
