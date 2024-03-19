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

#include "texgine_text_blob.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::shared_ptr<RSTextBlob> TexgineTextBlob::GetTextBlob() const
{
    return textBlob_;
}

void TexgineTextBlob::SetTextBlob(const std::shared_ptr<RSTextBlob> textBlob)
{
    textBlob_ = textBlob;
}

void TexgineTextBlob::GetGlyphIDs(std::vector<uint16_t>& glyphIds)
{
    RSTextBlob::GetDrawingGlyphIDforTextBlob(textBlob_.get(), glyphIds);
}

RSPath TexgineTextBlob::GetPathbyGlyphID(const uint16_t& glyphId)
{
    return RSTextBlob::GetDrawingPathforTextBlob(glyphId, textBlob_.get());
}

} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
