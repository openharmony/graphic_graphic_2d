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

#include "picture_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "image/picture.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_ARRAY_SIZE = 5000;
} // namespace

namespace Drawing {
bool PictureFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Picture picture;
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_ARRAY_SIZE + 1;
    char* dataText = new char[length];
    if (dataText == nullptr) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    picture.Deserialize(dataVal);
    picture.Serialize();
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
    OHOS::Rosen::Drawing::PictureFuzzTest(data, size);
    return 0;
}