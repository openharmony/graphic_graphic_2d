/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
/*
 * 测试以下 Pen 接口：
 * 1. SetColor(...)
 * 2. GetColorSpace()
 * 3. GetColor4f()
 */
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

/*
 * 测试以下 Pen 接口：
 * 1. SetAlpha(...)
 * 2. SetAlphaF(...)
 * 3. GetAlpha()
 * 4. SetWidth(...)
 * 5. GetWidth()
 * 6. SetMiterLimit(...)
 * 7. GetMiterLimit()
 * 8. SetCapStyle(...)
 * 9. GetCapStyle()
 * 10. SetJoinStyle(...)
 * 11. GetJoinStyle()
 */
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

/*
 * 测试以下 Pen 接口：
 * 1. SetBlendMode(...)
 * 2. GetBlendMode()
 * 3. SetAntiAlias(...)
 * 4. IsAntiAlias()
 * 5. SetPathEffect(...)
 * 6. GetPathEffect()
 * 7. GetPathEffectPtr()
 * 8. SetFilter(...)
 * 9. GetFilter()
 * 10. SetShaderEffect(...)
 * 11. GetShaderEffect()
 * 12. GetShaderEffectPtr()
 * 13. Reset()
 */
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
    pen.GetPathEffectPtr();

    Filter filter;
    pen.SetFilter(filter);
    pen.GetFilter();

    ColorQuad colorQuad = GetObject<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(colorQuad);
    pen.SetShaderEffect(shaderEffect);
    pen.GetShaderEffect();
    pen.GetShaderEffectPtr();

    pen.Reset();
}

/*
 * 测试以下 Pen 接口：
 * 1. Pen()
 * 2. Pen(const Pen&)
 * 3. Pen(const Color&)
 * 4. Pen(int)
 * 5. GetColor()
 * 6. GetColorSpacePtr()
 * 7. GetAlphaF()
 * 8. GetRedF()
 * 9. GetGreenF()
 * 10. GetBlueF()
 * 11. SetBlender(...)
 * 12. GetBlender()
 * 13. GetBlenderPtr()
 * 14. HasFilter()
 * 15. SetLooper(...)
 * 16. GetLooper()
 * 17. GetFillPath(...)
 * 18. operator==
 * 19. operator!=
 */
void PenFuzzTestInner04(Pen& pen)
{
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color(alpha, red, blue, green);
    Pen PenOne = Pen(pen);
    Pen PenTwo = Pen(color);
    int rgba = GetObject<int>();
    Pen PenThree = Pen(rgba);
    PenTwo.GetColor();
    PenTwo.GetColorSpacePtr();
    PenTwo.GetAlphaF();
    PenTwo.GetRedF();
    PenTwo.GetGreenF();
    PenTwo.GetBlueF();
    BlendMode mode = GetObject<BlendMode>();
    std::shared_ptr<Blender> blender = Blender::CreateWithBlendMode(mode);
    PenTwo.SetBlender(blender);
    PenTwo.GetBlender();
    PenTwo.GetBlenderPtr();
    PenTwo.HasFilter();
    float blurRadius = GetObject<float>();
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    std::shared_ptr<BlurDrawLooper> blurDrawLooper = BlurDrawLooper::CreateBlurDrawLooper(blurRadius, dx, dy, color);
    PenTwo.SetLooper(blurDrawLooper);
    PenTwo.GetLooper();
    Path src;
    Path dst;
    Rect rect = GetObject<Rect>();
    Matrix matrix;
    PenTwo.GetFillPath(src, dst, &rect, matrix);

    if (PenOne == PenTwo) {}
    if (PenOne != PenTwo) {}
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
    PenFuzzTestInner04(pen);

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