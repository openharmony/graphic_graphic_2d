/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "dm/display_manager.h"
#include "securec.h"

#include "draw/canvas.h"
#include "image/bitmap.h"

#include "transaction/rs_transaction.h"
#include "ui/rs_surface_node.h"
#include "transaction/rs_interfaces.h"

using namespace OHOS;
using namespace OHOS::Rosen;

constexpr uint32_t SLEEP_TIME = 3;
constexpr uint32_t POINTER_WIDTH = 100;
constexpr uint32_t POINTER_HEIGHT = 200;
constexpr uint32_t POINTER_WINDOW_INIT_SIZE = 64;
std::shared_ptr<RSSurfaceNode> surfaceNode;
uint64_t screenId = 0;

namespace {
void Resize(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t width, int32_t height)
{
    width = (width / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    height = (height / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    surfaceNode->SetBoundsWidth(width);
    surfaceNode->SetBoundsHeight(height);
}

void MoveTo(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t x, int32_t y)
{
    surfaceNode->SetBounds(
        x, y, surfaceNode->GetStagingProperties().GetBounds().z_, surfaceNode->GetStagingProperties().GetBounds().w_);
}

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
    canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);
    Drawing::Point startPt(10, 10);
    Drawing::Point endPt(50, 50);
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
    sptr<SurfaceBuffer> buffer = GetSurfaceBuffer(ohosSurface, surfaceNode->GetStagingProperties().GetBounds().z_,
        surfaceNode->GetStagingProperties().GetBounds().w_);
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
    surfaceNodeConfig.SurfaceNodeName = "pointer window";
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    std::cout << "RSSurfaceNode::Create" <<std::endl;
    surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);

    if (!surfaceNode) {
        return false;
    }

    std::cout << "SetFrameGravity" <<std::endl;
    surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
    surfaceNode->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    int width = (POINTER_WIDTH / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    int height = (POINTER_HEIGHT / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    surfaceNode->SetBounds(100, 300, width, height);
    surfaceNode->SetBackgroundColor(SK_ColorGREEN);

    // Attach RSSurfaceNode to RSDisplayNode through dms
    // DisplayManager::GetInstance().AddSurfaceNodeToDisplay(displayId, surfaceNode);

    // Attach RSSurfaceNode to RSDisplayNode through the AttachToDisplay interface of RSSurfaceNode
    std::cout << "GetDisplayById: " << std::endl;
    screenId = DisplayManager::GetInstance().GetDisplayById(displayId)->GetId();
    std::cout << "ScreenId: " << screenId << std::endl;
    surfaceNode->AttachToDisplay(screenId);

    std::cout << "RSTranscation::FlushImplicitTransaction" << std::endl;
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);
    return true;
}

bool isRemote = true;

void PrintCallback()
{
    std::cout << "OnRemoteDied Callback" << std::endl;
    isRemote = false;
}
}

int main()
{
    OnRemoteDiedCallback callback = PrintCallback;
    RSInterfaces::GetInstance().SetOnRemoteDiedCallback(callback);

    std::cout << "rs pointer window demo start!" << std::endl;
    std::cout << "rs pointer window demo stage 1 Init" << std::endl;
    InitSurface();

    // Attach and Detach
    std::cout << "rs pointer window demo stage 2 Detach" << std::endl;
    surfaceNode->DetachToDisplay(screenId);
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);
    std::cout << "rs pointer window demo stage 2 Attach" << std::endl;
    surfaceNode->AttachToDisplay(screenId);
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);

    // Resize
    std::cout << "rs pointer window demo stage 3 Resize" << std::endl;
    Resize(surfaceNode, POINTER_WIDTH / 2, POINTER_HEIGHT / 2);
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);

    // SetStyle
    std::cout << "rs pointer window demo stage 4 SetStyle" << std::endl;
    InitSurfaceStyle(surfaceNode);
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);

    // Hide
    std::cout << "rs pointer window demo stage 5 Hide" << std::endl;
    surfaceNode->SetVisible(false);
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);
    // Show
    std::cout << "rs pointer window demo stage 6 Show" << std::endl;
    surfaceNode->SetVisible(true);
    RSTransaction::FlushImplicitTransaction();
    sleep(SLEEP_TIME);

    std::cout << "rs pointer window demo stage MoveTo" << std::endl;
    while (isRemote) {
    // while (true) {
        // MoveTo
        MoveTo(surfaceNode, 0, 0);
        RSTransaction::FlushImplicitTransaction();
        sleep(SLEEP_TIME);
        MoveTo(surfaceNode, 100, 160);
        RSTransaction::FlushImplicitTransaction();
        sleep(SLEEP_TIME);
        MoveTo(surfaceNode, 320, 640);
        RSTransaction::FlushImplicitTransaction();
        sleep(SLEEP_TIME);
        while (!isRemote) {
            std::cout << "ReInitSurface" << std::endl;
            isRemote = InitSurface();
        }
    }

    std::cout << "rs pointer window demo end!" << std::endl;
    return 0;
}