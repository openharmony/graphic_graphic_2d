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

#include "pen_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "draw/pen.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void PenFuzzTestInner01(Pen& pen)
{
    scalar red = GetObject<scalar>();
    scalar gree = GetObject<scalar>();
    scalar blue = GetObject<scalar>();
    scalar alpha = GetObject<scalar>();
    Color4f color4f {
        red,
        gree,
        blue,
        alpha,
    };
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    pen.SetColor(color4f, colorSpace);
    pen.GetColorSpace();
    pen.GetColor4f();
}

void PenFuzzTestInner02(Pen& pen)
{
    uint32_t alpha1 = GetObject<uint32_t>();
    scalar alpha2 = GetObject<scalar>();
    pen.SetAlpha(alpha1);
    pen.SetAlphaF(alpha2);
    pen.GetAlpha();

    scalar width = GetObject<scalar>();
    pen.SetWidth(width);
    pen.GetWidth();

    scalar limit = GetObject<scalar>();
    pen.SetMiterLimit(limit);
    pen.GetMiterLimit();

    Pen::CapStyle capStyle = GetObject<Pen::CapStyle>();
    pen.SetCapStyle(capStyle);
    pen.GetCapStyle();

    Pen::JoinStyle joinStyle = GetObject<Pen::JoinStyle>();
    pen.SetJoinStyle(joinStyle);
    pen.GetJoinStyle();
}

void PenFuzzTestInner03(Pen& pen)
{
    BlendMode mode = GetObject<BlendMode>();
    pen.SetBlendMode(mode);
    pen.GetBlendMode();

    bool isAntiAlias = GetObject<bool>();
    pen.SetAntiAlias(isAntiAlias);
    pen.IsAntiAlias();

    scalar radius = GetObject<scalar>();
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateCornerPathEffect(radius);
    pen.SetPathEffect(pathEffect);
    pen.GetPathEffect();

    Filter filter;
    pen.SetFilter(filter);
    pen.GetFilter();

    ColorQuad colorQuad = GetObject<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(colorQuad);
    pen.SetShaderEffect(shaderEffect);
    pen.GetShaderEffect();

    pen.Reset();
}

bool PenFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Pen pen;
    Color color;

    int color1 = GetObject<int>();
    int red = GetObject<int>();
    int gree = GetObject<int>();
    int blue = GetObject<int>();
    int alpha = GetObject<int>();
    pen.SetColor(color);
    pen.SetColor(color1);
    pen.SetARGB(red, gree, blue, alpha);

    PenFuzzTestInner01(pen);
    PenFuzzTestInner02(pen);
    PenFuzzTestInner03(pen);

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PenFuzzTest(data, size);
    return 0;
}