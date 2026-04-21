/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "uicolor_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
/**
 * 该函数主要测试了 Color 的下列方法：
 * - SetRed(...)
 * - SetGreen(...)
 * - SetBlue(...)
 * - SetRgb(...)
* - SetAlpha(...)
 * - SetSetRgbF(...)
 * - GetAlpha()
 * - GetRed()
 * - GetGreen()
 * - GetBlue()
 */
bool UIColorFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float red = GetObject<float>();
    float green = GetObject<float>();
    float blue = GetObject<float>();
    float alpha = GetObject<float>();
    float headroom = GetObject<float>();

    UIColor uiColor;
    uiColor.SetRed(red);
    uiColor.SetGreen(green);
    uiColor.SetBlue(blue);
    uiColor.SetAlpha(alpha);
    uiColor.SetHeadroom(headroom);
    
    uiColor.SetRgb(red, green, blue);
    uiColor.SetRgba(red, green, blue, alpha);

    uiColor.GetAlpha();
    uiColor.GetRed();
    uiColor.GetGreen();
    uiColor.GetBlue();

    Brush brush;
    brush.SetUIColor(uiColor);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::Drawing::UIColorFuzzTest001(data, size);
    return 0;
}
