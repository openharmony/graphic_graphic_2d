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

#include "include/core/SkFontArguments.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkRect.h"
#include "include/core/SkRSXform.h"
#include "include/core/SkSamplingOptions.h"
#include "include/core/SkString.h"
#include "include/core/SkTextBlob.h"
#ifndef USE_M133_SKIA
#include "src/gpu/GrPerfMonitorReporter.h"
#endif

#include "text/font_arguments.h"
#include "text/font_metrics.h"
#include "text/font_style.h"
#include "text/rs_xform.h"
#include "text/text_blob_builder.h"
#include "utils/perfevent.h"
#include "utils/rect.h"
#include "utils/sampling_options.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

#ifdef USE_M133_SKIA
struct SamplingOptionsUtils {
    int maxAniso = 0;
    bool useCubic = false;
    CubicResampler cubic = {0, 0};
    FilterMode filter = FilterMode::NEAREST;
    MipmapMode mipmap = MipmapMode::NONE;
};

inline void ConvertSamplingOptions(SamplingOptionsUtils& tempSamplingOptions, const SamplingOptions& sampling)
{
    tempSamplingOptions.useCubic = sampling.GetUseCubic();
    tempSamplingOptions.cubic.cubicCoffB = sampling.GetCubicCoffB();
    tempSamplingOptions.cubic.cubicCoffC = sampling.GetCubicCoffC();
    tempSamplingOptions.filter = sampling.GetFilterMode();
    tempSamplingOptions.mipmap = sampling.GetMipmapMode();
}
#endif

class SkiaConvertUtils {
public:
    static void DrawingFontMetricsCastToSkFontMetrics(const FontMetrics& fontMetrics, SkFontMetrics& skFontMetrics)
    {
        // Tell the compiler there is no alias and to select wider load/store instructions.
        uint32_t fFlags = fontMetrics.fFlags;
        scalar fTop = fontMetrics.fTop;
        scalar fAscent = fontMetrics.fAscent;
        scalar fDescent = fontMetrics.fDescent;
        scalar fBottom = fontMetrics.fBottom;
        scalar fLeading = fontMetrics.fLeading;
        scalar fAvgCharWidth = fontMetrics.fAvgCharWidth;
        scalar fMaxCharWidth = fontMetrics.fMaxCharWidth;
        scalar fXMin = fontMetrics.fXMin;
        scalar fXMax = fontMetrics.fXMax;
        scalar fXHeight = fontMetrics.fXHeight;
        scalar fCapHeight = fontMetrics.fCapHeight;
        scalar fUnderlineThickness = fontMetrics.fUnderlineThickness;
        scalar fUnderlinePosition = fontMetrics.fUnderlinePosition;
        scalar fStrikeoutThickness = fontMetrics.fStrikeoutThickness;
        scalar fStrikeoutPosition = fontMetrics.fStrikeoutPosition;

        skFontMetrics.fFlags = fFlags;
        skFontMetrics.fTop = fTop;
        skFontMetrics.fAscent = fAscent;
        skFontMetrics.fDescent = fDescent;
        skFontMetrics.fBottom = fBottom;
        skFontMetrics.fLeading = fLeading;
        skFontMetrics.fAvgCharWidth = fAvgCharWidth;
        skFontMetrics.fMaxCharWidth = fMaxCharWidth;
        skFontMetrics.fXMin = fXMin;
        skFontMetrics.fXMax = fXMax;
        skFontMetrics.fXHeight = fXHeight;
        skFontMetrics.fCapHeight = fCapHeight;
        skFontMetrics.fUnderlineThickness = fUnderlineThickness;
        skFontMetrics.fUnderlinePosition = fUnderlinePosition;
        skFontMetrics.fStrikeoutThickness = fStrikeoutThickness;
        skFontMetrics.fStrikeoutPosition = fStrikeoutPosition;
    }

