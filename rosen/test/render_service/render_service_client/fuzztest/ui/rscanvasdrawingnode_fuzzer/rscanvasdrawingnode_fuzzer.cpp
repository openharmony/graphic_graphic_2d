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

#include "rscanvasdrawingnode_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

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
const uint8_t DO_NODE = 0;
const uint8_t DO_CREATE = 1;
const uint8_t DO_GET_BITMAP = 2;
const uint8_t DO_GET_PIXELMAP = 3;
const uint8_t DO_RESET_SURFACE = 4;
const uint8_t DO_TEXTURE_EXPORT = 5;
const uint8_t TARGET_SIZE = 6;
}

void DoRSCanvasDrawingNode(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
}

void DoCreate(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    RSCanvasDrawingNode::SharedPtr node = RSCanvasDrawingNode::Create(isRenderServiceNode, isTextureExportNode);
}

void DoGetBitmap(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
    Drawing::Bitmap bitmap;
    int w = fdp.ConsumeIntegral<int>();
    int h = fdp.ConsumeIntegral<int>();
    auto list = std::make_shared<Drawing::DrawCmdList>(w, h);
    float fLeft = fdp.ConsumeFloatingPoint<float>();
    float fTop = fdp.ConsumeFloatingPoint<float>();
    float fRight = fdp.ConsumeFloatingPoint<float>();
    float fBottom = fdp.ConsumeFloatingPoint<float>();
    Drawing::Rect rect { fLeft, fTop, fRight, fBottom };
    canvasDrawingNode.GetBitmap(bitmap, list, &rect);
}

void DoGetPixelmap(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
    auto pixelmap = std::make_shared<Media::PixelMap>();
    int w = fdp.ConsumeIntegral<int>();
    int h = fdp.ConsumeIntegral<int>();
    auto list = std::make_shared<Drawing::DrawCmdList>(w, h);
    float fLeft = fdp.ConsumeFloatingPoint<float>();
    float fTop = fdp.ConsumeFloatingPoint<float>();
    float fRight = fdp.ConsumeFloatingPoint<float>();
    float fBottom = fdp.ConsumeFloatingPoint<float>();
    Drawing::Rect rect { fLeft, fTop, fRight, fBottom };
    canvasDrawingNode.GetPixelmap(pixelmap, list, &rect);
}

void DoResetSurface(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
    int w = fdp.ConsumeIntegral<int>();
    int h = fdp.ConsumeIntegral<int>();
    canvasDrawingNode.ResetSurface(w, h);
}

void DoCreateTextureExportNodeInRT(FuzzedDataProvider& fdp)
{
    bool isRenderServiceNode = fdp.ConsumeBool();
    bool isTextureExportNode = fdp.ConsumeBool();
    RSCanvasDrawingNode canvasDrawingNode(isRenderServiceNode, isTextureExportNode);
    canvasDrawingNode.CreateRenderNodeForTextureExportSwitch();
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_NODE:
            OHOS::Rosen::DoRSCanvasDrawingNode(fdp);
            break;
        case OHOS::Rosen::DO_CREATE:
            OHOS::Rosen::DoCreate(fdp);
            break;
        case OHOS::Rosen::DO_GET_BITMAP:
            OHOS::Rosen::DoGetBitmap(fdp);
            break;
        case OHOS::Rosen::DO_GET_PIXELMAP:
            OHOS::Rosen::DoGetPixelmap(fdp);
            break;
        case OHOS::Rosen::DO_RESET_SURFACE:
            OHOS::Rosen::DoResetSurface(fdp);
            break;
        case OHOS::Rosen::DO_TEXTURE_EXPORT:
            OHOS::Rosen::DoCreateTextureExportNodeInRT(fdp);
            break;
        default:
            break;
    }
    return 0;
}
