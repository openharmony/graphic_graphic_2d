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

#include "cmap_parser.h"

#include "texgine/utils/exlog.h"

#include "opentype_basic_type.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define FORMAT4 4
#define FORMAT12 12
#define SEG_SIZE 2
#define SEG_OFFSET 3
#define OFFSET 1

struct CmapSubtable {
    OpenTypeBasicType::Uint16 format;
};

namespace {
struct EncodingRecord {
    OpenTypeBasicType::Uint16 platformID;
    OpenTypeBasicType::Uint16 encodingID;
    OpenTypeBasicType::Uint32 subtableOffset;

    const struct CmapSubtable *GetSubtable(const void *data) const
    {
        return reinterpret_cast<const struct CmapSubtable *>(
            reinterpret_cast<const char *>(data) + subtableOffset.Get());
    }
};

struct CmapTable {
    OpenTypeBasicType::Uint16 version;
    OpenTypeBasicType::Uint16 numTables;
    struct EncodingRecord encodingRecords[];
};

struct CmapSubtableFormat4 {
    struct CmapSubtable base;
    OpenTypeBasicType::Uint16 length;
    OpenTypeBasicType::Uint16 language;
    OpenTypeBasicType::Uint16 segCountX2;
    OpenTypeBasicType::Uint16 searchRange;
    OpenTypeBasicType::Uint16 entrySelector;
    OpenTypeBasicType::Uint16 rangeShift;
    OpenTypeBasicType::Uint16 array[];

    int16_t GetSegCount() const
    {
        return segCountX2.Get() / SEG_SIZE;
    }

    const OpenTypeBasicType::Uint16 *GetEndCodes() const
    {
        return array;
    }

    const OpenTypeBasicType::Uint16 *GetStartCodes() const
    {
        return array + GetSegCount() + OFFSET;
    }

    const OpenTypeBasicType::Int16 *GetIdDeltas() const
    {
        return reinterpret_cast<const OpenTypeBasicType::Int16 *>(array + GetSegCount() * SEG_SIZE + OFFSET);
    }

    const OpenTypeBasicType::Uint16 *GetIdRangeOffsets() const
    {
        return array + GetSegCount() * SEG_OFFSET + OFFSET;
    }
};

struct SequentialMapGroup {
    OpenTypeBasicType::Uint32 startCharCode;
    OpenTypeBasicType::Uint32 endCharCode;
    OpenTypeBasicType::Uint32 startGlyphId;
};

struct CmapSubtableFormat12 {
    struct CmapSubtable base;
    OpenTypeBasicType::Uint16 reserved;
    OpenTypeBasicType::Uint32 length;
    OpenTypeBasicType::Uint32 language;
    OpenTypeBasicType::Uint32 numGroups;
    struct SequentialMapGroup groups[];
};
} // namespace

int CmapParser::Parse(const char *data, int32_t size)
{
    int ret = 1;
    if (data == nullptr) {
        return ret;
    }

    const auto &cmap = *reinterpret_cast<const struct CmapTable *>(data);
    for (auto i = 0; i < cmap.numTables.Get(); i++) {
        const auto &record = cmap.encodingRecords[i];
        const auto &subtable = *record.GetSubtable(data);
        if (subtable.format.Get() == FORMAT4) {
            auto offset = record.subtableOffset.Get();
            if (ret = ParseFormat4(subtable, size - offset); ret) {
                LOGSO_FUNC_LINE(WARN) << "ParseFormat4 failed with " << ret;
            } else {
                ret = 0;
            }
        }

        if (subtable.format.Get() == FORMAT12) {
            auto offset = record.subtableOffset.Get();
            if (ret = ParseFormat12(subtable, size - offset); ret) {
                LOGSO_FUNC_LINE(WARN) << "ParseFormat12 failed with " << ret;
            } else {
                ret = 0;
            }
        }
    }
    return ret;
}

int32_t CmapParser::GetGlyphId(int32_t codepoint) const
{
    return ranges_.GetGlyphId(codepoint);
}

void CmapParser::Dump() const
{
    ranges_.Dump();
}

int CmapParser::ParseFormat4(const CmapSubtable &subtable, const std::size_t size)
{
    const auto &subtable4 = *reinterpret_cast<const struct CmapSubtableFormat4 *>(&subtable);
    const auto &endCodes = subtable4.GetEndCodes();
    const auto &startCodes = subtable4.GetStartCodes();
    const auto &idDeltas = subtable4.GetIdDeltas();
    const auto &idRangeOffsets = subtable4.GetIdRangeOffsets();

    if (size < sizeof(CmapSubtableFormat4)) {
        return 1;
    }

    if (size < sizeof(CmapSubtableFormat4) + sizeof(uint16_t) * (subtable4.GetSegCount() * FORMAT4 + 1)) {
        return 1;
    }

    for (int16_t i = 0; i < subtable4.GetSegCount(); i++) {
        uint32_t end = endCodes[i].Get();
        uint32_t start = startCodes[i].Get();
        if (end < start) {
            return 1;
        }

        uint32_t idRangeOffset = idRangeOffsets[i].Get();
        if (idRangeOffset == 0) {
            int32_t delta = idDeltas[i].Get();
            ParseFormat4NoOffset(delta, start, end);
            continue;
        }
        for (uint32_t j = start; j <= end; j++) {
            const auto &gid = idRangeOffsets[idRangeOffset / 2 + i + j - start].Get();
            if (gid) {
                ranges_.AddRange({j, j + 1, static_cast<int32_t>(gid)});
            }
        }
    }
    return 0;
}

void CmapParser::ParseFormat4NoOffset(int32_t delta, uint32_t start, uint32_t end)
{
    if (((static_cast<uint32_t>(delta) + end) & 0xffff) > end - start) {
        ranges_.AddRange({start, end + 1, delta});
    } else {
        for (uint32_t j = start; j <= end; j++) {
            if ((j + static_cast<uint32_t>(delta)) & 0xffff) {
                ranges_.AddRange({j, j + 1, delta});
            }
        }
    }
}

int CmapParser::ParseFormat12(const CmapSubtable &subtable, const std::size_t size)
{
    const auto &subtable12 = *reinterpret_cast<const struct CmapSubtableFormat12 *>(&subtable);
    const auto &length = subtable12.length.Get();

    if (size < sizeof(CmapSubtableFormat12)) {
        return 1;
    }

    if (size < length) {
        return 1;
    }

    for (uint32_t i = 0; i < subtable12.numGroups.Get(); i++) {
        const auto &[scc, ecc, sgi] = subtable12.groups[i];
        int32_t delta = static_cast<int32_t>(scc.Get());
        delta -= static_cast<int32_t>(sgi.Get());
        ranges_.AddRange({scc.Get(), ecc.Get() + 1, delta});
    }
    return 0;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
