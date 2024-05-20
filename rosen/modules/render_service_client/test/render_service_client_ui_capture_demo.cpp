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

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include "display_type.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "graphic_common.h"
#include "image/bitmap.h"
#include "include/core/SkColor.h"
#include "pixel_map.h"
#include "png.h"
#include "render_context/render_context.h"
#include "surface_type.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "window.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace std;

using WriteToPngParam = struct {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t bitDepth;
    const uint8_t *data;
};
    
constexpr int BITMAP_DEPTH = 8;

shared_ptr<RSNode> rootNode;
shared_ptr<RSCanvasNode> canvasNode;
shared_ptr<RSCanvasNode> canvasNode2;
shared_ptr<RSSurfaceNode> surfaceNode1;
shared_ptr<RSSurfaceNode> surfaceNode2;

#ifdef ACE_ENABLE_GPU
    RenderContext* rc_ = nullptr;
#endif

bool WriteToPng(const string &fileName, const WriteToPngParam &param)
{
    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        cout << "png_create_write_struct error, nullptr!" << endl;
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        cout << "png_create_info_struct error, nullptr!" <<endl;
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }
    FILE *fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr) {
        cout << "open file error, nullptr!" << endl;
        png_destroy_write_struct(&pngStruct, &pngInfo);
        return false;
    }
    png_init_io(pngStruct, fp);

    // set png header
    png_set_IHDR(pngStruct, pngInfo,
        param.width, param.height,
        param.bitDepth,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);
    png_set_packing(pngStruct); // set packing info
    png_write_info(pngStruct, pngInfo); // write to header

    for (uint32_t i = 0; i < param.height; i++) {
        png_write_row(pngStruct, param.data + (i * param.stride));
    }

    png_write_end(pngStruct, pngInfo);

    // free
    png_destroy_write_struct(&pngStruct, &pngInfo);
    int ret = fclose(fp);
    if (ret != 0) {
        cout << "close fp failed" << endl;
    }
    return true;
}

void RenderContextInit()
{
#ifdef ACE_ENABLE_GPU
    cout << "ACE_ENABLE_GPU is true" << endl;
    cout << "Init RenderContext start" << endl;
    rc_ = RenderContextFactory::GetInstance().CreateEngine();
    if (rc_) {
        cout << "Init RenderContext success" << endl;
        rc_->InitializeEglContext();
    } else {
        cout << "Init RenderContext failed, RenderContext is nullptr" << endl;
    }
#endif
}

void DrawSurfaceNode(shared_ptr<RSSurfaceNode> surfaceNode)
{
    SkRect surfaceGeometry = SkRect::MakeXYWH(100, 50, 300, 600);
    auto x = surfaceGeometry.x();
    auto y = surfaceGeometry.y();
    auto width = surfaceGeometry.width();
    auto height = surfaceGeometry.height();
    surfaceNode->SetBounds(x, y, width, height);
    shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        cout << "surface is nullptr" << endl;
        return;
    }
#ifdef ACE_ENABLE_GPU
    if (rc_) {
        rsSurface->SetRenderContext(rc_);
    } else {
        cout << "DrawSurface: RenderContext is nullptr" << endl;
    }
#endif
    auto framePtr = rsSurface->RequestFrame(width, height);
    if (!framePtr) {
        cout << "framePtr is nullptr" << endl;
        return;
    }
    auto canvas = framePtr->GetCanvas();
    if (!canvas) {
        cout << "canvas is nullptr" << endl;
        return;
    }
    canvas->Сlear(SK_ColorWHITE);

    Brush brush;
    brush.SetColor(SK_ColorGREEN);
    brush.SetAntiAlias(true);

    string scaleInfo = "Hello World";
    Font font = Font();
    font.SetSize(16); // text size 16
    std::shared_ptr<TextBlob> scaleInfoTextBlob = TextBlob::MakeFromString(scaleInfo.c_str(),
        font, TextEncoding::UTF8);
    canvas->AttachBrush(brush);
    canvas->DrawTextBlob(scaleInfoTextBlob.get(), 20, 50); // start point is (20, 50)
    canvas->DetachBrush();
    framePtr->SetDamageRegion(0, 0, width, height);
    rsSurface->FlushFrame(framePtr);
    return;
}

bool WriteToPngWithPixelMap(const string &fileName, Media::PixelMap &pixelMap)
{
    WriteToPngParam param;
    param.width = static_cast<uint32_t>(pixelMap.GetWidth());
    param.height = static_cast<uint32_t>(pixelMap.GetHeight());
    param.data = pixelMap.GetPixels();
    param.stride = static_cast<uint32_t>(pixelMap.GetRowBytes());
    param.bitDepth = BITMAP_DEPTH;
    return WriteToPng(fileName, param);
}

void Init(shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    cout << "rs local surface capture demo Init Rosen Backend!" << endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorRED);

    rsUiDirector->SetRoot(rootNode->GetId());
    canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(10, 10, 600, 1000);
    canvasNode->SetFrame(10, 10, 600, 1000);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    rootNode->AddChild(canvasNode, -1);

    canvasNode2 = RSCanvasNode::Create();
    canvasNode2->SetBounds(5, 5, 400, 800);
    canvasNode2->SetFrame(5, 5, 400, 800);
    canvasNode2->SetBackgroundColor(SK_ColorBLUE);
    canvasNode->AddChild(canvasNode2, -1);

    RSSurfaceNodeConfig config;
    surfaceNode1 = RSSurfaceNode::Create(config, false);
    RenderContextInit();
    DrawSurfaceNode(surfaceNode1);
    canvasNode2->AddChild(surfaceNode1, -1);
}

class MyOffscreenRenderCallback : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        if (pixelmap == nullptr) {
            cout << "RSUIDirector::LocalCapture failed to get pixelmap, return nullptr!" << endl;
            return;
        }
        cout << "rs local surface demo drawPNG" << endl;
        int ret = WriteToPngWithPixelMap("/data/local/test.jpg", *pixelmap);
        if (!ret) {
            cout << "pixelmap write to png failed" << endl;
        }
        cout << "pixelmap write to png sucess" << endl;
    }
};

int main()
{
    cout << "rs local surface capture demo" << endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({0, 0, 680, 1100});
    string demoName = "offscreen_render_demo";
    auto window = Window::Create(demoName, option);

    window->Show();
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        cout << "rs local surface demo create window failed: " << rect.width_ << " " << rect.height_ << endl;
        window->Hide();
        window->Destroy();
        window = Window::Create(demoName, option);
        window->Show();
        rect = window->GetRect();
    }
    cout << "rs local surface demo create window success: " << rect.width_ << " " << rect.height_ << endl;
    auto surfaceNode = window->GetSurfaceNode();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();

    cout << "rs local surface demo init" << endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(4);

    cout << "rs local surface demo cratePixelMap" << endl;
    std::shared_ptr<MyOffscreenRenderCallback> myOffscreenRenderCallback =
        std::make_shared<MyOffscreenRenderCallback>();
    cout << "rootNode id is" << rootNode->GetId() << endl;
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(rootNode, myOffscreenRenderCallback, 1, 1);
    sleep(2);
    rootNode->RemoveFromTree();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(rootNode, myOffscreenRenderCallback, 1, 1);
    sleep(2);
    return 0;
}
