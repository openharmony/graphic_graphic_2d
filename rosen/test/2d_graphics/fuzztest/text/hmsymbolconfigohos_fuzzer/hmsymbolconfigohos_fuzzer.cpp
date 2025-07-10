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

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "hmsymbolconfigohos_fuzzer.h"
#include "skia_txt/default_symbol_config.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t TYPE_SIZE = 9;
constexpr size_t SUBTYPE_SIZE = 2;
}
namespace Drawing {
bool HmSymbolConfigOhosFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t glyphId = GetObject<uint32_t>();
    OHOS::Rosen::Symbol::DefaultSymbolConfig::GetInstance()->GetSymbolLayersGroups(glyphId);
    return true;
}

bool HmSymbolConfigOhosFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t type = GetObject<uint32_t>();
    uint32_t commonSubType = GetObject<uint32_t>();
    uint16_t groupSum = GetObject<uint16_t>();
    uint16_t animationMode = GetObject<uint16_t>();
    OHOS::Rosen::Symbol::DefaultSymbolConfig::GetInstance()->GetGroupParameters(
        static_cast<DrawingAnimationType>(type % TYPE_SIZE), groupSum, animationMode,
        static_cast<DrawingCommonSubType>(commonSubType % SUBTYPE_SIZE));
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::HmSymbolConfigOhosFuzzTest001(data, size);
    OHOS::Rosen::Drawing::HmSymbolConfigOhosFuzzTest002(data, size);
    return 0;
}