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

#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <securec.h>
#include <sstream>
#include <unistd.h>

#include <display_type.h>
#include <vsync_helper.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "utils/log.h"
#include "draw/color.h"

#include "image/bitmap.h"

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
using namespace Rosen::Drawing;

using TestFunc = std::function<void(Canvas&, uint32_t, uint32_t)>;

int64_t GetNowTime()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}
void PostTask(std::function<void()> func, uint32_t delayTime = 0);

class Main : public IWindowChangeListenerClazz {
    constexpr static int32_t MAX_TEST_FPS_COUNT = 25;
public:
    void Init(int32_t width, int32_t height);
    int32_t DoDraw(uint8_t *addr, uint32_t width, uint32_t height);
    static void TestDrawBase(Canvas &canvas, uint32_t width, uint32_t height);
    static void TestDrawPathEffect(Canvas &canvas, uint32_t width, uint32_t height);
    static void TestDrawBitmap(Canvas &canvas, uint32_t width, uint32_t height);
    static void TestDrawPixelmap(Canvas &canvas, uint32_t width, uint32_t height);
    static void TestDrawPath(Canvas &canvas, uint32_t width, uint32_t height);
    static void TestDrawFilter(Canvas &canvas, uint32_t width, uint32_t height);
    static void TestMatrix(Canvas &canvas, uint32_t width, uint32_t height);
    static void TestCamera(Canvas &canvas, uint32_t width, uint32_t height);
    static void TestDrawShader(Canvas &canvas, uint32_t width, uint32_t height);
    static void TestDrawShadow(Canvas &canvas, uint32_t width, uint32_t height);
    void Draw();
    void Sync(int64_t time, void *data);
    void OnWindowCreate(int32_t pid, int32_t wid) override;
    void OnWindowDestroy(int32_t pid, int32_t wid) override;

private:
    sptr<Window> window = nullptr;
    std::vector<TestFunc> testFuncVec;
    int32_t testFunIndex = 0;
    int32_t freq = 30;
    int32_t count = 0;
};

void Main::TestDrawBase(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawBase");

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Color::COLOR_RED);
    canvas.AttachPen(pen);

    canvas.DrawLine(Point(0, 0), Point(width, height));

    pen.SetColor(Color::COLOR_BLACK);
    pen.SetWidth(100);
    Point point;
    point.SetX(width / 2.0);
    point.SetY(height / 2.0);
    canvas.AttachPen(pen);
    canvas.DrawPoint(point);

    Brush brush;
    brush.SetColor(Color::COLOR_BLUE);
    canvas.AttachBrush(brush);
    canvas.DrawRect({1200, 100, 1500, 700});
    LOGI("------- TestDrawBase");
}

void Main::TestDrawPathEffect(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPathEffect");

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Color::COLOR_RED);
    pen.SetWidth(10);
    scalar vals[] = {10.0, 20.0};
    pen.SetPathEffect(PathEffect::CreateDashPathEffect(vals, 2, 25));
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Color::COLOR_BLUE);
    canvas.AttachBrush(brush);
    canvas.DrawRect({1200, 300, 1500, 600});

    canvas.DetachPen();
    //canvas.DetachBrush();
    canvas.DrawRect({1200, 700, 1500, 1000});
    LOGI("------- TestDrawPathEffect");
}

void Main::TestDrawFilter(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawFilter");

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Color::COLOR_BLUE);
    pen.SetColor(pen.GetColor4f(), Rosen::Drawing::ColorSpace::CreateSRGBLinear());
    // pen.SetARGB(0x00, 0xFF, 0x00, 0x80);
    pen.SetAlpha(0x44);
    pen.SetWidth(10);
    Filter filter;
    filter.SetColorFilter(ColorFilter::CreateBlendModeColorFilter(Color::COLOR_RED, BlendMode::SRC_ATOP));
    filter.SetMaskFilter(MaskFilter::CreateBlurMaskFilter(BlurType::NORMAL, 10));
    // filter.SetFilterQuality(Filter::FilterQuality::NONE);
    pen.SetFilter(filter);
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Color::COLOR_BLUE);
    brush.SetColor(brush.GetColor4f(), Rosen::Drawing::ColorSpace::CreateSRGBLinear());
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

