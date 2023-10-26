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

#ifndef TEXGINE_GET_OBJECT_H
#define TEXGINE_GET_OBJECT_H

#include <string>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
static const uint8_t* g_data = nullptr;
static size_t g_size = 0;
static size_t g_pos = 0;
constexpr int g_len = 10;

template<class T>
T GetObject()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    memcpy(&object, g_data + g_pos, objectSize);
    g_pos += objectSize;
    return object;
}

std::string GetStringFromData()
{
    char cstr[g_len];
    cstr[g_len - 1] = '\0';
    for (int i = 0; i < g_len - 1; i++) {
        cstr[i] = GetObject<char>();
    }
    std::string str(cstr);
    return str;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
#endif // TEXGINE_GET_OBJECT_H