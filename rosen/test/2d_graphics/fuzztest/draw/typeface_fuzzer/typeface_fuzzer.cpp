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

#include "typeface_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "text/typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

bool TypefaceFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();

    typeface->GetFamilyName();
    typeface->GetFontStyle();
    uint32_t tag = GetObject<uint32_t>();
    typeface->GetTableSize(tag);
    uint32_t tag2 = GetObject<uint32_t>();
    size_t offset = GetObject<size_t>();
    size_t length = GetObject<size_t>();
    typeface->GetTableData(tag2, offset, length, nullptr);
    typeface->GetItalic();
    typeface->GetUniqueID();
    typeface->GetUnitsPerEm();

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::TypefaceFuzzTest(data, size);
    return 0;
}
