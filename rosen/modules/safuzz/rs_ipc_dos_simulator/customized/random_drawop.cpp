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

#include "customized/random_drawop.h"

#include <random>
#include <vector>
#include <utility>

#include "common/safuzz_log.h"
#include "customized/random_matrix.h"
#include "customized/random_pixel_map.h"
#include "customized/random_typeface.h"
#include "draw/color.h"
#include "draw/paint.h"
#include "draw/brush.h"
#include "draw/blend_mode.h"
#include "draw/core_canvas.h"
#include "draw/clip.h"
#include "effect/color_space.h"
#include "draw/path.h"
#include "utils/matrix.h"
#include "utils/point3.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "utils/region.h"
#include "utils/sampling_options.h"
#include "utils/vertices.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "image/picture.h"
#include "pipeline/rs_draw_cmd.h"
#include "random/random_data.h"
#include "random/random_engine.h"
#include "recording/record_cmd.h"
#include "text/hm_symbol.h"
#include "text/rs_xform.h"
#include "text/typeface.h"
#include "text/text_blob.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static DrawOpItem::Type GetRandomDrawOpType()
{
    static constexpr int DRAWOP_INDEX_MAX = 56;
    int drawOpIndex = RandomEngine::GetRandomIndex(DRAWOP_INDEX_MAX);
    return static_cast<DrawOpItem::Type>(drawOpIndex);
}

static uint32_t GenerateRandomRGBNumber()
{
    static constexpr int RGB_MAX = 255;
    return static_cast<uint32_t>(RandomEngine::GetRandomIndex(RGB_MAX));
}

static Color GetRandomColor()
{
    uint32_t r = GenerateRandomRGBNumber();
    uint32_t g = GenerateRandomRGBNumber();
    uint32_t b = GenerateRandomRGBNumber();
    uint32_t a = GenerateRandomRGBNumber();
    return Color(r, g, b, a);
}

static std::shared_ptr<ColorSpace> GetRandomColorSpace()
{
    std::vector<std::shared_ptr<ColorSpace>> colorSpaceList;
    colorSpaceList.emplace_back(ColorSpace::CreateSRGB());
    colorSpaceList.emplace_back(ColorSpace::CreateSRGBLinear());
    uint32_t CMSTransferFuncTypeIndex = RandomEngine::GetRandomIndex(3);
    uint32_t CMSMatrixTypeIndex = RandomEngine::GetRandomIndex(4);
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateRGB(
        static_cast<CMSTransferFuncType>(CMSTransferFuncTypeIndex),
        static_cast<CMSMatrixType>(CMSMatrixTypeIndex));
    colorSpaceList.emplace_back(colorSpace);

    uint32_t colorSpaceIndex = RandomEngine::GetRandomIndex(colorSpaceList.size() - 1);
    return colorSpaceList[colorSpaceIndex];
}

static Brush GetRandomBrush()
{
    Color color = GetRandomColor();
    return Brush(color);
}

static Point3 GetRandomPoint3()
{
    float x = RandomData::GetRandomFloat();
    float y = RandomData::GetRandomFloat();
    float z = RandomData::GetRandomFloat();
    return Point3(x, y, z);
}

static Point GetRandomPoint()
{
    float x = RandomData::GetRandomFloat();
    float y = RandomData::GetRandomFloat();
    return Point(x, y);
}

static ShadowFlags GetRandomShadowFlags()
{
    uint32_t index = RandomEngine::GetRandomIndex(3);
    return static_cast<ShadowFlags>(index);
}

static Rect GetRandomRect()
{
    float left = RandomData::GetRandomFloat();
    float top = RandomData::GetRandomFloat();
    float width = RandomData::GetRandomFloat();
    float height = RandomData::GetRandomFloat();
    Rect rect(left, top, width, height);
    return rect;
}

static Path GetRandomPath()
{
    Path path;
    Rect rect = GetRandomRect();
    path.AddRect(rect);
    return path;
}

static Paint GetRandomPaint()
{
    Color color = GetRandomColor();
    std::shared_ptr<ColorSpace> colorSpace = GetRandomColorSpace();
    Paint paint(color, colorSpace);
    return paint;
}

static PointMode GetRandomPointMode()
{
    uint32_t index = RandomEngine::GetRandomIndex(2);
    return static_cast<PointMode>(index);
}

