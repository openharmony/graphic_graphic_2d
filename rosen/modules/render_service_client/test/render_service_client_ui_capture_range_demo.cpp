/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "accesstoken_kit.h"
#include "dm/display_manager.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "graphic_common.h"
#include "image/bitmap.h"
#include "image_source.h"
#include "include/core/SkColor.h"
#include "nativetoken_kit.h"
#include "pixel_map.h"
#include "png.h"
#include "render_context/render_context.h"
#include "surface_type.h"
#include "token_setproc.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_drawing_node.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_effect_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_texture_export.h"
#include "ui/rs_ui_director.h"
#include "window.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace std;
uint64_t screenId = 0;

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
shared_ptr<RSEffectNode> effectNode;
shared_ptr<RSCanvasDrawingNode> canvasDrawingNode;
shared_ptr<RSNode> myLittleRootNode;

#ifdef RS_ENABLE_GPU
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
#ifdef RS_ENABLE_GPU
    cout << "RS_ENABLE_GPU is true" << endl;
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
    int surfaceX = 100;
    int surfaceY = 50;
    int surfaceWidth = 300;
    int surfaceHeight = 600;
    SkRect surfaceGeometry = SkRect::MakeXYWH(surfaceX, surfaceY, surfaceWidth, surfaceHeight);
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
#ifdef RS_ENABLE_GPU
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
    canvas->Clear(SK_ColorWHITE);

    Brush brush;
    brush.SetColor(SK_ColorGREEN);
    brush.SetAntiAlias(true);

    Font font = Font();
    font.SetSize(16); // text size 16
    std::shared_ptr<TextBlob> scaleInfoTextBlob = TextBlob::MakeFromString("Hello World",
        font, TextEncoding::UTF8);
    canvas->AttachBrush(brush);
    int startpointX = 20;
    int startpointY = 50;
    canvas->DrawTextBlob(scaleInfoTextBlob.get(), startpointX, startpointY); // start point is (20, 50)
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

shared_ptr<Media::PixelMap> DecodePixelMap(const string& pathName, const Media::AllocatorType& allocatorType)
{
    cout << "decode start: ------------ " << pathName << endl;
    cout << "decode 1: CreateImageSource" << endl;
    uint32_t errCode = 0;
    std::unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(pathName, Media::SourceOptions(), errCode);
    if (imageSource == nullptr || errCode != 0) {
        cout << "imageSource : " << (imageSource != nullptr) << ", err:" << errCode << std::endl;
        return nullptr;
    }

    cout << "decode 2: CreatePixelMap" << endl;
    Media::DecodeOptions decodeOpt;
    decodeOpt.allocatorType = allocatorType;
    shared_ptr<Media::PixelMap> pixelmap = imageSource->CreatePixelMap(decodeOpt, errCode);
    if (pixelmap == nullptr || errCode != 0) {
        cout << "pixelmap == nullptr, err:" << errCode << endl;
        return nullptr;
    }

    cout << "w x h: " << pixelmap->GetWidth() << "x" << pixelmap->GetHeight() << endl;
    cout << "AllocatorType: " << (int)pixelmap->GetAllocatorType() << endl;
    cout << "fd: " << (!pixelmap->GetFd() ? "null" : to_string(*(int*)pixelmap->GetFd())) << endl;
    cout << "decode success: ------------ " << endl;
    return pixelmap;
}

class MySurfaceCaptureCallback : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        static int32_t count = 0;
        if (pixelmap == nullptr) {
            cout << "RSUIDirector::LocalCapture failed to get pixelmap, return nullptr!" << endl;
            return;
        }
        cout << "rs local surface demo drawPNG" << endl;
        string filename = "/data/local/test_" + to_string(count++) + ".jpg";
        int ret = WriteToPngWithPixelMap(filename, *pixelmap);
        if (!ret) {
            cout << "pixelmap write to png failed" << endl;
        }
        cout << "pixelmap write to png sucess" << endl;
    }
    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
};

void InitNativeTokenInfo()
{
    uint64_t tokenId;
    const char *perms[1];
    perms[0] = "ohos.permission.SYSTEM_FLOAT_WINDOW";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "rs_uni_render_pixelmap_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void Init(shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    cout << "rs local capture range demo Init Rosen Backend!" << endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorRED);

    rsUiDirector->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
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

    effectNode = RSEffectNode::Create();
    effectNode->SetBounds(5, 5, 250, 550);
    effectNode->SetFrame(5, 5, 250, 550);
    effectNode->SetBackgroundColor(SK_ColorRED);
    surfaceNode1->AddChild(effectNode, -1);

    myLittleRootNode = RSRootNode::Create();
    myLittleRootNode->SetBounds(5, 5, 200, 500);
    myLittleRootNode->SetFrame(5, 5, 200, 500);
    myLittleRootNode->SetBackgroundColor(SK_ColorYELLOW);
    surfaceNode1->AddChild(myLittleRootNode, -1);

    canvasDrawingNode = RSCanvasDrawingNode::Create();
    canvasDrawingNode->SetBounds(5, 5, 100, 300);
    canvasDrawingNode->SetFrame(5, 5, 100, 300);
    canvasDrawingNode->SetBackgroundColor(SK_ColorGREEN);
    myLittleRootNode->AddChild(canvasDrawingNode, -1);
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
        cout << "pixelmap write to png success" << endl;
    }
    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
};

int main()
{
    InitNativeTokenInfo();

    cout << "rs local capture range demo" << endl;
    DisplayId displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "capture_range_demo";
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    cout << "RSSurfaceNode:: Create" << endl;
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    if (!surfaceNode) {
        return -1;
    }
    surfaceNode->SetBounds(0, 0, 1260, 2720);
    surfaceNode->SetFrame(0, 0, 1260, 2720);
    surfaceNode->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    cout << "GetDisplayId: " << endl;
    screenId = DisplayManager::GetInstance().GetDisplayById(displayId)->GetId();
    cout << "ScreenId: " << screenId << endl;
    surfaceNode->AttachToDisplay(screenId);

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    cout << "rs local capture range demo init" << endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);

    Init(rsUiDirector, 1260, 2720);
    RSTransaction::FlushImplicitTransaction();
    sleep(4);

    cout << "rs local capture range demo createPixelmap" << endl;
    std::shared_ptr<MySurfaceCaptureCallback> mySurfaceCaptureCallback =
        std::make_shared<MySurfaceCaptureCallback>();
    cout << "rootNode id is " << rootNode->GetId() << endl;

    // 1. red, yellow
    RSInterfaces::GetInstance().TakeUICaptureInRange(
        rootNode, canvasNode, true, mySurfaceCaptureCallback, 1, 1, false);
    cout << "1. red, yellow, start: root -> end: canvas1" << endl;
    sleep(2);

    // 2. red, yellow, blue
    RSInterfaces::GetInstance().TakeUICaptureInRange(
        rootNode, canvasNode2, true, mySurfaceCaptureCallback, 1, 1, false);
    cout << "2. red, yellow, blue, start: root -> canvas1 -> end: canvas2" << endl;
    sleep(2);

    // 3. yellow...
    RSInterfaces::GetInstance().TakeUICaptureInRange(
        canvasNode, rootNode, true, mySurfaceCaptureCallback, 1, 1, false);
    cout << "3. yellow..., start: canvas1 -> end: root" << endl;
    sleep(2);

    // 4. blue
    RSInterfaces::GetInstance().TakeUICaptureInRange(
        canvasNode2, canvasNode2, true, mySurfaceCaptureCallback, 1, 1, false);
    cout << "4.blue, start: canvas2 -> end: canvas2" << endl;
    sleep(2);

    // 5. red...
    RSInterfaces::GetInstance().TakeUICaptureInRange(
        rootNode, nullptr, true, mySurfaceCaptureCallback, 1, 1, false);
    cout << "5. red, yellow, blue, yellow, green, start: root -> end: nullptr" << endl;
    sleep(2);
    return 0;
}