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

#include "text/text_blob.h"

#include <cstring>

#include "impl_interface/text_blob_impl.h"
#include "static_factory.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
TextBlob::TextBlob(std::shared_ptr<TextBlobImpl> textBlobImpl) noexcept : textBlobImpl_(textBlobImpl) {}

std::shared_ptr<TextBlob> TextBlob::MakeFromText(const void* text, size_t byteLength,
    const Font& font, TextEncoding encoding)
{
    return StaticFactory::MakeFromText(text, byteLength, font, encoding);
}

std::shared_ptr<TextBlob> TextBlob::MakeFromString(const char* str, const Font& font, TextEncoding encoding)
{
    if (!str) {
        LOGE("str nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    return MakeFromText(str, strlen(str), font, encoding);
}

std::shared_ptr<TextBlob> TextBlob::MakeFromRSXform(const void* text, size_t byteLength,
    const RSXform xform[], const Font& font, TextEncoding encoding)
{
    return StaticFactory::MakeFromRSXform(text, byteLength, xform, font, encoding);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
