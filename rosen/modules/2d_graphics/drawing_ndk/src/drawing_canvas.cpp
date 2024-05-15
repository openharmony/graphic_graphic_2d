/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include "drawing_canvas_utils.h"
#include "image_pixel_map_mdk.h"
#include "native_pixel_map.h"
#include "native_pixel_map_manager.h"
#include "pixelmap_native_impl.h"
#include "recording/recording_canvas.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Canvas* CastToCanvas(OH_Drawing_Canvas* cCanvas)
{
    return reinterpret_cast<Canvas*>(cCanvas);
}

static const Path& CastToPath(const OH_Drawing_Path& cPath)
{
    return reinterpret_cast<const Path&>(cPath);
}

static const Brush& CastToBrush(const OH_Drawing_Brush& cBrush)
{
    return reinterpret_cast<const Brush&>(cBrush);
}

static const Pen& CastToPen(const OH_Drawing_Pen& cPen)
{
    return reinterpret_cast<const Pen&>(cPen);
}

static const OHOS::Rosen::Drawing::Region& CastToRegion(const OH_Drawing_Region& cRegion)
{
    return reinterpret_cast<const OHOS::Rosen::Drawing::Region&>(cRegion);
}

static const Bitmap& CastToBitmap(const OH_Drawing_Bitmap& cBitmap)
{
    return reinterpret_cast<const Bitmap&>(cBitmap);
}

static const Drawing::Rect& CastToRect(const OH_Drawing_Rect& cRect)
{
    return reinterpret_cast<const Drawing::Rect&>(cRect);
}

static const Point& CastToPoint(const OH_Drawing_Point& cPoint)
{
    return reinterpret_cast<const Point&>(cPoint);
}

static const Point CastToPoint(const OH_Drawing_Point2D& cPoint)
{
    return {cPoint.x, cPoint.y};
}

static Point3 CastToPoint3(OH_Drawing_Point3D& cPoint3)
{
    return {cPoint3.x, cPoint3.y, cPoint3.z};
}

static const RoundRect& CastToRoundRect(const OH_Drawing_RoundRect& cRoundRect)
{
    return reinterpret_cast<const RoundRect&>(cRoundRect);
}

static const TextBlob* CastToTextBlob(const OH_Drawing_TextBlob* cTextBlob)
{
    return reinterpret_cast<const TextBlob*>(cTextBlob);
}

static const Matrix& CastToMatrix(const OH_Drawing_Matrix& cMatrix)
{
    return reinterpret_cast<const Matrix&>(cMatrix);
}

static const Image& CastToImage(const OH_Drawing_Image& cImage)
{
    return reinterpret_cast<const Image&>(cImage);
}

static const SamplingOptions& CastToSamplingOptions(const OH_Drawing_SamplingOptions& cSamplingOptions)
{
    return reinterpret_cast<const SamplingOptions&>(cSamplingOptions);
}

OH_Drawing_Canvas* OH_Drawing_CanvasCreate()
{
    return (OH_Drawing_Canvas*)new Canvas;
}

void OH_Drawing_CanvasDestroy(OH_Drawing_Canvas* cCanvas)
{
    delete CastToCanvas(cCanvas);
}

void OH_Drawing_CanvasBind(OH_Drawing_Canvas* cCanvas, OH_Drawing_Bitmap* cBitmap)
{
    if (cBitmap == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Bind(CastToBitmap(*cBitmap));
}

void OH_Drawing_CanvasAttachPen(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Pen* cPen)
{
    if (cPen == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->AttachPen(CastToPen(*cPen));
}

void OH_Drawing_CanvasDetachPen(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DetachPen();
}

void OH_Drawing_CanvasAttachBrush(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->AttachBrush(CastToBrush(*cBrush));
}

void OH_Drawing_CanvasDetachBrush(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DetachBrush();
}

void OH_Drawing_CanvasSave(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Save();
}

void OH_Drawing_CanvasSaveLayer(OH_Drawing_Canvas* cCanvas,
    const OH_Drawing_Rect* cRect, const OH_Drawing_Brush* cBrush)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }

    std::unique_ptr<Drawing::Rect> bounds = nullptr;
    std::unique_ptr<Brush> brush = nullptr;
    if (cRect != nullptr) {
        bounds = std::make_unique<Drawing::Rect>();
        *bounds = CastToRect(*cRect);
    }
    if (cBrush != nullptr) {
        brush = std::make_unique<Brush>();
        *brush = CastToBrush(*cBrush);
    }

    SaveLayerOps slr = SaveLayerOps(bounds.get(), brush.get());
    canvas->SaveLayer(slr);
}

