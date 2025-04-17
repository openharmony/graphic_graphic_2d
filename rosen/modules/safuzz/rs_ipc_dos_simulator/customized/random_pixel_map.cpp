/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "customized/random_pixel_map.h"

#include <random>
#include "securec.h"

#include "common/safuzz_log.h"
#include "media_errors.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAXLENGTH = 4096;

constexpr uint32_t MAX_LENGTH_MODULO = 1024;
constexpr uint32_t PIXELFORMAT_MODULO = 8;
constexpr uint32_t ALPHATYPE_MODULO = 4;
constexpr uint32_t SCALEMODE_MODULO = 2;
constexpr uint32_t BOOL_MODULO = 2;
constexpr uint32_t DMA_WIDTH = 1024;
constexpr uint32_t DMA_HEIGHT = 512;

const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos = 0;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

uint8_t* GetRandomUInt8Array()
{
    uint8_t* data = new uint8_t[MAXLENGTH];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);
    for (uint32_t i = 0; i < MAXLENGTH; ++i) {
        *(data + i) = dis(gen);
    }
    return data;
}

Media::InitializationOptions GetInitialRandomOpts()
{
    int32_t width = GetData<uint16_t>() % (MAX_LENGTH_MODULO + (MAX_LENGTH_MODULO >> 3));
    int32_t height = GetData<uint16_t>() % (MAX_LENGTH_MODULO + (MAX_LENGTH_MODULO >> 3));
    Media::InitializationOptions opts;
    opts.useDMA = GetData<uint8_t>() % BOOL_MODULO;
    if (opts.useDMA) {
        width += DMA_WIDTH;
        height += DMA_HEIGHT;
    }
    opts.size.width = width;
    opts.size.height = height;
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>() % PIXELFORMAT_MODULO);
    opts.pixelFormat = opts.srcPixelFormat;
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>() % ALPHATYPE_MODULO);
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>() % SCALEMODE_MODULO);
    opts.editable = GetData<uint8_t>() % BOOL_MODULO;
    opts.useSourceIfMatch = GetData<uint8_t>() % BOOL_MODULO;
    return opts;
}

std::shared_ptr<Media::PixelMap> CreateFromOpt()
{
    Media::InitializationOptions opts = GetInitialRandomOpts();
    auto pixelmap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> ret = std::move(pixelmap);
    return ret;
}
} // namespace

std::shared_ptr<Media::PixelMap> RandomPixelMap::GetRandomPixelMap()
{
    static constexpr int MAX_RETRY_TIMES = 20;
    int retry = 0;
    std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
    while (pixelmap == nullptr) {
        if (++retry > MAX_RETRY_TIMES) {
            break;
        }
        pixelmap = GetRandomPixelMapFromOpt(GetRandomUInt8Array(), MAXLENGTH);
    }
    if (pixelmap == nullptr) {
        SAFUZZ_LOGE("RandomPixelMap::GetRandomPixelMap failed, return nullptr");
    }
    return pixelmap;
}

std::shared_ptr<Media::PixelMap> RandomPixelMap::GetRandomPixelMapFromOpt(uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return nullptr;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<Media::PixelMap> ret = CreateFromOpt();
    delete[] data;
    return ret;
}
} // namespace Rosen
} // namespace OHOS
