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

#include "recording_canvas_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "recording/recording_canvas.h"

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_SIZE = 5000;
} // namespace

namespace Drawing {
void RecordingCanvasFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    bool addDrawOpImmediate = GetObject<bool>();
    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    scalar ex = GetObject<scalar>();
    scalar ey = GetObject<scalar>();
    PointMode mode = GetObject<PointMode>();
    RecordingCanvas recordcanvas = RecordingCanvas(width, height);
    RecordingCanvas recordcanvas1 = RecordingCanvas(width, height, addDrawOpImmediate);
    recordcanvas.GetDrawCmdList();
    recordcanvas1.GetDrawCmdList();
    recordcanvas.GetDrawingType();
    recordcanvas1.GetDrawingType();
    recordcanvas.GetGPUContext();
    recordcanvas1.GetGPUContext();
    recordcanvas.Clear();
    recordcanvas1.Clear();
    recordcanvas.Reset(width, height);
    recordcanvas1.Reset(width, height);
    recordcanvas.Reset(width, height, addDrawOpImmediate);
    recordcanvas1.Reset(width, height, addDrawOpImmediate);
    Point point(x, y);
    recordcanvas.DrawPoint(point);
    recordcanvas1.DrawPoint(point);
    std::vector<Point> pts = { { x, y }, { x, y } };
    recordcanvas.DrawPoints(mode, pts.size(), pts.data());
    recordcanvas1.DrawPoints(mode, pts.size(), pts.data());
    Point point1(ex, ey);
    recordcanvas.DrawLine(point, point1);
    recordcanvas1.DrawLine(point, point1);
    recordcanvas.ResetHybridRenderSize(width, height);
    recordcanvas1.ResetHybridRenderSize(width, height);
}

void RecordingCanvasFuzzTest001(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    bool addDrawOpImmediate = GetObject<bool>();
    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();
    RecordingCanvas recordcanvas = RecordingCanvas(width, height);
    RecordingCanvas recordcanvas1 = RecordingCanvas(width, height, addDrawOpImmediate);
    Rect rect(l, t, r, b);
    recordcanvas.DrawRect(rect);
    recordcanvas1.DrawRect(rect);
    RoundRect roundRect(rect, l, t);
    recordcanvas.DrawRoundRect(roundRect);
    recordcanvas1.DrawRoundRect(roundRect);
    RoundRect inner(rect, r, b);
    recordcanvas.DrawNestedRoundRect(roundRect, inner);
    recordcanvas1.DrawNestedRoundRect(roundRect, inner);
    recordcanvas.DrawArc(rect, l, t);
    recordcanvas1.DrawArc(rect, l, t);
    recordcanvas.DrawPie(rect, l, t);
    recordcanvas1.DrawPie(rect, l, t);
    recordcanvas.DrawOval(rect);
    recordcanvas1.DrawOval(rect);
    Point point(l, t);
    recordcanvas.DrawCircle(point, r);
    recordcanvas1.DrawCircle(point, r);
    Path path;
    path.AddRect(rect);
    recordcanvas.DrawPath(path);
    recordcanvas1.DrawPath(path);
}

void RecordingCanvasFuzzTest002(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    bool addDrawOpImmediate = GetObject<bool>();
    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();
    int il = GetObject<int>();
    int it = GetObject<int>();
    int ir = GetObject<int>();
    int ib = GetObject<int>();
    Color ambientColor = Color(GetObject<ColorQuad>());
    Color spotColor = Color(GetObject<ColorQuad>());
    ShadowFlags flag = GetObject<ShadowFlags>();
    RecordingCanvas recordcanvas = RecordingCanvas(width, height);
    RecordingCanvas recordcanvas1 = RecordingCanvas(width, height, addDrawOpImmediate);
    Brush brush(spotColor);
    recordcanvas.DrawBackground(brush);
    recordcanvas1.DrawBackground(brush);
    Rect rect(l, t, r, b);
    Path path;
    path.AddRect(rect);
    Point3 planeParams(l, t, r);
    Point3 devLightPos(l, r, b);
    recordcanvas.DrawShadow(path, planeParams, devLightPos, l, ambientColor, spotColor, flag);
    recordcanvas1.DrawShadow(path, planeParams, devLightPos, l, ambientColor, spotColor, flag);
    recordcanvas.DrawShadowStyle(path, planeParams, devLightPos, l, ambientColor, spotColor, flag, addDrawOpImmediate);
    recordcanvas1.DrawShadowStyle(path, planeParams, devLightPos, l, ambientColor, spotColor, flag, addDrawOpImmediate);
    Region region;
    region.SetRect(RectI(il, it, ir, ib));
    recordcanvas.DrawRegion(region);
    recordcanvas1.DrawRegion(region);
}

