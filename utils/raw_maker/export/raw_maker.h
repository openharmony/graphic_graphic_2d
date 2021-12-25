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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_RAW_MAKER_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_RAW_MAKER_H

#include <raw_parser.h>

#include <memory>
#include <fstream>
#include <string>

namespace OHOS {
class RawMaker {
public:
    void SetFilename(const std::string &filename);
    void SetWidth(uint32_t width);
    void SetHeight(uint32_t height);
    void SetHeaderType(RawHeaderType type);
    int32_t WriteNextData(const uint8_t *addr);

private:
    int32_t DoFirstFrame();
    int32_t PrepareInNone();
    int32_t PrepareInRaw(const uint8_t *addr);
    int32_t PrepareInCompress(const uint8_t *addr);
    int32_t UpdateLastFrame(const uint8_t *addr);
    int32_t WriteData();
    void WriteInt32(int32_t integer);
    void CompareWithLastFrame(const uint8_t *addr);

    std::string filename = "";
    uint32_t width = 0;
    uint32_t height = 0;
    int32_t size = 0;
    RawHeaderType type = RAW_HEADER_TYPE_NONE;
    bool firstFrame = true;
    std::ofstream ofs;
    std::unique_ptr<uint8_t[]> lastFrame;
    std::unique_ptr<uint8_t[]> compressed;

    struct {
        RawHeaderType type;
        int32_t offset;
        int32_t length;
        int32_t compressedLength;
        const uint8_t *data;
    } writing;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_RAW_MAKER_H
