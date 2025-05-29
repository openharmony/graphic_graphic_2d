/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_GRAPHIC_TEST_UTILS_H
#define RS_GRAPHIC_TEST_UTILS_H

#include "pixel_map.h"

#include <chrono>
#include <string>
#include <thread>

namespace OHOS {
namespace Rosen {
static constexpr int64_t SEC_TO_NANOSEC = 1000000000;
static constexpr int64_t UNIT_MS_TO_NS = 1000000;
static constexpr int64_t UNIT_SEC_TO_MS = 1000;
static constexpr int64_t ANIMATION_VSYNC_TIME_MS = 8;

using WriteToPngParam = struct {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t bitDepth;
    const uint8_t *data;
};

bool WriteToPngWithPixelMap(const std::string& fileName, OHOS::Media::PixelMap& pixelMap);
void WaitTimeout(int ms);

}
}
#endif // RS_GRAPHIC_TEST_UTILS_H
