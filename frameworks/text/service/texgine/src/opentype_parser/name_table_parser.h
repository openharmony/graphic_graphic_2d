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

#ifndef ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_NAME_TABLE_PARSER_H
#define ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_NAME_TABLE_PARSER_H

#include <cstdint>
#include <string>

#include "opentype_basic_type.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct NameRecord {
    OpenTypeBasicType::Uint16 platformId;
    OpenTypeBasicType::Uint16 encodingId;
    OpenTypeBasicType::Uint16 languageId;
    OpenTypeBasicType::Uint16 nameId;
    OpenTypeBasicType::Uint16 length;
    OpenTypeBasicType::Uint16 stringOffset;
};

struct NameTable {
    OpenTypeBasicType::Uint16 version;
    OpenTypeBasicType::Uint16 count;
    OpenTypeBasicType::Uint16 storageOffset;
    struct NameRecord nameRecord[];
};

class NameTableParser {
public:
    NameTableParser(const char* data, int32_t size) : data_(data), size_(size) {}
    static const struct NameTable* Parse(const char* data, int32_t size);
    void Dump() const;

private:
    const char* data_ = nullptr;
    int32_t size_ = 0;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_NAME_TABLE_PARSER_H
