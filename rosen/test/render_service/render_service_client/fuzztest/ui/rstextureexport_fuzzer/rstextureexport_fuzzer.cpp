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

#include "rstextureexport_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_root_node.h"
#include "ui/rs_texture_export.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_RSTEXTURE_EXPORT = 0;
const uint8_t DO_DOTEXTURE_EXPORT = 1;
const uint8_t DO_STOPTEXTURE_EXPORT = 2;
const uint8_t TARGET_SIZE = 3;

const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

void DoRSTextureExport()
{
    SurfaceId surfaceId = GetData<SurfaceId>();
    bool isRenderServiceNode = GetData<bool>();
    std::shared_ptr<RSNode> node = RSRootNode::Create(isRenderServiceNode);
    RSTextureExport text(node, surfaceId);
}

void DoStopTextureExport()
{
    SurfaceId surfaceId = GetData<SurfaceId>();
    bool isRenderServiceNode = GetData<bool>();
    std::shared_ptr<RSNode> node = RSRootNode::Create(isRenderServiceNode);
    RSTextureExport text(node, surfaceId);
    text.StopTextureExport();
}

bool UpdateBufferInfo()
{
    // test
    SurfaceId surfaceId = GetData<SurfaceId>();
    bool isRenderServiceNode = GetData<bool>();
    float x = GetData<float>();
    float y = GetData<float>();
    float width = GetData<float>();
    float height = GetData<float>();
    std::shared_ptr<RSNode> node = RSRootNode::Create(isRenderServiceNode);
    RSTextureExport text(node, surfaceId);
    text.UpdateBufferInfo(x, y, width, height);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::DoRSTextureExport();
    OHOS::Rosen::UpdateBufferInfo();
    OHOS::Rosen::DoStopTextureExport();
    return 0;
}
