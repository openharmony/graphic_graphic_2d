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
#ifndef USE_ROSEN_DRAWING
sk_sp<SkTextBlob> TexgineTextBlob::GetTextBlob() const
#else
std::shared_ptr<RSTextBlob> TexgineTextBlob::GetTextBlob() const
#endif
{
    return textBlob_;
}

#ifndef USE_ROSEN_DRAWING
void TexgineTextBlob::SetTextBlob(const sk_sp<SkTextBlob> textBlob)
#else
void TexgineTextBlob::SetTextBlob(const std::shared_ptr<RSTextBlob> textBlob)
#endif
{
    textBlob_ = textBlob;
}

#ifndef USE_ROSEN_DRAWING
void TexgineTextBlob::GetGlyphIDs(std::vector<SkGlyphID>& glyphIds)
{
    GetGlyphIDforTextBlob(textBlob_.get(), glyphIds);
}

SkPath TexgineTextBlob::GetPathbyGlyphID(const SkGlyphID& glyphId)
{
    return GetPathforTextBlob(glyphId, textBlob_.get());
}
#else
void TexgineTextBlob::GetGlyphIDs(std::vector<uint16_t>& glyphIds)
{
    RSTextBlob::GetDrawingGlyphIDforTextBlob(textBlob_.get(), glyphIds);
}

RSPath TexgineTextBlob::GetPathbyGlyphID(const uint16_t& glyphId)
{
    return RSTextBlob::GetDrawingPathforTextBlob(glyphId, textBlob_.get());
}
#endif

} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
