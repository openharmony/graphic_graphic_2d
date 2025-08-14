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

#include "size_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/size.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void SizeFuzzTest000(const uint8_t* data, size_t size)
{
    scalar w = GetObject<scalar>();
    scalar h = GetObject<scalar>();

    SizeF sizef;
    SizeF sizef1 = SizeF(w, h);
    SizeF sizef2 = SizeF(sizef1);

    sizef.IsZero();
    sizef.IsEmpty();
    sizef.SetWidth(w);
    sizef.SetHeight(h);
    sizef.Width();
    sizef.Height();
    if (sizef1 == sizef2) {}
    if (sizef1 != sizef2) {}
}

void SizeFuzzTest001(const uint8_t* data, size_t size)
{
    int w = GetObject<int>();
    int h = GetObject<int>();

    SizeI s;
    SizeI s1 = SizeI(w, h);
    SizeI s2 = SizeI(s1);

    s.IsZero();
    s.IsEmpty();
    s.SetWidth(w);
    s.SetHeight(h);
    s.Width();
    s.Height();
    if (s1 == s2) {}
    if (s1 != s2) {}
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
    OHOS::Rosen::Drawing::SizeFuzzTest000(data, size);
    OHOS::Rosen::Drawing::SizeFuzzTest001(data, size);

    return 0;
}