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

#include <algorithm>
#include <iostream>
#include <surface.h>
#include <cmath>
#include <securec.h>

#include "command/rs_base_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "ui/rs_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"
#include "core/transaction/rs_interfaces.h"
#include "core/ui/rs_display_node.h"
#include "core/ui/rs_surface_node.h"
// temporary debug
#include "foundation/graphic/graphic_2d/rosen/modules/render_service_base/src/platform/ohos/rs_surface_frame_ohos.h"
#include "foundation/graphic/graphic_2d/rosen/modules/render_service_base/src/platform/ohos/rs_surface_ohos.h"

#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_bitmap.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_brush.h"
#include "drawing_types.h"

#include "utils/log.h"

using namespace OHOS;
using namespace Media;
using namespace Rosen;
using namespace std;

using TestFunc = std::function<void(OH_Drawing_Canvas*, uint32_t, uint32_t)>;
static std::vector<TestFunc> testFuncVec;
constexpr static int32_t WIDTH = 720;
constexpr static int32_t HEIGHT = 1280;

static void TestDrawPathPro(OH_Drawing_Canvas* cCanvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPathPro");
    // 300 means length of pentagram border
    int len = 300;

    // point a position
    float aX = 500;
    float aY = 500;

    // point d position, 18 means degree of pentagram
    float dX = aX - len * std::sin(18.0f);
    float dY = aY + len * std::cos(18.0f);

    // point c position, 18 means degree of pentagram
    float cX = aX + len * std::sin(18.0f);
    float cY = dY;

    // point b position
    float bX = aX + (len / 2.0);
    float bY = aY + std::sqrt((cX - dX) * (cX - dX) + (len / 2.0) * (len / 2.0));

    // point e position
    float eX = aX - (len / 2.0);
    float eY = bY;

    OH_Drawing_Path* cPath = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(cPath, aX, aY);
    OH_Drawing_PathLineTo(cPath, bX, bY);
    OH_Drawing_PathLineTo(cPath, cX, cY);
    OH_Drawing_PathLineTo(cPath, dX, dY);
    OH_Drawing_PathLineTo(cPath, eX, eY);
    OH_Drawing_PathClose(cPath);

    OH_Drawing_Pen* cPen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(cPen, true);
    OH_Drawing_PenSetColor(cPen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    // 10.0 means pentagram border line width
    OH_Drawing_PenSetWidth(cPen, 10.0);
    OH_Drawing_PenSetJoin(cPen, LINE_ROUND_JOIN);
    OH_Drawing_CanvasAttachPen(cCanvas, cPen);

    OH_Drawing_Brush* cBrush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(cBrush, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0x00));
    OH_Drawing_CanvasAttachBrush(cCanvas, cBrush);

    OH_Drawing_CanvasDrawPath(cCanvas, cPath);

    // (500, 500) means start position of line
    float lineStartX = 500;
    float lineStartY = 500;

    // (500, 1000) means end position of line
    float lineEndX = 500;
    float lineEndY = 1000;

    // 20.0 means line width
    OH_Drawing_PenSetWidth(cPen, 20.0);
    OH_Drawing_PenSetCap(cPen, LINE_ROUND_CAP);
    OH_Drawing_CanvasAttachPen(cCanvas, cPen);
    OH_Drawing_CanvasDetachBrush(cCanvas);
    OH_Drawing_CanvasDrawLine(cCanvas, lineStartX, lineStartY, lineEndX, lineEndY);

    OH_Drawing_BrushDestroy(cBrush);
    OH_Drawing_PenDestroy(cPen);
    OH_Drawing_PathDestroy(cPath);
    LOGI("+++++++ TestDrawPathPro");
}

static void DoDraw(uint8_t *addr, uint32_t width, uint32_t height, size_t index)
{
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);

    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);

    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));

    testFuncVec[index](cCanvas, width, height);

    // 4 means stride
    constexpr uint32_t stride = 4;
    uint32_t addrSize = width * height * stride;
    void* bitmapAddr = OH_Drawing_BitmapGetPixels(cBitmap);
    std::copy(addr, addr + addrSize, static_cast<uint8_t*>(bitmapAddr));
    OH_Drawing_CanvasDestroy(cCanvas);
    OH_Drawing_BitmapDestroy(cBitmap);
}

static void DrawSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t width, int32_t height, size_t index)
{
    sptr<Surface> surface = surfaceNode->GetSurface();
    if (surface == nullptr) {
        return;
    }

    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    BufferRequestConfig config = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    };

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, config);
    LOGI("request buffer ret is: %{public}s", SurfaceErrorStr(ret).c_str());

    if (buffer == nullptr) {
        LOGE("request buffer failed: buffer is nullptr");
        return;
    }
    if (buffer->GetVirAddr() == nullptr) {
        LOGE("get virAddr failed: virAddr is nullptr");
        return;
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    LOGI("buffer width:%{public}d, height:%{public}d", buffer->GetWidth(), buffer->GetHeight());
    DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), index);

    BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    ret = surface->FlushBuffer(buffer, -1, flushConfig);
    LOGI("flushBuffer ret is: %{public}s", SurfaceErrorStr(ret).c_str());
}

static std::shared_ptr<RSSurfaceNode> CreateSurface()
{
    RSSurfaceNodeConfig config;
    return RSSurfaceNode::Create(config);
}

int main()
{
    testFuncVec.push_back(TestDrawPathPro);
    auto surfaceNode = CreateSurface();
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    for (size_t i = 0; i < testFuncVec.size(); i++) {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            continue;
        }
        // sleep 2s
        sleep(2);
        displayNode->AddChild(surfaceNode, -1);
        surfaceNode->SetBounds(0, 0, WIDTH, HEIGHT);
        transactionProxy->FlushImplicitTransaction();
        DrawSurface(surfaceNode, WIDTH, HEIGHT, i);
        // sleep 8s
        sleep(8);
        displayNode->RemoveChild(surfaceNode);
        transactionProxy->FlushImplicitTransaction();
    }
    return 0;
}