static std::vector<Point> GetRandomPointVector()
{
    int length = RandomEngine::GetRandomMiddleVectorLength();
    std::vector<Point> pointList;
    for (int i = 0; i < length; i++) {
        pointList.emplace_back(GetRandomPoint());
    }
    return pointList;
}

static RoundRect GetRandomRoundRect()
{
    Rect rect = GetRandomRect();
    float xRad = RandomData::GetRandomFloat();
    float yRad = RandomData::GetRandomFloat();
    return RoundRect(rect, xRad, yRad);
}

static BlendMode GetRandomBlendMode()
{
    uint32_t index = RandomEngine::GetRandomIndex(28);
    return static_cast<BlendMode>(index);
}

static ColorType GetRandomColorType()
{
    uint32_t index = RandomEngine::GetRandomIndex(10);
    return static_cast<ColorType>(index);
}

static AlphaType GetRandomAlphaType()
{
    uint32_t index = RandomEngine::GetRandomIndex(3);
    return static_cast<AlphaType>(index);
}

static BitmapFormat GetRandomBitmapFormat()
{
    BitmapFormat bitMapFormat = { GetRandomColorType(), GetRandomAlphaType() };
    return bitMapFormat;
}

static Bitmap GetRandomBitmap()
{
    static constexpr int BITMAP_SIZE_MAX = 4096;
    static constexpr int BITMAP_SIZE_MIN = 1;
    int32_t width = RandomEngine::GetRandomIndex(BITMAP_SIZE_MAX, BITMAP_SIZE_MIN);
    int32_t height = RandomEngine::GetRandomIndex(BITMAP_SIZE_MAX, BITMAP_SIZE_MIN);
    BitmapFormat bitMapFormat = GetRandomBitmapFormat();
    int32_t stride = RandomEngine::GetRandomIndex(std::min<int>(width, height), 0);
    Bitmap bitMap;
    if (!bitMap.Build(width, height, bitMapFormat, stride)) {
        SAFUZZ_LOGE("GetRandomBitmap bitmap build failed");
    }
    return bitMap;
}

static FilterMode GetRandomFilterMode()
{
    uint32_t index = RandomEngine::GetRandomIndex(1);
    return static_cast<FilterMode>(index);
}

static MipmapMode GetRandomMipmapMode()
{
    uint32_t index = RandomEngine::GetRandomIndex(2);
    return static_cast<MipmapMode>(index);
}

static SamplingOptions GetRandomSamplingOptions()
{
    FilterMode filterMode = GetRandomFilterMode();
    MipmapMode mipmapMode = GetRandomMipmapMode();
    SamplingOptions samplingOptions(filterMode, mipmapMode);
    return samplingOptions;
}

static Image GetRandomImage()
{
    Bitmap bitMap = GetRandomBitmap();
    Image image;
    if (!image.BuildFromBitmap(bitMap)) {
        SAFUZZ_LOGE("GetRandomImage BuildFromBitmap failed");
    }
    return image;
}

static SrcRectConstraint GetRandomSrcRectConstraint()
{
    uint32_t index = RandomEngine::GetRandomIndex(1);
    return static_cast<SrcRectConstraint>(index);
}

static ClipOp GetRandomClipOp()
{
    uint32_t index = RandomEngine::GetRandomIndex(1);
    return static_cast<ClipOp>(index);
}

static RectI GetRandomRectI()
{
    int32_t l = RandomData::GetRandomInt32();
    int32_t t = RandomData::GetRandomInt32();
    int32_t r = RandomData::GetRandomInt32();
    int32_t b = RandomData::GetRandomInt32();
    return RectI(l, t, r, b);
}

static Region GetRandomRegion()
{
    Region region;
    float x = RandomData::GetRandomFloat();
    float y = RandomData::GetRandomFloat();
    float w = RandomData::GetRandomFloat();
    float h = RandomData::GetRandomFloat();
    RectI rect(x, y, w, h);
    region.SetRect(rect);
    return region;
}

static SaveLayerOps GetRandomSaveLayerOps()
{
    Rect rect = GetRandomRect();
    Brush brush = GetRandomBrush();
    uint32_t saveLayerFlags = RandomData::GetRandomUint8();
    SaveLayerOps saveLayerOps(&rect, &brush, saveLayerFlags);
    return saveLayerOps;
}

