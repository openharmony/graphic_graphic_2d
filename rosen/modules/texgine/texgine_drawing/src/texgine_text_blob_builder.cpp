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
std::shared_ptr<SkTextBlobBuilder> TexgineTextBlobBuilder::GetTextBlobBuilder()
{
    return textBlobBuilder_;
}

TexgineTextBlobBuilder::RunBuffer &TexgineTextBlobBuilder::AllocRunPos(const TexgineFont &font, int count)
{
    auto &runBuffer = textBlobBuilder_->allocRunPos(*font.GetFont(), count);
    buffer_.glyphs_ = runBuffer.glyphs;
    buffer_.pos_ = runBuffer.pos;
    buffer_.utf8text_ = runBuffer.utf8text;
    buffer_.clusters_ = runBuffer.clusters;
    return buffer_;
}

std::shared_ptr<TexgineTextBlob> TexgineTextBlobBuilder::Make()
{
    auto tb = std::make_shared<TexgineTextBlob>();
    auto blob = textBlobBuilder_->make();
    if (blob == nullptr) {
        return nullptr;
    }
    tb->SetTextBlob(blob);
    return tb;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
