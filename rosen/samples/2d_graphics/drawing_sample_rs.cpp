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

#include <iostream>
#include <surface.h>
#include <cmath>

#include "command/rs_base_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "display_type.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "ui/rs_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"
#include "core/transaction/rs_interfaces.h"
#include "core/ui/rs_display_node.h"
#include "core/ui/rs_surface_node.h"
// temporary debug
#include "foundation/graphic/standard/rosen/modules/render_service_base/src/platform/ohos/rs_surface_frame_ohos.h"
#include "foundation/graphic/standard/rosen/modules/render_service_base/src/platform/ohos/rs_surface_ohos.h"

#include "draw/color.h"

#include "image/bitmap.h"
#include "image/image.h"

#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/pen.h"
#include "draw/path.h"
#include "draw/clip.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "utils/rect.h"

#include "utils/matrix.h"
#include "utils/camera3d.h"

#include "image_type.h"
#include "pixel_map.h"

using namespace OHOS;
using namespace Media;
using namespace Rosen;
using namespace Drawing;
using namespace std;

using TestFunc = std::function<void(Canvas&, uint32_t, uint32_t)>;
namespace {
#define LOG(fmt, ...) ::OHOS::HiviewDFX::HiLog::Info(             \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "DrawingSampleRS"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)
}

std::vector<TestFunc> testFuncVec;

void TestDrawPath(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPath");
    Path path1;
    path1.MoveTo(200, 200);
    path1.QuadTo(600, 200, 600, 600);
    path1.Close();

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_GRAY);
    pen.SetWidth(10);
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    canvas.AttachBrush(brush);

    Path path2;
    path2.AddOval({200, 200, 600, 1000});

    Path dest;
    dest.Op(path1, path2, PathOp::UNION);
    canvas.DrawPath(dest);
    LOGI("+++++++ TestDrawPath");
}

void TestDrawPathPro(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPathPro");
    int len = 300;
    Point a(500, 500);

    Point c;
    Point d;

    d.SetX(a.GetX() - len * std::sin(18.0f));
    d.SetY(a.GetY() + len * std::cos(18.0f));

    c.SetX(a.GetX() + len * std::sin(18.0f));
    c.SetY(d.GetY());

    Point b;
    b.SetX(a.GetX() + (len / 2.0));
    b.SetY(a.GetY() + std::sqrt((c.GetX() - d.GetX()) * (c.GetX() - d.GetX()) + (len / 2.0) * (len / 2.0)));

    Point e;
    e.SetX(a.GetX() - (len / 2.0));
    e.SetY(b.GetY());

    Path path;
    path.MoveTo(a.GetX(), a.GetY());
    path.LineTo(b.GetX(), b.GetY());
    path.LineTo(c.GetX(), c.GetY());
    path.LineTo(d.GetX(), d.GetY());
    path.LineTo(e.GetX(), e.GetY());
    path.Close();

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(10);
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    canvas.AttachBrush(brush);

    canvas.AttachPen(pen).AttachBrush(brush).DrawPath(path);
    LOGI("+++++++ TestDrawPathPro");
}

void TestDrawBase(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawBase");

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen);

    canvas.DrawLine(Point(0, 0), Point(width, height));

    pen.Reset();
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(100);
    Point point;
    point.SetX(width / 2.0);
    point.SetY(height / 2.0);
    canvas.AttachPen(pen);
    canvas.DrawPoint(point);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    brush.Reset();
    canvas.AttachBrush(brush);
    canvas.DrawRect({1200, 100, 1500, 700});
    LOGI("------- TestDrawBase");
}

void TestDrawPathEffect(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPathEffect");

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(10);
    scalar vals[] = {10.0, 20.0};
    pen.SetPathEffect(PathEffect::CreateDashPathEffect(vals, 2, 25));
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    canvas.AttachBrush(brush);
    canvas.DrawRect({1200, 300, 1500, 600});

    canvas.DetachPen();
    //canvas.DetachBrush();
    canvas.DrawRect({1200, 700, 1500, 1000});
    LOGI("------- TestDrawPathEffect");
}

