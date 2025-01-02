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

#include "rsshadow_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "render/rs_shadow.h"

namespace OHOS {
namespace Rosen {

auto rsShadow = std::make_shared<RSShadow>();

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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
bool DoSetColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    RSColor color(r, g, b, a);
    rsShadow->SetColor(color);
    return true;
}
bool DoSetOffsetX(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float offsetX = GetData<float>();
    rsShadow->SetOffsetX(offsetX);
    return true;
}
bool DoSetOffsetY(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float offsetY = GetData<float>();
    rsShadow->SetOffsetY(offsetY);
    return true;
}
bool DoSetAlpha(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float alpha = GetData<float>();
    rsShadow->SetAlpha(alpha);
    return true;
}
bool DoSetElevation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float elevation = GetData<float>();
    rsShadow->SetElevation(elevation);
    return true;
}
bool DoSetRadius(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float radius = GetData<float>();
    rsShadow->SetRadius(radius);
    return true;
}
bool DoSetMask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    bool imageMask = GetData<bool>();
    rsShadow->SetMask(imageMask);
    return true;
}
bool DoSetIsFilled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    bool isFilled = GetData<bool>();
    rsShadow->SetIsFilled(isFilled);
    return true;
}
bool DoSetColorStrategy(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int colorStrategy = GetData<int>();
    rsShadow->SetColorStrategy(colorStrategy);
    return true;
}
} // namespace Rosen
} // namespace OHOS
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSetColor(data, size);
    OHOS::Rosen::DoSetOffsetX(data, size);
    OHOS::Rosen::DoSetOffsetY(data, size);
    OHOS::Rosen::DoSetAlpha(data, size);
    OHOS::Rosen::DoSetElevation(data, size);
    OHOS::Rosen::DoSetRadius(data, size);
    OHOS::Rosen::DoSetMask(data, size);
    OHOS::Rosen::DoSetIsFilled(data, size);
    OHOS::Rosen::DoSetColorStrategy(data, size);
    return 0;
}