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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_ANY_SPAN_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_ANY_SPAN_H

#include "texgine_canvas.h"
#include "texgine/text_style.h"
#include "texgine/typography_types.h"
#include "texgine/utils/memory_object.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MAXRGB 255
#define MAXALPHA 255

/*
 * @brief AnySpanAlignment is the alignment of the AnySpan in a Typography.
 */
enum class AnySpanAlignment {
    OFFSET_AT_BASELINE,  // The bottom edge of the AnySpan is aligned with
                         // the offset position below the baseline.
    ABOVE_BASELINE,      // The bottom edge of the AnySpan is aligned with baseline.
    BELOW_BASELINE,      // The top edge of the AnySpan is aligned with baseline.
    TOP_OF_ROW_BOX,      // The top edge of the AnySpan is aligned with the
                         // top edge of the row box.
    BOTTOM_OF_ROW_BOX,   // The bottom edge of the AnySpan is aligned with the
                         // bottom edge of the row box.
    CENTER_OF_ROW_BOX,   // The vertical centerline of the AnySpan is aligned with the
                         // vertical centerline of the row box.
};

/*
 * @brief AnySpan is a span that can be any width, height and draw any content.
 */
class AnySpan : public MemoryObject {
public:
    virtual ~AnySpan() = default;

    /*
     * @brief Returns the width of the span.
     */
    virtual double GetWidth() const = 0;

    /*
     * @brief Returns the height of the span.
     */
    virtual double GetHeight() const = 0;

    /*
     * @brief Returns the alignment of the span.
     */
    virtual AnySpanAlignment GetAlignment() const = 0;

    /*
     * @brief Returns the baseline of the span.
     */
    virtual TextBaseline GetBaseline() const = 0;

    /*
     * @brief Returns the offset of the span.
     */
    virtual double GetLineOffset() const = 0;

    /*
     * @brief This method will be called when the Typography is drawn.
     * @param canvas Canvas to be drawn.
     * @param offsetx The Offset in x-asix of the starting point for drawing the AnySpan
     * @param offsety The Offset in y-asix of the starting point for drawing the AnySpan
     */
    virtual void Paint(TexgineCanvas& canvas, double offsetx, double offsety) = 0;

    void SetTextStyle(const TextStyle& style)
    {
        xs_ = style;
    }

    void SetRoundRectType(const RoundRectType type)
    {
        roundRectType_ = type;
    }

    void SetTopInGroup(const double top)
    {
        topInGroup_ = top;
    }

    double GetTopInGroup() const
    {
        return topInGroup_;
    }

    void SetBottomInGroup(const double bottom)
    {
        bottomInGroup_ = bottom;
    }

    double GetBottomInGroup() const
    {
        return bottomInGroup_;
    }

    void SetMaxRoundRectRadius(const double radius)
    {
        maxRoundRectRadius_ = radius;
    }

    double GetMaxRoundRectRadius() const
    {
        return maxRoundRectRadius_;
    }

private:
    friend void ReportMemoryUsage(const std::string& member, const AnySpan& that, const bool needThis);
    void ReportMemoryUsage(const std::string& member, const bool needThis) const override;

    double topInGroup_ = 0.0;
    double bottomInGroup_ = 0.0;
    double maxRoundRectRadius_ = 0.0;
    TextStyle xs_;
    RoundRectType roundRectType_ = RoundRectType::NONE;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_ANY_SPAN_H
