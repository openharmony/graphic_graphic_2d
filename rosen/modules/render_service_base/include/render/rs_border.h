/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BORDER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BORDER_H

#include <iostream>
#include <sstream>

#include "common/rs_color_palette.h"
#include "common/rs_vector4.h"
#include "property/rs_properties_def.h"
#include "platform/common/rs_log.h"

class SkPaint;

namespace OHOS {
namespace Rosen {
enum class BorderStyle : uint32_t {
    SOLID = 0,
    DASHED,
    DOTTED,
    NONE
};

class RSBorder final {
public:
    RSBorder() = default;
    ~RSBorder() {}

    enum BorderType : int {
        LEFT = 0,
        TOP,
        RIGHT,
        BOTTOM,
    };

    void SetColor(Color color);
    void SetWidth(float width);
    void SetStyle(BorderStyle style);
    Color GetColor(int idx = RSBorder::LEFT) const;
    float GetWidth(int idx = RSBorder::LEFT) const;
    BorderStyle GetStyle(int idx = RSBorder::LEFT) const;

    void SetColorFour(Vector4<Color> color);
    void SetWidthFour(Vector4f width);
    void SetStyleFour(Vector4<BorderStyle> style);
    Vector4<Color> GetColorFour() const;
    Vector4f GetWidthFour() const;
    Vector4<BorderStyle> GetStyleFour() const;

    bool HasBorder() const
    {
        return !colors_.empty() && !widths_.empty() && !styles_.empty();
    }

    std::string ToString() const;
    bool ApplyFillStyle(SkPaint& paint) const;
    bool ApplyPathStyle(SkPaint& paint) const;
    bool ApplyFourLine(SkPaint& paint) const;
    bool ApplyLineStyle(SkPaint& paint, int borderIdx, float length) const;
private:
    // Vectors containing uniform or four-sided border attributes.
    // If four-sided, the order of contents is left, top, right, bottom.
    std::vector<Color> colors_;
    std::vector<float> widths_;
    std::vector<BorderStyle> styles_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BORDER_H
