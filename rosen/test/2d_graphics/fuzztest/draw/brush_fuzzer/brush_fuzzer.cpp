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

#include "brush_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void BrushFuzzTestInner01(Brush& brush)
{
    float redF = GetObject<float>();
    float greeF = GetObject<float>();
    float blueF = GetObject<float>();
    float alphaF = GetObject<float>();
    uint32_t alpha1 = GetObject<uint32_t>();

    Color4f color4f {
        redF,
        greeF,
        blueF,
        alphaF,
    };
    brush.GetColor4f();
    brush.GetColorSpace();

    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    brush.SetColor(color4f, colorSpace);
    brush.SetAlpha(alpha1);
    brush.GetAlpha();
    scalar scalarG = GetObject<scalar>();
    brush.SetAlphaF(scalarG);
    brush.GetBlendMode();
}

void BrushFuzzTestInner02(Brush& brush)
{
    BlendMode mode = GetObject<BlendMode>();
    brush.SetBlendMode(mode);
    Filter filter;
    brush.SetFilter(filter);
    brush.GetFilter();
    brush.IsAntiAlias();
    bool isAntiAlias = GetObject<bool>();
    brush.SetAntiAlias(isAntiAlias);
    brush.Reset();
}

bool BrushFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Brush brush;
    Color color;
    int colorF = GetObject<int>();
    int red = GetObject<int>();
    int gree = GetObject<int>();
    int blue = GetObject<int>();
    int alpha = GetObject<int>();

    brush.SetColor(color);
    brush.SetColor(colorF);
    brush.GetColor();
    brush.SetARGB(red, gree, blue, alpha);

    BrushFuzzTestInner01(brush);

    BrushFuzzTestInner02(brush);

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::BrushFuzzTest(data, size);
    return 0;
}
