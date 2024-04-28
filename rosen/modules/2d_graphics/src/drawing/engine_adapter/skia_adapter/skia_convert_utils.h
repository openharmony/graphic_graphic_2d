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

#ifndef SKIA_CONVERT_UTILS_H
#define SKIA_CONVERT_UTILS_H

#include <memory>
#include <string>

#include "include/core/SkFontMetrics.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkRect.h"
#include "include/core/SkRSXform.h"
#include "include/core/SkSamplingOptions.h"
#include "include/core/SkString.h"
#include "include/core/SkTextBlob.h"

#include "text/font_metrics.h"
#include "text/font_style.h"
#include "text/rs_xform.h"
#include "text/text_blob_builder.h"
#include "utils/rect.h"
#include "utils/sampling_options.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaConvertUtils {
public:
    static void DrawingFontMetricsCastToSkFontMetrics(const FontMetrics& fontMetrics, SkFontMetrics& skFontMetrics)
    {
        skFontMetrics.fFlags = fontMetrics.fFlags;
        skFontMetrics.fTop = fontMetrics.fTop;
        skFontMetrics.fAscent = fontMetrics.fAscent;
        skFontMetrics.fDescent = fontMetrics.fDescent;
        skFontMetrics.fBottom = fontMetrics.fBottom;
        skFontMetrics.fLeading = fontMetrics.fLeading;
        skFontMetrics.fAvgCharWidth = fontMetrics.fAvgCharWidth;
        skFontMetrics.fMaxCharWidth = fontMetrics.fMaxCharWidth;
        skFontMetrics.fXMin = fontMetrics.fXMin;
        skFontMetrics.fXMax = fontMetrics.fXMax;
        skFontMetrics.fXHeight = fontMetrics.fXHeight;
        skFontMetrics.fCapHeight = fontMetrics.fCapHeight;
        skFontMetrics.fUnderlineThickness = fontMetrics.fUnderlineThickness;
        skFontMetrics.fUnderlinePosition = fontMetrics.fUnderlinePosition;
        skFontMetrics.fStrikeoutThickness = fontMetrics.fStrikeoutThickness;
        skFontMetrics.fStrikeoutPosition = fontMetrics.fStrikeoutPosition;
    }

    static void SkFontMetricsCastToDrawingFontMetrics(const SkFontMetrics& skFontMetrics, FontMetrics& fontMetrics)
    {
        fontMetrics.fFlags = skFontMetrics.fFlags;
        fontMetrics.fTop = skFontMetrics.fTop;
        fontMetrics.fAscent = skFontMetrics.fAscent;
        fontMetrics.fDescent = skFontMetrics.fDescent;
        fontMetrics.fBottom = skFontMetrics.fBottom;
        fontMetrics.fLeading = skFontMetrics.fLeading;
        fontMetrics.fAvgCharWidth = skFontMetrics.fAvgCharWidth;
        fontMetrics.fMaxCharWidth = skFontMetrics.fMaxCharWidth;
        fontMetrics.fXMin = skFontMetrics.fXMin;
        fontMetrics.fXMax = skFontMetrics.fXMax;
        fontMetrics.fXHeight = skFontMetrics.fXHeight;
        fontMetrics.fCapHeight = skFontMetrics.fCapHeight;
        fontMetrics.fUnderlineThickness = skFontMetrics.fUnderlineThickness;
        fontMetrics.fUnderlinePosition = skFontMetrics.fUnderlinePosition;
        fontMetrics.fStrikeoutThickness = skFontMetrics.fStrikeoutThickness;
        fontMetrics.fStrikeoutPosition = skFontMetrics.fStrikeoutPosition;
    }

    static inline void DrawingRectCastToSkRect(const Rect& rect, SkRect& skRect)
    {
        skRect.fLeft = rect.GetLeft();
        skRect.fTop = rect.GetTop();
        skRect.fRight = rect.GetRight();
        skRect.fBottom = rect.GetBottom();
    }

    static inline void SkRectCastToDrawingRect(const SkRect& skRect, Rect& rect)
    {
        rect.SetLeft(skRect.fLeft);
        rect.SetTop(skRect.fTop);
        rect.SetRight(skRect.fRight);
        rect.SetBottom(skRect.fBottom);
    }

    static inline void SkRunBufferCastToDrawingRunBuffer(const SkTextBlobBuilder::RunBuffer& skRunBuffer,
        TextBlobBuilder::RunBuffer& runBuffer)
    {
        runBuffer.glyphs = skRunBuffer.glyphs;
        runBuffer.pos = skRunBuffer.pos;
        runBuffer.utf8text = skRunBuffer.utf8text;
        runBuffer.clusters = skRunBuffer.clusters;
    }

    static inline void DrawingFontStyleCastToSkFontStyle(const FontStyle& fontStyle, SkFontStyle& skFontStyle)
    {
        skFontStyle = SkFontStyle{fontStyle.GetWeight(), fontStyle.GetWidth(),
            static_cast<SkFontStyle::Slant>(fontStyle.GetSlant())};
    }

    static inline void SkFontStyleCastToDrawingFontStyle(const SkFontStyle& skFontStyle, FontStyle& fontStyle)
    {
        fontStyle = FontStyle{skFontStyle.weight(), skFontStyle.width(),
            static_cast<FontStyle::Slant>(skFontStyle.slant())};
    }

    static inline void StdStringCastToSkString(const std::string& str, SkString& skStr)
    {
        skStr = str.c_str();
    }

    static inline void SkStringCastToStdString(const SkString& skStr, std::string& str)
    {
        str = skStr.c_str();
    }

    static inline void DrawingRSXformCastToSkXform(const RSXform& xform, SkRSXform& skXform)
    {
        skXform.fSCos = xform.cos_;
        skXform.fSSin = xform.sin_;
        skXform.fTx = xform.tx_;
        skXform.fTy = xform.ty_;
    }

    static inline void DrawingSamplingCastToSkSampling(const SamplingOptions& sampling, SkSamplingOptions& skSampling)
    {
        if (sampling.GetUseCubic()) {
            skSampling = SkSamplingOptions({ sampling.GetCubicCoffB(), sampling.GetCubicCoffC() });
        } else {
            skSampling = SkSamplingOptions(static_cast<SkFilterMode>(sampling.GetFilterMode()),
                static_cast<SkMipmapMode>(sampling.GetMipmapMode()));
        }
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif