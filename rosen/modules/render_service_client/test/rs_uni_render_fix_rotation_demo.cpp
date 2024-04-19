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

#include <iostream>
#include <surface.h>
#include "SkColor.h"
#include "foundation/window/window_manager/interfaces/innerkits/dm/display_manager.h"

#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
    constexpr uint32_t SLEEP_TIME = 1;
    constexpr uint32_t RETURN_WAIT_TIME = 1000;
    constexpr float NODE_BOUND_LEFT = 500;
    constexpr float NODE_BOUND_TOP = 500;
    constexpr float NODE_BOUND_WIDTH = 1000;
    constexpr float NODE_BOUND_HEIGHT = 1500;
    constexpr int32_t BUFFER_WIDTH = 1440;
    constexpr int32_t BUFFER_HEIGHT = 1080;
    constexpr float START_POINT_POSITION = 10;
    constexpr float END_POINT_POSITION = 100;
    constexpr Drawing::scalar PEN_WIDTH = 10;
}
std::shared_ptr<RSSurfaceNode> surfaceNode;
uint64_t screenId = 0;


sptr<SurfaceBuffer> GetSurfaceBuffer(sptr<Surface> ohosSurface, int32_t width, int32_t height)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = 0;
    BufferRequestConfig config = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    };

    SurfaceError ret = ohosSurface->RequestBuffer(buffer, releaseFence, config);
    if (ret != SURFACE_ERROR_OK) {
        return nullptr;
    }
    return buffer;
}

void DoDraw(uint8_t* addr, uint32_t width, uint32_t height)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, format);

    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(Drawing::Color::COLOR_RED);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_GREEN);
    Drawing::scalar penWidth = PEN_WIDTH;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);
    Drawing::Point startPt(START_POINT_POSITION, START_POINT_POSITION);
    Drawing::Point endPt(END_POINT_POSITION, END_POINT_POSITION);
    canvas.DrawLine(startPt, endPt);
    static constexpr uint32_t stride = 4;
    uint32_t addrSize = width * height * stride;
    memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
}

void InitSurfaceStyle(std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    auto ohosSurface = surfaceNode->GetSurface();
    if (ohosSurface == nullptr) {
        return;
    }
    ohosSurface->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_270);
    sptr<SurfaceBuffer> buffer = GetSurfaceBuffer(ohosSurface, BUFFER_WIDTH, BUFFER_HEIGHT);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        return;
    }
    auto addr = static_cast<uint8_t*>(buffer->GetVirAddr());
    DoDraw(addr, buffer->GetWidth(), buffer->GetHeight());
    BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        }
    };
    ohosSurface->FlushBuffer(buffer, -1, flushConfig);
}

bool InitSurface()
{
    std::cout << "InitSurface" << std::endl;
    DisplayId displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "test window";
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    std::cout << "RSSurfaceNode::Create" << std::endl;
    surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    if (!surfaceNode) {
        return false;
    }

    surfaceNode->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    surfaceNode->SetBounds(NODE_BOUND_LEFT, NODE_BOUND_TOP, NODE_BOUND_WIDTH, NODE_BOUND_HEIGHT);
    surfaceNode->SetBackgroundColor(SK_ColorBLACK);

    std::cout << "GetDisplayById: " << std::endl;
    screenId = DisplayManager::GetInstance().GetDisplayById(displayId)->GetId();
    std::cout << "ScreenId: " << screenId << std::endl;
    surfaceNode->AttachToDisplay(screenId);

    std::cout << "RSTranscation::FlushImplicitTransaction" << std::endl;
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);
    return true;
}

int main()
{
    std::cout << "rs fix rotation demo start!" << std::endl;
    InitSurface();

    Rosen::RSSurfaceNodeConfig surfaceNodeConfig = { .SurfaceNodeName = "test",
        .isTextureExportNode = false };
    auto hardwareNode = RSSurfaceNode::Create(surfaceNodeConfig, false);
    hardwareNode->SetHardwareEnabled(true);
    hardwareNode->SetBounds(0, 0, NODE_BOUND_WIDTH, NODE_BOUND_HEIGHT);
    hardwareNode->SetBackgroundColor(SK_ColorGREEN);

    surfaceNode->AddChild(hardwareNode, -1);
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    InitSurfaceStyle(hardwareNode);
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);
    hardwareNode->SetForceHardwareAndFixRotation(true);
    RSTransaction::FlushImplicitTransaction();

    sleep(RETURN_WAIT_TIME);
    return 0;
}