static DrawCmdList::UnmarshalMode GetRandomUnmarshalMode()
{
    int index = RandomEngine::GetRandomIndex(1);
    return static_cast<DrawCmdList::UnmarshalMode>(index);
}

static std::shared_ptr<RecordCmd> GetRandomRecordCmd()
{
    int32_t w = RandomData::GetRandomUint8();
    int32_t h = RandomData::GetRandomUint8();
    DrawCmdList::UnmarshalMode mode = GetRandomUnmarshalMode();
    auto cmdList = std::make_shared<DrawCmdList>(w, h, mode);
    Rect bounds = GetRandomRect();
    return std::make_shared<RecordCmd>(cmdList, bounds);
}

static std::vector<int> GetRandomVectorInt()
{
    int length = RandomEngine::GetRandomMiddleVectorLength();
    std::vector<int> vec;
    for (int i = 0; i < length; i++) {
        vec.push_back(RandomData::GetRandomInt());
    }
    return vec;
}

static Lattice::RectType GetRandomRectType()
{
    int index = RandomEngine::GetRandomIndex(2);
    return static_cast<Lattice::RectType>(index);
}

static std::vector<Lattice::RectType> GetRandomVectorRectType()
{
    int length = RandomEngine::GetRandomMiddleVectorLength();
    std::vector<Lattice::RectType> vec;
    for (int i = 0; i < length; i++) {
        vec.push_back(GetRandomRectType());
    }
    return vec;
}

static Lattice GetRandomLattice()
{
    std::vector<int> fXDivs = GetRandomVectorInt();
    std::vector<int> fYDivs = GetRandomVectorInt();
    std::vector<Lattice::RectType> fRectTypes = GetRandomVectorRectType();
    uint32_t fXCount = RandomData::GetRandomUint32();
    uint32_t fYCount = RandomData::GetRandomUint32();
    Lattice lattice = {
        .fXDivs = fXDivs,
        .fYDivs = fYDivs,
        .fRectTypes = fRectTypes,
        .fXCount = fXCount,
        .fYCount = fYCount,
    };
    return lattice;
}

static RSXform GetRandomRSXform()
{
    float cos = RandomData::GetRandomFloat();
    float sin = RandomData::GetRandomFloat();
    float tx = RandomData::GetRandomFloat();
    float ty = RandomData::GetRandomFloat();
    RSXform xForm = RSXform::Make(cos, sin, tx, ty);
    return xForm;
}

static std::vector<RSXform> GetRandomVectorRSXform()
{
    int length = RandomEngine::GetRandomMiddleVectorLength();
    std::vector<RSXform> vec;
    for (int i = 0; i < length; i++) {
        vec.emplace_back(GetRandomRSXform());
    }
    return vec;
}

static std::vector<Rect> GetRandomVectorRect()
{
    int length = RandomEngine::GetRandomMiddleVectorLength();
    std::vector<Rect> vec;
    for (int i = 0; i < length; i++) {
        vec.emplace_back(GetRandomRect());
    }
    return vec;
}

static std::vector<uint32_t> GetRandomVectorUint32()
{
    int length = RandomEngine::GetRandomMiddleVectorLength();
    std::vector<uint32_t> vec;
    for (int i = 0; i < length; i++) {
        vec.emplace_back(RandomData::GetRandomUint8());
    }
    return vec;
}

static DrawingHMSymbolData GetRandomDrawingHMSymbolData()
{
    int length1 = RandomEngine::GetRandomSmallVectorLength();
    int length2 = RandomEngine::GetRandomSmallVectorLength();
    std::vector<std::vector<size_t>> layers;
    for (int i = 0; i < length1; i++) {
        std::vector<size_t> layer;
        for (int j = 0; j < length2; j++) {
            layer.push_back(RandomData::GetRandomUint8());
        }
        layers.push_back(layer);
    }
    uint16_t symbolGlyphId = 0;
    DrawingSymbolLayers symbolLayers = { symbolGlyphId, layers };
    Path path = GetRandomPath();
    uint64_t symbolId = RandomData::GetRandomUint8();
    DrawingHMSymbolData symbolData = { symbolLayers, path, symbolId };
    return symbolData;
}

static Font GetRandomFont()
{
    std::shared_ptr<Typeface> typeface = RandomTypeface::GetRandomTypeface();
    float size = RandomData::GetRandomFloat();
    float scaleX = RandomData::GetRandomFloat();
    float scaleY = RandomData::GetRandomFloat();
    Font font(typeface, size, scaleX, scaleY);
    return font;
}

