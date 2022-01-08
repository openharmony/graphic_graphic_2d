/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef PEN_H
#define PEN_H

#include "draw/brush.h"
#include "draw/color.h"
#include "effect/filter.h"
#include "effect/path_effect.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Pen {
public:
    enum class JoinStyle {
        MITER_JOIN,
        ROUND_JOIN,
        BEVEL_JOIN,
    };

    enum class CapStyle {
        FLAT_CAP,
        SQUARE_CAP,
        ROUND_CAP,
    };

    Pen() noexcept;
    Pen(const Pen &p) noexcept = default;
    Pen(const Color &c) noexcept;
    Pen(int rgba) noexcept;
    ~Pen() {};

    Color GetColor() const;
    void SetColor(const Color &c);
    void SetColor(int c);
    void SetARGB(int r, int g, int b, int a);

    Color4f GetColor4f();
    std::shared_ptr<ColorSpace> GetColorSpace() const;
    void SetColor(const Color4f &cf, std::shared_ptr<ColorSpace> s);

    int GetAlpha() const;
    void SetAlpha(int a);
    void SetAlphaF(scalar a);

    scalar GetWidth() const;
    void SetWidth(scalar width);

    scalar GetMiterLimit() const;
    void SetMiterLimit(scalar limit);

    CapStyle GetCapStyle() const;
    void SetCapStyle(CapStyle cs);

    JoinStyle GetJoinStyle() const;
    void SetJoinStyle(JoinStyle js);

    BlendMode GetBlendMode() const;
    void SetBlendMode(BlendMode mode);

    bool IsAntiAlias() const;
    void SetAntiAlias(bool aa);

    void SetPathEffect(std::shared_ptr<PathEffect> e);
    std::shared_ptr<PathEffect> GetPathEffect() const;

    void SetFilter(const Filter &filter);
    Filter GetFilter() const;

    void SetShaderEffect(std::shared_ptr<ShaderEffect> e);
    std::shared_ptr<ShaderEffect> GetShaderEffect() const;

    void Reset();

    friend bool operator==(const Pen &p1, const Pen &p2);
    friend bool operator!=(const Pen &p1, const Pen &p2);
private:
    scalar width_;
    scalar miterLimit_;
    JoinStyle join_;
    CapStyle cap_;
    std::shared_ptr<PathEffect> pathEffect_;

    Brush brush_;
};
}
}
}
#endif