void OH_Drawing_CanvasRestore(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Restore();
}

uint32_t OH_Drawing_CanvasGetSaveCount(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return 0;
    }
    return canvas->GetSaveCount();
}

void OH_Drawing_CanvasRestoreToCount(OH_Drawing_Canvas* cCanvas, uint32_t saveCount)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->RestoreToCount(saveCount);
}

void OH_Drawing_CanvasDrawLine(OH_Drawing_Canvas* cCanvas, float x1, float y1, float x2, float y2)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    Point startPt(x1, y1);
    Point endPt(x2, y2);
    canvas->DrawLine(startPt, endPt);
}

void OH_Drawing_CanvasDrawPath(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Path* cPath)
{
    if (cPath == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawPath(CastToPath(*cPath));
}

static PointMode pointModeCastToPointMode(const OH_Drawing_PointMode& pointMode)
{
    PointMode mode = PointMode::POINTS_POINTMODE;
    switch (pointMode) {
        case POINT_MODE_POINTS:
            mode = PointMode::POINTS_POINTMODE;
            break;
        case POINT_MODE_LINES:
            mode = PointMode::LINES_POINTMODE;
            break;
        case POINT_MODE_POLYGON:
            mode = PointMode::POLYGON_POINTMODE;
            break;
        default:
            break;
    }
    return mode;
}

void OH_Drawing_CanvasDrawPoints(OH_Drawing_Canvas* cCanvas, OH_Drawing_PointMode mode,
    uint32_t count, const OH_Drawing_Point2D* pts)
{
    if (pts == nullptr || count == 0) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    Point* point = new Point[count];
    if (point == nullptr) {
        return;
    }
    for (uint32_t i = 0; i < count; ++i) {
        point[i] = CastToPoint(pts[i]);
    }
    canvas->DrawPoints(pointModeCastToPointMode(mode), count, point);
    delete [] point;
}

static VertexMode vertexMmodeCastToVertexMmode(const OH_Drawing_VertexMode& vertexMmode)
{
    VertexMode mode = VertexMode::TRIANGLES_VERTEXMODE;
    switch (vertexMmode) {
        case VERTEX_MODE_TRIANGLES:
            mode = VertexMode::TRIANGLES_VERTEXMODE;
            break;
        case VERTEX_MODE_TRIANGLES_STRIP:
            mode = VertexMode::TRIANGLESSTRIP_VERTEXMODE;
            break;
        case VERTEX_MODE_TRIANGLE_FAN:
            mode = VertexMode::TRIANGLEFAN_VERTEXMODE;
            break;
        default:
            break;
    }
    return mode;
}

void OH_Drawing_CanvasDrawVertices(OH_Drawing_Canvas* cCanvas, OH_Drawing_VertexMode vertexMode,
    int32_t vertexCount, const OH_Drawing_Point2D* positions, const OH_Drawing_Point2D* texs,
    const uint32_t* colors, int32_t indexCount, const uint16_t* indices, OH_Drawing_BlendMode mode)
{
    if (positions == nullptr || texs == nullptr || colors == nullptr || indices == nullptr
        || vertexCount < 0 || indexCount < 0) {
        return;
    }

    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }

    Point* positionsPoint = new Point[vertexCount];
    if (positionsPoint == nullptr) {
        return;
    }
    for (int32_t i = 0; i < vertexCount; ++i) {
        positionsPoint[i] = CastToPoint(positions[i]);
    }
    Point* texsPoint = new Point[vertexCount];
    if (texsPoint == nullptr) {
        delete [] positionsPoint;
        return;
    }
    for (int32_t i = 0; i < vertexCount; ++i) {
        texsPoint[i] = CastToPoint(texs[i]);
    }
    Vertices* vertices = new Vertices();
    vertices->MakeCopy(vertexMmodeCastToVertexMmode(vertexMode), vertexCount, positionsPoint,
        texsPoint, colors, indexCount, indices);
    canvas->DrawVertices(*vertices, static_cast<BlendMode>(mode));
    delete vertices;
    delete [] positionsPoint;
    delete [] texsPoint;
}

