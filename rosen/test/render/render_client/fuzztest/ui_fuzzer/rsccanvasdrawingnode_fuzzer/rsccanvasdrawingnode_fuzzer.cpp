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

#include "rsccanvasdrawingnode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_canvas_drawing_node.h"
#include "command/rs_canvas_drawing_node_command.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_canvas_render_node.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

bool DoRSCanvasDrawingNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
    return true;
}

bool DoCreate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSCanvasDrawingNode::SharedPtr node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
    return true;
}

bool DoGetBitmap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
    Drawing::Bitmap bitmap;
    int w = GetData<int>();
    int h = GetData<int>();
    auto list = std::make_shared<Drawing::DrawCmdList>(w, h);
    float fLeft = GetData<float>();
    float fTop = GetData<float>();
    float fRight = GetData<float>();
    float fBottom = GetData<float>();
    Drawing::Rect rect { fLeft, fTop, fRight, fBottom };
    canvasDrawingNode.GetBitmap(bitmap, list, &rect);
    return true;
}

bool DoGetPixelmap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
    std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
    int w = GetData<int>();
    int h = GetData<int>();
    auto list = std::make_shared<Drawing::DrawCmdList>(w, h);
    float fLeft = GetData<float>();
    float fTop = GetData<float>();
    float fRight = GetData<float>();
    float fBottom = GetData<float>();
    Drawing::Rect rect { fLeft, fTop, fRight, fBottom };
    canvasDrawingNode.GetPixelmap(pixelmap, list, &rect);
    return true;
}

bool DoResetSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
    int w = GetData<int>();
    int h = GetData<int>();
    canvasDrawingNode.ResetSurface(w, h);
    return true;
}

bool DoCreateTextureExportRenderNodeInRT(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
    canvasDrawingNode.CreateRenderNodeForTextureExportSwitch();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRSCanvasDrawingNode(data, size);
    OHOS::Rosen::DoCreate(data, size);
    OHOS::Rosen::DoGetBitmap(data, size);
    OHOS::Rosen::DoGetPixelmap(data, size);
    OHOS::Rosen::DoResetSurface(data, size);
    OHOS::Rosen::DoCreateTextureExportRenderNodeInRT(data, size);
    return 0;
}

