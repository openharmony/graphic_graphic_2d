/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#include <cinttypes>
#include <codecvt>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <locale>
#include <memory>
#include <securec.h>
#include <sstream>
#include <string>
#include <surface.h>
#include <unistd.h>

#include "command/rs_base_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "core/transaction/rs_interfaces.h"
#include "core/ui/rs_display_node.h"
#include "core/ui/rs_surface_node.h"
#include "display_type.h"
#include "draw/canvas.h"
#include "draw/pen.h"
#include "foundation/graphic/standard/rosen/modules/render_service_base/src/platform/ohos/rs_surface_frame_ohos.h"
#include "foundation/graphic/standard/rosen/modules/render_service_base/src/platform/ohos/rs_surface_ohos.h"
#include "image/bitmap.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "rosen_text/properties/text_style.h"
#include "rosen_text/properties/typography_properties.h"
#include "rosen_text/ui/font_collection.h"
#include "rosen_text/ui/typography.h"
#include "rosen_text/ui/typography_create.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"

using namespace std;
using namespace OHOS;
using namespace Rosen;
using namespace rosen;
using namespace Drawing;
using namespace Media;

std::u16string TextToUtf16(std::string str)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(str);
}

void DoDraw(uint8_t *addr, uint32_t width, uint32_t height, size_t index)
{
    Bitmap bitmapCache;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUYE};
    bitmapCache.Build(width, height, format);

    Canvas canvas;
    canvas.Bind(bitmapCache);
    canvas.Clear(Drawing::Color::COLOR_WHITE);
    TypographyStyle typoStype;
    typoStype.textDirection_ = TextDirection::LTR;
    typoStype.textAlign_ = TextAlign::START;
    typoStype.maxLines_ = 1000; // maxLines 1000
    typoStype.locale_ = "en";
    typoStype.wordBreakType_ = WordBreakType::WordBreakTypeBreakWord;
    std::unique_ptr<TypographyCreate> builder = TypographyCreate::CreateRosenBuilder(
        typoStype, FontCollection::GetInstance());
    TextStyle textStyle;

    textStyle.fontFamilies_ = std::vector<std::string>(1, "Roboto");
    textStyle.color_ = Drawing::Color::COLOR_BLACK;
    builder->PushStyle(textStyle);
    const std::string utf8 = "hello world!\n";

    const std::u16string u16Text = TextToUtf16(utf8);
    builder->AddText(u16Text);
    builder->Pop();
    // layout
    std::unique_ptr<rosen::Typography> typography;
    typography = builder->Build();
    if (typography == nullptr) {
        LOGD("typography == nullptr");
    }
    double lastLayoutMaxWidth = 1000.0; // width 1000.0
    typography->Layout(lastLayoutMaxWidth);
    typography->Paint(&canvas, 10.0, 15.0); // pos to paint 10.0, 15.0
    constexpr uint32_t stride = 4;
    int32_t addrSize = width * height * stride;
    auto ret = memcpy_s(addr, addrSize, bitmapCache.GetPixels(), addrSize);
    if (ret != EOK) {
        LOGD("memcpy_s failed");
    }
}

void DrawSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t width, int32_t height, size_t index)
{
    sptr<OHOS::Surface> surface = surfaceNode->GetSurface();
    if (surface == nullptr) {
        return;
    }

    sptr<OHOS::SurfaceBuffer> buffer;
    int32_t releaseFence;
    OHOS::BufferRequestConfig config = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    };

    OHOS::SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, config);
    LOGD("request buffer ret is: %{public}s", SurfaceErrorStr(ret).c_str());

    if (buffer == nullptr) {
        LOGD("request buffer failed: buffer is nullptr");
        return;
    }
    if (buffer->GetVirAddr() == nullptr) {
        LOGD("get virAddr failed: virAddr is nullptr");
        return;
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    LOGD("buffer width:%{public}d, height:%{public}d", buffer->GetWidth(), buffer->GetHeight());

    DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), index);
    LOGD("DoDraw end");

    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    ret = surface->FlushBuffer(buffer, -1, flushConfig);
    LOGD("draw pointer FlushBuffer ret is: %{public}s", SurfaceErrorStr(ret).c_str());
}

std::shared_ptr<RSSurfaceNode> CreateSurface()
{
    RSSurfaceNodeConfig config;
    return RSSurfaceNode::Create(config);
}

int main(int argc, char** argv)
{
    auto surfaceNode = CreateSurface();
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    for (size_t i = 0; i < 5; i++) { // Draw 5 times
        sleep(2); // delay 2 second
        displayNode->AddChild(surfaceNode, -1);
        surfaceNode->SetBounds(0, 0, 2560, 1600); // Draw Range 2560, 1600
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        DrawSurface(surfaceNode, 2560, 1600, i); // Draw Range 2560, 1600
        sleep(4); // delay 4 second
        displayNode->RemoveChild(surfaceNode);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    }
    return 0;
}
