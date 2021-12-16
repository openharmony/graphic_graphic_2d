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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_RAW_PARSER_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_RAW_PARSER_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS {
enum {
    ZLIBINFO_TYPE_NONE,
    ZLIBINFO_TYPE_RAW,
    ZLIBINFO_TYPE_COMPRESSED,
};

struct ZlibInfo {
    uint32_t type;
    uint32_t offset;
    uint32_t length;
    uint32_t clen;
    uint8_t *mem;
};

class RawParser {
public:
    // 0 for success
    int32_t Parse(const std::string &file);

    uint32_t GetWidth() const
    {
        return width;
    }

    uint32_t GetHeight() const
    {
        return height;
    }

    uint32_t GetSize() const
    {
        return width * height * 0x4;
    }

    int32_t GetCount() const
    {
        return infos.size();
    }

    // 0 for success
    int32_t GetNextData(void *addr);
    int32_t GetNowData(void *addr);

private:
    int32_t ReadFile(const std::string &file, std::unique_ptr<uint8_t[]> &ptr);

    // 0 for success
    int32_t Uncompress(std::unique_ptr<uint8_t[]> &dst, uint32_t dstlen, uint8_t *cmem, uint32_t clen);

    std::unique_ptr<uint8_t[]> compressed = nullptr;
    uint32_t clength = 0;

    std::vector<struct ZlibInfo> infos;
    std::unique_ptr<uint8_t[]> uncompressed = nullptr;

    int32_t lastID = -1;
    std::unique_ptr<uint8_t[]> lastData = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;

    static constexpr int32_t magicHeaderLength = 16;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_RAW_PARSER_H
