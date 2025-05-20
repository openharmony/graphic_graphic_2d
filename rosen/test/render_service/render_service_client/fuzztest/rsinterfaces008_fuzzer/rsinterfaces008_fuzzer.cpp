/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rsinterfaces008_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_CREATE_PIXEL_MAP_FROM_SURFACE = 0;
const uint8_t DO_GET_SCREEN_HDR_CAPABILITY = 1;
const uint8_t DO_SET_PIXEL_FORMAT = 2;
const uint8_t DO_GET_PIXEL_FORMAT = 3;
const uint8_t DO_GET_SCREEN_SUPPORTED_HDR_FORMATS = 4;
const uint8_t DO_GET_SCREEN_HDR_FORMAT = 5;
const uint8_t DO_SET_SCREEN_HDR_FORMAT = 6;
const uint8_t DO_GET_SCREEN_SUPPORTED_COLORSPACES = 7;
const uint8_t DO_GET_SCREEN_COLORSPACE = 8;
const uint8_t DO_SET_SCREEN_COLORSPACE = 9;
const uint8_t DO_GET_SCREEN_TYPE = 10;
const uint8_t DO_SET_SCREEN_SKIP_FRAME_INTERVAL = 11;
const uint8_t DO_GET_BITMAP = 12;
const uint8_t DO_GET_PIXELMAP = 13;
const uint8_t TARGET_SIZE = 14;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

template<>
std::string GetData()
{
    size_t objectSize = GetData<uint8_t>();
    std::string object(objectSize, '\0');
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(DATA + g_pos), objectSize);
    g_pos += objectSize;
    return object;
}

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

namespace Mock {

} // namespace Mock

void DoCreatePixelMapFromSurface()
{}

void DoGetScreenHDRCapability()
{}

void DoSetPixelFormat()
{}

void DoGetPixelFormat()
{}

void DoGetScreenSupportedHDRFormats()
{}

void DoGetScreenHDRFormat()
{}

void DoSetScreenHDRFormat()
{}

void DoGetScreenSupportedColorSpaces()
{}

void DoGetScreenColorSpace()
{}

void DoSetScreenColorSpace()
{}

void DoGetScreenType()
{}

void DoSetScreenSkipFrameInterval()
{}

void DoGetBitmap()
{}

void DoGetPixelmap()
{}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE_PIXEL_MAP_FROM_SURFACE:
            OHOS::Rosen::DoCreatePixelMapFromSurface();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_CAPABILITY:
            OHOS::Rosen::DoGetScreenHDRCapability();
            break;
        case OHOS::Rosen::DO_SET_PIXEL_FORMAT:
            OHOS::Rosen::DoSetPixelFormat();
            break;
        case OHOS::Rosen::DO_GET_PIXEL_FORMAT:
            OHOS::Rosen::DoGetPixelFormat();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_HDR_FORMATS:
            OHOS::Rosen::DoGetScreenSupportedHDRFormats();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoGetScreenHDRFormat();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoSetScreenHDRFormat();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLORSPACES:
            OHOS::Rosen::DoGetScreenSupportedColorSpaces();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_COLORSPACE:
            OHOS::Rosen::DoGetScreenColorSpace();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_COLORSPACE:
            OHOS::Rosen::DoSetScreenColorSpace();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_TYPE:
            OHOS::Rosen::DoGetScreenType();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SKIP_FRAME_INTERVAL:
            OHOS::Rosen::DoSetScreenSkipFrameInterval();
            break;
        case OHOS::Rosen::DO_GET_BITMAP:
            OHOS::Rosen::DoGetBitmap();
            break;
        case OHOS::Rosen::DO_GET_PIXELMAP:
            OHOS::Rosen::DoGetPixelmap();
            break;
        default:
            return -1;
    }
    return 0;
}