void TestDrawFilter(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawFilter");

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetColor(pen.GetColor4f(), Drawing::ColorSpace::CreateSRGBLinear());
    // pen.SetARGB(0x00, 0xFF, 0x00, 0x80);
    pen.SetAlpha(0x44);
    pen.SetWidth(10);
    Filter filter;
    filter.SetColorFilter(ColorFilter::CreateBlendModeColorFilter(Drawing::Color::COLOR_RED, BlendMode::SRC_ATOP));
    filter.SetMaskFilter(MaskFilter::CreateBlurMaskFilter(BlurType::NORMAL, 10));
    // filter.SetFilterQuality(Filter::FilterQuality::NONE);
    pen.SetFilter(filter);
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    brush.SetColor(brush.GetColor4f(), Drawing::ColorSpace::CreateSRGBLinear());
    // brush.SetARGB(0x00, 0xFF, 0x00, 0x80);
    brush.SetAlpha(0x44);
    brush.SetBlendMode(BlendMode::SRC_ATOP);
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);
    canvas.DrawRect({1200, 300, 1500, 600});

    canvas.DetachPen();
    // canvas.DetachBrush();
    canvas.DrawRect({1200, 700, 1500, 1000});
    LOGI("------- TestDrawFilter");
}

void TestDrawBitmap(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawBitmap");
    Bitmap bmp;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUYE};
    bmp.Build(200, 200, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetWidth(10);
    canvas.AttachPen(pen);

    canvas.DrawBitmap(bmp, 500, 500);
    //canvas.DrawRect({1200, 100, 1500, 700});

    LOGI("------- TestDrawBitmap");
}

void TestDrawImage(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawImage");
    Bitmap bmp;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUYE};
    bmp.Build(300, 300, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);

    Image image;
    image.BuildFromBitmap(bmp);
    int imageWidth = image.GetWidth();
    int imageHeight = image.GetHeight();
    LOGI("image width = %{public}d, image height = %{public}d", imageWidth, imageHeight);
    Matrix matrix;
    matrix.Rotate(45, 0, 0);
    SamplingOptions sampling = SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    auto e = ShaderEffect::CreateImageShader(image, TileMode::REPEAT, TileMode::MIRROR, sampling, matrix);
    LOGI("sampling useCubic = %{public}d, filter = %{public}d, mipmap = %{public}d",
            sampling.GetUseCubic(), sampling.GetFilterMode(), sampling.GetMipmapMode());
    auto c = Drawing::ColorSpace::CreateRefImage(image);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetColor(pen.GetColor4f(), c);
    pen.SetWidth(10);
    pen.SetShaderEffect(e);
    canvas.AttachPen(pen);
    canvas.DrawImage(image, 500, 500, sampling);

    LOGI("------- TestDrawImage");
}

void TestMatrix(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestMatrix");
    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    canvas.AttachBrush(brush);
    canvas.DrawRect({1200, 100, 1500, 400});

    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachBrush(brush);
    Matrix m;
    m.Scale(0.5, 1.5, 0, 0);

    canvas.SetMatrix(m);
    canvas.DrawRect({1000, 0, 1300, 300});

    Matrix n;
    n.SetMatrix(1, 0, 100,
         0, 1, 300,
         0, 0, 1);
    brush.SetColor(Drawing::Color::COLOR_GREEN);
    canvas.AttachBrush(brush);
    canvas.SetMatrix(n);
    canvas.DrawRect({1200, 100, 1500, 400});

    Matrix m1;
    Matrix m2;
    m1.Translate(100, 300);
    m2.Rotate(45, 0, 0);
    m = m1 * m2;

    brush.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachBrush(brush);
    canvas.SetMatrix(m);
    canvas.DrawRect({1200, 100, 1500, 400});

    Matrix a;
    a.SetMatrix(1, 1, 0, 1, 0, 0, 0, 0, 0);
    Matrix b;
    b.SetMatrix(1, 1, 0, 1, 0, 0, 0, 0, 0);
    Matrix c;
    c.SetMatrix(1, 1, 0, 1, 0, 0, 0, 1, 0);

    if (a == b) {
        LOGI("+++++++ matrix a is equals to matrix b");
    } else {
        LOGI("+++++++ matrix a is not equals to matrix b");
    }

    if (a == c) {
        LOGI("+++++++ matrix a is equals to matrix c");
    } else {
        LOGI("+++++++ matrix a is not equals to matrix c");
    }

    LOGI("------- TestMatrix");
}

void TestCamera(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestCamera");

    Brush brush;
    Matrix m0;
    m0.Translate(width / 2.0, height / 2.0);
    canvas.SetMatrix(m0);

    Camera3D camera;
    camera.RotateXDegrees(-25);
    camera.RotateYDegrees(45);
    camera.Translate(-50, 50, 50);
    Drawing::Rect r(0, 0, 500, 500);

    canvas.Save();
    camera.Save();
    camera.RotateYDegrees(90);
    Matrix m1;
    camera.ApplyToMatrix(m1);
    camera.Restore();
    canvas.ConcatMatrix(m1);
    brush.SetColor(Drawing::Color::COLOR_LTGRAY);
    canvas.AttachBrush(brush);
    canvas.DrawRect(r);
    canvas.Restore();
    LOGI("------- TestCamera");
}

void TestDrawShader(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawShader");

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    std::vector<ColorQuad> colors{Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_RED};
    std::vector<scalar> pos{0.0, 0.5, 1.0};
    auto e = ShaderEffect::CreateLinearGradient({1200, 700}, {1300, 800}, colors, pos, TileMode::MIRROR);
    brush.SetShaderEffect(e);

    canvas.AttachBrush(brush);
    canvas.DrawRect({1200, 700, 1500, 1000});
    LOGI("------- TestDrawShader");
}

void TestDrawShadow(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawShadow");

    Path path;
    path.AddOval({200, 200, 600, 1000});
    Point3 planeParams = { 540.0, 0.0, 600.0 };
    Point3 devLightPos = {0, 0, 0};
    scalar lightRadius = 0.5;
    Drawing::Color ambientColor = Drawing::Color::ColorQuadSetARGB(0, 0, 0, 0);
    Drawing::Color spotColor = Drawing::Color::COLOR_RED;
    ShadowFlags flag = ShadowFlags::TRANSPARENT_OCCLUDER;
    canvas.DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    LOGI("------- TestDrawShadow");
}

std::unique_ptr<PixelMap> ConstructPixmap()
{
    int32_t pixelMapWidth = 50;
    int32_t pixelMapHeight = 50;
    std::unique_ptr<PixelMap> pixelMap = std::make_unique<PixelMap>();
    ImageInfo info;
    info.size.width = pixelMapWidth;
    info.size.height = pixelMapHeight;
    info.pixelFormat = Media::PixelFormat::RGBA_8888;
    info.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    info.colorSpace = Media::ColorSpace::SRGB;
    pixelMap->SetImageInfo(info);
    LOGI("Constructed pixelMap info: width = %{public}d, height = %{public}d, pixelformat = %{public}d, alphatype = %{public}d, colorspace = %{public}d",
        info.size.width, info.size.height, info.pixelFormat, info.alphaType, info.colorSpace);

    int32_t rowDataSize = pixelMapWidth;
    uint32_t bufferSize = rowDataSize * pixelMapHeight;
    void *buffer = malloc(bufferSize);
    if (buffer == nullptr) {
        LOGE("alloc memory size:[%{public}d] error.", bufferSize);
        return nullptr;
    }
    char *ch = (char *)buffer;
    for (unsigned int i = 0; i < bufferSize; i++) {
        *(ch++) = (char)i;
    }

    pixelMap->SetPixelsAddr(buffer, nullptr, bufferSize, AllocatorType::HEAP_ALLOC, nullptr);

    return pixelMap;
}

void TestDrawPixelmap(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPixelmap");
    std::unique_ptr<PixelMap> pixelmap = ConstructPixmap();

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetWidth(10);
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachBrush(brush);

    canvas.DrawBitmap(*pixelmap.get(), 500, 500);
    LOGI("------- TestDrawPixelmap");
}

void DoDraw(uint8_t *addr, uint32_t width, uint32_t height, size_t index)
{
    Bitmap bitmap;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUYE};
    bitmap.Build(width, height, format);

    Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(Drawing::Color::COLOR_WHITE);

    testFuncVec[index](canvas, width, height);

    constexpr uint32_t stride = 4;
    int32_t addrSize = width * height * stride;
    auto ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        LOGI("memcpy_s failed");
    }
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
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
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

int main()
{
    testFuncVec.push_back(TestDrawPathPro);
    testFuncVec.push_back(TestCamera);
    testFuncVec.push_back(TestDrawBase);
    testFuncVec.push_back(TestDrawPath);
    testFuncVec.push_back(TestDrawPathEffect);
    testFuncVec.push_back(TestDrawBitmap);
    testFuncVec.push_back(TestDrawImage);
    testFuncVec.push_back(TestDrawFilter);
    testFuncVec.push_back(TestDrawShader);
    testFuncVec.push_back(TestDrawShadow);
    testFuncVec.push_back(TestDrawPixelmap);
    testFuncVec.push_back(TestMatrix);

    auto surfaceNode = CreateSurface();
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    for (size_t i = 0; i < testFuncVec.size(); i++) {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            continue;
        }
        sleep(2);
        displayNode->AddChild(surfaceNode, -1);
        surfaceNode->SetBounds(0, 0, 2560, 1600);
        transactionProxy->FlushImplicitTransaction();
        DrawSurface(surfaceNode, 2560, 1600, i);
        sleep(4);
        displayNode->RemoveChild(surfaceNode);
        transactionProxy->FlushImplicitTransaction();
    }
    return 0;
}
