/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "registertypeface_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "draw/typeface.h"
#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_REGISTER_TYPEFACE = 0;
const uint8_t DO_UNREGISTER_TYPEFACE = 1;
const uint8_t TARGET_SIZE = 2;
constexpr uint32_t STR_LEN = 10;

void DoRegisterTypeface(FuzzedDataProvider& fdp)
{
    // Test 1: nullptr to cover null pointer check
    std::shared_ptr<Drawing::Typeface> tf1 = nullptr;
    g_rsInterfaces->RegisterTypeface(tf1);

    // Test 2: MakeDefault() - creates typeface without dependencies
    std::shared_ptr<Drawing::Typeface> tf2 = Drawing::Typeface::MakeDefault();
    g_rsInterfaces->RegisterTypeface(tf2);

    // Test 3: MakeFromFile() - try with random string path (may fail, which is valid for fuzzer)
    std::string path1 = fdp.ConsumeRandomLengthString(STR_LEN);
    std::shared_ptr<Drawing::Typeface> tf3 = Drawing::Typeface::MakeFromFile(path1.c_str());
    if (tf3 != nullptr) { // Only call if construction succeeded
        g_rsInterfaces->RegisterTypeface(tf3);
    }

    // Test 4: MakeFromName() - random family name and style
    std::string familyName = fdp.ConsumeRandomLengthString(STR_LEN);
    auto fontStyle = static_cast<Drawing::FontStyle>(fdp.ConsumeIntegral<uint8_t>());
    std::shared_ptr<Drawing::Typeface> tf4 = Drawing::Typeface::MakeFromName(familyName.c_str(), fontStyle);
    if (tf4 != nullptr) { // Only call if construction succeeded
        g_rsInterfaces->RegisterTypeface(tf4);
    }
}

void DoUnRegisterTypeface(FuzzedDataProvider& fdp)
{
    uint32_t id = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->UnRegisterTypeface(id);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Initialize RSInterfaces
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_REGISTER_TYPEFACE:
            OHOS::Rosen::DoRegisterTypeface(fdp);
            break;
        case OHOS::Rosen::DO_UNREGISTER_TYPEFACE:
            OHOS::Rosen::DoUnRegisterTypeface(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