void Main::TestDrawPath(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPath");
    Path path1;
    path1.MoveTo(200, 200);
    path1.QuadTo(600, 200, 600, 600);
    path1.Close();

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Color::COLOR_GRAY);
    pen.SetWidth(10);
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Color::COLOR_BLUE);
    canvas.AttachBrush(brush);

    Path path2;
    path2.AddOval({200, 200, 600, 1000});

    Path dest;
    dest.Op(path1, path2, PathOp::UNION);
    canvas.DrawPath(dest);
    LOGI("-------- TestDrawPath");
}

void Main::TestDrawBitmap(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawBitmap");
    Bitmap bmp;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUYE};
    bmp.Build(200, 200, format);
    bmp.ClearWithColor(Color::COLOR_BLUE);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Color::COLOR_BLUE);
    pen.SetWidth(10);
    canvas.AttachPen(pen);

    canvas.DrawBitmap(bmp, 500, 500);
    //canvas.DrawRect({1200, 100, 1500, 700});

    LOGI("------- TestDrawBitmap");
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

void Main::TestDrawPixelmap(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPixelmap");
    std::unique_ptr<PixelMap> pixelmap = ConstructPixmap();

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Color::COLOR_BLUE);
    pen.SetWidth(10);
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Color::COLOR_RED);
    canvas.AttachBrush(brush);

    canvas.DrawBitmap(*pixelmap.get(), 500, 500);
    LOGI("------- TestDrawPixelmap");
}

void Main::TestMatrix(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestMatrix");
    Brush brush;
    brush.SetColor(Color::COLOR_BLUE);
    canvas.AttachBrush(brush);
    canvas.DrawRect({1200, 100, 1500, 400});

    brush.SetColor(Color::COLOR_RED);
    canvas.AttachBrush(brush);
    Matrix m;
    m.Scale(0.5, 1.5, 0, 0);

    canvas.SetMatrix(m);
    canvas.DrawRect({1000, 0, 1300, 300});

    Matrix n;
    n.SetMatrix(1, 0, 100,
         0, 1, 300,
         0, 0, 1);
    brush.SetColor(Color::COLOR_GREEN);
    canvas.AttachBrush(brush);
    canvas.SetMatrix(n);
    canvas.DrawRect({1200, 100, 1500, 400});

    Matrix m1;
    Matrix m2;
    m1.Translate(100, 300);
    m2.Rotate(45, 0, 0);
    m = m1 * m2;

    brush.SetColor(Color::COLOR_BLACK);
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

void Main::TestCamera(Canvas &canvas, uint32_t width, uint32_t height)
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
    Rosen::Drawing::Rect r(0, 0, 500, 500);

    canvas.Save();
    camera.Save();
    camera.RotateYDegrees(90);
    Matrix m1;
    camera.ApplyToMatrix(m1);
    camera.Restore();
    canvas.ConcatMatrix(m1);
    brush.SetColor(Color::COLOR_LTGRAY);
    canvas.AttachBrush(brush);
    canvas.DrawRect(r);
    canvas.Restore();
    LOGI("------- TestCamera");
}

void Main::TestDrawShader(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawShader");

    Brush brush;
    brush.SetColor(Color::COLOR_BLUE);
    std::vector<ColorQuad> colors{Color::COLOR_GREEN, Color::COLOR_BLUE, Color::COLOR_RED};
    std::vector<scalar> pos{0.0, 0.5, 1.0};
    auto e = ShaderEffect::CreateLinearGradient({1200, 700}, {1300, 800}, colors, pos, TileMode::MIRROR);
    brush.SetShaderEffect(e);

    canvas.AttachBrush(brush);
    canvas.DrawRect({1200, 700, 1500, 1000});
    LOGI("------- TestDrawShader");
}

void Main::TestDrawShadow(Canvas &canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawShadow");

    Path path;
    path.AddOval({200, 200, 600, 1000});
    Point3 planeParams = { 540.0, 0.0, 600.0 };
    Point3 devLightPos = {0, 0, 0};
    scalar lightRadius = 0.5;
    Color ambientColor = Color::ColorQuadSetARGB(0, 0, 0, 0);
    Color spotColor = Color::COLOR_RED;
    ShadowFlags flag = ShadowFlags::TRANSPARENT_OCCLUDER;
    canvas.DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    LOGI("------- TestDrawShadow");
}

