/*
 * Copyright (c) 2024-2026 Huawei Device Co., Ltd.
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

#include "canvas_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>

#include "get_object.h"

#include "draw/canvas.h"
#include "draw/color.h"
#include "draw/core_canvas.h"
#include "effect/particle_builder.h"
#include "image/gpu_context.h"
#include "image/picture.h"
#include "text/glyph_cache.h"
#include "text/font.h"
#include "text/hm_symbol.h"
#include "text/rs_xform.h"
#include "utils/matrix.h"
#include "utils/point3.h"
#include "utils/round_rect.h"
#include "utils/sampling_options.h"
#include "utils/scalar.h"
#include "utils/vertices.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr int32_t BITMAP_WIDTH = 300;
static constexpr int32_t BITMAP_HEIGHT = 300;
static constexpr size_t MAX_SIZE = 5000;
constexpr size_t COLORFORMAT_SIZE = 12;
constexpr size_t ALPHAFORMAT_SIZE = 4;
static constexpr int32_t BITMAP_WIDTH_MAX_SIZE = 5000;
static constexpr int32_t BITMAP_HEIGHT_MAX_SIZE = 5000;
static constexpr int32_t RGBA_CHANNELS_NUM = 4;

class DDGRSDFShapeBaseTest : public Drawing::SDFShapeBase {
public:
    const std::string& Getshader() const override
    {
        return shaderCode_;
    }

    void SetShader(const std::string& shader)
    {
        static const std::string prefix(R"(uniform float width;
            vec4 fillStroke(float d, float th, vec3 fillCol, vec3 strokeCol)
            {
                float w = 2.0 / width;
                float a1 = step(0., -d);
                float a2 = smoothstep(-w-th, -th, d) * smoothstep(w-th, th, d);
                vec3 col = mix(fillCol, strokeCol, a2);
                float a = max(a1, a2);
                return vec4(col, a);
            }
            vec4 shading(float d)
            {
                const vec3 fillCol = vec3(0.85, 0.8, 0.95);
                const vec3 strokeCol = vec3(0.9, 0.9, 1.);
                const float thickness = 0.001; // thickness of stroke
                vec4 col = vec4(0.);
                col = fillStroke(d, thickness, fillCol, strokeCol);
                return col;
            })");
        shaderCode_ = prefix + shader;
    }

    float GetSize() const override
    {
        return 0.0f;
    }
    float GetTranslateX() const override
    {
        return 0.0f;
    }
    float GetTranslateY() const override
    {
        return 0.0f;
    }
    int GetParaNum() const override
    {
        return 0;
    }
    int GetFillType() const override
    {
        return 0;
    }
    std::vector<float> GetPara() const override
    {
        return {};
    }
    std::vector<float> GetTransPara() const override
    {
        return {};
    }
    std::vector<float> GetColorPara() const override
    {
        return {};
    }
    void SetBoundsRect(const Rect&) override {}
    Rect GetBoundsRect() const override
    {
        Rect rect(1, 1, 1, 1);
        return rect;
    }
    int GetShapeId() const override
    {
        return 0;
    }
    void SetTransparent(int) override {}
    int GetTransparent() const override
    {
        return 0;
    }
    std::vector<float> GetPaintPara() const override
    {
        return {};
    }
    std::vector<float> GetPointAndColorPara() const override
    {
        return {};
    }
    void UpdateTime(float) override {}
    void BuildShader() override {}
    void SetSize(float) override {}
    void SetTranslate(float, float) override {}

private:
    std::string shaderCode_;
};

bool CanvasFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    uint32_t count = GetObject<uint32_t>();
    bool flag = GetObject<bool>();
    canvas.AddCanvas(&canvas);
    canvas.RemoveAll();
    canvas.RestoreToCount(count);
    canvas.GetRecordingState();
    canvas.SetRecordingState(flag);
    std::shared_ptr<Drawing::Canvas> cCanvas;
    OverDrawCanvas overDrawCanvas = OverDrawCanvas(cCanvas);
    overDrawCanvas.GetDrawingType();
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    NoDrawCanvas noDrawCanvas = NoDrawCanvas(width, height);
    noDrawCanvas.GetDrawingType();
    bool doSave = GetObject<bool>();
    AutoCanvasRestore(canvas, doSave);
    return true;
}

bool CanvasFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    uint32_t count = GetObject<uint32_t>();
    bool flag = GetObject<bool>();
    canvas.AddCanvas(&canvas);
    canvas.RemoveAll();
    canvas.RestoreToCount(count);
    canvas.GetRecordingState();
    canvas.SetRecordingState(flag);
    std::shared_ptr<Drawing::Canvas> cCanvas;
    OverDrawCanvas overDrawCanvas = OverDrawCanvas(cCanvas);
    overDrawCanvas.GetDrawingType();
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    NoDrawCanvas noDrawCanvas = NoDrawCanvas(width, height);
    noDrawCanvas.GetDrawingType();

    Bitmap bmp;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE};
    bmp.Build(BITMAP_WIDTH, BITMAP_HEIGHT, format); // bitmap width and height
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    Image image;
    image.BuildFromBitmap(bmp);
    // DrawImageEffectHPS
    Rect rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    scalar sigma = GetObject<scalar>();
    float saturation = GetObject<float>();
    float brightness = GetObject<float>();
    std::vector<std::shared_ptr<HpsEffectParameter>> hpsEffectParams;
    hpsEffectParams.push_back(std::make_shared<Drawing::HpsBlurEffectParameter>
        (rect, rect, sigma, saturation, brightness));
    canvas.DrawImageEffectHPS(image, hpsEffectParams);

    bool doSave = GetObject<bool>();
    AutoCanvasRestore(canvas, doSave);
    return true;
}

bool CanvasFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    uint32_t count = GetObject<uint32_t>();
    bool flag = GetObject<bool>();
    canvas.AddCanvas(&canvas);
    canvas.RemoveAll();
    canvas.RestoreToCount(count);
    canvas.GetRecordingState();
    canvas.SetRecordingState(flag);
    std::shared_ptr<Drawing::Canvas> cCanvas;
    OverDrawCanvas overDrawCanvas = OverDrawCanvas(cCanvas);
    overDrawCanvas.GetDrawingType();
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    NoDrawCanvas noDrawCanvas = NoDrawCanvas(width, height);
    noDrawCanvas.GetDrawingType();

    Bitmap bmp;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE};
    bmp.Build(BITMAP_WIDTH, BITMAP_HEIGHT, format); // bitmap width and height
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    Image image;
    image.BuildFromBitmap(bmp);
    // GetLocalShadowBound
    Rect rect;
    Path path;
    Rect oval = GetObject<Rect>();
    path.AddOval(oval, GetObject<PathDirection>());
    path.Close();

    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    scalar sz = GetObject<scalar>();
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    scalar dz = GetObject<scalar>();
    scalar tx = GetObject<scalar>();
    scalar ty = GetObject<scalar>();
    scalar tz = GetObject<scalar>();
    Matrix matrix;
    matrix.SetMatrix(tx, ty, tz, sx, sy, sz, dx, dy, dz);

    Point3 planeParams{GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>()};
    Point3 devLightPos{GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>()};
    scalar lightRadius = GetObject<scalar>();
    ShadowFlags shadowFlag = ShadowFlags::ALL;
    bool isLimitElevation = false;

    canvas.GetLocalShadowBounds(matrix, path, planeParams, devLightPos, lightRadius, shadowFlag, isLimitElevation,
        rect);

    return true;
}

bool CanvasFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    Canvas canvas;
    Bitmap bitmap;
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    BitmapFormat format { static_cast<ColorType>(colorType % COLORFORMAT_SIZE),
                          static_cast<AlphaType>(alphaType % ALPHAFORMAT_SIZE) };
    if (!bitmap.Build(BITMAP_WIDTH, BITMAP_HEIGHT, format)) { // bitmap width and height
        return false;
    }
    canvas.Bind(bitmap);
    canvas.IsOpaque();
    return true;
}

bool CanvasFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    BuilderFlags flags = BuilderFlags::HAS_COLORS_BUILDER_FLAG;
    int32_t indexCount = 0;
    int32_t vertexCount = 3;
    Point points[] = { { GetObject<scalar>(), GetObject<scalar>() }, { GetObject<scalar>(), GetObject<scalar>() },
        { GetObject<scalar>(), GetObject<scalar>() } };
    Brush brush;
    brush.SetColor(GetObject<ColorQuad>());
    Drawing::Vertices::Builder builder(
        Drawing::VertexMode::TRIANGLES_VERTEXMODE, vertexCount, indexCount, static_cast<uint32_t>(flags));
    if (!builder.IsValid()) {
        return false;
    }
    Drawing::Point* posPtr = builder.Positions();
    if (posPtr == nullptr) {
        return false;
    }
    for (int32_t i = 0; i < vertexCount; i++) {
        posPtr[i] = points[i] + Drawing::Point(GetObject<scalar>(), GetObject<scalar>());
    }
    canvas.AttachBrush(brush);
    canvas.DrawVertices(*builder.Detach(), Drawing::BlendMode::DST);
    canvas.DetachBrush();

    // OpCalculateAfter
    auto rect = Rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    canvas.OpCalculateAfter(rect);

    Matrix matrix;
    canvas.OpCalculateBefore(matrix);

    canvas.QuickReject(rect);

    bool doSave = GetObject<bool>();
    AutoCanvasRestore(canvas, doSave);
    return true;
}

bool CanvasFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    std::shared_ptr<Drawing::Canvas> canvasPtr = std::make_shared<Drawing::Canvas>();
    OverDrawCanvas canvas2(canvasPtr);
    std::shared_ptr<GPUContext> gpuContext = std::make_shared<GPUContext>();
    canvas2.SetGrContext(gpuContext);

    // BuildOverDraw
    canvas.BuildOverDraw(canvasPtr);

    Path path;
    path.MoveTo(GetObject<scalar>(), GetObject<scalar>());
    Path path1;
    path1.MoveTo(GetObject<scalar>(), GetObject<scalar>());
    path1.LineTo(GetObject<scalar>(), GetObject<scalar>());
    path = path1;

    bool doSave = GetObject<bool>();
    AutoCanvasRestore(canvas, doSave);
    return true;
}

bool CanvasFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Rect rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    scalar sigma = GetObject<scalar>();
    float saturation = GetObject<float>();
    float brightness = GetObject<float>();
    HpsBlurParameter hpsArgs(rect, rect, sigma, saturation, brightness);
    Canvas canvas;
    canvas.CalcHpsBluredImageDimension(hpsArgs);

    // DrawAtlas
    Bitmap bmp;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    if (!bmp.Build(static_cast<int32_t>(GetObject<uint32_t>() % BITMAP_WIDTH),
        static_cast<int32_t>(GetObject<uint32_t>() % BITMAP_HEIGHT), format)) {
        return false;
    }
    bmp.ClearWithColor(GetObject<ColorQuad>());
    Image image;
    image.BuildFromBitmap(bmp);
    Brush brush;
    brush.SetAntiAlias(true);
    RSXform rsxform[] = { RSXform::Make(
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>()) };
    Rect tex[] = { rect };
    ColorQuad colors[] = { GetObject<ColorQuad>() };
    canvas.AttachBrush(brush);
    canvas.DrawAtlas(
        &image, rsxform, tex, colors, 1, Drawing::BlendMode::SRC_IN, SamplingOptions(FilterMode::NEAREST), nullptr);
    canvas.DrawBlurImage(image, hpsArgs);
    canvas.DetachBrush();
    bool doSave = GetObject<bool>();
    AutoCanvasRestore(canvas, doSave);
    return true;
}

bool CanvasFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    Matrix matrix;
    matrix.SetMatrix(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    Path path;
    path.MoveTo(GetObject<scalar>(), GetObject<scalar>());
    Point3 planeParams { GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>() };
    Point3 devLightPos { GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>() };
    scalar lightRadius = GetObject<scalar>();
    ShadowFlags flag = GetObject<ShadowFlags>();
    bool isLimitElevation = GetObject<bool>();
    auto rect = Rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    canvas.GetLocalShadowBounds(matrix, path, planeParams, devLightPos, lightRadius, flag, isLimitElevation, rect);
    bool doSave = GetObject<bool>();
    AutoCanvasRestore(canvas, doSave);
    return true;
}

bool CanvasFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    Brush brush;
    brush.SetColor(GetObject<ColorQuad>());
    canvas.AttachBrush(brush);
    auto p = Point(GetObject<scalar>(), GetObject<scalar>());
    canvas.DrawCircle(p, GetObject<scalar>());
    canvas.DrawColor(GetObject<ColorQuad>());

    Bitmap bmp;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bmp.Build(GetObject<uint32_t>() % BITMAP_WIDTH, GetObject<uint32_t>() % BITMAP_HEIGHT,
        format); // bitmap width and height
    bmp.ClearWithColor(GetObject<ColorQuad>());
    Image image;
    image.BuildFromBitmap(bmp);
    SamplingOptions sampling = SamplingOptions(FilterMode::NEAREST, MipmapMode::NEAREST);
    canvas.DrawImage(image, GetObject<scalar>(), GetObject<scalar>(), sampling);

    Lattice lattice {};
    auto rect = Rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    canvas.DrawImageLattice(&image, lattice, rect, FilterMode::LINEAR);

    canvas.DrawImageNine(&image,
        RectI(GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>()), rect,
        FilterMode::NEAREST);

    RoundRect rrect = RoundRect(rect, GetObject<scalar>(), GetObject<scalar>());
    RoundRect rrect1 = RoundRect(rect, GetObject<scalar>(), GetObject<scalar>());
    canvas.DrawNestedRoundRect(rrect, rrect1);

    Point cubics[12] = { { GetObject<scalar>(), GetObject<scalar>() }, { GetObject<scalar>(), GetObject<scalar>() },
        { GetObject<scalar>(), GetObject<scalar>() }, { GetObject<scalar>(), GetObject<scalar>() },
        { GetObject<scalar>(), GetObject<scalar>() }, { GetObject<scalar>(), GetObject<scalar>() },
        { GetObject<scalar>(), GetObject<scalar>() }, { GetObject<scalar>(), GetObject<scalar>() },
        { GetObject<scalar>(), GetObject<scalar>() }, { GetObject<scalar>(), GetObject<scalar>() },
        { GetObject<scalar>(), GetObject<scalar>() }, { GetObject<scalar>(), GetObject<scalar>() } };
    ColorQuad colors[RGBA_CHANNELS_NUM] = { GetObject<ColorQuad>(), GetObject<ColorQuad>(), GetObject<ColorQuad>(),
        GetObject<ColorQuad>() };
    Point texCoords[4] = { { GetObject<scalar>(), GetObject<scalar>() }, { GetObject<scalar>(), GetObject<scalar>() },
        { GetObject<scalar>(), GetObject<scalar>() }, { GetObject<scalar>(), GetObject<scalar>() } };
    canvas.DrawPatch(cubics, colors, texCoords, BlendMode::PLUS);
    std::shared_ptr<Picture> picture = std::make_shared<Picture>();
    canvas.DrawPicture(*picture.get());
    bool doSave = GetObject<bool>();
    AutoCanvasRestore(canvas, doSave);
    return true;
}

bool CanvasFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;

    uint32_t count = GetObject<uint32_t>();
    bool flag = GetObject<bool>();
    canvas.AddCanvas(&canvas);
    canvas.RemoveAll();
    canvas.RestoreToCount(count);
    canvas.GetRecordingState();
    canvas.SetRecordingState(flag);
    Bitmap bmp;
    int32_t width = static_cast<int32_t>(GetObject<uint32_t>() % BITMAP_WIDTH_MAX_SIZE);
    int32_t height = static_cast<int32_t>(GetObject<uint32_t>() % BITMAP_HEIGHT_MAX_SIZE);
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    if (!bmp.Build(width, height, format)) {
        return false;
    }
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    canvas.Bind(bmp);
    return true;
}

bool CanvasFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    uint32_t count = GetObject<uint32_t>();
    bool flag = GetObject<bool>();
    canvas.AddCanvas(&canvas);
    canvas.RemoveAll();
    canvas.RestoreToCount(count);
    canvas.GetRecordingState();
    canvas.SetRecordingState(flag);
    Bitmap bmp;
    int32_t width = static_cast<int32_t>(GetObject<uint32_t>() % BITMAP_WIDTH_MAX_SIZE);
    int32_t height = static_cast<int32_t>(GetObject<uint32_t>() % BITMAP_HEIGHT_MAX_SIZE);
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    if (!bmp.Build(width, height, format)) {
        return false;
    }
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    void* pixels = bmp.GetPixels();
    if (pixels == nullptr) {
        return false;
    }
    canvas.ReadPixels(bmp.GetImageInfo(), pixels, static_cast<size_t>(bmp.GetRowBytes()), 0, 0);

    return true;
}

bool CanvasFuzzTest011(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    uint32_t count = GetObject<uint32_t>();
    bool flag = GetObject<bool>();
    canvas.AddCanvas(&canvas);
    canvas.RemoveAll();
    canvas.RestoreToCount(count);
    canvas.GetRecordingState();
    canvas.SetRecordingState(flag);
    Bitmap bmp;
    int32_t width = static_cast<int32_t>(GetObject<uint32_t>() % BITMAP_WIDTH_MAX_SIZE);
    int32_t height = static_cast<int32_t>(GetObject<uint32_t>() % BITMAP_HEIGHT_MAX_SIZE);
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    if (!bmp.Build(width, height, format)) {
        return false;
    }
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    canvas.DrawBitmap(bmp, 0, 0);

    return true;
}

bool CanvasFuzzTest012(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    auto rect = Rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    canvas.DrawPie(rect, GetObject<scalar>(), GetObject<scalar>());
    canvas.DrawPoint(Point(GetObject<scalar>(), GetObject<scalar>()));
    DDGRSDFShapeBaseTest shape;
    static const std::string str(
        R"(float sdCircle(vec2 p, float r) {
            return length(p) - r;
        }

        vec4 main(vec2 drawing_coord)
        {
            vec2 p = drawing_coord.xy / width;
            p.y = 1.0 - p.y; // Flip y-coordinate
            p -= 0.5;
            p *= 2.0;
            vec2 p0 = p;
            float d1 = sdCircle(p0, 0.6);
            vec4 col = shading(d1);
            return vec4(col.rgb * col.a, col.a);
        })");
    shape.SetShader(str);
    canvas.DrawSdf(shape);

    DrawingHMSymbolData drawingHMSymbolData;
    Path path;
    drawingHMSymbolData.path_ = path;
    DrawingRenderGroup group;
    int32_t max = 5000;
    std::vector<size_t> layerIndex { GetObject<size_t>() % max };
    std::vector<size_t> maskIndex { GetObject<size_t>() % max };
    DrawingGroupInfo info = { layerIndex, maskIndex };
    group.groupInfos = { info };
    drawingHMSymbolData.symbolInfo_.renderGroups = { group };
    Point locate;
    canvas.DrawSymbol(drawingHMSymbolData, locate);

    bool doSave = GetObject<bool>();
    AutoCanvasRestore(canvas, doSave);
    return true;
}

bool CanvasFuzzTestHpsEdgeLight(const uint8_t* data, size_t size)
{
    Canvas canvas;
    Bitmap bmp;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE};
    bmp.Build(BITMAP_WIDTH, BITMAP_HEIGHT, format); // bitmap width and height
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    Image image;
    image.BuildFromBitmap(bmp);
    // Edge Light
    Rect rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    std::vector<float> edgeColor = { GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>() };
    std::vector<float> colors = { GetObject<float>(), GetObject<float>(), GetObject<float>() };
    std::vector<float> positions = { GetObject<float>(), GetObject<float>(), GetObject<float>() };
    auto hpsRadialMaskArgs = std::make_shared<Drawing::HpsRadialGradientMaskParameter>(GetObject<float>(),
        GetObject<float>(), GetObject<float>(), GetObject<float>(), colors, positions);
    std::vector<float> edgeDetectColor = { GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>() };
    Drawing::HpsEdgeLightParameter::EdgeSobelParameter edgeSobelParams = { GetObject<float>(), GetObject<float>(),
        GetObject<float>(), edgeDetectColor };
    auto hpsRadialArgs = std::make_shared<Drawing::HpsEdgeLightParameter>(rect, rect,
        GetObject<float>(), GetObject<bool>(), GetObject<bool>(), edgeColor,
        std::static_pointer_cast<Drawing::HpsMaskParameter>(hpsRadialMaskArgs), edgeSobelParams, 1);
    std::vector<std::shared_ptr<HpsEffectParameter>> hpsEffectParams;
    hpsEffectParams.push_back(hpsRadialArgs);
    canvas.DrawImageEffectHPS(image, hpsEffectParams);
    return true;
}

bool CanvasFuzzTestInsertOpaqueRegion(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;

    // Generate random opaque rects count (0-10)
    uint32_t rectCount = GetObject<uint32_t>() % 11;
    std::vector<RectI> opaqueRects;

    for (uint32_t i = 0; i < rectCount; i++) {
        int32_t left = GetObject<int32_t>() % 1000;
        int32_t top = GetObject<int32_t>() % 1000;
        int32_t right = GetObject<int32_t>() % 1000;
        int32_t bottom = GetObject<int32_t>() % 1000;
        opaqueRects.emplace_back(left, top, right, bottom);
    }

    // Call InsertOpaqueRegion with generated rects
    canvas.InsertOpaqueRegion(opaqueRects);

    // Test with empty rects
    std::vector<RectI> emptyRects;
    canvas.InsertOpaqueRegion(emptyRects);

    return true;
}

bool CanvasFuzzTestParticle(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    if (!builder) {
        return false;
    }
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    builder->SetUpdateCode(std::string(dataText));
    uint32_t maxParticleCount = GetObject<uint32_t>() % 100000;
    auto effect = builder->MakeParticleEffect(maxParticleCount);
    if (!effect) {
        if (dataText != nullptr) {
            delete [] dataText;
            dataText = nullptr;
        }
        return false;
    }
    canvas.DrawParticle(effect);
    bool doSave = GetObject<bool>();
    AutoCanvasRestore(canvas, doSave);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }

    return true;
}

bool CanvasFuzzTestUIColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    float red = GetObject<float>();
    float green = GetObject<float>();
    float blue = GetObject<float>();
    float alpha = GetObject<float>();
    float headroom = GetObject<float>();
    UIColor uiColor(red, green, blue, alpha, headroom);
    canvas.DrawUIColor(uiColor, BlendMode::SRC_IN);
    return true;
}

bool CanvasFuzzTestDrawSingleCharacterWithFeatures(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    Font font;
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    font.SetTypeface(typeface);
    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* text = new char[length];
    for (size_t i = 0; i < length; i++) {
        text[i] = GetObject<char>();
    }
    text[length - 1] = '\0';
    std::shared_ptr<DrawingFontFeatures> features = std::make_shared<DrawingFontFeatures>();
    canvas.DrawSingleCharacterWithFeatures(text, font, x, y, features);
    canvas.DrawSingleCharacterWithFeatures(text, font, x, y, nullptr);
    if (text != nullptr) {
        delete[] text;
        text = nullptr;
    }
    GlyphCache::Instance().Clear();
    return true;
}

bool CanvasFuzzTestGlyphCache(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    GlyphCache& cache = GlyphCache::Instance();
    cache.Clear();
    uint32_t maxSize = GetObject<uint32_t>();
    cache.SetMaxSize(maxSize);
    int32_t unicode = GetObject<int32_t>();
    uint32_t typefaceHash = GetObject<uint32_t>();
    GlyphCacheKey key = {unicode, typefaceHash, {}};
    uint16_t glyphId = GetObject<uint16_t>();
    cache.Put(key, glyphId);
    uint16_t retrievedValue = 0;
    cache.Get(key, retrievedValue);
    cache.Clear();
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::Drawing::CanvasFuzzTestParticle(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTestUIColor(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest001(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest002(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest003(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest004(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest005(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest006(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest007(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest008(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest009(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest010(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest011(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest012(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTestHpsEdgeLight(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTestInsertOpaqueRegion(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTestDrawSingleCharacterWithFeatures(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTestGlyphCache(data, size);
    return 0;
}
