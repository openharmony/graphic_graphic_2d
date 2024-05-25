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

#include "skia_text_blob_builder.h"

#include "skia_adapter/skia_convert_utils.h"
#include "skia_adapter/skia_font.h"
#include "skia_adapter/skia_text_blob.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<TextBlob> SkiaTextBlobBuilder::Make()
{
    sk_sp<SkTextBlob> skTextBlob = skTextBlobBuilder_.make();
    if (!skTextBlob) {
        return nullptr;
    }
    std::shared_ptr<TextBlobImpl> textBlobImpl = std::make_shared<SkiaTextBlob>(skTextBlob);
    return std::make_shared<TextBlob>(textBlobImpl);
}

const TextBlobBuilder::RunBuffer& SkiaTextBlobBuilder::AllocRunPos(const Font& font, int count, const Rect* bounds)
{
    auto skiaFont = font.GetImpl<SkiaFont>();
    if (!skiaFont) {
        LOGD("skiaFont nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return drawingRunBuffer_;
    }
    if (!bounds) {
        const SkTextBlobBuilder::RunBuffer& skRunBuffer =
            skTextBlobBuilder_.allocRunPos(skiaFont->GetFont(), count, nullptr);
        SkiaConvertUtils::SkRunBufferCastToDrawingRunBuffer(skRunBuffer, drawingRunBuffer_);
        return drawingRunBuffer_;
    }
    SkRect skRect;
    SkiaConvertUtils::DrawingRectCastToSkRect(*bounds, skRect);
    const SkTextBlobBuilder::RunBuffer& skRunBuffer =
        skTextBlobBuilder_.allocRunPos(skiaFont->GetFont(), count, &skRect);
    SkiaConvertUtils::SkRunBufferCastToDrawingRunBuffer(skRunBuffer, drawingRunBuffer_);
    return drawingRunBuffer_;
}

const TextBlobBuilder::RunBuffer& SkiaTextBlobBuilder::AllocRunRSXform(const Font& font, int count)
{
    auto skiaFont = font.GetImpl<SkiaFont>();
    if (!skiaFont) {
        LOGD("skiaFont nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return drawingRunBuffer_;
    }
    const SkTextBlobBuilder::RunBuffer& skRunBuffer = skTextBlobBuilder_.allocRunRSXform(skiaFont->GetFont(), count);
    SkiaConvertUtils::SkRunBufferCastToDrawingRunBuffer(skRunBuffer, drawingRunBuffer_);
    return drawingRunBuffer_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS