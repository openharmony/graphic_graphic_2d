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

#include "brush_fuzzer.h"

#include <stddef.h>
#include <stdint.h>

#include "get_object.h"

#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
bool BrushFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    Brush brush;

    Color color;
    int c = GetObject<int>();
    int r = GetObject<int>();
    int g = GetObject<int>();
    int b = GetObject<int>();
    int a = GetObject<int>();

    uint32_t cc = GetObject<uint32_t>();

    float color1 = GetObject<float>();
    float color2 = GetObject<float>();
    float color3 = GetObject<float>();
    float color4 = GetObject<float>();

    brush.GetColor();
    brush.SetColor(color);
    brush.SetColor(c);
    brush.SetARGB(r, g, b, a);
    Color4f color4f {
        color1,
        color2,
        color3,
        color4,
    };
    brush.GetColor4f();
    brush.GetColorSpace();
    std::shared_ptr<ColorSpace> s = ColorSpace::CreateSRGB();
    brush.SetColor(color4f, s);

    brush.GetAlpha();
    brush.SetAlpha(cc);
    scalar sca = GetObject<scalar>();
    brush.SetAlphaF(sca);
    brush.GetBlendMode();
    BlendMode mode = GetObject<BlendMode>();
    brush.SetBlendMode(mode);

    Filter filter;
    brush.SetFilter(filter);
    brush.GetFilter();

    // std::shared_ptr<ShaderEffect> e=std::make_shared<ShaderEffect>();
    // brush.SetShaderEffect(e);
    brush.IsAntiAlias();
    bool aa = GetObject<bool>();
    brush.SetAntiAlias(aa);

    brush.Reset();

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