int32_t Main::DoDraw(uint8_t *addr, uint32_t width, uint32_t height)
{
    Bitmap bitmap;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUYE};
    bitmap.Build(width, height, format);

    Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(Color::COLOR_WHITE);

    if (count < MAX_TEST_FPS_COUNT) {
        count++;
        testFuncVec[testFunIndex](canvas, width, height);
    } else {
        count = 0;
        testFunIndex++;
        if (testFunIndex >= testFuncVec.size()) {
            testFunIndex = 0;
        }
    }

    constexpr uint32_t stride = 4;
    int32_t addrSize = width * height * stride;
    auto ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        LOGI("memcpy_s failed");
    }

    return 0;
}

void Main::Draw()
{
    sptr<Surface> surface = window->GetSurface();
    if (surface == nullptr) {
        LOGI("surface is nullptr");
        return;
    }

    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;
    BufferRequestConfig config = {
        .width = surface->GetDefaultWidth(),
        .height = surface->GetDefaultHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = surface->GetDefaultUsage(),
    };

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, config);
    if (ret == SURFACE_ERROR_NO_BUFFER) {
        LOGI("RequestBuffer failed: SURFACE_ERROR_NO_BUFFER");
    }
    if (ret) {
        LOGI("RequestBuffer failed: %{public}s", SurfaceErrorStr(ret).c_str());
    }
    if (buffer == nullptr) {
        LOGI("RequestBuffer failed: buffer is nullptr");
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    DoDraw(addr, buffer->GetWidth(), buffer->GetHeight());
    BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    ret = surface->FlushBuffer(buffer, -1, flushConfig);
}

void Main::Sync(int64_t, void *data)
{
    Draw();
    struct FrameCallback cb = {
        .frequency_ = freq,
        .timestamp_ = 0,
        .userdata_ = data,
        .callback_ = std::bind(&Main::Sync, this, SYNC_FUNC_ARG),
    };

    VsyncError ret = VsyncHelper::Current()->RequestFrameCallback(cb);
    if (ret) {
        LOGI("RequestFrameCallback inner %{public}d\n", ret);
    }
}

void Main::Init(int32_t width, int32_t height)
{
    const auto &wmi = WindowManager::GetInstance();
    auto option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    option->SetWidth(width);
    option->SetHeight(height);
    option->SetX(0);
    option->SetY(0);

    auto wret = wmi->CreateWindow(window, option);
    if (wret != WM_OK || window == nullptr) {
        LOGI("WindowManager::CreateWindow() return %{public}s", WMErrorStr(wret).c_str());
        exit(1);
    }
    window->SwitchTop();

    const auto &wmsc = WindowManagerServiceClient::GetInstance();
    wret = wmsc->Init();
    if (wret != WM_OK) {
        LOGI("WindowManagerServiceClient::Init() return %{public}s", WMErrorStr(wret).c_str());
        exit(1);
    }

    testFuncVec.push_back(Main::TestCamera); 
    testFuncVec.push_back(Main::TestDrawBase);
    testFuncVec.push_back(Main::TestDrawPath);
    testFuncVec.push_back(Main::TestDrawPathEffect);
    testFuncVec.push_back(Main::TestDrawBitmap);
    testFuncVec.push_back(Main::TestDrawPixelmap);
    testFuncVec.push_back(Main::TestMatrix);
    testFuncVec.push_back(Main::TestDrawFilter);
    testFuncVec.push_back(Main::TestDrawShader);
    testFuncVec.push_back(Main::TestDrawShadow);

    std::vector<uint32_t> freqs;
    VsyncHelper::Current()->GetSupportedVsyncFrequencies(freqs);
    if (freqs.size() >= 0x2) {
        freq = freqs[1];
    }

    const auto &wms = wmsc->GetService();
    wms->OnWindowListChange(this);

    Sync(0, nullptr);
}

void Main::OnWindowCreate(int32_t pid, int32_t)
{
}

void Main::OnWindowDestroy(int32_t, int32_t)
{
}

int main(int argc, const char *argv[])
{
    const auto &wmi = WindowManager::GetInstance();
    auto wret = wmi->Init();
    if (wret != WM_OK) {
        LOGI("WindowManager::Init() return %{public}s", WMErrorStr(wret).c_str());
        return 1;
    }

    std::vector<struct WMDisplayInfo> displays;
    wret = wmi->GetDisplays(displays);
    if (wret != WM_OK) {
        LOGI("WindowManager::GetDisplays() return %{public}s", WMErrorStr(wret).c_str());
        return 1;
    }

    if (displays.size() == 0) {
        LOGI("no display, cannot continue");
        return 1;
    }

    LOGI("renderService start");
    Main m;

    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&Main::Init, &m, displays[0].width, displays[0].height));
    runner->Run();
    return 0;
}