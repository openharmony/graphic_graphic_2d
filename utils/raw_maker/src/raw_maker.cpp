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

#include "raw_maker.h"

#include <cassert>
#include <cerrno>
#include <string>

#include <gslogger.h>
#include <securec.h>
#include <zlib.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("RawMaker");
} // namespace

void RawMaker::SetFilename(const std::string &filename)
{
    if (firstFrame) {
        this->filename = filename;
    }
    assert(this->height == height);
}

void RawMaker::SetWidth(uint32_t width)
{
    if (firstFrame) {
        this->width = width;
    }
    assert(this->width == width);
    size = width * height * 0x4;
}

void RawMaker::SetHeight(uint32_t height)
{
    if (firstFrame) {
        this->height = height;
    }
    assert(this->height == height);
    size = width * height * 0x4;
}

void RawMaker::SetHeaderType(RawHeaderType type)
{
    this->type = type;
    assert(type != RAW_HEADER_TYPE_NONE);
}

int32_t RawMaker::WriteNextData(const uint8_t *addr)
{
    int32_t ret = 0;
    if (firstFrame) {
        ret = DoFirstFrame();
        writing.offset = 0;
        writing.length = size;
    } else {
        CompareWithLastFrame(addr);
    }

    ofs.open(filename, std::ofstream::app | std::ofstream::binary | std::ofstream::out);
    if (errno) {
        GSLOG2HI(ERROR) << "open " << filename << ", because " << strerror(errno);
        return errno;
    }

    if (writing.length == 0) {
        ret = PrepareInNone();
    } else if (type == RAW_HEADER_TYPE_RAW) {
        ret = PrepareInRaw(addr);
    } else if (type == RAW_HEADER_TYPE_COMPRESSED) {
        ret = PrepareInCompress(addr);
    }

    if (ret) {
        GSLOG2SO(ERROR) << "failed at prepare";
        ofs.close();
        return ret;
    }

    WriteData();
    ofs.close();
    return 0;
}

int32_t RawMaker::PrepareInNone()
{
    writing.type = RAW_HEADER_TYPE_NONE;
    writing.offset = 0;
    writing.length = 0;
    writing.compressedLength = 0;
    writing.data = nullptr;
    return 0;
}

int32_t RawMaker::PrepareInRaw(const uint8_t *addr)
{
    writing.type = RAW_HEADER_TYPE_RAW;
    writing.compressedLength = writing.length;
    writing.data = addr + writing.offset;
    return UpdateLastFrame(addr);
}

int32_t RawMaker::PrepareInCompress(const uint8_t *addr)
{
    writing.type = RAW_HEADER_TYPE_COMPRESSED;

    uLongf clen = compressBound(writing.length);
    compressed = std::make_unique<uint8_t[]>(clen);
    auto ret = compress(compressed.get(), &clen, addr + writing.offset, writing.length);
    if (ret != Z_OK) {
        GSLOG2HI(ERROR) << "compress failed with " << ret;
        return ret;
    }

    writing.compressedLength = clen;
    writing.data = compressed.get();
    return UpdateLastFrame(addr);
}

int32_t RawMaker::UpdateLastFrame(const uint8_t *addr)
{
    auto dst = lastFrame.get() + writing.offset;
    auto dstlen = size - writing.offset;
    auto src = writing.data;
    auto srclen = writing.length;
    auto ret = memcpy_s(dst, dstlen, src, srclen);
    if (ret) {
        GSLOG2HI(ERROR) << "memcpy_s failed with <" << strerror(errno) << ">"
            << ", params: " << "dstlen(" << dstlen << "), srclen(" << srclen << ")";
        return ret;
    }
    return 0;
}

int32_t RawMaker::WriteData()
{
    WriteInt32(writing.type);
    WriteInt32(writing.offset);
    WriteInt32(writing.length);
    WriteInt32(writing.compressedLength);
    ofs.write(reinterpret_cast<const char *>(writing.data), writing.compressedLength);

    int32_t align = writing.compressedLength % 0x4;
    while (align) {
        align = (align + 1) % 0x4;
        ofs.write("\0", 1);
    }
    return 0;
}

void RawMaker::WriteInt32(int32_t integer)
{
    ofs.write(reinterpret_cast<const char *>(&integer), sizeof(integer));
}

void RawMaker::CompareWithLastFrame(const uint8_t *addr)
{
    writing.length = 0;
    writing.offset = size;
    for (int32_t i = 0; i < size; i++) {
        if (addr[i] != lastFrame[i]) {
            writing.offset = i;
            break;
        }
    }

    for (int32_t i = size - 1; i >= writing.offset; i++) {
        if (addr[i] != lastFrame[i]) {
            writing.length = i - writing.offset + 1;
            break;
        }
    }
}

int32_t RawMaker::DoFirstFrame()
{
    firstFrame = false;
    ofs.open(filename, std::ofstream::trunc | std::ofstream::binary | std::ofstream::out);
    if (errno) {
        GSLOG2HI(ERROR) << "open " << filename << ", because " << strerror(errno);
        return errno;
    }

    lastFrame = std::make_unique<uint8_t[]>(size);
    ofs.write("RAW.dif2", 0x8);
    WriteInt32(width);
    WriteInt32(height);
    ofs.close();
    return 0;
}
} // namespace OHOS