void OH_Drawing_CanvasDrawBackground(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawBackground(CastToBrush(*cBrush));
}

void OH_Drawing_CanvasDrawRegion(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Region* cRegion)
{
    if (cRegion == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawRegion(CastToRegion(*cRegion));
}

void OH_Drawing_CanvasDrawBitmap(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Bitmap* cBitmap, float left, float top)
{
    if (cBitmap == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawBitmap(CastToBitmap(*cBitmap), left, top);
}

void OH_Drawing_CanvasDrawPixelMapRect(OH_Drawing_Canvas* cCanvas, OH_Drawing_PixelMap* pixelMap,
    const OH_Drawing_Rect* src, const OH_Drawing_Rect* dst, const OH_Drawing_SamplingOptions* cSampingOptions)
{
#ifdef OHOS_PLATFORM
    std::shared_ptr<Media::PixelMap> p = nullptr;
    switch (NativePixelMapManager::GetInstance().GetNativePixelMapType(pixelMap)) {
        case NativePixelMapType::OBJECT_FROM_C:
            if (pixelMap) {
                p = reinterpret_cast<OH_PixelmapNative*>(pixelMap)->GetInnerPixelmap();
            }
            break;
        case NativePixelMapType::OBJECT_FROM_JS:
            p = Media::PixelMapNative_GetPixelMap(reinterpret_cast<NativePixelMap_*>(pixelMap));
            break;
        default:
            break;
    }
    DrawingCanvasUtils::DrawPixelMapRect(CastToCanvas(cCanvas), p,
        reinterpret_cast<const Drawing::Rect*>(src), reinterpret_cast<const Drawing::Rect*>(dst),
        reinterpret_cast<const Drawing::SamplingOptions*>(cSampingOptions));
#endif
}

void OH_Drawing_CanvasDrawBitmapRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Bitmap* cBitmap,
    const OH_Drawing_Rect* src, const OH_Drawing_Rect* dst, const OH_Drawing_SamplingOptions* sampling)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cBitmap == nullptr || dst == nullptr || sampling == nullptr) {
        return;
    }
    const Bitmap& bitmap = CastToBitmap(*cBitmap);
    auto image = bitmap.MakeImage();
    if (image == nullptr) {
        return;
    }
    if (src == nullptr) {
        canvas->DrawImageRect(*image, CastToRect(*dst), CastToSamplingOptions(*sampling));
    } else {
        canvas->DrawImageRect(*image, CastToRect(*src),
            CastToRect(*dst), CastToSamplingOptions(*sampling));
    }
}

