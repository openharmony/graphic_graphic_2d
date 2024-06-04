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
#include "rscpuoverdrawcanvaslistener_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <securec.h>
#include <unistd.h>

#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"

namespace OHOS {
namespace Rosen {
constexpr size_t SCREEN_WIDTH = 100;
constexpr size_t SCREEN_HEIGHT = 100;
Drawing::Canvas canvas(SCREEN_WIDTH, SCREEN_HEIGHT);
auto rsCpu = std::make_shared<RSCPUOverdrawCanvasListener>(canvas);
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    size_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoDrawPoint(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float x = GetData<float>();
    float y = GetData<float>();
    Drawing::Point point(x, y);
    rsCpu->DrawPoint(point);
    return true;
}

bool DoDrawLine(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float x = GetData<float>();
    float y = GetData<float>();
    Drawing::Point startPt(x, y);

    float a = GetData<float>();
    float b = GetData<float>();
    Drawing::Point endPt(a, b);

    rsCpu->DrawLine(startPt, endPt);
    return true;
}

bool DoRect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float left = GetData<float>();
    float right = GetData<float>();
    float top = GetData<float>();
    float bottom = GetData<float>();
    Drawing::Rect rect(left, right, top, bottom);
    rsCpu->DrawRect(rect);
    return true;
}

bool DoDrawRoundRect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float left = GetData<float>();
    float right = GetData<float>();
    float top = GetData<float>();
    float bottom = GetData<float>();
    Drawing::Rect rect(left, right, top, bottom);

    float xRad = GetData<float>();
    float yRad = GetData<float>();
    Drawing::RoundRect roundRect(rect, xRad, yRad);

    rsCpu->DrawRoundRect(roundRect);
    return true;
}

bool DoDrawNestedRoundRect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float left = GetData<float>();
    float right = GetData<float>();
    float top = GetData<float>();
    float bottom = GetData<float>();
    Drawing::Rect rect(left, right, top, bottom);

    float xRad = GetData<float>();
    float yRad = GetData<float>();
    Drawing::RoundRect outer(rect, xRad, yRad);

    left = GetData<float>();
    right = GetData<float>();
    top = GetData<float>();
    bottom = GetData<float>();
    Drawing::Rect rectInner(left, right, top, bottom);

    xRad = GetData<float>();
    yRad = GetData<float>();
    Drawing::RoundRect inner(rectInner, xRad, yRad);

    rsCpu->DrawNestedRoundRect(outer, inner);
    return true;
}

bool DoDrawArc(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float left = GetData<float>();
    float right = GetData<float>();
    float top = GetData<float>();
    float bottom = GetData<float>();
    Drawing::Rect rect(left, right, top, bottom);

    float startAngle = GetData<float>();
    float sweepAngle = GetData<float>();

    rsCpu->DrawArc(rect, startAngle, sweepAngle);
    return true;
}

bool DoDrawPie(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float left = GetData<float>();
    float right = GetData<float>();
    float top = GetData<float>();
    float bottom = GetData<float>();
    Drawing::Rect rect(left, right, top, bottom);

    float startAngle = GetData<float>();
    float sweepAngle = GetData<float>();

    rsCpu->DrawPie(rect, startAngle, sweepAngle);
    return true;
}

bool DoDrawOval(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float left = GetData<float>();
    float right = GetData<float>();
    float top = GetData<float>();
    float bottom = GetData<float>();
    Drawing::Rect rect(left, right, top, bottom);

    rsCpu->DrawOval(rect);
    return true;
}

bool DoDrawCircle(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    float x = GetData<float>();
    float y = GetData<float>();
    Drawing::Point point(x, y);

    float radius = GetData<float>();
    rsCpu->DrawCircle(point, radius);
    return true;
}

bool DoDrawPath(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Drawing::Path path;
    rsCpu->DrawPath(path);
    return true;
}

