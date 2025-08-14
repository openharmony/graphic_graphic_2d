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

#include "fontarguments_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "text/font_arguments.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
bool FontArgumentsFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    FontArguments fontArguments;
    int fontCollectionIndex = GetObject<int>();
    fontArguments.SetCollectionIndex(fontCollectionIndex);
    fontArguments.GetCollectionIndex();

    return true;
}

bool FontArgumentsFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    FontArguments fontArguments;
    uint32_t axis = GetObject<uint32_t>();
    float coValue = GetObject<float>();
    FontArguments::VariationPosition::Coordinate coordinate { axis, coValue };
    int coordinateCount = GetObject<int>();
    FontArguments::VariationPosition position { &coordinate, coordinateCount };
    fontArguments.SetVariationDesignPosition(position);
    fontArguments.GetVariationDesignPosition();
    return true;
}

bool FontArgumentsFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    FontArguments fontArguments;
    int index = GetObject<int>();
    uint32_t color = GetObject<uint32_t>();
    FontArguments::Palette::Override overrides { index, color };
    int paletteIndex = GetObject<int>();
    int overrideCount = GetObject<int>();
    FontArguments::Palette palette { paletteIndex, &overrides, overrideCount };
    fontArguments.SetPalette(palette);
    fontArguments.GetPalette();
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::Drawing::FontArgumentsFuzzTest001(data, size);
    OHOS::Rosen::Drawing::FontArgumentsFuzzTest002(data, size);
    OHOS::Rosen::Drawing::FontArgumentsFuzzTest003(data, size);
    return 0;
}