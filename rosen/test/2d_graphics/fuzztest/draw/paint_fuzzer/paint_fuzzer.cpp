/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "paint_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "draw/paint.h"
#include "draw/brush.h"
#include "draw/pen.h"
#include "effect/filter.h"
#include "get_object.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
/*
 * 测试以下 Paint 接口：
 * 1. Paint()
 * 2. Paint(const Paint&)
 * 3. Paint(const Color&, const ColorSpace&)
 * 4. CanCombinePaint(...)
 * 5. AttachBrush(...)
 * 6. AttachPen(...)
 * 7. SetStyle(...)
 * 8. GetStyle()
 * 9. IsValid()
 * 10. HasStrokeStyle()
 * 11. SetColor(...)
 * 12. SetARGB(...)
 * 13. SetColor(...)
 * 14. GetColor()
 * 15. GetColor4f()
 * 16. GetColorSpace()
 * 17. SetAlpha(...)
 * 18. SetAlphaF(...)
 * 19. GetAlpha()
 * 20. GetAlphaF()
 * 21. SetWidth(...)
 * 22. GetWidth()
 * 23. SetMiterLimit(...)
 * 24. GetMiterLimit()
 */
bool PaintFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Paint paint = Paint();
    Paint paintCopy = Paint(paint);
    Color color;
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    Paint paintColor = Paint(color, colorSpace);
    Paint::CanCombinePaint(paint, paintCopy);
    Brush brush;
    paint.AttachBrush(brush);
    Pen pen;
    paint.AttachPen(pen);

    uint32_t style = GetObject<uint32_t>();
    paint.SetStyle(static_cast<Paint::PaintStyle>(style));
    paint.GetStyle();
    paint.IsValid();
    paint.HasStrokeStyle();

    paint.SetColor(color);
    int a = GetObject<int>();
    int r = GetObject<int>();
    int g = GetObject<int>();
    int b = GetObject<int>();
    paint.SetARGB(a, r, g, b);
    Color4f cf {GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>()};
    paint.SetColor(cf, colorSpace);
    paint.GetColor();
    paint.GetColor4f();
    paint.GetColorSpace();

    paint.SetAlpha(a);
    scalar f = GetObject<scalar>();
    paint.SetAlphaF(f);
    paint.GetAlpha();
    paint.GetAlphaF();

    scalar width = GetObject<scalar>();
    paint.SetWidth(width);
    paint.GetWidth();
    scalar limit = GetObject<scalar>();
    paint.SetMiterLimit(limit);
    paint.GetMiterLimit();
    return true;
}

/*
 * 测试以下 Paint 接口：
 * 1. Paint()
 * 2. SetCapStyle(...)
 * 3. GetCapStyle()
 * 4. SetJoinStyle(...)
 * 5. GetJoinStyle()
 * 6. SetBlendMode(...)
 * 7. GetBlendMode()
 * 8. SetFilter(...)
 * 9. GetFilter()
 * 10. HasFilter()
 * 11. SetShaderEffect(...)
 * 12. GetShaderEffect()
 * 13. SetPathEffect(...)
 * 14. GetPathEffect()
 * 15. SetBlender(...)
 * 16. GetBlender()
 * 17. SetLooper(...)
 * 18. GetLooper()
 */
bool PaintFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Paint paint = Paint();
    uint32_t style = GetObject<uint32_t>();
    paint.SetCapStyle(static_cast<Pen::CapStyle>(style));
    paint.GetCapStyle();
    paint.SetJoinStyle(static_cast<Pen::JoinStyle>(style));
    paint.GetJoinStyle();
    uint32_t mode = GetObject<uint32_t>();
    paint.SetBlendMode(static_cast<BlendMode>(mode));
    paint.GetBlendMode();
    Filter filter;
    paint.SetFilter(filter);
    paint.GetFilter();
    paint.HasFilter();
    ColorQuad color = GetObject<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(color);
    paint.SetShaderEffect(shaderEffect);
    paint.GetShaderEffect();
    scalar radius = GetObject<scalar>();
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateCornerPathEffect(radius);
    paint.SetPathEffect(pathEffect);
    paint.GetPathEffect();

    std::shared_ptr<Blender> blender = Blender::CreateWithBlendMode(static_cast<BlendMode>(mode));
    paint.SetBlender(blender);
    paint.GetBlender();
    float blurRadius = GetObject<float>();
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    std::shared_ptr<BlurDrawLooper> blurDrawLooper = BlurDrawLooper::CreateBlurDrawLooper(blurRadius, dx, dy, color);
    paint.SetLooper(blurDrawLooper);
    paint.GetLooper();
    return true;
}

/*
 * 测试以下 Paint 接口：
 * 1. Paint()
 * 2. SetAntiAlias(...)
 * 3. IsAntiAlias()
 * 4. Reset()
 * 5. Disable()
 * 6. Paint(const Paint&)
 * 7. operator==
 * 8. operator!=
 */
bool PaintFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Paint paint = Paint();
    bool aa = GetObject<bool>();
    paint.SetAntiAlias(aa);
    paint.IsAntiAlias();
    paint.Reset();
    paint.Disable();
    Paint paintCopy = Paint(paint);
    if (paint == paintCopy) {}
    if (paint != paintCopy) {}
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PaintFuzzTest001(data, size);
    OHOS::Rosen::Drawing::PaintFuzzTest002(data, size);
    OHOS::Rosen::Drawing::PaintFuzzTest003(data, size);
    return 0;
}