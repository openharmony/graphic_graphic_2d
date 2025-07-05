/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "drawing_text_font_descriptor_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS::Rosen::Drawing {
void OHDrawingTextFontDescriptorTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OH_Drawing_Array* descs =
        OH_Drawing_GetSystemFontFullNamesByType(static_cast<OH_Drawing_SystemFontType>(fdp.ConsumeIntegral<uint8_t>()));
    size_t length = OH_Drawing_GetDrawingArraySize(descs);
    for (size_t i = 0; i < length; i++) {
        const OH_Drawing_String* fullName = OH_Drawing_GetSystemFontFullNameByIndex(descs, i);
        OH_Drawing_FontDescriptor* desc = OH_Drawing_GetFontDescriptorByFullName(
            fullName, static_cast<OH_Drawing_SystemFontType>(fdp.ConsumeIntegral<uint8_t>()));
        size_t count = 0;
        OH_Drawing_FontDescriptor* matched = OH_Drawing_MatchFontDescriptors(desc, &count);
        OH_Drawing_DestroyFontDescriptors(matched, count);
        OH_Drawing_DestroyFontDescriptor(desc);
    }
    OH_Drawing_DestroySystemFontFullNames(descs);
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHDrawingTextFontDescriptorTest(data, size);
    return 0;
}
} // namespace OHOS::Rosen::Drawing