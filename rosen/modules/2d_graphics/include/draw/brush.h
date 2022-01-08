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

#ifndef BRUSH_H
#define BRUSH_H

#include "draw/color.h"
#include "effect/color_space.h"
#include "effect/filter.h"
#include "effect/shader_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Brush {
public:
    Brush() noexcept;
    Brush(const Brush &b) noexcept = default;
    Brush(const Color &c) noexcept;
    Brush(const std::shared_ptr<ShaderEffect> e) noexcept;
    Brush(int rgba) noexcept;

    ~Brush() {}
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

    BlendMode GetBlendMode() const;
    void SetBlendMode(BlendMode mode);

    void SetFilter(const Filter &filter);
    Filter GetFilter() const;

    void SetShaderEffect(std::shared_ptr<ShaderEffect> e);
    std::shared_ptr<ShaderEffect> GetShaderEffect() const;

    bool IsAntiAlias() const;
    void SetAntiAlias(bool aa);

    void Reset();

    friend bool operator==(const Brush &b1, const Brush &b2);
    friend bool operator!=(const Brush &b1, const Brush &b2);

private:
    Color color_;
    BlendMode blendMode_;
    Filter filter_;
    std::shared_ptr<ColorSpace> colorSpace_;
    std::shared_ptr<ShaderEffect> shaderEffect_;

    bool antiAlias_;
};
}
}
}
#endif