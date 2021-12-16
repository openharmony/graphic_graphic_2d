/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "raw_parser.h"

#include <cstdio>
#include <fstream>
#include <mutex>
#include <securec.h>
#include <sstream>

#include <zlib.h>

namespace OHOS {
struct MagicInfo {
    char magic[8];
    uint32_t width;
    uint32_t height;
};

struct HeaderInfo {
    uint32_t type;
    uint32_t offset;
    uint32_t length;
    uint32_t clen;
    uint8_t mem[0];
};

int32_t RawParser::Parse(const std::string &file)
{
    int32_t ret = ReadFile(file, compressed);
    if (ret) {
        return ret;
    }

    auto minfo = reinterpret_cast<struct MagicInfo*>(&compressed[0]);
    if (strncmp(minfo->magic, "RAW.diff", 0x8) != 0) {
        return -1;
    }

    width = minfo->width;
    height = minfo->height;
    lastData = std::make_unique<uint8_t[]>(GetSize());

    struct HeaderInfo *info = reinterpret_cast<struct HeaderInfo *>(&compressed[magicHeaderLength]);
    uint32_t ipos = reinterpret_cast<uint8_t *>(info) - reinterpret_cast<uint8_t *>(minfo);
    while (ipos < clength) {
        if (info->clen < 0) {
            return -1;
        }

        struct ZlibInfo zi = { info->type, info->offset, info->length, info->clen, info->mem };
        infos.push_back(zi);

        // for BUS_ADRALN
        constexpr uint32_t memalign = 4;
        uint32_t align = info->clen - info->clen / memalign * memalign;
        if (align) {
            align = memalign - align;
        }
        info = reinterpret_cast<struct HeaderInfo *>(info->mem + info->clen + align);
        ipos = reinterpret_cast<uint8_t *>(info) - reinterpret_cast<uint8_t *>(minfo);
    }

    if (infos.empty()) {
        return -1;
    }

    return 0;
}

int32_t RawParser::GetNextData(void* addr)
{
    uint32_t count = (lastID + 1) % infos.size();
    if (count >= infos.size()) {
        return -1;
    }

    auto offset = infos[count].offset;
    auto length = infos[count].length;
    if (length == 0) {
        return 0;
    }

    uncompressed = std::make_unique<uint8_t[]>(length);
    int32_t ret = Uncompress(uncompressed, length, infos[count].mem, infos[count].clen);
    if (ret) {
        return -1;
    }

    lastID = count;
    if (length > 0 && memcpy_s(lastData.get() + offset, GetSize() - offset,
             uncompressed.get(), length) != EOK) {
        return -1;
    }

    return GetNowData(addr);
}

int32_t RawParser::GetNowData(void* addr)
{
    if (memcpy_s(addr, GetSize(), lastData.get(), GetSize()) != EOK) {
        return -1;
    }
    return 0;
}

int32_t RawParser::ReadFile(const std::string &file, std::unique_ptr<uint8_t[]> &ptr)
{
    std::ifstream ifs(file, std::ifstream::in | std::ifstream::binary);
    if (!ifs.good()) {
        return 1;
    }

    ifs.seekg(0, ifs.end);
    clength = ifs.tellg();
    ifs.seekg (0, ifs.beg);

    ptr = std::make_unique<uint8_t[]>(clength);
    ifs.read(reinterpret_cast<char *>(ptr.get()), clength);
    return 0;
}

int32_t RawParser::Uncompress(std::unique_ptr<uint8_t[]> &dst, uint32_t dstlen, uint8_t *cmem, uint32_t clen)
{
    unsigned long ulength = dstlen;
    auto ret = uncompress(dst.get(), &ulength, cmem, clen);
    return ret;
}
} // namespace OHOS
