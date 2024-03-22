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

#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_bitmap.h"
#include "drawing_font_collection.h"
#include "drawing_text_declaration.h"
#include "drawing_types.h"
#include "drawing_text_typography.h"

#include <securec.h>

#include "core/transaction/rs_interfaces.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Media;
using namespace Rosen;
using namespace std;

constexpr static int32_t WIDTH = 720;
constexpr static int32_t HEIGHT = 1280;

namespace {
void DoDraw(uint8_t *addr, uint32_t width, uint32_t height, size_t index)
{
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);

    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));

    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_LTR);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_LEFT);

    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle,
        OH_Drawing_CreateFontCollection());

    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 80;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 1);
    const char* fontFamilies[] = {"Roboto"};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);
    OH_Drawing_SetTextStyleLocale(txtStyle, "en");

    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);

    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    double position[2] = {10.0, 15.0};
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);

    constexpr uint32_t stride = 4;
    int32_t addrSize = width * height * stride;
    void* bitmapAddr = OH_Drawing_BitmapGetPixels(cBitmap);
    auto ret = memcpy_s(addr, addrSize, bitmapAddr, addrSize);
    if (ret != EOK) {
        LOGI("memcpy_s failed");
    }
    OH_Drawing_CanvasDestroy(cCanvas);
    OH_Drawing_BitmapDestroy(cBitmap);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

void DrawSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t width, int32_t height, size_t index)
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

std::shared_ptr<RSSurfaceNode> CreateSurface()
{
    RSSurfaceNodeConfig config;
    return RSSurfaceNode::Create(config);
}
}

int main()
{
    auto surfaceNode = CreateSurface();
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    int testNumber = 5;
    for (size_t i = 0; i < testNumber; i++) {
        int restTime = 4;
        sleep(restTime);
        displayNode->AddChild(surfaceNode, -1);
        surfaceNode->SetBounds(0, 0, WIDTH, HEIGHT);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        DrawSurface(surfaceNode, WIDTH, HEIGHT, i);
        sleep(restTime);
        displayNode->RemoveChild(surfaceNode);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    }
    return 0;
}