bool DoDrawBackground(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    int rgba = GetData<int>();
    Drawing::Brush brush(rgba);
    rsCpu->DrawBackground(brush);
    return true;
}

bool DoDrawShadow(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Drawing::Path path;

    float x = GetData<float>();
    float y = GetData<float>();
    float z = GetData<float>();
    Drawing::Point3 planeParams(x, y, z);

    x = GetData<float>();
    y = GetData<float>();
    z = GetData<float>();
    Drawing::Point3 devLightPos(x, y, z);

    float lightRadius = GetData<float>();

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    Drawing::Color ambientColor(r, g, b, a);

    r = GetData<uint32_t>();
    g = GetData<uint32_t>();
    b = GetData<uint32_t>();
    a = GetData<uint32_t>();
    Drawing::Color spotColor(r, g, b, a);

    int value = GetData<int>();
    auto flag = (Drawing::ShadowFlags)value;

    rsCpu->DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    return true;
}

bool DoDrawRegion(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Drawing::Region region;
    rsCpu->DrawRegion(region);
    return true;
}

bool DoDrawTextBlob(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Drawing::TextBlob* blob = nullptr;
    float x = GetData<float>();
    float y = GetData<float>();
    rsCpu->DrawTextBlob(blob, x, y);
    return true;
}

bool DoDrawBitmap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Drawing::Bitmap bitmap;
    float x = GetData<float>();
    float y = GetData<float>();
    rsCpu->DrawBitmap(bitmap, x, y);
    return true;
}

bool DoDrawImage(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Drawing::Image image;
    Drawing::SamplingOptions sampling;

    float x = GetData<float>();
    float y = GetData<float>();
    rsCpu->DrawImage(image, x, y, sampling);
    return true;
}

bool DoDrawImageRect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Drawing::Image image;

    float left = GetData<float>();
    float right = GetData<float>();
    float top = GetData<float>();
    float bottom = GetData<float>();
    Drawing::Rect src(left, right, top, bottom);

    left = GetData<float>();
    right = GetData<float>();
    top = GetData<float>();
    bottom = GetData<float>();
    Drawing::Rect dst(left, right, top, bottom);

    Drawing::SamplingOptions sampling;

    rsCpu->DrawImageRect(image, src, dst, sampling);
    rsCpu->DrawImageRect(image, dst, sampling);
    return true;
}

bool DoDrawPicture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Drawing::Picture picture;
    rsCpu->DrawPicture(picture);
    return true;
}

bool DoClear(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    uint32_t color = GetData<uint32_t>();
    rsCpu->Clear(color);
    return true;
}

bool DoAppendRegion(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Drawing::Path path;
    rsCpu->AppendRegion(path);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoDrawPoint(data, size);
    OHOS::Rosen::DoDrawLine(data, size);
    OHOS::Rosen::DoRect(data, size);
    OHOS::Rosen::DoDrawRoundRect(data, size);
    OHOS::Rosen::DoDrawNestedRoundRect(data, size);
    OHOS::Rosen::DoDrawArc(data, size);
    OHOS::Rosen::DoDrawPie(data, size);
    OHOS::Rosen::DoDrawOval(data, size);
    OHOS::Rosen::DoDrawCircle(data, size);
    OHOS::Rosen::DoDrawPath(data, size);
    OHOS::Rosen::DoDrawBackground(data, size);
    OHOS::Rosen::DoDrawShadow(data, size);
    OHOS::Rosen::DoDrawRegion(data, size);
    OHOS::Rosen::DoDrawTextBlob(data, size);
    OHOS::Rosen::DoDrawBitmap(data, size);
    OHOS::Rosen::DoDrawImage(data, size);
    OHOS::Rosen::DoDrawImageRect(data, size);
    OHOS::Rosen::DoDrawPicture(data, size);
    OHOS::Rosen::DoClear(data, size);
    OHOS::Rosen::DoAppendRegion(data, size);
    return 0;
}