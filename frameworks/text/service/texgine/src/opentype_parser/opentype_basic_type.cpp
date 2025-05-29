/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "opentype_basic_type.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
namespace OpenTypeBasicType {
#define MAX_NUM 65536.0

constexpr union {
    unsigned int i;
    unsigned char big;
} G_ENDIAN{1};

const std::string Tag::Get() const
{
    // 5 is the size of open type table tags length
    int size = 5;
    char tagsWithZero[size];
    tagsWithZero[0] = tags[0];  // 0 means array subscripts
    tagsWithZero[1] = tags[1];  // 1 means array subscripts
    tagsWithZero[2] = tags[2];  // 2 means array subscripts
    tagsWithZero[3] = tags[3];  // 3 means array subscripts
    tagsWithZero[4] = 0;  // 4 means array subscripts
    return tagsWithZero;
}

int16_t Int16::Get() const
{
    if (G_ENDIAN.big != '\0') {
        // 8 means offset
        return ((static_cast<uint16_t>(data) & 0x0000ff00) >> 8) | ((static_cast<uint16_t>(data) & 0x000000ff) << 8);
    } else {
        return data;
    }
}

uint16_t Uint16::Get() const
{
    if (G_ENDIAN.big != '\0') {
        return ((static_cast<uint32_t>(data) & 0x0000ff00) >> 8) | // 8 means offset
               ((static_cast<uint32_t>(data) & 0x000000ff) << 8); // 8 means offset
    } else {
        return data;
    }
}

int32_t Int32::Get() const
{
    if (G_ENDIAN.big != '\0') {
        return ((static_cast<uint32_t>(data) & 0xff000000) >> 24) |     // 24 means offset
            ((static_cast<uint32_t>(data) & 0x00ff0000) >> 8) |         // 8 means offset
            ((static_cast<uint32_t>(data) & 0x0000ff00) << 8) |         // 8 means offset
            ((static_cast<uint32_t>(data) & 0x000000ff) << 24);         // 24 means offset
    } else {
        return data;
    }
}

uint32_t Uint32::Get() const
{
    if (G_ENDIAN.big != '\0') {
        return ((data & 0xff000000) >> 24) | ((data & 0x00ff0000) >> 8) |   // 8 & 24 means offset
            ((data & 0x0000ff00) << 8) | ((data & 0x000000ff) << 24);
    } else {
        return data;
    }
}

float Fixed::Get() const
{
    return data.Get() / MAX_NUM;
}
} // namespace OpenTypeBasicType
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
