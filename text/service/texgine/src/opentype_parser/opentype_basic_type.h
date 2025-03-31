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

#ifndef ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_OPENTYPE_BASIC_TYPE_H
#define ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_OPENTYPE_BASIC_TYPE_H

#include <string>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
namespace OpenTypeBasicType {
struct Tag {
    char tags[4];
    const std::string Get() const;
};

struct Int16 {
    int16_t data;
    int16_t Get() const;
};

struct Uint16 {
    uint16_t data;
    uint16_t Get() const;
};

struct Int32 {
    int32_t data;
    int32_t Get() const;
};

struct Uint32 {
    uint32_t data;
    uint32_t Get() const;
};

struct Fixed {
    Uint32 data;
    float Get() const;
};
} // namespace OpenTypeBasicType
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_OPENTYPE_BASIC_TYPE_H
