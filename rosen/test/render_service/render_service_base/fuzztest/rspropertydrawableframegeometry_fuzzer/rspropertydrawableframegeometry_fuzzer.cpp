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

#include "rspropertydrawableframegeometry_fuzzer.h"

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
#include "property/rs_property_drawable_frame_geometry.h"

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

bool DoRSFrameGeometryDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    RSProperties properties;
    auto rsFrameGeometryDrawable = std::make_shared<RSFrameGeometryDrawable>();
    rsFrameGeometryDrawable->Draw(content, cacheCanvas);
    RSFrameGeometryDrawable::Generate(content);
    return true;
}

bool DoRSClipFrameDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    RSProperties properties;
    auto rsClipFrameDrawable = std::make_shared<RSClipFrameDrawable>();
    rsClipFrameDrawable->Draw(content, cacheCanvas);
    RSClipFrameDrawable::Generate(content);
    return true;
}

bool DoRSColorFilterDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Canvas canvas(width, height);
    RSPaintFilterCanvas cacheCanvas(&canvas);
    RSRenderContent content;
    RSProperties properties;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    Color c(r, g, b, a);
    Brush brush(c);
    auto rsColorFilterDrawable = std::make_shared<RSColorFilterDrawable>(std::move(brush));
    rsColorFilterDrawable->Draw(content, cacheCanvas);
    RSColorFilterDrawable::Generate(content);
    rsColorFilterDrawable->Update(content);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRSFrameGeometryDrawable(data, size); // RSFrameGeometryDrawable
    OHOS::Rosen::DoRSClipFrameDrawable(data, size);     // RSClipFrameDrawable
    OHOS::Rosen::DoRSColorFilterDrawable(data, size);   // RSColorFilterDrawable
    return 0;
}