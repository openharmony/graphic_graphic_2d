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

#include "dynamicfontprovider_fuzzer.h"

#include "get_object.h"
#include "texgine/dynamic_font_provider.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
void DynamicFontProviderLoadFontFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DynamicFontProvider> dynamicFontProvider = DynamicFontProvider::Create();
    if (dynamicFontProvider == nullptr) {
        return;
    }
    std::string familyName = GetStringFromData();
    dynamicFontProvider->LoadFont(familyName, data, size);
    return;
}

void DynamicFontProviderMatchFamilyFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DynamicFontProvider> dynamicFontProvider = DynamicFontProvider::Create();
    if (dynamicFontProvider == nullptr) {
        return;
    }
    std::string familyName = GetStringFromData();
    dynamicFontProvider->MatchFamily(familyName);
    return;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::TextEngine::DynamicFontProviderLoadFontFuzzTest(data, size);
    OHOS::Rosen::TextEngine::DynamicFontProviderMatchFamilyFuzzTest(data, size);
    return 0;
}