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
#include <mutex>
#include <securec.h>

#include <zlib.h>

#include "util.h"

namespace OHOS {
sptr<RawParser> RawParser::instance = nullptr;
sptr<RawParser> RawParser::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new RawParser();
        }
    }

    return instance;
}

RawParser::RawParser() : infos()
{
}

RawParser::~RawParser()
{
}

int32_t RawParser::Parse(std::string &filename)
{
    int32_t ret = ReadFile(filename);
    if (ret) {
        LOG("ReadFile failed");
        return ret;
    }

    auto magic = reinterpret_cast<char *>(&compressed[0]);
    if (strstr(magic, "RAW.diff") == nullptr) {
        constexpr uint32_t magicHeaderStringLength = magicHeaderLength + 2;
        char fileMagic[magicHeaderStringLength];
        memcpy_s(fileMagic, sizeof(fileMagic) - 1, magic, magicHeaderLength);
        LOG("file magic is wrong, %{public}s", fileMagic);
        return -1;
    }

    struct HeaderInfo {
        uint32_t type;
        uint32_t offset;
        uint32_t length;
        uint32_t clen;
        uint8_t mem[0];
    };

    struct HeaderInfo *info = reinterpret_cast<struct HeaderInfo *>(&compressed[magicHeaderLength]);
    uint32_t ipos = reinterpret_cast<uint8_t *>(info) - reinterpret_cast<uint8_t *>(magic);
    while (ipos < clength) {
        LOG("[%{public}d, %{public}d], type: %{public}d, "
            "offset: %{public}d, length: %{public}d, clen: %{public}d, mem: %{public}p",
            infos.size() + 1, ipos, info->type, info->offset, info->length, info->clen, info->mem);

        if (info->clen < 0) {
            LOG("info->clen less then 0");
            return -1;
        }

        struct ZlibInfo zi = {
            .type = info->type,
            .offset = info->offset,
            .length = info->length,
            .clen = info->clen,
            .mem = info->mem,
        };
        infos.push_back(zi);

        // for BUS_ADRALN
        constexpr uint32_t memalign = 4;
        uint32_t align = info->clen - info->clen / memalign * memalign;
        if (align) {
            align = memalign - align;
        }
        info = reinterpret_cast<struct HeaderInfo *>(info->mem + info->clen + align);
        ipos = reinterpret_cast<uint8_t *>(info) - reinterpret_cast<uint8_t *>(magic);
    }

    if (infos.empty()) {
        LOG("file have no zip");
        return -1;
    }

    return 0;
}

int32_t RawParser::GetData(uint32_t count, uint8_t* &pData, uint32_t &offset, uint32_t &length)
{
    if (count >= infos.size()) {
        return -1;
    }

    offset = infos[count].offset;
    length = infos[count].length;
    if (length == 0) {
        pData = nullptr;
        return 0;
    }

    uncompressed = std::make_unique<uint8_t[]>(length);
    int32_t ret = Uncompress(uncompressed, length, infos[count].mem, infos[count].clen);
    if (ret) {
        return -1;
    }

    pData = uncompressed.get();
    return 0;
}

int32_t RawParser::ReadFile(std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if (fp == nullptr) {
        LOG("open %{public}s failed, %{public}d", filename.c_str(), errno);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    clength = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (clength < magicHeaderLength) {
        LOG("%{public}s is too small", filename.c_str());
        fclose(fp);
        return -1;
    }

    compressed = std::make_unique<uint8_t[]>(clength);
    fread(&compressed[0], sizeof(uint8_t), clength, fp);
    fclose(fp);

    LOG("compressed: %{public}p, length: %{public}d", compressed.get(), clength);
    return 0;
}

int32_t RawParser::Uncompress(std::unique_ptr<uint8_t[]> &dst, uint32_t dstlen, uint8_t *cmem, uint32_t clen)
{
    unsigned long ulength = dstlen;
    auto ret = uncompress(dst.get(), &ulength, cmem, clen);
    if (ret) {
        LOG("uncompress failed with %{public}d", ret);
    }
    return ret;
}
} // namespace OHOS