void OH_Drawing_CanvasDrawRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect)
{
    if (cRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawRect(CastToRect(*cRect));
}

void OH_Drawing_CanvasDrawCircle(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Point* cPoint, float radius)
{
    if (cPoint == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawCircle(CastToPoint(*cPoint), radius);
}

void OH_Drawing_CanvasDrawOval(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect)
{
    if (cRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawOval(CastToRect(*cRect));
}

void OH_Drawing_CanvasDrawArc(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect,
    float startAngle, float sweepAngle)
{
    if (cRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawArc(CastToRect(*cRect), startAngle, sweepAngle);
}

void OH_Drawing_CanvasDrawRoundRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_RoundRect* cRoundRect)
{
    if (cRoundRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawRoundRect(CastToRoundRect(*cRoundRect));
}

void OH_Drawing_CanvasDrawTextBlob(OH_Drawing_Canvas* cCanvas, const OH_Drawing_TextBlob* cTextBlob, float x, float y)
{
    if (cTextBlob == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->DrawTextBlob(CastToTextBlob(cTextBlob), x, y);
}

static ClipOp CClipOpCastToClipOp(OH_Drawing_CanvasClipOp cClipOp)
{
    ClipOp clipOp = ClipOp::INTERSECT;
    switch (cClipOp) {
        case DIFFERENCE:
            clipOp = ClipOp::DIFFERENCE;
            break;
        case INTERSECT:
            clipOp = ClipOp::INTERSECT;
            break;
        default:
            break;
    }
    return clipOp;
}

void OH_Drawing_CanvasClipRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect,
    OH_Drawing_CanvasClipOp cClipOp, bool doAntiAlias)
{
    if (cRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->ClipRect(CastToRect(*cRect), CClipOpCastToClipOp(cClipOp), doAntiAlias);
}

void OH_Drawing_CanvasClipRoundRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_RoundRect* cRoundRect,
    OH_Drawing_CanvasClipOp cClipOp, bool doAntiAlias)
{
    if (cRoundRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->ClipRoundRect(CastToRoundRect(*cRoundRect), CClipOpCastToClipOp(cClipOp), doAntiAlias);
}

void OH_Drawing_CanvasClipPath(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Path* cPath,
    OH_Drawing_CanvasClipOp cClipOp, bool doAntiAlias)
{
    if (cPath == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->ClipPath(CastToPath(*cPath), CClipOpCastToClipOp(cClipOp), doAntiAlias);
}

void OH_Drawing_CanvasRotate(OH_Drawing_Canvas* cCanvas, float degrees, float px, float py)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Rotate(degrees, px, py);
}

void OH_Drawing_CanvasTranslate(OH_Drawing_Canvas* cCanvas, float dx, float dy)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Translate(dx, dy);
}

void OH_Drawing_CanvasScale(OH_Drawing_Canvas* cCanvas, float sx, float sy)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Scale(sx, sy);
}

void OH_Drawing_CanvasSkew(OH_Drawing_Canvas* cCanvas, float sx, float sy)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Shear(sx, sy);
}

void OH_Drawing_CanvasClear(OH_Drawing_Canvas* cCanvas, uint32_t color)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->Clear(color);
}

int32_t OH_Drawing_CanvasGetWidth(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return 0;
    }
    return canvas->GetWidth();
}

int32_t OH_Drawing_CanvasGetHeight(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return 0;
    }
    return canvas->GetHeight();
}

void OH_Drawing_CanvasGetLocalClipBounds(OH_Drawing_Canvas* cCanvas, OH_Drawing_Rect* cRect)
{
    if (cRect == nullptr) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    Drawing::Rect rect = canvas->GetLocalClipBounds();
    Drawing::Rect* outRect = reinterpret_cast<Drawing::Rect*>(cRect);
    *outRect = rect;
}

void OH_Drawing_CanvasGetTotalMatrix(OH_Drawing_Canvas* cCanvas, OH_Drawing_Matrix* cMatrix)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cMatrix == nullptr) {
        return;
    }
    Matrix matrix = canvas->GetTotalMatrix();
    Matrix* outMatrix = reinterpret_cast<Matrix*>(cMatrix);
    *outMatrix = matrix;
}

void OH_Drawing_CanvasConcatMatrix(OH_Drawing_Canvas* cCanvas, OH_Drawing_Matrix* cMatrix)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cMatrix == nullptr) {
        return;
    }
    canvas->ConcatMatrix(*reinterpret_cast<Matrix*>(cMatrix));
}