    static void SkFontMetricsCastToDrawingFontMetrics(const SkFontMetrics& skFontMetrics, FontMetrics& fontMetrics)
    {
        // Tell the compiler there is no alias and to select wider load/store instructions.
        uint32_t fFlags = skFontMetrics.fFlags;
        scalar fTop = skFontMetrics.fTop;
        scalar fAscent = skFontMetrics.fAscent;
        scalar fDescent = skFontMetrics.fDescent;
        scalar fBottom = skFontMetrics.fBottom;
        scalar fLeading = skFontMetrics.fLeading;
        scalar fAvgCharWidth = skFontMetrics.fAvgCharWidth;
        scalar fMaxCharWidth = skFontMetrics.fMaxCharWidth;
        scalar fXMin = skFontMetrics.fXMin;
        scalar fXMax = skFontMetrics.fXMax;
        scalar fXHeight = skFontMetrics.fXHeight;
        scalar fCapHeight = skFontMetrics.fCapHeight;
        scalar fUnderlineThickness = skFontMetrics.fUnderlineThickness;
        scalar fUnderlinePosition = skFontMetrics.fUnderlinePosition;
        scalar fStrikeoutThickness = skFontMetrics.fStrikeoutThickness;
        scalar fStrikeoutPosition = skFontMetrics.fStrikeoutPosition;

        fontMetrics.fFlags = fFlags;
        fontMetrics.fTop = fTop;
        fontMetrics.fAscent = fAscent;
        fontMetrics.fDescent = fDescent;
        fontMetrics.fBottom = fBottom;
        fontMetrics.fLeading = fLeading;
        fontMetrics.fAvgCharWidth = fAvgCharWidth;
        fontMetrics.fMaxCharWidth = fMaxCharWidth;
        fontMetrics.fXMin = fXMin;
        fontMetrics.fXMax = fXMax;
        fontMetrics.fXHeight = fXHeight;
        fontMetrics.fCapHeight = fCapHeight;
        fontMetrics.fUnderlineThickness = fUnderlineThickness;
        fontMetrics.fUnderlinePosition = fUnderlinePosition;
        fontMetrics.fStrikeoutThickness = fStrikeoutThickness;
        fontMetrics.fStrikeoutPosition = fStrikeoutPosition;
    }

    static inline void DrawingRectCastToSkRect(const Rect& rect, SkRect& skRect)
    {
        scalar fLeft = rect.GetLeft();
        scalar fTop = rect.GetTop();
        scalar fRight = rect.GetRight();
        scalar fBottom = rect.GetBottom();
        skRect.fLeft = fLeft;
        skRect.fTop = fTop;
        skRect.fRight = fRight;
        skRect.fBottom = fBottom;
    }

    static inline void SkRectCastToDrawingRect(const SkRect& skRect, Rect& rect)
    {
        SkScalar fLeft = skRect.fLeft;
        SkScalar fTop = skRect.fTop;
        SkScalar fRight = skRect.fRight;
        SkScalar fBottom = skRect.fBottom;
        rect.SetLeft(fLeft);
        rect.SetTop(fTop);
        rect.SetRight(fRight);
        rect.SetBottom(fBottom);
    }

    static inline void SkRunBufferCastToDrawingRunBuffer(const SkTextBlobBuilder::RunBuffer& skRunBuffer,
        TextBlobBuilder::RunBuffer& runBuffer)
    {
        uint16_t* glyphs = skRunBuffer.glyphs;
        float* pos = skRunBuffer.pos;
        char* utf8text = skRunBuffer.utf8text;
        uint32_t* clusters = skRunBuffer.clusters;
        runBuffer.glyphs = glyphs;
        runBuffer.pos = pos;
        runBuffer.utf8text = utf8text;
        runBuffer.clusters = clusters;
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

    static void DrawingFontArgumentsCastToSkFontArguments(const FontArguments& fontArgs, SkFontArguments& skFontArgs)
    {
        skFontArgs.setCollectionIndex(fontArgs.GetCollectionIndex());

        SkFontArguments::VariationPosition pos;
        pos.coordinates = reinterpret_cast<const SkFontArguments::VariationPosition::Coordinate*>(
            fontArgs.GetVariationDesignPosition().coordinates);
        pos.coordinateCount = fontArgs.GetVariationDesignPosition().coordinateCount;
        skFontArgs.setVariationDesignPosition(pos);

        SkFontArguments::Palette pal;
        pal.overrides = reinterpret_cast<const SkFontArguments::Palette::Override*>(
            fontArgs.GetPalette().overrides);
        pal.index = fontArgs.GetPalette().index;
        pal.overrideCount = fontArgs.GetPalette().overrideCount;
        skFontArgs.setPalette(pal);
    }

#ifndef TODO_M133_SKIA
    static inline void DrawingTextureEventToRsTextureEvent(const TextureEvent& grTextureEvent,
        RsTextureEvent& rsTextureEvent)
    {
        rsTextureEvent.fPid = grTextureEvent.fPid;
        rsTextureEvent.fMaxBytes = grTextureEvent.fMaxBytes;
        rsTextureEvent.fBudgetedBytes = grTextureEvent.fBudgetedBytes;
        rsTextureEvent.fAllocTime = grTextureEvent.fAllocTime;
        rsTextureEvent.fClearCache = grTextureEvent.fClearCache;
    }

    static inline void DrawingBlurEventToRsBlurEvent(const BlurEvent& grBlurEvent, RsBlurEvent& rsBlurEvent)
    {
        rsBlurEvent.fPid = grBlurEvent.fPid;
        rsBlurEvent.fFilterType = grBlurEvent.fFilterType;
        rsBlurEvent.fBlurRadius = grBlurEvent.fBlurRadius;
        rsBlurEvent.fWidth = grBlurEvent.fWidth;
        rsBlurEvent.fHeight = grBlurEvent.fHeight;
        rsBlurEvent.fBlurTime = grBlurEvent.fBlurTime;
    }
#endif
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
