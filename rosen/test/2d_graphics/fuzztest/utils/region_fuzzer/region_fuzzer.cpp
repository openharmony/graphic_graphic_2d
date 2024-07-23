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

#include "region_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/data.h"
#include "utils/region.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_SIZE = 5000;
constexpr size_t OPTYPE_SIZE = 6;
} // namespace
namespace Drawing {
bool RegionFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Region region = Region();
    Region regionOne = Region(region);
    region.Clone(regionOne);
    region.GetDrawingType();
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    region.Contains(x, y);
    int32_t left = GetObject<int32_t>();
    int32_t top = GetObject<int32_t>();
    int32_t right = GetObject<int32_t>();
    int32_t bottom = GetObject<int32_t>();
    RectI rect { left, top, right, bottom };
    region.SetRect(rect);
    Path path;
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* str = new char[count];
    for (size_t i = 0; i < count; i++) {
        str[i] =  GetObject<char>();
    }
    str[count - 1] = '\0';
    path.BuildFromSVGString(str);
    region.SetPath(path, regionOne);
    region.GetBoundaryPath(&path);
    region.IsIntersects(regionOne);
    region.IsEmpty();
    region.IsRect();
    region.IsRegionContained(regionOne);
    uint32_t op = GetObject<uint32_t>();
    region.Op(regionOne, static_cast<RegionOp>(op % OPTYPE_SIZE));
    region.QuickReject(rect);
    if (str != nullptr) {
        delete[] str;
        str = nullptr;
    }
    return true;
}

bool RegionFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Region region = Region();
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    region.Serialize();
    region.Deserialize(dataVal);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::RegionFuzzTest001(data, size);
    OHOS::Rosen::Drawing::RegionFuzzTest002(data, size);
    return 0;
}