static ShadowFlags CClipOpCastToClipOp(OH_Drawing_CanvasShadowFlags cFlag)
{
    ShadowFlags shadowFlags = ShadowFlags::NONE;
    switch (cFlag) {
        case OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_NONE:
            shadowFlags = ShadowFlags::NONE;
            break;
        case OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_TRANSPARENT_OCCLUDER:
            shadowFlags = ShadowFlags::TRANSPARENT_OCCLUDER;
            break;
        case OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_GEOMETRIC_ONLY:
            shadowFlags = ShadowFlags::GEOMETRIC_ONLY;
            break;
        case OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_ALL:
            shadowFlags = ShadowFlags::ALL;
            break;
        default:
            break;
    }
    return shadowFlags;
}

void OH_Drawing_CanvasDrawShadow(OH_Drawing_Canvas* cCanvas, OH_Drawing_Path* cPath, OH_Drawing_Point3D cPlaneParams,
    OH_Drawing_Point3D cDevLightPos, float lightRadius, uint32_t ambientColor, uint32_t spotColor,
    OH_Drawing_CanvasShadowFlags flag)
{
    if (cCanvas == nullptr || cPath == nullptr ||
        flag < SHADOW_FLAGS_NONE || flag > SHADOW_FLAGS_ALL) {
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    canvas->DrawShadow(*reinterpret_cast<Path*>(cPath), CastToPoint3(cPlaneParams),
        CastToPoint3(cDevLightPos), lightRadius, Color(ambientColor), Color(spotColor),
        CClipOpCastToClipOp(flag));
}

void OH_Drawing_CanvasSetMatrix(OH_Drawing_Canvas* cCanvas, OH_Drawing_Matrix* matrix)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || matrix == nullptr) {
        return;
    }
    canvas->SetMatrix(CastToMatrix(*matrix));
}

void OH_Drawing_CanvasResetMatrix(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return;
    }
    canvas->ResetMatrix();
}

void OH_Drawing_CanvasDrawImageRectWithSrc(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Image* cImage,
    const OH_Drawing_Rect* src, const OH_Drawing_Rect* dst, const OH_Drawing_SamplingOptions* cSampingOptions,
    OH_Drawing_SrcRectConstraint constraint)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cImage == nullptr || src == nullptr || dst == nullptr || cSampingOptions == nullptr) {
        return;
    }
    canvas->DrawImageRect(CastToImage(*cImage), CastToRect(*src), CastToRect(*dst),
        CastToSamplingOptions(*cSampingOptions), static_cast<SrcRectConstraint>(constraint));
}

void OH_Drawing_CanvasDrawImageRect(OH_Drawing_Canvas* cCanvas, OH_Drawing_Image* cImage, OH_Drawing_Rect* dst,
    OH_Drawing_SamplingOptions* cSampingOptions)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cImage == nullptr || dst == nullptr || cSampingOptions == nullptr) {
        return;
    }
    canvas->DrawImageRect(CastToImage(*cImage), CastToRect(*dst), CastToSamplingOptions(*cSampingOptions));
}

bool OH_Drawing_CanvasReadPixels(OH_Drawing_Canvas* cCanvas, OH_Drawing_Image_Info* cImageInfo,
    void* dstPixels, uint32_t dstRowBytes, int32_t srcX, int32_t srcY)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cImageInfo == nullptr || dstPixels == nullptr) {
        return false;
    }
    ImageInfo imageInfo(cImageInfo->width, cImageInfo->height,
        static_cast<ColorType>(cImageInfo->colorType), static_cast<AlphaType>(cImageInfo->alphaType));
    return canvas->ReadPixels(imageInfo, dstPixels, dstRowBytes, srcX, srcY);
}

bool OH_Drawing_CanvasReadPixelsToBitmap(OH_Drawing_Canvas* cCanvas, OH_Drawing_Bitmap* cBitmap, int32_t srcX, int32_t srcY)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cBitmap == nullptr) {
        return false;
    }
    return canvas->ReadPixels(CastToBitmap(*cBitmap), srcX, srcY);
}