static std::shared_ptr<TextBlob> GetRandomTextBlob()
{
    Font font = GetRandomFont();
    std::string str = RandomData::GetRandomString();
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromText(str.c_str(), str.size(), font, TextEncoding::UTF8);
    return textBlob;
}

static AdaptiveImageInfo GetRandomAdaptiveImageInfo()
{
    uint32_t fitNum = RandomData::GetRandomUint8();
    uint32_t repeatNum = RandomData::GetRandomUint8();
    AdaptiveImageInfo adaptiveImageInfo = { fitNum, repeatNum };
    return adaptiveImageInfo;
}

static RecordingCanvas::DrawFunc GetRandomDrawFunc()
{
    Rect rect = GetRandomRect();
    return [rect](Canvas* c, const Rect* r) {
        if (c == nullptr) {
            SAFUZZ_LOGE("GetRandomDrawFunc canvas is nullptr");
            return;
        }
        c->DrawRect(rect);
    };
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawWithPaintOpItem()
{
    Paint paint = GetRandomPaint();
    DrawOpItem::Type type = GetRandomDrawOpType();
    return std::make_shared<DrawWithPaintOpItem>(paint, type);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawShadowStyleOpItem()
{
    Path path = GetRandomPath();
    Point3 planeParams = GetRandomPoint3();
    Point3 devLightPos = GetRandomPoint3();
    float lightRadius = RandomData::GetRandomFloat();
    Color ambientColor = GetRandomColor();
    Color spotColor = GetRandomColor();
    ShadowFlags flag = GetRandomShadowFlags();
    bool isLimitElevation = RandomData::GetRandomBool();
    return std::make_shared<DrawShadowStyleOpItem>(path, planeParams, devLightPos, lightRadius,
        ambientColor, spotColor, flag, isLimitElevation);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawPointOpItem()
{
    Point point = GetRandomPoint();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawPointOpItem>(point, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawPointsOpItem()
{
    std::vector<Point> pointList = GetRandomPointVector();
    Paint paint = GetRandomPaint();
    PointMode pointMode = GetRandomPointMode();
    return std::make_shared<DrawPointsOpItem>(pointMode, pointList, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawLineOpItem()
{
    Point startPt = GetRandomPoint();
    Point endPt = GetRandomPoint();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawLineOpItem>(startPt, endPt, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawRectOpItem()
{
    Rect rect = GetRandomRect();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawRectOpItem>(rect, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawRoundRectOpItem()
{
    RoundRect roundRect = GetRandomRoundRect();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawRoundRectOpItem>(roundRect, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawNestedRoundRectOpItem()
{
    RoundRect outerRoundRect = GetRandomRoundRect();
    RoundRect innerRoundRect = GetRandomRoundRect();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawNestedRoundRectOpItem>(outerRoundRect, innerRoundRect, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawArcOpItem()
{
    Rect oval = GetRandomRect();
    float startAngle = RandomData::GetRandomFloat();
    float sweepAngle = RandomData::GetRandomFloat();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawArcOpItem>(oval, startAngle, sweepAngle, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawPieOpItem()
{
    Rect oval = GetRandomRect();
    float startAngle = RandomData::GetRandomFloat();
    float sweepAngle = RandomData::GetRandomFloat();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawPieOpItem>(oval, startAngle, sweepAngle, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawOvalOpItem()
{
    Rect rect = GetRandomRect();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawOvalOpItem>(rect, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawCircleOpItem()
{
    Point center = GetRandomPoint();
    float radius = RandomData::GetRandomFloat();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawCircleOpItem>(center, radius, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawPathOpItem()
{
    Path path = GetRandomPath();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawPathOpItem>(path, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawBackgroundOpItem()
{
    Brush brush = GetRandomBrush();
    return std::make_shared<DrawBackgroundOpItem>(brush);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawShadowOpItem()
{
    Path path = GetRandomPath();
    Point3 planeParams = GetRandomPoint3();
    Point3 devLightPos = GetRandomPoint3();
    float lightRadius = RandomData::GetRandomFloat();
    Color ambientColor = GetRandomColor();
    Color spotColor = GetRandomColor();
    ShadowFlags flag = GetRandomShadowFlags();
    return std::make_shared<DrawShadowOpItem>(path, planeParams, devLightPos, lightRadius,
        ambientColor, spotColor, flag);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawRegionOpItem()
{
    Region region = GetRandomRegion();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawRegionOpItem>(region, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawVerticesOpItem()
{
    Vertices vertices;
    BlendMode mode = GetRandomBlendMode();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawVerticesOpItem>(vertices, mode, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawColorOpItem()
{
    uint32_t color = RandomData::GetRandomUint32();
    BlendMode mode = GetRandomBlendMode();
    return std::make_shared<DrawColorOpItem>(color, mode);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawImageNineOpItem()
{
    Image image = GetRandomImage();
    RectI center = GetRandomRectI();
    Rect dst = GetRandomRect();
    FilterMode mode = GetRandomFilterMode();
    Brush brush = GetRandomBrush();
    return std::make_shared<DrawImageNineOpItem>(&image, center, dst, mode, &brush);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawImageLatticeOpItem()
{
    Image image = GetRandomImage();
    Lattice lattice = GetRandomLattice();
    Rect rect = GetRandomRect();
    FilterMode mode = GetRandomFilterMode();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawImageLatticeOpItem>(&image, lattice, rect, mode, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawAtlasOpItem()
{
    Image atlas = GetRandomImage();
    std::vector<RSXform> xForm = GetRandomVectorRSXform();
    std::vector<Rect> tex = GetRandomVectorRect();
    std::vector<uint32_t> colors = GetRandomVectorUint32();
    BlendMode mode = GetRandomBlendMode();
    SamplingOptions options = GetRandomSamplingOptions();
    bool hasCullRect = RandomData::GetRandomBool();
    Rect cullRect = GetRandomRect();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawAtlasOpItem>(&atlas, xForm, tex, colors, mode,
        options, hasCullRect, cullRect, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawBitmapOpItem()
{
    Bitmap bitMap = GetRandomBitmap();
    float px = RandomData::GetRandomFloat();
    float py = RandomData::GetRandomFloat();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawBitmapOpItem>(bitMap, px, py, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawImageOpItem()
{
    Image image = GetRandomImage();
    float px = RandomData::GetRandomFloat();
    float py = RandomData::GetRandomFloat();
    SamplingOptions samplingOptions = GetRandomSamplingOptions();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawImageOpItem>(image, px, py, samplingOptions, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawImageRectOpItem()
{
    Image image = GetRandomImage();
    Rect srcRect = GetRandomRect();
    Rect dstRect = GetRandomRect();
    SamplingOptions samplingOptions = GetRandomSamplingOptions();
    SrcRectConstraint constraint = GetRandomSrcRectConstraint();
    Paint paint = GetRandomPaint();
    bool isForeground = RandomData::GetRandomBool();
    return std::make_shared<DrawImageRectOpItem>(
        image, srcRect, dstRect, samplingOptions, constraint, paint, isForeground);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawRecordCmdOpItem()
{
    std::shared_ptr<RecordCmd> cmdList = GetRandomRecordCmd();
    Matrix matrix = RandomDrawingMatrix::GetRandomDrawingMatrix();
    Brush brush = GetRandomBrush();
    return std::make_shared<DrawRecordCmdOpItem>(cmdList, &matrix, &brush);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawPictureOpItem()
{
    Picture picture;
    return std::make_shared<DrawPictureOpItem>(picture);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawTextBlobOpItem()
{
    std::shared_ptr<TextBlob> textBlob = GetRandomTextBlob();
    if (textBlob == nullptr) {
        SAFUZZ_LOGE("RandomDrawOp::GetRandomDrawTextBlobOpItem textBlob is nullptr");
        return nullptr;
    }
    float x = RandomData::GetRandomFloat();
    float y = RandomData::GetRandomFloat();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawTextBlobOpItem>(textBlob.get(), x, y, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawSymbolOpItem()
{
    DrawingHMSymbolData symbolData = GetRandomDrawingHMSymbolData();
    Point locate = GetRandomPoint();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawSymbolOpItem>(symbolData, locate, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomClipRectOpItem()
{
    Rect rect = GetRandomRect();
    ClipOp op = GetRandomClipOp();
    bool isAntiAlias = RandomData::GetRandomBool();
    return std::make_shared<ClipRectOpItem>(rect, op, isAntiAlias);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomClipIRectOpItem()
{
    RectI rect = GetRandomRectI();
    ClipOp op = GetRandomClipOp();
    return std::make_shared<ClipIRectOpItem>(rect, op);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomClipRoundRectOpItem()
{
    RoundRect roundRect = GetRandomRoundRect();
    ClipOp op = GetRandomClipOp();
    bool isAntiAlias = RandomData::GetRandomBool();
    return std::make_shared<ClipRoundRectOpItem>(roundRect, op, isAntiAlias);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomClipPathOpItem()
{
    Path path = GetRandomPath();
    ClipOp op = GetRandomClipOp();
    bool isAntiAlias = RandomData::GetRandomBool();
    return std::make_shared<ClipPathOpItem>(path, op, isAntiAlias);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomClipRegionOpItem()
{
    Region region = GetRandomRegion();
    ClipOp op = GetRandomClipOp();
    return std::make_shared<ClipRegionOpItem>(region, op);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomSetMatrixOpItem()
{
    Matrix matrix = RandomDrawingMatrix::GetRandomDrawingMatrix();
    return std::make_shared<SetMatrixOpItem>(matrix);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomResetMatrixOpItem()
{
    return std::make_shared<ResetMatrixOpItem>();
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomConcatMatrixOpItem()
{
    Matrix matrix = RandomDrawingMatrix::GetRandomDrawingMatrix();
    return std::make_shared<ConcatMatrixOpItem>(matrix);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomTranslateOpItem()
{
    float dx = RandomData::GetRandomFloat();
    float dy = RandomData::GetRandomFloat();
    return std::make_shared<TranslateOpItem>(dx, dy);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomScaleOpItem()
{
    float sx = RandomData::GetRandomFloat();
    float sy = RandomData::GetRandomFloat();
    return std::make_shared<ScaleOpItem>(sx, sy);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomRotateOpItem()
{
    float deg = RandomData::GetRandomFloat();
    float sx = RandomData::GetRandomFloat();
    float sy = RandomData::GetRandomFloat();
    return std::make_shared<RotateOpItem>(deg, sx, sy);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomShearOpItem()
{
    float sx = RandomData::GetRandomFloat();
    float sy = RandomData::GetRandomFloat();
    return std::make_shared<ShearOpItem>(sx, sy);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomFlushOpItem()
{
    return std::make_shared<FlushOpItem>();
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomClearOpItem()
{
    uint32_t color = RandomData::GetRandomUint8();
    return std::make_shared<ClearOpItem>(color);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomSaveOpItem()
{
    return std::make_shared<SaveOpItem>();
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomSaveLayerOpItem()
{
    SaveLayerOps saveLayerOps = GetRandomSaveLayerOps();
    return std::make_shared<SaveLayerOpItem>(saveLayerOps);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomRestoreOpItem()
{
    return std::make_shared<RestoreOpItem>();
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDiscardOpItem()
{
    return std::make_shared<DiscardOpItem>();
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomClipAdaptiveRoundRectOpItem()
{
    std::vector<Point> pointList = GetRandomPointVector();
    return std::make_shared<ClipAdaptiveRoundRectOpItem>(pointList);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawImageWithParmOpItem()
{
    Image image = GetRandomImage();
    std::shared_ptr<Image> imagePtr = std::make_shared<Image>(image);
    std::shared_ptr<Data> data = std::make_shared<Data>();
    AdaptiveImageInfo info = GetRandomAdaptiveImageInfo();
    SamplingOptions options = GetRandomSamplingOptions();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawImageWithParmOpItem>(imagePtr, data, info, options, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawPixelMapWithParmOpItem()
{
    std::shared_ptr<Media::PixelMap> pixelMap = RandomPixelMap::GetRandomPixelMap();
    AdaptiveImageInfo info = GetRandomAdaptiveImageInfo();
    SamplingOptions sampling = GetRandomSamplingOptions();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawPixelMapWithParmOpItem>(pixelMap, info, sampling, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawPixelMapRectOpItem()
{
    std::shared_ptr<Media::PixelMap> pixelMap = RandomPixelMap::GetRandomPixelMap();
    Rect dstRect = GetRandomRect();
    Rect srcRect = GetRandomRect();
    SamplingOptions sampling = GetRandomSamplingOptions();
    SrcRectConstraint constraint = GetRandomSrcRectConstraint();
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawPixelMapRectOpItem>(pixelMap, dstRect, srcRect, sampling, constraint, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawFuncOpItem()
{
    RecordingCanvas::DrawFunc drawFunc = GetRandomDrawFunc();
    return std::make_shared<DrawFuncOpItem>(std::move(drawFunc));
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawSurfaceBufferOpItem()
{
    DrawingSurfaceBufferInfo info;
    Paint paint = GetRandomPaint();
    return std::make_shared<DrawSurfaceBufferOpItem>(info, paint);
}

std::shared_ptr<Drawing::DrawOpItem> RandomDrawOp::GetRandomDrawOp()
{
    static const std::vector<std::function<std::shared_ptr<Drawing::DrawOpItem>()>> drawOpRandomGenerator = {
        RandomDrawOp::GetRandomDrawWithPaintOpItem,
        RandomDrawOp::GetRandomDrawShadowStyleOpItem,
        RandomDrawOp::GetRandomDrawPointOpItem,
        RandomDrawOp::GetRandomDrawPointsOpItem,
        RandomDrawOp::GetRandomDrawLineOpItem,
        RandomDrawOp::GetRandomDrawRectOpItem,
        RandomDrawOp::GetRandomDrawRoundRectOpItem,
        RandomDrawOp::GetRandomDrawNestedRoundRectOpItem,
        RandomDrawOp::GetRandomDrawArcOpItem,
        RandomDrawOp::GetRandomDrawPieOpItem,
        RandomDrawOp::GetRandomDrawOvalOpItem,
        RandomDrawOp::GetRandomDrawCircleOpItem,
        RandomDrawOp::GetRandomDrawPathOpItem,
        RandomDrawOp::GetRandomDrawBackgroundOpItem,
        RandomDrawOp::GetRandomDrawShadowOpItem,
        RandomDrawOp::GetRandomDrawRegionOpItem,
        RandomDrawOp::GetRandomDrawVerticesOpItem,
        RandomDrawOp::GetRandomDrawColorOpItem,
        RandomDrawOp::GetRandomDrawImageNineOpItem,
        RandomDrawOp::GetRandomDrawImageLatticeOpItem,
        RandomDrawOp::GetRandomDrawAtlasOpItem,
        RandomDrawOp::GetRandomDrawBitmapOpItem,
        RandomDrawOp::GetRandomDrawImageOpItem,
        RandomDrawOp::GetRandomDrawImageRectOpItem,
        RandomDrawOp::GetRandomDrawRecordCmdOpItem,
        RandomDrawOp::GetRandomDrawPictureOpItem,
        RandomDrawOp::GetRandomDrawTextBlobOpItem,
        RandomDrawOp::GetRandomDrawSymbolOpItem,
        RandomDrawOp::GetRandomClipRectOpItem,
        RandomDrawOp::GetRandomClipIRectOpItem,
        RandomDrawOp::GetRandomClipRoundRectOpItem,
        RandomDrawOp::GetRandomClipPathOpItem,
        RandomDrawOp::GetRandomClipRegionOpItem,
        RandomDrawOp::GetRandomSetMatrixOpItem,
        RandomDrawOp::GetRandomResetMatrixOpItem,
        RandomDrawOp::GetRandomConcatMatrixOpItem,
        RandomDrawOp::GetRandomTranslateOpItem,
        RandomDrawOp::GetRandomScaleOpItem,
        RandomDrawOp::GetRandomRotateOpItem,
        RandomDrawOp::GetRandomShearOpItem,
        RandomDrawOp::GetRandomFlushOpItem,
        RandomDrawOp::GetRandomClearOpItem,
        RandomDrawOp::GetRandomSaveOpItem,
        RandomDrawOp::GetRandomSaveLayerOpItem,
        RandomDrawOp::GetRandomRestoreOpItem,
        RandomDrawOp::GetRandomDiscardOpItem,
        RandomDrawOp::GetRandomClipAdaptiveRoundRectOpItem,
        RandomDrawOp::GetRandomDrawImageWithParmOpItem,
        RandomDrawOp::GetRandomDrawPixelMapWithParmOpItem,
        RandomDrawOp::GetRandomDrawPixelMapRectOpItem,
        RandomDrawOp::GetRandomDrawFuncOpItem,
        RandomDrawOp::GetRandomDrawSurfaceBufferOpItem,
    };
    int index = RandomEngine::GetRandomIndex(drawOpRandomGenerator.size() - 1);
    auto drawOp = drawOpRandomGenerator[index]();
    return drawOp;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
