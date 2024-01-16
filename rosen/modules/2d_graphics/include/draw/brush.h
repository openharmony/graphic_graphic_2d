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
#include "effect/blender.h"
#include "effect/filter.h"
#include "effect/shader_effect.h"
#include "utils/drawing_macros.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API Brush {
public:
    Brush() noexcept;
    Brush(const Brush& b) noexcept = default;
    Brush(const Color& c) noexcept;
    Brush(const std::shared_ptr<ShaderEffect> e) noexcept;
    Brush(int rgba) noexcept;

    ~Brush() {}
    const Color& GetColor() const;
    void SetColor(const Color& c);
    void SetColor(uint32_t c);
    void SetARGB(int a, int r, int g, int b);

    Color4f GetColor4f();
    std::shared_ptr<ColorSpace> GetColorSpace() const;
    void SetColor(const Color4f& cf, std::shared_ptr<ColorSpace> s);

    inline uint32_t GetAlpha() const
    {
        return color_.GetAlpha();
    }

    inline scalar GetAlphaF() const
    {
        return color_.GetAlphaF();
    }

    void SetAlpha(uint32_t a);
    void SetAlphaF(scalar a);

    const BlendMode& GetBlendMode() const { return blendMode_; }
    void SetBlendMode(const BlendMode& mode);

    void SetFilter(const Filter& filter);
    const Filter& GetFilter() const;
    bool HasFilter() const { return hasFilter_; }

    void SetShaderEffect(std::shared_ptr<ShaderEffect> e);
    std::shared_ptr<ShaderEffect> GetShaderEffect() const;

    void SetBlender(std::shared_ptr<Blender> blender);
    std::shared_ptr<Blender> GetBlender() const { return blender_; }

    bool IsAntiAlias() const;
    void SetAntiAlias(bool aa);

    bool CanComputeFastBounds();
    const Rect& ComputeFastBounds(const Rect& orig, Rect* storage);

    bool AsBlendMode();
    void Reset();

    friend DRAWING_API bool operator==(const Brush& b1, const Brush& b2);
    friend DRAWING_API bool operator!=(const Brush& b1, const Brush& b2);

private:
    Color color_;
    BlendMode blendMode_;
    Filter filter_;
    std::shared_ptr<ColorSpace> colorSpace_;
    std::shared_ptr<ShaderEffect> shaderEffect_;
    std::shared_ptr<Blender> blender_;

    bool antiAlias_;
    bool hasFilter_ = false;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif