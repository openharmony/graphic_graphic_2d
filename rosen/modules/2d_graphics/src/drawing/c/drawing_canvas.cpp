/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "c/drawing_canvas.h"

#include "draw/canvas.h"
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
    if (canvas->GetDrawingType() == DrawingType::RECORDING) {
        (static_cast<RecordingCanvas*>(canvas))->SetIsCustomTextType(true);
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