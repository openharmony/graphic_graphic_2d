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

#include "rspropertydrawableutilities_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "draw/canvas.h"
#include "pipeline/rs_render_content.h"
#include "property/rs_property_drawable_utilities.h"

namespace OHOS {
namespace Rosen {

using namespace Drawing;
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

bool DoRSAliasDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint8_t value = 0;
    RSPropertyDrawableSlot slot = (RSPropertyDrawableSlot)value;
    auto rsAliasDrawable = std::make_shared<RSAliasDrawable>(slot);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    rsAliasDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoRSSaveDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t value = GetData<uint32_t>();
    auto content = std::make_shared<uint32_t>(value);
    auto rsSaveDrawable = std::make_shared<RSSaveDrawable>(content);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content1;
    rsSaveDrawable->Draw(content1, cacheCanvas);
    return true;
}

bool DoRSRestoreDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t value = GetData<uint32_t>();
    auto content = std::make_shared<uint32_t>(value);
    auto rsRestoreDrawable = std::make_shared<RSRestoreDrawable>(content);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content1;
    rsRestoreDrawable->Draw(content1, cacheCanvas);
    return true;
}

bool DoRSCustomSaveDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto content = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
    uint8_t value = GetData<uint8_t>();
    RSPaintFilterCanvas::SaveType type = (RSPaintFilterCanvas::SaveType)value;

    auto rsCustomSaveDrawable = std::make_shared<RSCustomSaveDrawable>(content, type);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content1;
    rsCustomSaveDrawable->Draw(content1, cacheCanvas);
    return true;
}

bool DoRSCustomRestoreDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto content = std::make_shared<RSPaintFilterCanvas::SaveStatus>();
    auto rsCustomRestoreDrawable = std::make_shared<RSCustomRestoreDrawable>(content);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content1;
    rsCustomRestoreDrawable->Draw(content1, cacheCanvas);
    return true;
}
bool DoRSAlphaDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float alpha = GetData<float>();
    auto rsAlphaDrawable = std::make_shared<RSAlphaDrawable>(alpha);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    rsAlphaDrawable->Draw(content, cacheCanvas);
    RSAlphaDrawable::Generate(content);
    return true;
}

bool DoRSAlphaOffscreenDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float alpha = GetData<float>();
    auto rsAlphaOffscreenDrawable = std::make_shared<RSAlphaOffscreenDrawable>(alpha);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    rsAlphaOffscreenDrawable->Draw(content, cacheCanvas);
    return true;
}

bool DoRSModifierDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int16_t value = GetData<int16_t>();
    RSModifierType type = (RSModifierType)value;
    auto rsModifierDrawable = std::make_shared<RSModifierDrawable>(type);

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    rsModifierDrawable->Draw(content, cacheCanvas);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRSAliasDrawable(data, size);          // RSAliasDrawable
    OHOS::Rosen::DoRSSaveDrawable(data, size);           // RSSaveDrawable
    OHOS::Rosen::DoRSRestoreDrawable(data, size);        // RSRestoreDrawable
    OHOS::Rosen::DoRSCustomSaveDrawable(data, size);     // RSCustomSaveDrawable
    OHOS::Rosen::DoRSCustomRestoreDrawable(data, size);  // RSCustomRestoreDrawable
    OHOS::Rosen::DoRSAlphaDrawable(data, size);          // RSAlphaDrawable  RSAlphaDrawable
    OHOS::Rosen::DoRSAlphaOffscreenDrawable(data, size); // RSAlphaOffscreenDrawable
    OHOS::Rosen::DoRSModifierDrawable(data, size);       // RSModifierDrawable
    return 0;
}
