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

#include "texgine/any_span.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class MyAnySpan : public AnySpan {
public:
    MyAnySpan(double width, double height,
        AnySpanAlignment align = AnySpanAlignment::ABOVE_BASELINE,
        TextBaseline baseline = TextBaseline::ALPHABETIC,
        double offset = 0.0, uint32_t color = SK_ColorGRAY) : width_(width), height_(height),
        align_(align), baseline_(baseline), offset_(offset), color_(color) {}

    ~MyAnySpan() = default;

    double GetWidth() const override
    {
        return width_;
    }

    double GetHeight() const override
    {
        return height_;
    }

    AnySpanAlignment GetAlignment() const override
    {
        return align_;
    }

    TextBaseline GetBaseline() const override
    {
        return baseline_;
    }

    double GetLineOffset() const override
    {
        return offset_;
    }

    void Paint(TexgineCanvas &canvas, double offsetx, double offsety) override
    {
        TexginePaint paint;
        paint.SetColor(color_);
        paint.SetStyle(TexginePaint::FILL);
        auto rect = TexgineRect::MakeXYWH(offsetx, offsety, width_, height_);
        canvas.DrawRect(rect, paint);
    }

private:
    double width_ = 0.0;
    double height_ = 0.0;
    AnySpanAlignment align_ = AnySpanAlignment::ABOVE_BASELINE;
    TextBaseline baseline_ = TextBaseline::ALPHABETIC;
    double offset_ = 0.0;
    uint32_t color_ = SK_ColorGRAY;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