void RecordingCanvasFuzzTest003(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    bool addDrawOpImmediate = GetObject<bool>();
    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    scalar ex = GetObject<scalar>();
    scalar ey = GetObject<scalar>();
    ColorQuad color = GetObject<ColorQuad>();
    uint32_t str_size = GetObject<uint32_t>() % MAX_SIZE + 1;
    RecordingCanvas recordcanvas = RecordingCanvas(width, height);
    RecordingCanvas recordcanvas1 = RecordingCanvas(width, height, addDrawOpImmediate);
    std::vector<Point> cubics = { { x, y }, { ex, ey }, { x, y }, { ex, ey }, { x, y }, { ex, ey }, { x, y },
        { ex, ey }, { x, y }, { ex, ey }, { x, y }, { ex, ey } };
    std::vector<Point> texCoords = { { x, y }, { ex, ey }, { x, y }, { ex, ey } };
    std::vector<ColorQuad> colors = { width, height, width, height };
    BlendMode blendMode = GetObject<BlendMode>();
    recordcanvas.DrawPatch(cubics.data(), colors.data(), texCoords.data(), blendMode);
    recordcanvas1.DrawPatch(cubics.data(), colors.data(), texCoords.data(), blendMode);
    Vertices vertices;
    recordcanvas.DrawVertices(vertices, blendMode);
    recordcanvas1.DrawVertices(vertices, blendMode);
    recordcanvas.DrawColor(color);
    recordcanvas1.DrawColor(color, blendMode);
    Picture pic;
    recordcanvas.DrawPicture(pic);
    recordcanvas1.DrawPicture(pic);
    char* text = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        text[i] = GetObject<char>();
    }
    text[str_size - 1] = '\0';
    Font font;
    std::shared_ptr<TextBlob> blob = TextBlob::MakeFromText(text, str_size, font);
    TextBlob* row_blob = blob.get();
    recordcanvas.DrawTextBlob(row_blob, x, y);
    recordcanvas1.DrawTextBlob(row_blob, x, y);
    recordcanvas1.SetIsCustomTextType(true);
    recordcanvas1.DrawTextBlob(row_blob, x, y);
    recordcanvas.GenerateCachedOpForTextblob(row_blob, x, y);
    Paint paint;
    recordcanvas.GenerateCachedOpForTextblob(row_blob, x, y, paint);
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }
}

void RecordingCanvasFuzzTest004(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    bool addDrawOpImmediate = GetObject<bool>();
    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    scalar ex = GetObject<scalar>();
    scalar ey = GetObject<scalar>();
    int il = GetObject<int>();
    int it = GetObject<int>();
    int ir = GetObject<int>();
    int ib = GetObject<int>();
    FilterMode filterMode = GetObject<FilterMode>();
    SrcRectConstraint constraint = GetObject<SrcRectConstraint>();
    RecordingCanvas recordcanvas = RecordingCanvas(width, height);
    RecordingCanvas recordcanvas1 = RecordingCanvas(width, height, addDrawOpImmediate);
    Bitmap bitmap;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return;
    }
    Image image;
    bool buildImage = image.BuildFromBitmap(bitmap);
    if (!buildImage) {
        return;
    }
    Rect rect(x, y, ex, ey);
    RectI recti(il, it, ir, ib);
    Brush brush;
    recordcanvas.DrawImageNine(&image, recti, rect, filterMode);
    recordcanvas1.DrawImageNine(&image, recti, rect, filterMode, &brush);
    recordcanvas.DrawBitmap(bitmap, x, y);
    recordcanvas1.DrawBitmap(bitmap, x, y);
    SamplingOptions samplingOptions;
    recordcanvas.DrawImage(image, x, y, samplingOptions);
    recordcanvas1.DrawImage(image, x, y, samplingOptions);
    recordcanvas.DrawImageRect(image, rect, rect, samplingOptions);
    recordcanvas1.DrawImageRect(image, rect, rect, samplingOptions, constraint);
    recordcanvas.DrawImageRect(image, rect, samplingOptions);
    recordcanvas1.DrawImageRect(image, rect, samplingOptions);
}

void RecordingCanvasFuzzTest005(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    bool addDrawOpImmediate = GetObject<bool>();
    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();
    int il = GetObject<int>();
    int it = GetObject<int>();
    int ir = GetObject<int>();
    int ib = GetObject<int>();
    ClipOp clipOp = GetObject<ClipOp>();
    RecordingCanvas recordcanvas = RecordingCanvas(width, height);
    RecordingCanvas recordcanvas1 = RecordingCanvas(width, height, addDrawOpImmediate);
    Rect rect(l, t, r, b);
    recordcanvas.ClipRect(rect, clipOp, addDrawOpImmediate);
    recordcanvas1.ClipRect(rect, clipOp, addDrawOpImmediate);
    RectI recti(il, it, ir, ib);
    recordcanvas.ClipIRect(recti, clipOp);
    recordcanvas1.ClipIRect(recti, clipOp);
    RoundRect roundRect(rect, l, t);
    std::vector<Point> pts = { { l, t }, { r, b } };
    recordcanvas.ClipRoundRect(roundRect, clipOp, addDrawOpImmediate);
    recordcanvas1.ClipRoundRect(roundRect, clipOp, addDrawOpImmediate);
    recordcanvas.ClipRoundRect(rect, pts, addDrawOpImmediate);
    recordcanvas1.ClipRoundRect(rect, pts, addDrawOpImmediate);
    Path path;
    path.AddRect(rect);
    recordcanvas.ClipPath(path, clipOp, addDrawOpImmediate);
    recordcanvas1.ClipPath(path, clipOp, addDrawOpImmediate);
    Region region;
    region.SetRect(recti);
    recordcanvas.ClipRegion(region, clipOp);
    recordcanvas1.ClipRegion(region, clipOp);
    recordcanvas.ClipAdaptiveRoundRect(pts);
    recordcanvas1.ClipAdaptiveRoundRect(pts);
}

