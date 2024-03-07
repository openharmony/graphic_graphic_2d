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

#include "texgine_text_blob_builder.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::shared_ptr<RSTextBlobBuilder> TexgineTextBlobBuilder::GetTextBlobBuilder() const
{
    return textBlobBuilder_;
}

const RSTextBlobBuilder::RunBuffer& TexgineTextBlobBuilder::AllocRunPos(const TexgineFont &font, int count)
{
    return textBlobBuilder_->AllocRunPos(*font.GetFont(), count);
}

std::shared_ptr<TexgineTextBlob> TexgineTextBlobBuilder::Make()
{
    if (textBlobBuilder_ == nullptr) {
        return nullptr;
    }
    auto tb = std::make_shared<TexgineTextBlob>();
    auto blob = textBlobBuilder_->Make();
    if (blob == nullptr) {
        return nullptr;
    }
    tb->SetTextBlob(blob);
    return tb;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
