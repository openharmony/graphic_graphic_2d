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

#ifndef TEST_FUZZTEST_GRAPHICS_EFFECT_GET_OBJECT_H
#define TEST_FUZZTEST_GRAPHICS_EFFECT_GET_OBJECT_H

#include <cstdint>
#include <string>

#include "securec.h"

namespace OHOS {
namespace Rosen {
namespace GETest {

namespace {
    const int32_t STR_MAX_LEN = 1024;
}

static const uint8_t* g_data = nullptr;
static size_t g_size = 0;
static size_t g_pos = 0;

/*
 * describe: get plain old data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetPlainData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

/*
 * get a string from g_data
 */
std::string GetStringFromData(int strlen)
{
    if (strlen <= 0) {
        return "fuzz";
    }
    if (strlen > STR_MAX_LEN) {
        strlen = STR_MAX_LEN;
    }
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        char tmp = GetPlainData<char>();
        if (tmp == '\0') {
            tmp = '1';
        }
        cstr[i] = tmp;
    }
    std::string str(cstr);
    return str;
}

} // namespace GETest
} // namespace Rosen
} // namespace OHOS

#endif // GET_OBJECT_H
