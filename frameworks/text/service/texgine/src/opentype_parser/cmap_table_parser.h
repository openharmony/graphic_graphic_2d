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

#ifndef ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_CMAP_TABLE_PARSER_H
#define ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_CMAP_TABLE_PARSER_H

#include <cstdint>
#include <string>

#include "opentype_basic_type.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct EncodingRecord {
    OpenTypeBasicType::Uint16 platformID;
    OpenTypeBasicType::Uint16 encodingID;
    OpenTypeBasicType::Uint32 subtableOffset;
};

struct CmapTables {
    OpenTypeBasicType::Uint16 version;
    OpenTypeBasicType::Uint16 numTables;
    struct EncodingRecord encodingRecords[];
};

class CmapTableParser {
public:
    CmapTableParser() {}
    CmapTableParser(const char* data, int32_t size) : data_(data), size_(size) {}
    static const struct CmapTables* Parse(const char* data, int32_t size);
    void Dump() const;

private:
    const char* data_ = nullptr;
    int32_t size_ = 0;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_CMAP_TABLE_PARSER_H
