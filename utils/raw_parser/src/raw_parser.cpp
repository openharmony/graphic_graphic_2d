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
#include <gslogger.h>
#include <mutex>
#include <securec.h>
#include <sstream>

#include <zlib.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("RawParser");
} // namespace

int32_t RawParser::Parse(const std::string &file)
{
    int32_t ret = ReadFile(file, compressed);
    if (ret) {
        GSLOG2HI(ERROR) << "ReadFile failed with" << ret;
        return ret;
    }

    auto minfo = reinterpret_cast<struct RawHeaderInfo*>(&compressed[0]);
    if (strncmp(minfo->magic, "RAW.dif2", 0x8) != 0) {
        GSLOG2HI(ERROR) << "magic header mistake, is " << minfo->magic;
        return -1;
    }

    width = minfo->width;
    height = minfo->height;
    lastData = std::make_unique<uint8_t[]>(GetSize());

    struct RawFrameInfo *info = reinterpret_cast<struct RawFrameInfo *>(&compressed[magicHeaderLength]);
    uint32_t ipos = reinterpret_cast<uint8_t *>(info) - reinterpret_cast<uint8_t *>(minfo);
    while (ipos < clength) {
        GSLOG2HI(DEBUG) << info->type << ", " << info->offset << ", " << info->length << ", " << info->clen;
        if (info->clen < 0) {
            GSLOG2HI(ERROR) << "clen < 0";
            return -1;
        }

        struct RawFrameInfoPtr zi = { info->type, info->offset, info->length, info->clen, info->mem };
        infos.push_back(zi);

        // for BUS_ADRALN
        constexpr uint32_t memalign = 4;
        uint32_t align = info->clen - info->clen / memalign * memalign;
        if (align) {
            align = memalign - align;
        }
        info = reinterpret_cast<struct RawFrameInfo *>(info->mem + info->clen + align);
        ipos = reinterpret_cast<uint8_t *>(info) - reinterpret_cast<uint8_t *>(minfo);
    }

    if (infos.empty()) {
        GSLOG2HI(ERROR) << "infos is empty";
        return -1;
    }

    return 0;
}

int32_t RawParser::GetNextData(uint32_t *addr)
{
    uint32_t count = (lastID + 1) % infos.size();
    auto type = infos[count].type;
    if (type == RAW_HEADER_TYPE_NONE) {
        lastID = static_cast<int32_t>(count);
        return GetNowData(addr);
    }

    auto offset = infos[count].offset;
    auto length = infos[count].length;
    auto clen = infos[count].clen;
    if (type == RAW_HEADER_TYPE_COMPRESSED) {
        if (length == 0) {
            GSLOG2HI(INFO) << "length == 0";
            lastID = static_cast<int32_t>(count);
            return GetNowData(addr);
        }

        uncompressed = std::make_unique<uint8_t[]>(length);
        int32_t ret = Uncompress(uncompressed, length, infos[count].mem, clen);
        if (ret) {
            GSLOG2HI(ERROR) << "uncompress failed";
            return -1;
        }
    } else if (type == RAW_HEADER_TYPE_RAW) {
        uncompressed = std::make_unique<uint8_t[]>(length);
        if (memcpy_s(uncompressed.get(), length, infos[count].mem, clen)) {
            GSLOG2HI(ERROR) << "memcpy failed";
            return -1;
        }
    }

    lastID =  static_cast<int32_t>(count);
    if (length > 0 && memcpy_s(lastData.get() + offset, GetSize() - offset,
                               uncompressed.get(), length) != EOK) {
        GSLOG2HI(ERROR) << "memcpy failed";
        return -1;
    }

    return GetNowData(addr);
}

int32_t RawParser::GetNowData(uint32_t *addr)
{
    if (memcpy_s(addr, GetSize(), lastData.get(), GetSize()) != EOK) {
        GSLOG2HI(ERROR) << "memcpy failed";
        return -1;
    }
    return 0;
}

int32_t RawParser::ReadFile(const std::string &file, std::unique_ptr<uint8_t[]> &ptr)
{
    std::ifstream ifs(file, std::ifstream::in | std::ifstream::binary);
    if (!ifs.good()) {
        GSLOG2HI(ERROR) << "read file failed";
        return 1;
    }

    ifs.seekg(0, ifs.end);
    clength = static_cast<uint32_t>(ifs.tellg());
    ifs.seekg (0, ifs.beg);

    ptr = std::make_unique<uint8_t[]>(static_cast<unsigned int>(clength));
    ifs.read(reinterpret_cast<char *>(ptr.get()), clength);
    return 0;
}

int32_t RawParser::Uncompress(std::unique_ptr<uint8_t[]> &dst, uint32_t dstlen, uint8_t *cmem, uint32_t clen)
{
    unsigned long ulength = dstlen;
    auto ret = uncompress(dst.get(), &ulength, cmem, clen);
    if (ret) {
        GSLOG2HI(ERROR) << "uncompress failed";
    }
    return ret;
}
} // namespace OHOS