void RecordingCanvasFuzzTest006(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    bool addDrawOpImmediate = GetObject<bool>();
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    scalar dz = GetObject<scalar>();
    scalar df = GetObject<scalar>();
    ColorQuad color = GetObject<ColorQuad>();
    RecordingCanvas recordcanvas = RecordingCanvas(width, height);
    RecordingCanvas recordcanvas1 = RecordingCanvas(width, height, addDrawOpImmediate);
    Matrix matrix;
    recordcanvas.SetMatrix(matrix);
    recordcanvas1.SetMatrix(matrix);
    recordcanvas.ResetMatrix();
    recordcanvas1.ResetMatrix();
    recordcanvas.ConcatMatrix(matrix);
    matrix.Reset();
    recordcanvas1.ConcatMatrix(matrix);
    recordcanvas.Translate(dx, dy);
    recordcanvas1.Translate(dx, dy);
    recordcanvas.Scale(dx, dy);
    recordcanvas1.Scale(dx, dy);
    recordcanvas.Rotate(dx, dy, dz);
    recordcanvas1.Rotate(dx, dy, dz);
    recordcanvas.Shear(dx, dy);
    recordcanvas1.Shear(dx, dy);
    recordcanvas.Flush();
    recordcanvas1.Flush();
    recordcanvas.Clear(color);
    recordcanvas1.Clear(color);
    recordcanvas.Save();
    recordcanvas1.Save();
    Rect rect(dx, dy, dz, df);
    Brush brush;
    SaveLayerOps saveLayerRec(&rect, &brush);
    recordcanvas.SaveLayer(saveLayerRec);
    recordcanvas1.SaveLayer(saveLayerRec);
    recordcanvas.Restore();
    recordcanvas1.Restore();
    recordcanvas.GetSaveCount();
    recordcanvas1.GetSaveCount();
}

void RecordingCanvasFuzzTest007(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    bool addDrawOpImmediate = GetObject<bool>();
    RecordingCanvas recordcanvas = RecordingCanvas(width, height);
    RecordingCanvas recordcanvas1 = RecordingCanvas(width, height, addDrawOpImmediate);
    recordcanvas.Discard();
    recordcanvas1.Discard();
    recordcanvas.SetIsCustomTextType(addDrawOpImmediate);
    recordcanvas1.SetIsCustomTextType(addDrawOpImmediate);
    recordcanvas.IsCustomTextType();
    recordcanvas1.IsCustomTextType();
    recordcanvas.SetIsCustomTypeface(addDrawOpImmediate);
    recordcanvas1.SetIsCustomTypeface(addDrawOpImmediate);
    recordcanvas.IsCustomTypeface();
    recordcanvas1.IsCustomTypeface();
    recordcanvas.SetGrRecordingContext(nullptr);
    recordcanvas1.SetGrRecordingContext(nullptr);
}

void RecordingCanvasFuzzTest008(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    RecordingCanvas recordcanvas = RecordingCanvas(width, height);
    Drawing::Image image;
    Drawing::Lattice lattice;
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    scalar dz = GetObject<scalar>();
    scalar df = GetObject<scalar>();
    Rect rect(dx, dy, dz, df);
    recordcanvas.DrawImageLattice(&image, lattice, rect, Drawing::FilterMode::LINEAR);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::RecordingCanvasFuzzTest000(data, size);
    OHOS::Rosen::Drawing::RecordingCanvasFuzzTest001(data, size);
    OHOS::Rosen::Drawing::RecordingCanvasFuzzTest002(data, size);
    OHOS::Rosen::Drawing::RecordingCanvasFuzzTest003(data, size);
    OHOS::Rosen::Drawing::RecordingCanvasFuzzTest004(data, size);
    OHOS::Rosen::Drawing::RecordingCanvasFuzzTest005(data, size);
    OHOS::Rosen::Drawing::RecordingCanvasFuzzTest006(data, size);
    OHOS::Rosen::Drawing::RecordingCanvasFuzzTest007(data, size);
    OHOS::Rosen::Drawing::RecordingCanvasFuzzTest008(data, size);
    return 0;
}
