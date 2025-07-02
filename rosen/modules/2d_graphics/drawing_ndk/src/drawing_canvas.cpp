/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifdef USE_M133_SKIA
#include "src/base/SkUTF.h"
#else
#include "src/utils/SkUTF.h"
#endif

#include "drawing_canvas_utils.h"
#include "drawing_font_utils.h"
#include "drawing_helper.h"
#include "image_pixel_map_mdk.h"
#include "native_pixel_map.h"
#include "native_pixel_map_manager.h"
#include "pixelmap_native_impl.h"
#include "recording/recording_canvas.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

#ifdef OHOS_PLATFORM
static std::mutex g_canvasMutex;
static std::unordered_map<void*, std::shared_ptr<Media::PixelMap>> g_canvasMap;
#endif

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

static const Drawing::Rect* CastToRect(const OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<const Drawing::Rect*>(cRect);
}

static const Point& CastToPoint(const OH_Drawing_Point& cPoint)
{
    return reinterpret_cast<const Point&>(cPoint);
}

static const Point& CastToPoint(const OH_Drawing_Point2D& cPoint)
{
    return reinterpret_cast<const Point&>(cPoint);
}

static const Point3& CastToPoint3(OH_Drawing_Point3D& cPoint3)
{
    return reinterpret_cast<const Point3&>(cPoint3);
}

static const RoundRect& CastToRoundRect(const OH_Drawing_RoundRect& cRoundRect)
{
    return reinterpret_cast<const RoundRect&>(cRoundRect);
}

static const RoundRect* CastToRoundRect(const OH_Drawing_RoundRect* cRoundRect)
{
    return reinterpret_cast<const RoundRect*>(cRoundRect);
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

static const Font* CastToFont(const OH_Drawing_Font* cFont)
{
    return reinterpret_cast<const Font*>(cFont);
}

static Drawing::DrawingFontFeatures* CastToFontFeatures(OH_Drawing_FontFeatures* fontFeatures)
{
    return reinterpret_cast<Drawing::DrawingFontFeatures*>(fontFeatures);
}

OH_Drawing_Canvas* OH_Drawing_CanvasCreate()
{
    return (OH_Drawing_Canvas*)new Canvas;
}

OH_Drawing_Canvas* OH_Drawing_CanvasCreateWithPixelMap(OH_Drawing_PixelMap* pixelMap)
{
#ifdef OHOS_PLATFORM
    if (pixelMap == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> p = nullptr;
    switch (NativePixelMapManager::GetInstance().GetNativePixelMapType(pixelMap)) {
        case NativePixelMapType::OBJECT_FROM_C:
            p = reinterpret_cast<OH_PixelmapNative*>(pixelMap)->GetInnerPixelmap();
            break;
        case NativePixelMapType::OBJECT_FROM_JS:
            p = Media::PixelMapNative_GetPixelMap(reinterpret_cast<NativePixelMap_*>(pixelMap));
            break;
        default:
            break;
    }

    Bitmap bitmap;
    if (!DrawingCanvasUtils::ExtractDrawingBitmap(p, bitmap)) {
        LOGE("OH_Drawing_CanvasCreateWithPixelMap: pixelMap is invalid!");
        return nullptr;
    }
    Canvas* canvas = new Canvas();
    canvas->Bind(bitmap);
    {
        std::lock_guard<std::mutex> lock(g_canvasMutex);
        g_canvasMap.insert({canvas, p});
    }
    return (OH_Drawing_Canvas*)canvas;
#else
    return nullptr;
#endif
}

void OH_Drawing_CanvasDestroy(OH_Drawing_Canvas* cCanvas)
{
    if (!cCanvas) {
        return;
    }
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(cCanvas);
    if (iter != g_canvasMap.end()) {
        std::lock_guard<std::mutex> lock(g_canvasMutex);
        g_canvasMap.erase(iter);
    }
#endif
    delete CastToCanvas(cCanvas);
}

void OH_Drawing_CanvasBind(OH_Drawing_Canvas* cCanvas, OH_Drawing_Bitmap* cBitmap)
{
    if (cBitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->Bind(CastToBitmap(*cBitmap));
}

void OH_Drawing_CanvasAttachPen(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Pen* cPen)
{
    if (cPen == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->AttachPen(CastToPen(*cPen));
}

void OH_Drawing_CanvasDetachPen(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DetachPen();
}

void OH_Drawing_CanvasAttachBrush(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->AttachBrush(CastToBrush(*cBrush));
}

void OH_Drawing_CanvasDetachBrush(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DetachBrush();
}

void OH_Drawing_CanvasSave(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->Save();
}

void OH_Drawing_CanvasSaveLayer(OH_Drawing_Canvas* cCanvas,
    const OH_Drawing_Rect* cRect, const OH_Drawing_Brush* cBrush)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    SaveLayerOps slr = SaveLayerOps(reinterpret_cast<const Drawing::Rect*>(cRect),
        reinterpret_cast<const Drawing::Brush*>(cBrush));
    canvas->SaveLayer(slr);
}

void OH_Drawing_CanvasRestore(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->Restore();
}

uint32_t OH_Drawing_CanvasGetSaveCount(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return canvas->GetSaveCount();
}

void OH_Drawing_CanvasRestoreToCount(OH_Drawing_Canvas* cCanvas, uint32_t saveCount)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->RestoreToCount(saveCount);
}

void OH_Drawing_CanvasDrawLine(OH_Drawing_Canvas* cCanvas, float x1, float y1, float x2, float y2)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Point startPt(x1, y1);
    Point endPt(x2, y2);
    canvas->DrawLine(startPt, endPt);
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawPath(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Path* cPath)
{
    if (cPath == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawPath(CastToPath(*cPath));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawPoints(OH_Drawing_Canvas* cCanvas, OH_Drawing_PointMode mode,
    uint32_t count, const OH_Drawing_Point2D* pts)
{
    if (pts == nullptr || count == 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    if (mode < POINT_MODE_POINTS || mode > POINT_MODE_POLYGON) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    const Point* points = reinterpret_cast<const Point*>(pts);
    canvas->DrawPoints(static_cast<PointMode>(mode), count, points);
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawVertices(OH_Drawing_Canvas* cCanvas, OH_Drawing_VertexMode vertexMode,
    int32_t vertexCount, const OH_Drawing_Point2D* positions, const OH_Drawing_Point2D* texs,
    const uint32_t* colors, int32_t indexCount, const uint16_t* indices, OH_Drawing_BlendMode mode)
{
    // 3 means the minimum number of vertices required for a triangle
    if (positions == nullptr || vertexCount < 3 || (indexCount < 3 && indexCount != 0)) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    if (vertexMode < VERTEX_MODE_TRIANGLES || vertexMode > VERTEX_MODE_TRIANGLE_FAN) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }

    if (mode < BLEND_MODE_CLEAR || mode > BLEND_MODE_LUMINOSITY) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }

    Point* positionsPoint = new(std::nothrow) Point[vertexCount];
    if (positionsPoint == nullptr) {
        LOGE("OH_Drawing_CanvasDrawVertices: new position point failed.");
        return;
    }
    for (int32_t i = 0; i < vertexCount; ++i) {
        positionsPoint[i] = CastToPoint(positions[i]);
    }

    Point* texsPoint = nullptr;
    if (texs != nullptr) {
        texsPoint = new(std::nothrow) Point[vertexCount];
        if (texsPoint == nullptr) {
            delete [] positionsPoint;
            LOGE("OH_Drawing_CanvasDrawVertices: new texs point failed.");
            return;
        }
        for (int32_t i = 0; i < vertexCount; i++) {
            texsPoint[i] = CastToPoint(texs[i]);
        }
    }

    Vertices* vertices = new Vertices();
    bool result = vertices->MakeCopy(static_cast<VertexMode>(vertexMode), vertexCount, positionsPoint,
        texsPoint, colors, indices ? indexCount : 0, indices);
    if (result) {
        canvas->DrawVertices(*vertices, static_cast<BlendMode>(mode));
#ifdef OHOS_PLATFORM
        auto iter = g_canvasMap.find(canvas);
        if (iter != g_canvasMap.end() && iter->second != nullptr) {
            iter->second->MarkDirty();
        }
#endif
    }
    delete vertices;
    delete [] positionsPoint;
    delete [] texsPoint;
}

void OH_Drawing_CanvasDrawBackground(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawBackground(CastToBrush(*cBrush));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawRegion(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Region* cRegion)
{
    if (cRegion == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawRegion(CastToRegion(*cRegion));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawBitmap(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Bitmap* cBitmap, float left, float top)
{
    if (cBitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawBitmap(CastToBitmap(*cBitmap), left, top);
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawPixelMapNine(OH_Drawing_Canvas* cCanvas, OH_Drawing_PixelMap* pixelMap,
    const OH_Drawing_Rect* center, const OH_Drawing_Rect* dst, OH_Drawing_FilterMode mode)
{
#ifdef OHOS_PLATFORM
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || pixelMap == nullptr || dst == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
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
    OH_Drawing_ErrorCode errorCode = DrawingCanvasUtils::DrawPixelMapNine(canvas, p,
        CastToRect(center), CastToRect(dst), static_cast<FilterMode>(mode));
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
    return errorCode;
#else
    return OH_DRAWING_SUCCESS;
#endif
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
    auto iter = g_canvasMap.find(cCanvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawPixelMapRectConstraint(OH_Drawing_Canvas* cCanvas,
    OH_Drawing_PixelMap* pixelMap, const OH_Drawing_Rect* src, const OH_Drawing_Rect* dst,
    const OH_Drawing_SamplingOptions* cSamplingOptions, OH_Drawing_SrcRectConstraint cConstraint)
{
#ifdef OHOS_PLATFORM
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || pixelMap == nullptr || dst == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    std::shared_ptr<Media::PixelMap> p = nullptr;
    switch (NativePixelMapManager::GetInstance().GetNativePixelMapType(pixelMap)) {
        case NativePixelMapType::OBJECT_FROM_C:
            p = reinterpret_cast<OH_PixelmapNative*>(pixelMap)->GetInnerPixelmap();
            break;
        case NativePixelMapType::OBJECT_FROM_JS:
            p = Media::PixelMapNative_GetPixelMap(reinterpret_cast<NativePixelMap_*>(pixelMap));
            break;
        default:
            break;
    }
    OH_Drawing_ErrorCode errorCode = DrawingCanvasUtils::DrawPixelMapRectConstraint(CastToCanvas(cCanvas), p,
        reinterpret_cast<const Drawing::Rect*>(src), reinterpret_cast<const Drawing::Rect*>(dst),
        reinterpret_cast<const Drawing::SamplingOptions*>(cSamplingOptions),
        static_cast<SrcRectConstraint>(cConstraint));
    auto iter = g_canvasMap.find(cCanvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
    return errorCode;
#else
    return OH_DRAWING_SUCCESS;
#endif
}

void OH_Drawing_CanvasDrawBitmapRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Bitmap* cBitmap,
    const OH_Drawing_Rect* src, const OH_Drawing_Rect* dst, const OH_Drawing_SamplingOptions* cSampling)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cBitmap == nullptr || dst == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    const Bitmap& bitmap = CastToBitmap(*cBitmap);
    auto image = bitmap.MakeImage();
    if (image == nullptr) {
        return;
    }
    if (src == nullptr) {
        canvas->DrawImageRect(*image, CastToRect(*dst),
            cSampling ? CastToSamplingOptions(*cSampling) : Drawing::SamplingOptions());
    } else {
        canvas->DrawImageRect(*image, CastToRect(*src),
            CastToRect(*dst), cSampling ? CastToSamplingOptions(*cSampling) : Drawing::SamplingOptions());
    }
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect)
{
    if (cRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawRect(CastToRect(*cRect));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawCircle(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Point* cPoint, float radius)
{
    if (cPoint == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (radius <= 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawCircle(CastToPoint(*cPoint), radius);
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawOval(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect)
{
    if (cRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawOval(CastToRect(*cRect));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawArc(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect,
    float startAngle, float sweepAngle)
{
    if (cRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawArc(CastToRect(*cRect), startAngle, sweepAngle);
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawArcWithCenter(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect,
    float startAngle, float sweepAngle, bool useCenter)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const Drawing::Rect* rect = CastToRect(cRect);
    if (rect == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }

    if (useCenter) {
        canvas->DrawPie(*rect, startAngle, sweepAngle);
    } else {
        canvas->DrawArc(*rect, startAngle, sweepAngle);
    }
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
    return OH_DRAWING_SUCCESS;
}

void OH_Drawing_CanvasDrawRoundRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_RoundRect* cRoundRect)
{
    if (cRoundRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawRoundRect(CastToRoundRect(*cRoundRect));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawNestedRoundRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_RoundRect* outer,
    const OH_Drawing_RoundRect* inner)
{
    const RoundRect* roundRectOuter = CastToRoundRect(outer);
    if (roundRectOuter == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const RoundRect* roundRectInner = CastToRoundRect(inner);
    if (roundRectInner == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    canvas->DrawNestedRoundRect(*roundRectOuter, *roundRectInner);
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawSingleCharacter(OH_Drawing_Canvas* cCanvas, const char* str,
    const OH_Drawing_Font* cFont, float x, float y)
{
    if (str == nullptr || cFont == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    size_t len = strlen(str);
    if (len == 0) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const char* currentStr = str;
    int32_t unicode = SkUTF::NextUTF8(&currentStr, currentStr + len);
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::MatchThemeFont(font, unicode);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    canvas->DrawSingleCharacter(unicode, *font, x, y);
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawSingleCharacterWithFeatures(OH_Drawing_Canvas* cCanvas, const char* str,
    const OH_Drawing_Font* cFont, float x, float y, OH_Drawing_FontFeatures* fontFeatures)
{
    Drawing::DrawingFontFeatures* features =  CastToFontFeatures(fontFeatures);
    if (str == nullptr || cFont == nullptr || features == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    size_t len = strlen(str);
    if (len == 0) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    const char* currentStr = str;
    int32_t unicode = SkUTF::NextUTF8(&currentStr, currentStr + len);
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::MatchThemeFont(font, unicode);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    // copy UTF char to array
    int32_t utfCharLen = currentStr - str;
    std::vector<char> strBuffer(utfCharLen + 1);
    for (int32_t i = 0; i < utfCharLen; ++i) {
        strBuffer[i] = str[i];
    }
    strBuffer[utfCharLen] = 0;
    std::shared_ptr<Drawing::DrawingFontFeatures> featureCopy = std::make_shared<Drawing::DrawingFontFeatures>();
    for (const auto& entry : *features) {
        featureCopy->push_back(entry);
    }
    canvas->DrawSingleCharacterWithFeatures(strBuffer.data(), *font, x, y, featureCopy);
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
    return OH_DRAWING_SUCCESS;
}


void OH_Drawing_CanvasDrawTextBlob(OH_Drawing_Canvas* cCanvas, const OH_Drawing_TextBlob* cTextBlob, float x, float y)
{
    if (cTextBlob == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawTextBlob(CastToTextBlob(cTextBlob), x, y);
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasClipRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect,
    OH_Drawing_CanvasClipOp cClipOp, bool doAntiAlias)
{
    if (cRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    if (cClipOp < DIFFERENCE || cClipOp > INTERSECT) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    canvas->ClipRect(CastToRect(*cRect), static_cast<ClipOp>(cClipOp), doAntiAlias);
}

void OH_Drawing_CanvasClipRoundRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_RoundRect* cRoundRect,
    OH_Drawing_CanvasClipOp cClipOp, bool doAntiAlias)
{
    if (cRoundRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    if (cClipOp < DIFFERENCE || cClipOp > INTERSECT) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    canvas->ClipRoundRect(CastToRoundRect(*cRoundRect), static_cast<ClipOp>(cClipOp), doAntiAlias);
}

void OH_Drawing_CanvasClipPath(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Path* cPath,
    OH_Drawing_CanvasClipOp cClipOp, bool doAntiAlias)
{
    if (cPath == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }

    if (cClipOp < DIFFERENCE || cClipOp > INTERSECT) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    canvas->ClipPath(CastToPath(*cPath), static_cast<ClipOp>(cClipOp), doAntiAlias);
}

void OH_Drawing_CanvasRotate(OH_Drawing_Canvas* cCanvas, float degrees, float px, float py)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->Rotate(degrees, px, py);
}

void OH_Drawing_CanvasTranslate(OH_Drawing_Canvas* cCanvas, float dx, float dy)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->Translate(dx, dy);
}

void OH_Drawing_CanvasScale(OH_Drawing_Canvas* cCanvas, float sx, float sy)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->Scale(sx, sy);
}

void OH_Drawing_CanvasSkew(OH_Drawing_Canvas* cCanvas, float sx, float sy)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->Shear(sx, sy);
}

void OH_Drawing_CanvasClear(OH_Drawing_Canvas* cCanvas, uint32_t color)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->Clear(color);
}

int32_t OH_Drawing_CanvasGetWidth(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return canvas->GetWidth();
}

int32_t OH_Drawing_CanvasGetHeight(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return canvas->GetHeight();
}

void OH_Drawing_CanvasGetLocalClipBounds(OH_Drawing_Canvas* cCanvas, OH_Drawing_Rect* cRect)
{
    if (cRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
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
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
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
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->ConcatMatrix(*reinterpret_cast<Matrix*>(cMatrix));
}

void OH_Drawing_CanvasDrawShadow(OH_Drawing_Canvas* cCanvas, OH_Drawing_Path* cPath, OH_Drawing_Point3D cPlaneParams,
    OH_Drawing_Point3D cDevLightPos, float lightRadius, uint32_t ambientColor, uint32_t spotColor,
    OH_Drawing_CanvasShadowFlags flag)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cPath == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (flag < SHADOW_FLAGS_NONE || flag > SHADOW_FLAGS_ALL) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    canvas->DrawShadow(*reinterpret_cast<Path*>(cPath), CastToPoint3(cPlaneParams),
        CastToPoint3(cDevLightPos), lightRadius, Color(ambientColor), Color(spotColor),
        static_cast<ShadowFlags>(flag));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasSetMatrix(OH_Drawing_Canvas* cCanvas, OH_Drawing_Matrix* matrix)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || matrix == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->SetMatrix(CastToMatrix(*matrix));
}

void OH_Drawing_CanvasResetMatrix(OH_Drawing_Canvas* cCanvas)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->ResetMatrix();
}

void OH_Drawing_CanvasDrawImageRectWithSrc(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Image* cImage,
    const OH_Drawing_Rect* src, const OH_Drawing_Rect* dst, const OH_Drawing_SamplingOptions* cSampling,
    OH_Drawing_SrcRectConstraint constraint)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cImage == nullptr || src == nullptr || dst == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawImageRect(CastToImage(*cImage), CastToRect(*src), CastToRect(*dst), cSampling
        ? CastToSamplingOptions(*cSampling) : Drawing::SamplingOptions(), static_cast<SrcRectConstraint>(constraint));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

void OH_Drawing_CanvasDrawImageRect(OH_Drawing_Canvas* cCanvas, OH_Drawing_Image* cImage, OH_Drawing_Rect* dst,
    OH_Drawing_SamplingOptions* cSampling)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cImage == nullptr || dst == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    canvas->DrawImageRect(CastToImage(*cImage), CastToRect(*dst),
        cSampling ? CastToSamplingOptions(*cSampling) : Drawing::SamplingOptions());
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
}

bool OH_Drawing_CanvasReadPixels(OH_Drawing_Canvas* cCanvas, OH_Drawing_Image_Info* cImageInfo,
    void* dstPixels, uint32_t dstRowBytes, int32_t srcX, int32_t srcY)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cImageInfo == nullptr || dstPixels == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    ImageInfo imageInfo(cImageInfo->width, cImageInfo->height,
        static_cast<ColorType>(cImageInfo->colorType), static_cast<AlphaType>(cImageInfo->alphaType));
    return canvas->ReadPixels(imageInfo, dstPixels, dstRowBytes, srcX, srcY);
}

bool OH_Drawing_CanvasReadPixelsToBitmap(OH_Drawing_Canvas* cCanvas, OH_Drawing_Bitmap* cBitmap,
    int32_t srcX, int32_t srcY)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr || cBitmap == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return canvas->ReadPixels(CastToBitmap(*cBitmap), srcX, srcY);
}

OH_Drawing_ErrorCode OH_Drawing_CanvasIsClipEmpty(OH_Drawing_Canvas* cCanvas, bool* isClipEmpty)
{
    if (isClipEmpty == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *isClipEmpty = canvas->IsClipEmpty();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasGetImageInfo(OH_Drawing_Canvas* cCanvas, OH_Drawing_Image_Info* cImageInfo)
{
    if (cCanvas == nullptr || cImageInfo == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    ImageInfo imageInfo = CastToCanvas(cCanvas)->GetImageInfo();

    cImageInfo->width = imageInfo.GetWidth();
    cImageInfo->height = imageInfo.GetHeight();
    cImageInfo->colorType = static_cast<OH_Drawing_ColorFormat>(imageInfo.GetColorType());
    cImageInfo->alphaType = static_cast<OH_Drawing_AlphaFormat>(imageInfo.GetAlphaType());
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasClipRegion(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Region* cRegion,
    OH_Drawing_CanvasClipOp op)
{
    if (cRegion == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }

    if (op < DIFFERENCE || op > INTERSECT) {
        return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
    }
    canvas->ClipRegion(CastToRegion(*cRegion), static_cast<ClipOp>(op));
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawPoint(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Point2D* cPoint)
{
    if (cPoint == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    canvas->DrawPoint(CastToPoint(*cPoint));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawColor(OH_Drawing_Canvas* cCanvas, uint32_t color,
    OH_Drawing_BlendMode cBlendMode)
{
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }

    if (cBlendMode < BLEND_MODE_CLEAR || cBlendMode > BLEND_MODE_LUMINOSITY) {
        return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
    }

    canvas->DrawColor(color, static_cast<BlendMode>(cBlendMode));
#ifdef OHOS_PLATFORM
    auto iter = g_canvasMap.find(canvas);
    if (iter != g_canvasMap.end() && iter->second != nullptr) {
        iter->second->MarkDirty();
    }
#endif
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawRecordCmd(OH_Drawing_Canvas* cCanvas,
    OH_Drawing_RecordCmd* cRecordCmd)
{
    if (cCanvas == nullptr || cRecordCmd == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    auto recordCmdHandle = Helper::CastTo<OH_Drawing_RecordCmd*, NativeHandle<RecordCmd>*>(cRecordCmd);
    if (recordCmdHandle->value == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    DrawingCanvasUtils::DrawRecordCmd(canvas, recordCmdHandle->value);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasDrawRecordCmdNesting(OH_Drawing_Canvas* cCanvas,
    OH_Drawing_RecordCmd* cRecordCmd)
{
    if (cCanvas == nullptr || cRecordCmd == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    auto recordCmdHandle = Helper::CastTo<OH_Drawing_RecordCmd*, NativeHandle<RecordCmd>*>(cRecordCmd);
    if (recordCmdHandle->value == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    DrawingCanvasUtils::DrawRecordCmd(canvas, recordCmdHandle->value, true);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasQuickRejectPath(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Path* cPath,
    bool* quickReject)
{
    if (quickReject == nullptr || cPath == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *quickReject = canvas->QuickReject(CastToPath(*cPath));
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_CanvasQuickRejectRect(OH_Drawing_Canvas* cCanvas, const OH_Drawing_Rect* cRect,
    bool* quickReject)
{
    if (quickReject == nullptr || cRect == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Canvas* canvas = CastToCanvas(cCanvas);
    if (canvas == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *quickReject = canvas->QuickReject(CastToRect(*cRect));
    return OH_DRAWING_SUCCESS;
}