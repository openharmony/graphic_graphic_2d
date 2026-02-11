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

#include "ani_canvas.h"

#include <cstdint>

#include "ani.h"
#include "brush_ani/ani_brush.h"
#include "draw/path.h"
#include "effect/color_space.h"
#include "pixel_map_taihe_ani.h"
#include "image/image.h"
#include "text/text.h"

#include "pixel_map_taihe_ani.h"
#include "font_ani/ani_font.h"
#include "lattice_ani/ani_lattice.h"
#include "matrix_ani/ani_matrix.h"
#include "path_ani/ani_path.h"
#include "pen_ani/ani_pen.h"
#include "utils/point.h"
#include "utils/vertices.h"
#include "region_ani/ani_region.h"
#include "round_rect_ani/ani_round_rect.h"
#include "sampling_options_ani/ani_sampling_options.h"
#include "text_blob_ani/ani_text_blob.h"
#ifdef OHOS_PLATFORM
#include "pipeline/rs_recording_canvas.h"
#endif

#ifdef USE_M133_SKIA
#include "src/base/SkUTF.h"
#else
#include "src/utils/SkUTF.h"
#endif
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/canvas_napi/js_canvas.h"

namespace OHOS::Rosen {
using namespace Media;
#ifdef ROSEN_OHOS
namespace {
static std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(ColorSpace colorSpace)
{
    switch (colorSpace) {
        case ColorSpace::DISPLAY_P3:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        case ColorSpace::LINEAR_SRGB:
            return Drawing::ColorSpace::CreateSRGBLinear();
        case ColorSpace::SRGB:
            return Drawing::ColorSpace::CreateSRGB();
        default:
            return Drawing::ColorSpace::CreateSRGB();
    }
}

static Drawing::ColorType PixelFormatToDrawingColorType(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        case PixelFormat::RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case PixelFormat::BGRA_8888:
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        case PixelFormat::ALPHA_8:
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        case PixelFormat::RGBA_F16:
            return Drawing::ColorType::COLORTYPE_RGBA_F16;
        case PixelFormat::UNKNOWN:
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::CMYK:
        default:
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}

static Drawing::AlphaType AlphaTypeToDrawingAlphaType(AlphaType alphaType)
{
    switch (alphaType) {
        case AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}

struct PixelMapReleaseContext {
    explicit PixelMapReleaseContext(std::shared_ptr<PixelMap> pixelMap) : pixelMap_(pixelMap) {}

    ~PixelMapReleaseContext()
    {
        pixelMap_ = nullptr;
    }

private:
    std::shared_ptr<PixelMap> pixelMap_;
};

static void PixelMapReleaseProc(const void* /* pixels */, void* context)
{
    PixelMapReleaseContext* ctx = static_cast<PixelMapReleaseContext*>(context);
    if (ctx) {
        delete ctx;
        ctx = nullptr;
    }
}

std::shared_ptr<Drawing::Image> ExtractDrawingImage(std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (!pixelMap) {
        ROSEN_LOGE("Drawing_napi::pixelMap fail");
        return nullptr;
    }
    ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToDrawingColorType(imageInfo.pixelFormat),
        AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        ColorSpaceToDrawingColorSpace(imageInfo.colorSpace) };
    Drawing::Pixmap imagePixmap(drawingImageInfo,
        reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowStride());
    PixelMapReleaseContext* releaseContext = new PixelMapReleaseContext(pixelMap);
    auto image = Drawing::Image::MakeFromRaster(imagePixmap, PixelMapReleaseProc, releaseContext);
    if (!image) {
        ROSEN_LOGE("Drawing_napi :RSPixelMapUtil::ExtractDrawingImage fail");
        delete releaseContext;
        releaseContext = nullptr;
    }
    return image;
}

bool ExtracetDrawingBitmap(std::shared_ptr<Media::PixelMap> pixelMap, Drawing::Bitmap& bitmap)
{
    if (!pixelMap) {
        ROSEN_LOGE("Drawing_napi ::pixelMap fail");
        return false;
    }
    ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToDrawingColorType(imageInfo.pixelFormat),
        AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        ColorSpaceToDrawingColorSpace(imageInfo.colorSpace) };
    bitmap.Build(drawingImageInfo, pixelMap->GetRowStride());
    bitmap.SetPixels(const_cast<void*>(reinterpret_cast<const void*>(pixelMap->GetPixels())));
    return true;
}

void DrawingPixelMapMesh(std::shared_ptr<Media::PixelMap> pixelMap, int column, int row,
    float* vertices, uint32_t* colors, Drawing::Canvas* m_canvas)
{
    const int vertCounts = (column + 1) * (row + 1);
    int32_t size = 6; // triangle * 2
    const int indexCount = column * row * size;
    uint32_t flags = Drawing::BuilderFlags::HAS_TEXCOORDS_BUILDER_FLAG;
    if (colors) {
        flags |= Drawing::BuilderFlags::HAS_COLORS_BUILDER_FLAG;
    }
    Drawing::Vertices::Builder builder(Drawing::VertexMode::TRIANGLES_VERTEXMODE, vertCounts, indexCount, flags);
    if (memcpy_s(builder.Positions(), vertCounts * sizeof(Drawing::Point),
        vertices, vertCounts * sizeof(Drawing::Point)) != 0) {
        ROSEN_LOGE("Drawing_napi::DrawingPixelMapMesh memcpy points failed");
        return;
    }
    int32_t colorSize = 4; // size of color
    if (colors && (memcpy_s(builder.Colors(), vertCounts * colorSize, colors, vertCounts * colorSize) != 0)) {
        ROSEN_LOGE("Drawing_napi::DrawingPixelMapMesh memcpy colors failed");
        return;
    }
    Drawing::Point* texsPoint = builder.TexCoords();
    uint16_t* indices = builder.Indices();
    if (!texsPoint || !indices) {
        ROSEN_LOGE("DrawingPixelMapMesh: texsPoint or indices is nullptr");
        return;
    }

    const float height = static_cast<float>(pixelMap->GetHeight());
    const float width = static_cast<float>(pixelMap->GetWidth());

    if (column == 0 || row == 0) {
        ROSEN_LOGE("Drawing_napi::DrawingPixelMapMesh column or row is invalid");
        return;
    }
    const float dy = height / row;
    const float dx = width / column;

    Drawing::Point* texsPit = texsPoint;
    float y = 0;
    for (int i = 0; i <= row; i++) {
        if (i == row) {
            y = height;
        }
        float x = 0;
        for (int j = 0; j < column; j++) {
            texsPit->Set(x, y);
            texsPit += 1;
            x += dx;
        }
        texsPit->Set(width, y);
        texsPit += 1;
        y += dy;
    }

    uint16_t* dexIndices = indices;
    int indexIndices = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            *dexIndices++ = indexIndices;
            *dexIndices++ = indexIndices + column + 1;
            *dexIndices++ = indexIndices + column + 2; // 2 means the third index of the triangle

            *dexIndices++ = indexIndices;
            *dexIndices++ = indexIndices + column + 2; // 2 means the third index of the triangle
            *dexIndices++ = indexIndices + 1;

            indexIndices += 1;
        }
        indexIndices += 1;
    }

    if (!m_canvas->GetMutableBrush().IsValid()) {
        ROSEN_LOGE("Drawing_napi::DrawingPixelMapMesh paint is invalid");
        return;
    }
    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMap);
    if (image == nullptr) {
        ROSEN_LOGE("Drawing_napi::DrawingPixelMapMesh image is nullptr");
        return;
    }
    auto shader = Drawing::ShaderEffect::CreateImageShader(*image,
        Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    m_canvas->GetMutableBrush().SetShaderEffect(shader);
    m_canvas->DrawVertices(*builder.Detach(), Drawing::BlendMode::MODULATE);
}
}
#endif

namespace Drawing {

static constexpr int VERTEX_COUNT_MIN_SIZE = 3;

static const std::array g_methods = {
    ani_native_function { "constructorNative", nullptr, reinterpret_cast<void*>(AniCanvas::Constructor) },
    ani_native_function { "drawRect", "C{@ohos.graphics.common2D.common2D.Rect}:",
        reinterpret_cast<void*>(AniCanvas::DrawRectWithRect) },
    ani_native_function { "drawRect", "dddd:", reinterpret_cast<void*>(AniCanvas::DrawRect) },
    ani_native_function { "drawRoundRect", nullptr, reinterpret_cast<void*>(AniCanvas::DrawRoundRect) },
    ani_native_function { "drawNestedRoundRect", nullptr, reinterpret_cast<void*>(AniCanvas::DrawNestedRoundRect) },
    ani_native_function { "drawBackground", nullptr, reinterpret_cast<void*>(AniCanvas::DrawBackground) },
    ani_native_function { "drawShadow", "C{@ohos.graphics.drawing.drawing.Path}"
        "C{@ohos.graphics.common2D.common2D.Point3d}C{@ohos.graphics.common2D.common2D.Point3d}"
        "dC{@ohos.graphics.common2D.common2D.Color}C{@ohos.graphics.common2D.common2D.Color}"
        "C{@ohos.graphics.drawing.drawing.ShadowFlag}:",
        reinterpret_cast<void*>(AniCanvas::DrawShadow) },
    ani_native_function { "drawShadow", "C{@ohos.graphics.drawing.drawing.Path}"
        "C{@ohos.graphics.common2D.common2D.Point3d}C{@ohos.graphics.common2D.common2D.Point3d}"
        "dX{C{@ohos.graphics.common2D.common2D.Color}C{std.core.Int}}"
        "X{C{@ohos.graphics.common2D.common2D.Color}C{std.core.Int}}C{@ohos.graphics.drawing.drawing.ShadowFlag}:",
        reinterpret_cast<void*>(AniCanvas::DrawShadowWithOption) },
    ani_native_function { "drawCircle", nullptr, reinterpret_cast<void*>(AniCanvas::DrawCircle) },
    ani_native_function { "drawImage", nullptr, reinterpret_cast<void*>(AniCanvas::DrawImage) },
    ani_native_function { "drawImageLattice", nullptr, reinterpret_cast<void*>(AniCanvas::DrawImageLattice) },
    ani_native_function { "drawImageNine", nullptr, reinterpret_cast<void*>(AniCanvas::DrawImageNine) },
    ani_native_function { "drawImageRect", nullptr, reinterpret_cast<void*>(AniCanvas::DrawImageRect) },
    ani_native_function { "drawVertices", nullptr, reinterpret_cast<void*>(AniCanvas::DrawVertices) },
    ani_native_function { "drawImageRectWithSrc", nullptr, reinterpret_cast<void*>(AniCanvas::DrawImageRectWithSrc) },
    ani_native_function { "drawColor",
        "C{@ohos.graphics.common2D.common2D.Color}C{@ohos.graphics.drawing.drawing.BlendMode}:",
        reinterpret_cast<void*>(AniCanvas::DrawColorWithObject) },
    ani_native_function { "drawColor", "iiiiC{@ohos.graphics.drawing.drawing.BlendMode}:",
        reinterpret_cast<void*>(AniCanvas::DrawColorWithArgb) },
    ani_native_function { "drawColor", "iC{@ohos.graphics.drawing.drawing.BlendMode}:",
        reinterpret_cast<void*>(AniCanvas::DrawColor) },
    ani_native_function { "drawPath", nullptr, reinterpret_cast<void*>(AniCanvas::DrawPath) },
    ani_native_function { "drawLine", nullptr, reinterpret_cast<void*>(AniCanvas::DrawLine) },
    ani_native_function { "drawSingleCharacter", nullptr, reinterpret_cast<void*>(AniCanvas::DrawSingleCharacter) },
    ani_native_function { "drawTextBlob", nullptr, reinterpret_cast<void*>(AniCanvas::DrawTextBlob) },
    ani_native_function { "drawOval", nullptr, reinterpret_cast<void*>(AniCanvas::DrawOval) },
    ani_native_function { "drawArc", nullptr, reinterpret_cast<void*>(AniCanvas::DrawArc) },
    ani_native_function { "drawArcWithCenter", nullptr, reinterpret_cast<void*>(AniCanvas::DrawArcWithCenter) },
    ani_native_function { "drawPoint", nullptr, reinterpret_cast<void*>(AniCanvas::DrawPoint) },
    ani_native_function { "drawPoints", nullptr, reinterpret_cast<void*>(AniCanvas::DrawPoints) },
    ani_native_function { "drawPixelMapMesh", nullptr, reinterpret_cast<void*>(AniCanvas::DrawPixelMapMesh) },
    ani_native_function { "drawRegion", nullptr, reinterpret_cast<void*>(AniCanvas::DrawRegion) },
    ani_native_function { "attachPen", nullptr, reinterpret_cast<void*>(AniCanvas::AttachPen) },
    ani_native_function { "attachBrush", nullptr, reinterpret_cast<void*>(AniCanvas::AttachBrush) },
    ani_native_function { "detachPen", nullptr, reinterpret_cast<void*>(AniCanvas::DetachPen) },
    ani_native_function { "detachBrush", nullptr, reinterpret_cast<void*>(AniCanvas::DetachBrush) },
    ani_native_function { "save", nullptr, reinterpret_cast<void*>(AniCanvas::Save) },
    ani_native_function { "saveLayer", nullptr, reinterpret_cast<void*>(AniCanvas::SaveLayer) },
    ani_native_function { "clear", "C{@ohos.graphics.common2D.common2D.Color}:",
        reinterpret_cast<void*>(AniCanvas::Clear) },
    ani_native_function { "clear", "X{C{@ohos.graphics.common2D.common2D.Color}C{std.core.Int}}:",
        reinterpret_cast<void*>(AniCanvas::ClearWithOption) },
    ani_native_function { "restore", nullptr, reinterpret_cast<void*>(AniCanvas::Restore) },
    ani_native_function { "restoreToCount", nullptr, reinterpret_cast<void*>(AniCanvas::RestoreToCount) },
    ani_native_function { "getSaveCount", nullptr, reinterpret_cast<void*>(AniCanvas::GetSaveCount) },
    ani_native_function { "getWidth", nullptr, reinterpret_cast<void*>(AniCanvas::GetWidth) },
    ani_native_function { "getHeight", nullptr, reinterpret_cast<void*>(AniCanvas::GetHeight) },
    ani_native_function { "getLocalClipBounds", nullptr, reinterpret_cast<void*>(AniCanvas::GetLocalClipBounds) },
    ani_native_function { "getTotalMatrix", nullptr, reinterpret_cast<void*>(AniCanvas::GetTotalMatrix) },
    ani_native_function { "scale", nullptr, reinterpret_cast<void*>(AniCanvas::Scale) },
    ani_native_function { "skew", nullptr, reinterpret_cast<void*>(AniCanvas::Skew) },
    ani_native_function { "rotate", nullptr, reinterpret_cast<void*>(AniCanvas::Rotate) },
    ani_native_function { "translate", nullptr, reinterpret_cast<void*>(AniCanvas::Translate) },
    ani_native_function { "clipPath", nullptr, reinterpret_cast<void*>(AniCanvas::ClipPath) },
    ani_native_function { "clipRect", nullptr, reinterpret_cast<void*>(AniCanvas::ClipRect) },
    ani_native_function { "concatMatrix", nullptr, reinterpret_cast<void*>(AniCanvas::ConcatMatrix) },
    ani_native_function { "clipRegion", nullptr, reinterpret_cast<void*>(AniCanvas::ClipRegion) },
    ani_native_function { "clipRoundRect", nullptr, reinterpret_cast<void*>(AniCanvas::ClipRoundRect) },
    ani_native_function { "isClipEmpty", nullptr, reinterpret_cast<void*>(AniCanvas::IsClipEmpty) },
    ani_native_function { "setMatrix", nullptr, reinterpret_cast<void*>(AniCanvas::SetMatrix) },
    ani_native_function { "resetMatrix", nullptr, reinterpret_cast<void*>(AniCanvas::ResetMatrix) },
    ani_native_function { "quickRejectPath", nullptr, reinterpret_cast<void*>(AniCanvas::QuickRejectPath) },
    ani_native_function { "quickRejectRect", nullptr, reinterpret_cast<void*>(AniCanvas::QuickRejectRect) },
};

ani_status AniCanvas::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().canvas;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_CANVAS_NAME);
        return ANI_NOT_FOUND;
    }
    ani_status ret = env->Class_BindNativeMethods(cls, g_methods.data(), g_methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_CANVAS_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "canvasTransferStaticNative", nullptr, reinterpret_cast<void*>(CanvasTransferStatic) },
        ani_native_function { "getCanvasAddr", nullptr, reinterpret_cast<void*>(GetCanvasAddr) },
        ani_native_function { "getPixelMapAddr", nullptr, reinterpret_cast<void*>(GetPixelMapAddr) },
        ani_native_function { "getOwned", nullptr, reinterpret_cast<void*>(GetCanvasOwned) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_CANVAS_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniCanvas::NotifyDirty()
{
#ifdef ROSEN_OHOS
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
}

void AniCanvas::Constructor(ani_env* env, ani_object obj, ani_object pixelmapObj)
{
    if (IsUndefined(env, pixelmapObj)) {
        AniCanvas *aniCanvas = new AniCanvas();
        if (ANI_OK != env->Object_SetField_Long(
            obj, AniGlobalField::GetInstance().canvasNativeObj, reinterpret_cast<ani_long>(aniCanvas))) {
            ROSEN_LOGE("AniCanvas::Constructor failed create AniCanvas with nullptr");
            delete aniCanvas;
        }
        return;
    }
#ifdef ROSEN_OHOS
    std::shared_ptr<PixelMap> pixelMap = PixelMapTaiheAni::GetNativePixelMap(env, pixelmapObj);
    if (!pixelMap) {
        ROSEN_LOGE("AniCanvas::Constructor get pixelMap failed");
        AniThrowError(env, "Invalid params.");
        return;
    }
    Bitmap bitmap;
    if (!ExtracetDrawingBitmap(pixelMap, bitmap)) {
        return;
    }

    Canvas* canvas = new Canvas();
    canvas->Bind(bitmap);
    AniCanvas *aniCanvas = new AniCanvas(canvas, true);
    aniCanvas->mPixelMap_ = pixelMap;

    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().canvasNativeObj, reinterpret_cast<ani_long>(aniCanvas))) {
        ROSEN_LOGE("AniCanvas::Constructor failed create aniCanvas");
        delete aniCanvas;
        return;
    }
#endif
}

void AniCanvas::DrawRect(ani_env* env, ani_object obj,
    ani_double left, ani_double top, ani_double right, ani_double bottom)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }

    Drawing::Rect drawingRect = Drawing::Rect(left, top, right, bottom);
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->DrawRect(drawingRect);

    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawRectWithRect(ani_env* env, ani_object obj, ani_object aniRect)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }

    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRect, drawingRect)) {
        ROSEN_LOGE("AniCanvas::DrawRectWithRect get drawingRect failed");
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->DrawRect(drawingRect);

    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawRoundRect(ani_env* env, ani_object obj, ani_object roundRectObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawRoundRect canvas is nullptr.");
        return;
    }
    auto aniRoundRect = GetNativeFromObj<AniRoundRect>(env, roundRectObj,
        AniGlobalField::GetInstance().roundRectNativeObj);
    if (aniRoundRect == nullptr || aniRoundRect->GetRoundRect() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawRoundRect roundRect is nullptr.");
        return;
    }
    aniCanvas->GetCanvas()->DrawRoundRect(*aniRoundRect->GetRoundRect());
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawNestedRoundRect(ani_env* env, ani_object obj, ani_object outerObj, ani_object innerObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawNestedRoundRect canvas is nullptr.");
        return;
    }

    auto aniOuterRoundRect = GetNativeFromObj<AniRoundRect>(env, outerObj,
        AniGlobalField::GetInstance().roundRectNativeObj);
    if (aniOuterRoundRect == nullptr || aniOuterRoundRect->GetRoundRect() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawNestedRoundRect outerRoundRect is nullptr.");
        return;
    }
    auto aniInnerRoundRect = GetNativeFromObj<AniRoundRect>(env, innerObj,
        AniGlobalField::GetInstance().roundRectNativeObj);
    if (aniInnerRoundRect == nullptr || aniInnerRoundRect->GetRoundRect() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawNestedRoundRect innerRoundRect is nullptr.");
        return;
    }
    aniCanvas->GetCanvas()->DrawNestedRoundRect(
        *aniOuterRoundRect->GetRoundRect(), *aniInnerRoundRect->GetRoundRect());
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawBackground(ani_env* env, ani_object obj, ani_object brushObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawBackground canvas is nullptr.");
        return;
    }
    auto aniBrush = GetNativeFromObj<AniBrush>(env, brushObj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawBackground brush is nullptr.");
        return;
    }
    if (aniBrush->GetBrush() == nullptr) {
        ROSEN_LOGE("AniCanvas::OnDrawBackground brush is nullptr");
        return;
    }
    aniCanvas->GetCanvas()->DrawBackground(*aniBrush->GetBrush());
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawShadow(ani_env* env, ani_object obj, ani_object pathObj,
    ani_object planeParams, ani_object devLightPos, ani_double lightRadius,
    ani_object ambientColor, ani_object spotColor, ani_enum_item flagEnum)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawShadow canvas is nullptr.");
        return;
    }
    auto aniPath = GetNativeFromObj<AniPath>(env, pathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawShadow path is nullptr.");
        return;
    }

    Drawing::Point3 plane;
    Drawing::Point3 lightPos;
    if (!GetPoint3FromPoint3dObj(env, planeParams, plane) || !GetPoint3FromPoint3dObj(env, devLightPos, lightPos)) {
        ROSEN_LOGE("AniCanvas::DrawShadow params1 or params2 is invalid");
        return;
    }

    Drawing::ColorQuad ambient;
    Drawing::ColorQuad spot;
    if (!GetColorQuadFromParam(env, ambientColor, ambient) || !GetColorQuadFromParam(env, spotColor, spot)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED,
            "AniCanvas::DrawShadow the range of color channels must be [0, 255].");
        return;
    }

    ani_int shadowFlag;
    if (ANI_OK != env->EnumItem_GetValue_Int(flagEnum, &shadowFlag)) {
        ROSEN_LOGE("AniCanvas::DrawShadow params6 is invalid");
        return;
    }
    if (CheckInt32OutOfRange(shadowFlag, 0, static_cast<int32_t>(ShadowFlags::ALL))) {
        ROSEN_LOGE("AniCanvas::DrawShadow params6 is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawShadow shadowFlag is out of range.");
        return;
    }
    aniCanvas->GetCanvas()->DrawShadow(*aniPath->GetPath(), plane, lightPos, lightRadius, ambient, spot,
        static_cast<ShadowFlags>(shadowFlag));
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawShadowWithOption(ani_env* env, ani_object obj, ani_object pathObj,
    ani_object planeParams, ani_object devLightPos, ani_double lightRadius,
    ani_object ambientColorOps, ani_object spotColorOps, ani_enum_item flagEnum)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawShadowWithOption canvas is nullptr.");
        return;
    }
    auto aniPath = GetNativeFromObj<AniPath>(env, pathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawShadowWithOption path is nullptr.");
        return;
    }

    Drawing::Point3 plane;
    Drawing::Point3 lightPos;
    if (!GetPoint3FromPoint3dObj(env, planeParams, plane) || !GetPoint3FromPoint3dObj(env, devLightPos, lightPos)) {
        ROSEN_LOGE("AniCanvas::DrawShadowWithOption params1 or params2 is invalid");
        return;
    }

    Drawing::ColorQuad ambient;
    Drawing::ColorQuad spot;
    if (!GetColorQuadFromParam(env, ambientColorOps, ambient) || !GetColorQuadFromParam(env, spotColorOps, spot)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED,
            "AniCanvas::DrawShadow the range of color channels must be [0, 255].");
        return;
    }

    ani_int shadowFlag;
    if (ANI_OK != env->EnumItem_GetValue_Int(flagEnum, &shadowFlag)) {
        ROSEN_LOGE("AniCanvas::DrawShadowWithOption params6 is invalid");
        return;
    }
    if (CheckInt32OutOfRange(shadowFlag, 0, static_cast<int32_t>(ShadowFlags::ALL))) {
        ROSEN_LOGE("AniCanvas::DrawShadowWithOption params6 is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawShadowWithOption shadowFlag is out of range.");
        return;
    }
    aniCanvas->GetCanvas()->DrawShadow(*aniPath->GetPath(), plane, lightPos, lightRadius, ambient, spot,
        static_cast<ShadowFlags>(shadowFlag));
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawCircle(ani_env* env, ani_object obj, ani_double x, ani_double y, ani_double radius)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawCircle canvas is nullptr.");
        return;
    }

    Drawing::Point centerPt = Drawing::Point(x, y);
    aniCanvas->GetCanvas()->DrawCircle(centerPt, radius);
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawImage(ani_env* env, ani_object obj, ani_object pixelmapObj,
    ani_double left, ani_double top, ani_object samplingOptionsObj)
{
#ifdef ROSEN_OHOS
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawImage canvas is nullptr.");
        return;
    }
    std::shared_ptr<PixelMap> pixelMap = PixelMapTaiheAni::GetNativePixelMap(env, pixelmapObj);
    if (!pixelMap) {
        ROSEN_LOGE("AniCanvas::DrawImage get pixelMap failed");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawImage pixelMap is nullptr.");
        return;
    }

    Drawing::SamplingOptions samplingOptions;
    if (!IsUndefined(env, samplingOptionsObj)) {
        AniSamplingOptions* aniSamplingOptions = GetNativeFromObj<AniSamplingOptions>(env, samplingOptionsObj,
            AniGlobalField::GetInstance().samplingOptionsNativeObj);
        if (aniSamplingOptions == nullptr || aniSamplingOptions->GetSamplingOptions() == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::DrawImage samplingOptions is nullptr.");
            return;
        }
        samplingOptions = *(aniSamplingOptions->GetSamplingOptions());
    }

    Drawing::Canvas* canvas = aniCanvas->GetCanvas();
    if (canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        Drawing::Rect src(0, 0, pixelMap->GetWidth(), pixelMap->GetHeight());
        Drawing::Rect dst(left, top, left + pixelMap->GetWidth(), top + pixelMap->GetHeight());
        canvas_->DrawPixelMapRect(pixelMap, src, dst, samplingOptions);
        return;
    }

    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMap);
    if (image == nullptr) {
        ROSEN_LOGE("AniCanvas::DrawImage image is nullptr");
        return;
    }
    canvas->DrawImage(*image, left, top, samplingOptions);
    aniCanvas->NotifyDirty();
#endif
}

void AniCanvas::DrawImageLattice(ani_env* env, ani_object obj, ani_object pixelmapObj,
    ani_object latticeObj, ani_object dstRectObj, ani_enum_item filterModeEnum)
{
#ifdef ROSEN_OHOS
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawImageLattice canvas is nullptr.");
        return;
    }
    std::shared_ptr<PixelMap> pixelMap = PixelMapTaiheAni::GetNativePixelMap(env, pixelmapObj);
    if (!pixelMap) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawImageLattice pixelMap is nullptr.");
        return;
    }

    auto aniLattice = GetNativeFromObj<AniLattice>(env, latticeObj, AniGlobalField::GetInstance().latticeNativeObj);
    if (aniLattice == nullptr || aniLattice->GetLattice() == nullptr) {
        ROSEN_LOGE("AniCanvas::DrawImageLattice lattice is nullptr");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawImageLattice lattice is nullptr.");
        return;
    }
    Drawing::Rect dstRect;
    if (!GetRectFromAniRectObj(env, dstRectObj, dstRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "DrawImageLattice incorrect type rect.");
        return;
    }
    ani_int filterMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(filterModeEnum, &filterMode)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawImageLattice get filterMode failed.");
        return;
    }
    if (CheckInt32OutOfRange(filterMode, 0, static_cast<int32_t>(FilterMode::LINEAR))) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawImageLattice filterMode is out of range.");
        return;
    }

    Canvas* canvas = aniCanvas->GetCanvas();
    std::shared_ptr<Lattice> lattice = aniLattice->GetLattice();
    if (canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        canvas_->DrawPixelMapLattice(pixelMap, *lattice, dstRect, static_cast<FilterMode>(filterMode));
        return;
    }
    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMap);
    canvas->DrawImageLattice(image.get(), *lattice, dstRect, static_cast<FilterMode>(filterMode));
    aniCanvas->NotifyDirty();
#endif
}

void AniCanvas::DrawImageNine(ani_env* env, ani_object obj, ani_object pixelmapObj,
    ani_object centerRectObj, ani_object dstRectObj, ani_enum_item filterModeEnum)
{
#ifdef ROSEN_OHOS
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawImageNine canvas is nullptr.");
        return;
    }
    std::shared_ptr<PixelMap> pixelMap = PixelMapTaiheAni::GetNativePixelMap(env, pixelmapObj);
    if (!pixelMap) {
        ROSEN_LOGE("AniCanvas::DrawImageNine get pixelMap failed");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawImageNine pixelMap is nullptr.");
        return;
    }
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, centerRectObj, drawingRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "DrawImageLattice incorrect type centerRect.");
        return;
    }
    Drawing::RectI centerRectI = Drawing::RectI(drawingRect.GetLeft(), drawingRect.GetTop(),
        drawingRect.GetRight(), drawingRect.GetBottom());

    Drawing::Rect dstRect;
    if (!GetRectFromAniRectObj(env, dstRectObj, dstRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "DrawImageLattice incorrect type dstRect.");
        return;
    }

    ani_int filterMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(filterModeEnum, &filterMode)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawImageNine get filterMode failed.");
        return;
    }
    if (CheckInt32OutOfRange(filterMode, 0, static_cast<int32_t>(FilterMode::LINEAR))) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawImageNine filterMode is out of range.");
        return;
    }
    
    Canvas* canvas = aniCanvas->GetCanvas();
    if (canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(canvas);
        canvas_->DrawPixelMapNine(pixelMap, centerRectI, dstRect, static_cast<FilterMode>(filterMode));
        return;
    }
    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMap);
    canvas->DrawImageNine(image.get(), centerRectI, dstRect, static_cast<FilterMode>(filterMode), nullptr);
    aniCanvas->NotifyDirty();
#endif
}

void AniCanvas::DrawImageRect(ani_env* env, ani_object obj,
    ani_object pixelmapObj, ani_object rectObj, ani_object samplingOptionsObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
#ifdef ROSEN_OHOS
    std::shared_ptr<PixelMap> pixelMap = PixelMapTaiheAni::GetNativePixelMap(env, pixelmapObj);
    if (!pixelMap) {
        ROSEN_LOGE("AniCanvas::DrawImageRect get pixelMap failed");
        AniThrowError(env, "Invalid params.");
        return;
    }
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, rectObj, drawingRect)) {
        ROSEN_LOGE("AniCanvas::DrawImageRect get drawingRect failed");
        AniThrowError(env, "Invalid params.");
        return;
    }

    AniSamplingOptions* aniSamplingOptions = nullptr;
    if (!IsUndefined(env, samplingOptionsObj)) {
        aniSamplingOptions = GetNativeFromObj<AniSamplingOptions>(env, samplingOptionsObj,
            AniGlobalField::GetInstance().samplingOptionsNativeObj);
        if (aniSamplingOptions == nullptr || aniSamplingOptions->GetSamplingOptions() == nullptr) {
            AniThrowError(env, "Invalid params.");
            return;
        }
    }
    aniCanvas->DrawImageRectInner(pixelMap, drawingRect, aniSamplingOptions);
    aniCanvas->NotifyDirty();
#endif
}

#ifdef ROSEN_OHOS
void AniCanvas::DrawImageRectInner(std::shared_ptr<Media::PixelMap> pixelmap,
    Drawing::Rect& rect, AniSamplingOptions* samplingOptions)
{
    if (!m_canvas) {
        return;
    }
    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelmap);
    if (image == nullptr) {
        ROSEN_LOGE("AniCanvas::DrawImageRectInner image is nullptr");
        return;
    }
    if (samplingOptions && samplingOptions->GetSamplingOptions()) {
        m_canvas->DrawImageRect(*image, rect, *samplingOptions->GetSamplingOptions());
    } else {
        m_canvas->DrawImageRect(*image, rect, Drawing::SamplingOptions());
    }
}

void AniCanvas::DrawImageRectWithSrcInner(std::shared_ptr<Media::PixelMap> pixelmap, Drawing::Rect& srcRect,
    Drawing::Rect& dstRect, AniSamplingOptions* samplingOptions, int32_t srcRectConstraint)
{
    if (!m_canvas) {
        return;
    }
    Drawing::SamplingOptions drSamplingOptions;
    if (samplingOptions && samplingOptions->GetSamplingOptions()) {
        drSamplingOptions = *samplingOptions->GetSamplingOptions();
    }

    if (m_canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(m_canvas);
        canvas_->DrawPixelMapRect(pixelmap, srcRect, dstRect, drSamplingOptions,
            static_cast<Drawing::SrcRectConstraint>(srcRectConstraint));
        return;
    }

    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelmap);
    if (image == nullptr) {
        ROSEN_LOGE("AniCanvas::DrawImageRectWithSrcInner image is nullptr");
        return;
    }
    m_canvas->DrawImageRect(*image, srcRect, dstRect, drSamplingOptions,
        static_cast<Drawing::SrcRectConstraint>(srcRectConstraint));
    NotifyDirty();
}
#endif

#ifdef ROSEN_OHOS
bool AniCanvas::GetVertices(ani_env* env, ani_array verticesObj, float* vertices, uint32_t verticesSize)
{
    for (uint32_t i = 0; i < verticesSize; i++) {
        ani_double vertex;
        ani_ref vertexRef;
        if (ANI_OK !=  env->Array_Get(verticesObj, static_cast<ani_size>(i), &vertexRef) ||
            ANI_OK !=  env->Object_CallMethod_Double(
                static_cast<ani_object>(vertexRef), AniGlobalMethod::GetInstance().doubleGet, &vertex)) {
            delete []vertices;
            return false;
        }
        vertices[i] = vertex;
    }
    return true;
}

bool AniCanvas::GetVerticesUint16(ani_env* env, ani_array verticesObj, uint16_t* vertices, uint32_t verticesSize)
{
    for (uint32_t i = 0; i < verticesSize; i++) {
        ani_int vertex;
        ani_ref vertexRef;
        if (ANI_OK != env->Array_Get(
            verticesObj, static_cast<ani_size>(i), &vertexRef) ||
            ANI_OK != env->Object_CallMethod_Int(
                static_cast<ani_object>(vertexRef), AniGlobalMethod::GetInstance().intGet, &vertex)) {
            ROSEN_LOGE("AniCanvas::GetVerticesUint16 vertices is invalid");
            return false;
        }
        if (vertex > std::numeric_limits<uint16_t>::max() || vertex < 0) {
            ROSEN_LOGE("AniCanvas::GetVerticesUint16 vertices value out of uint16_t range [0, 65535]");
            return false;
        }
        vertices[i] = static_cast<uint16_t>(vertex);
    }
    return true;
}

bool AniCanvas::GetColorsUint32(ani_env* env, ani_array colorsObj, uint32_t* colors, uint32_t colorsSize)
{
    for (uint32_t i = 0; i < colorsSize; i++) {
        ani_int color;
        ani_ref colorRef;
        if (ANI_OK != env->Array_Get(colorsObj, static_cast<ani_size>(i), &colorRef) ||
            ANI_OK != env->Object_CallMethod_Int(
                static_cast<ani_object>(colorRef), AniGlobalMethod::GetInstance().intGet, &color)) {
            ROSEN_LOGE("AniCanvas::GetColorsUint32 colors is invalid");
            return false;
        }
        colors[i] = static_cast<uint32_t>(color);
    }
    return true;
}
#endif

#ifdef ROSEN_OHOS
void AniCanvas::GetColorsAndDraw(ani_env* env, ani_array colorsObj, int32_t colorOffset,
    DrawPixelMapMeshArgs& args, AniCanvas* aniCanvas)
{
    float* verticesMesh = args.verticesSize ? (args.vertices + args.vertOffset * 2) : nullptr;
    ani_size aniLength;
    if (ANI_OK != env->Array_GetLength(colorsObj, &aniLength)) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    uint32_t colorsSize = static_cast<uint32_t>(aniLength);
    int64_t tempColorsSize = (args.column + 1) * (args.row + 1) + colorOffset;
    if (colorsSize != 0 && colorsSize != tempColorsSize) {
        ROSEN_LOGE("AniCanvas::GetColorsAndDraw colors are invalid");
        AniThrowError(env, "Incorrect parameter5 type");
        return;
    }
    auto canvas = aniCanvas->GetCanvas();
    
    if (colorsSize == 0) {
        DrawingPixelMapMesh(args.pixelMap, args.column, args.row, verticesMesh, nullptr, canvas);
        aniCanvas->NotifyDirty();
        return;
    }

    auto colors = new (std::nothrow) uint32_t[colorsSize];
    if (!colors) {
        ROSEN_LOGE("AniCanvas::GetColorsAndDraw create array with size of colors failed");
        AniThrowError(env, "Size of colors exceed memory limit.");
        return;
    }
    for (uint32_t i = 0; i < colorsSize; i++) {
        ani_int color;
        ani_ref colorRef;
        if (ANI_OK != env->Array_Get(colorsObj, static_cast<ani_size>(i), &colorRef) ||
            ANI_OK != env->Object_CallMethod_Int(
                static_cast<ani_object>(colorRef), AniGlobalMethod::GetInstance().intGet, &color)) {
            delete []colors;
            AniThrowError(env, "Incorrect DrawPixelMapMesh parameter color type.");
            return;
        }
        colors[i] = static_cast<uint32_t>(color);
    }
    uint32_t* colorsMesh = colors + colorOffset;
    DrawingPixelMapMesh(args.pixelMap, args.column, args.row, verticesMesh, colorsMesh, canvas);
    aniCanvas->NotifyDirty();
    delete []colors;
}
#endif

void AniCanvas::DrawPixelMapMesh(ani_env* env, ani_object obj,
    ani_object pixelmapObj, ani_int aniMeshWidth, ani_int aniMeshHeight,
    ani_array verticesObj, ani_int aniVertOffset, ani_array colorsObj, ani_int aniColorOffset)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
#ifdef ROSEN_OHOS
    std::shared_ptr<PixelMap> pixelMap = PixelMapTaiheAni::GetNativePixelMap(env, pixelmapObj);
    if (!pixelMap) {
        ROSEN_LOGE("AniCanvas::DrawImageRect get pixelMap failed");
        AniThrowError(env, "Invalid params.");
        return;
    }

    int32_t column = aniMeshWidth;
    int32_t row = aniMeshHeight;
    int32_t vertOffset = aniVertOffset;
    if (column == 0 || row == 0) {
        ROSEN_LOGE("AniCanvas::DrawPixelMapMesh column or row is invalid");
        AniThrowError(env, "Invalid column or row params.");
        return;
    }

    ani_size aniLength;
    if (ANI_OK != env->Array_GetLength(verticesObj, &aniLength)) {
        AniThrowError(env, "Invalid params.");
        return;
    }

    uint32_t verticesSize = static_cast<uint32_t>(aniLength);
    int64_t tempVerticesSize = ((column + 1) * (row + 1) + vertOffset) * 2; // x and y two coordinates
    if (verticesSize != tempVerticesSize) {
        ROSEN_LOGE("AniCanvas::DrawPixelMapMesh vertices are invalid");
        AniThrowError(env, "Incorrect parameter3 type.");
        return;
    }
    auto vertices = new (std::nothrow) float[verticesSize];
    if (!vertices) {
        ROSEN_LOGE("AniCanvas::DrawPixelMapMesh create array with size of vertices failed");
        AniThrowError(env, "Size of vertices exceed memory limit.");
        return;
    }
    if (!GetVertices(env, verticesObj, vertices, verticesSize)) {
        AniThrowError(env, "Incorrect DrawPixelMapMesh parameter vertex type.");
        return;
    }
    DrawPixelMapMeshArgs args {pixelMap, vertices, verticesSize, vertOffset, column, row};
    GetColorsAndDraw(env, colorsObj, static_cast<int32_t>(aniColorOffset), args, aniCanvas);
#endif
}

bool AniCanvas::CheckDrawVerticesParams(ani_env* env, ani_int& vertexCount, ani_int& indexCount)
{
    if (vertexCount < VERTEX_COUNT_MIN_SIZE) {
        ROSEN_LOGE("AniCanvas::DrawVertices vertexCount is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid vertexCount params.");
        return false;
    }
    if (indexCount < VERTEX_COUNT_MIN_SIZE && indexCount != 0) {
        ROSEN_LOGE("AniCanvas::DrawVertices indexCount is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid indexCount params.");
        return false;
    }
    return true;
}

bool AniCanvas::GetPositions(ani_env* env, ani_int vertexCount,
    ani_array positionsObj, std::vector<Drawing::Point>& pointPositions)
{
    ani_size aniPositionsLength = 0;
    if (ANI_OK != env->Array_GetLength(positionsObj, &aniPositionsLength)) {
        ROSEN_LOGE("AniCanvas::DrawVertices positions is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid positions params.");
        return false;
    }
    if (static_cast<ani_int>(aniPositionsLength) != vertexCount) {
        ROSEN_LOGE("AniCanvas::DrawVertices positionsSize is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid positions params.");
        return false;
    }
    uint32_t pointSize = static_cast<uint32_t>(vertexCount);
    pointPositions.resize(pointSize);
    if (!ConvertFromAniPointsArray(env, positionsObj, pointPositions.data(), pointSize)) {
        ROSEN_LOGE("AniCanvas::DrawVertices GetPositionsPoints is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid positions params.");
        return false;
    }
    return true;
}

bool AniCanvas::GetTexs(ani_env* env, ani_int vertexCount,
    ani_object texsObj, std::vector<Drawing::Point>& pointTexs)
{
    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(texsObj, &isNull);
    if (isNull) {
        return true;
    }
    ani_size aniTexsLength = 0;
    ani_array arrayObj = reinterpret_cast<ani_array>(texsObj);
    env->Array_GetLength(arrayObj, &aniTexsLength);
    uint32_t texsSize = static_cast<uint32_t>(aniTexsLength);
    if (texsSize != 0) {
        if (static_cast<ani_int>(aniTexsLength) != vertexCount) {
            ROSEN_LOGE("AniCanvas::DrawVertices texsSize is invalid");
            ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid texsObj params.");
            return false;
        }
        pointTexs.resize(texsSize);
        if (!ConvertFromAniPointsArray(env, arrayObj, pointTexs.data(), texsSize)) {
            ROSEN_LOGE("AniCanvas::DrawVertices GetTexsPoints is invalid");
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid texsObj params.");
            return false;
        }
    }
    return true;
}

bool AniCanvas::GetColors(ani_env* env, ani_int vertexCount,
    ani_object colorsObj, std::unique_ptr<uint32_t[]>& colors)
{
    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(colorsObj, &isNull);
    if (isNull) {
        return true;
    }
    ani_size aniColorsLength = 0;
    ani_array arrayObj = reinterpret_cast<ani_array>(colorsObj);
    env->Array_GetLength(arrayObj, &aniColorsLength);
    int32_t colorsSize = static_cast<int32_t>(aniColorsLength);
    if (colorsSize != 0) {
        if (colorsSize != vertexCount) {
            ROSEN_LOGE("AniCanvas::DrawVertices colorsSize is invalid");
            ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid colors params.");
            return false;
        }
        colors = std::make_unique<uint32_t[]>(colorsSize);
        if (!GetColorsUint32(env, arrayObj, colors.get(), colorsSize)) {
            ROSEN_LOGE("AniCanvas::DrawVertices GetColors is invalid");
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid colors params.");
            return false;
        }
    }
    return true;
}

bool AniCanvas::GetIndices(ani_env* env, ani_int indexCount,
    ani_object indicesObj, std::unique_ptr<uint16_t[]>& indices)
{
    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsNull(indicesObj, &isNull);
    if (isNull) {
        return true;
    }
    ani_size aniIndicesLength = 0;
    ani_array arrayObj = reinterpret_cast<ani_array>(indicesObj);
    env->Array_GetLength(arrayObj, &aniIndicesLength);
    int32_t indicesSize = static_cast<int32_t>(aniIndicesLength);
    if (indicesSize != 0) {
        if (indicesSize != indexCount) {
            ROSEN_LOGE("AniCanvas::DrawVertices indicesSize is Invalid");
            ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid indices params.");
            return false;
        }
        indices = std::make_unique<uint16_t[]>(indicesSize);
        if (!GetVerticesUint16(env, arrayObj, indices.get(), indicesSize)) {
            ROSEN_LOGE("AniCanvas::DrawVertices GetIndices is invalid");
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid indices params.");
            return false;
        }
    }
    return true;
}

bool AniCanvas::GetVertexModeAndBlendMode(ani_env* env, ani_enum_item aniVertexMode,
    ani_enum_item aniBlendMode, VertexMode& vertexMode, BlendMode& blendMode)
{
    ani_int vertexModeTemp = -1;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniVertexMode, &vertexModeTemp) ||
        vertexModeTemp < 0 || vertexModeTemp > static_cast<int32_t>(VertexMode::LAST_VERTEXMODE)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid vertexMode params.");
        return false;
    }
    vertexMode = static_cast<VertexMode>(vertexModeTemp);
    ani_int blendModeTemp = -1;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniBlendMode, &blendModeTemp) ||
        blendModeTemp < 0 || blendModeTemp > static_cast<int32_t>(BlendMode::LUMINOSITY)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED, "Invalid blendMode params.");
        return false;
    }
    blendMode = static_cast<BlendMode>(blendModeTemp);
    return true;
}

void AniCanvas::DrawVertices(ani_env* env, ani_object obj, ani_enum_item aniVertexMode,
    ani_int aniVertexCount, ani_array positionsObj, ani_object texsObj, ani_object colorsObj,
    ani_int aniIndexCount, ani_object indicesObj, ani_enum_item aniBlendMode)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid Canvas params.");
        return;
    }
    int32_t vertexCount = aniVertexCount;
    int32_t indexCount = aniIndexCount;
    std::vector<Drawing::Point> pointPositions;
    std::vector<Drawing::Point> pointTexs;
    std::unique_ptr<uint32_t[]> colors;
    std::unique_ptr<uint16_t[]> indices;
    VertexMode vertexMode;
    BlendMode blendMode;
    if (CheckDrawVerticesParams(env, vertexCount, indexCount) == false ||
        GetPositions(env, vertexCount, positionsObj, pointPositions) == false ||
        GetTexs(env, vertexCount, texsObj, pointTexs) == false ||
        GetColors(env, vertexCount, colorsObj, colors) == false ||
        GetIndices(env, indexCount, indicesObj, indices) == false ||
        GetVertexModeAndBlendMode(env, aniVertexMode, aniBlendMode, vertexMode, blendMode) == false) {
        return;
    }
    std::unique_ptr<Vertices> vertices = std::make_unique<Vertices>();
    uint16_t* indicesPtr = indices.get();
    bool result = vertices->MakeCopy(vertexMode, vertexCount, pointPositions.data(),
        pointTexs.data(), colors.get(), indicesPtr ? indexCount : 0, indicesPtr);
    if (result) {
        aniCanvas->GetCanvas()->DrawVertices(*vertices, blendMode);
    }
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawImageRectWithSrc(ani_env* env, ani_object obj, ani_object pixelmapObj,
    ani_object srcRectObj, ani_object dstRectObj, ani_object samplingOptionsObj, ani_object constraintObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "DrawImageRectWithSrc canvas is nullptr.");
        return;
    }
#ifdef ROSEN_OHOS
    std::shared_ptr<PixelMap> pixelMap = PixelMapTaiheAni::GetNativePixelMap(env, pixelmapObj);
    if (!pixelMap) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "DrawImageRectWithSrc pixelMap is nullptr.");
        return;
    }
    Drawing::Rect srcRect;
    Drawing::Rect dstRect;
    if (!GetRectFromAniRectObj(env, srcRectObj, srcRect) || !GetRectFromAniRectObj(env, dstRectObj, dstRect)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "DrawImageRectWithSrc incorrect type rect.");
        return;
    }

    AniSamplingOptions* aniSamplingOptions = nullptr;
    if (!IsUndefined(env, samplingOptionsObj)) {
        aniSamplingOptions = GetNativeFromObj<AniSamplingOptions>(env, samplingOptionsObj,
            AniGlobalField::GetInstance().samplingOptionsNativeObj);
        if (aniSamplingOptions == nullptr || aniSamplingOptions->GetSamplingOptions() == nullptr) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "DrawImageRectWithSrc samplingOptions is nullptr.");
            return;
        }
    }

    int32_t srcRectConstraint = 0;
    if (!IsUndefined(env, constraintObj)) {
        ani_int constraint;
        if (ANI_OK != env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(constraintObj), &constraint)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "DrawImageRectWithSrc get constraint failed.");
            return;
        }
        if (CheckInt32OutOfRange(constraint, static_cast<int32_t>(SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT),
            static_cast<int32_t>(SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT))) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "DrawImageRectWithSrc constraint is out of range.");
            return;
        }
        srcRectConstraint = constraint;
    }
    aniCanvas->DrawImageRectWithSrcInner(pixelMap, srcRect, dstRect, aniSamplingOptions, srcRectConstraint);
#endif
}

void AniCanvas::DrawColorWithObject(ani_env* env, ani_object obj, ani_object colorObj, ani_object aniBlendMode)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawColorWithObject canvas is nullptr.");
        return;
    }

    Drawing::ColorQuad color;
    if (!GetColorQuadFromColorObj(env, colorObj, color)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawColorWithObject get color failed.");
        return;
    }

    Canvas* canvas = aniCanvas->GetCanvas();
    if (!IsUndefined(env, aniBlendMode)) {
        ani_int blendMode;
        if (ANI_OK != env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(aniBlendMode), &blendMode)) {
            ROSEN_LOGE("AniCanvas::DrawColor failed cause by blendMode");
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::DrawColorWithObject get blendMode failed.");
            return;
        }
        if (CheckInt32OutOfRange(blendMode, 0, static_cast<int32_t>(BlendMode::LUMINOSITY))) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::DrawColorWithObject blendMode is out of range.");
            return;
        }
        canvas->DrawColor(color, static_cast<BlendMode>(blendMode));
        aniCanvas->NotifyDirty();
        return;
    }

    canvas->DrawColor(color);
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawColorWithArgb(ani_env* env, ani_object obj, ani_int alpha, ani_int red,
    ani_int green, ani_int blue, ani_object aniBlendMode)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawColorWithArgb canvas is nullptr.");
        return;
    }

    if (CheckInt32OutOfRange(alpha, 0, RGBA_MAX)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "alpha is out of range, range is [0, 255]");
        return;
    }
    if (CheckInt32OutOfRange(red, 0, RGBA_MAX)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "red is out of range, range is [0, 255]");
        return;
    }
    if (CheckInt32OutOfRange(green, 0, RGBA_MAX)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "green is out of range, range is [0, 255]");
        return;
    }
    if (CheckInt32OutOfRange(blue, 0, RGBA_MAX)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "blue is out of range, range is [0, 255]");
        return;
    }
    auto color = Drawing::Color::ColorQuadSetARGB(alpha, red, green, blue);

    Canvas* canvas = aniCanvas->GetCanvas();
    if (!IsUndefined(env, aniBlendMode)) {
        ani_int blendMode;
        if (ANI_OK != env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(aniBlendMode), &blendMode)) {
            ROSEN_LOGE("AniCanvas::DrawColor failed cause by blendMode");
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::DrawColorWithArgb get blendMode failed.");
            return;
        }
        if (CheckInt32OutOfRange(blendMode, 0, static_cast<int32_t>(BlendMode::LUMINOSITY))) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::DrawColorWithArgb blendMode is out of range.");
            return;
        }
        canvas->DrawColor(color, static_cast<BlendMode>(blendMode));
        aniCanvas->NotifyDirty();
        return;
    }

    canvas->DrawColor(color);
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawColor(ani_env* env, ani_object obj, ani_int color, ani_object aniBlendMode)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawColor canvas is nullptr.");
        return;
    }

    Canvas* canvas = aniCanvas->GetCanvas();
    if (!IsUndefined(env, aniBlendMode)) {
        ani_int blendMode;
        if (ANI_OK != env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(aniBlendMode), &blendMode)) {
            ROSEN_LOGE("AniCanvas::DrawColor failed cause by blendMode");
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::DrawColor get blendMode failed.");
            return;
        }
        if (CheckInt32OutOfRange(blendMode, 0, static_cast<int32_t>(BlendMode::LUMINOSITY))) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::DrawColor blendMode is out of range.");
            return;
        }
        canvas->DrawColor(color, static_cast<BlendMode>(blendMode));
        aniCanvas->NotifyDirty();
        return;
    }

    canvas->DrawColor(color);
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawPath(ani_env* env, ani_object obj, ani_object pathObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawPath canvas is nullptr.");
        return;
    }
    auto aniPath = GetNativeFromObj<AniPath>(env, pathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawPath path is nullptr.");
        return;
    }

    aniCanvas->GetCanvas()->DrawPath(*aniPath->GetPath());
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawLine(ani_env* env, ani_object obj, ani_double x0,
    ani_double y0, ani_double x1, ani_double y1)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawLine canvas is nullptr.");
        return;
    }
    aniCanvas->GetCanvas()->DrawLine(Drawing::Point(x0, y0), Drawing::Point(x1, y1));
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawSingleCharacter(ani_env* env, ani_object obj, ani_string text,
    ani_object fontObj, ani_double x, ani_double y)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawSingleCharacter canvas is nullptr.");
        return;
    }
    auto aniFont = GetNativeFromObj<AniFont>(env, fontObj, AniGlobalField::GetInstance().fontNativeObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawSingleCharacter font is nullptr.");
        return;
    }

    ani_size len = 0;
    env->String_GetUTF8Size(text, &len);
    if (len == 0 || len > 4) { // 4 is the maximum length of a character encoded in UTF8.
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawSingleCharacter Parameter verification failed. text should be single character.");
        return;
    }

    char str[len + 1];
    ani_size realLen = 0;
    env->String_GetUTF8(text, str, len + 1, &realLen);
    str[realLen] = '\0';
    const char* currentStr = str;
    int32_t unicode = SkUTF::NextUTF8(&currentStr, currentStr + len);
    std::shared_ptr<Font> font = aniFont->GetFont();
    std::shared_ptr<Font> themeFont = MatchThemeFont(font, unicode);
    if (themeFont != nullptr) {
        font = themeFont;
    }
    size_t byteLen = currentStr - str;
    if (byteLen != len) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawSingleCharacter Parameter verification failed. text should be single character.");
        return;
    }
    aniCanvas->GetCanvas()->DrawSingleCharacter(unicode, *font, x, y);
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawTextBlob(ani_env* env, ani_object obj, ani_object textBlobObj, ani_double x, ani_double y)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawTextBlob canvas is nullptr.");
        return;
    }
    auto aniTextBlob = GetNativeFromObj<AniTextBlob>(env, textBlobObj, AniGlobalField::GetInstance().textBlobNativeObj);
    if (aniTextBlob == nullptr || aniTextBlob->GetTextBlob() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawTextBlob textBlob is nullptr.");
        return;
    }
    aniCanvas->GetCanvas()->DrawTextBlob(aniTextBlob->GetTextBlob().get(), x, y);
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawOval(ani_env* env, ani_object obj, ani_object rectObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawOval canvas is nullptr.");
        return;
    }

    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, rectObj, drawingRect)) {
        ROSEN_LOGE("AniCanvas::DrawOval get drawingRect failed");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawOval get rect failed.");
        return;
    }
    aniCanvas->GetCanvas()->DrawOval(drawingRect);
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawArc(ani_env* env, ani_object obj, ani_object rectObj,
    ani_double startAngle, ani_double sweepAngle)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawArc canvas is nullptr.");
        return;
    }

    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, rectObj, drawingRect)) {
        ROSEN_LOGE("AniCanvas::DrawArc get drawingRect failed");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawArc get rect failed.");
        return;
    }
    aniCanvas->GetCanvas()->DrawArc(drawingRect, startAngle, sweepAngle);
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawArcWithCenter(ani_env* env, ani_object obj, ani_object rectObj,
    ani_double startAngle, ani_double sweepAngle, ani_boolean useCenter)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawArcWithCenter canvas is nullptr.");
        return;
    }

    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, rectObj, drawingRect)) {
        ROSEN_LOGE("AniCanvas::DrawArcWithCenter get drawingRect failed");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::DrawArcWithCenter get rect failed.");
        return;
    }
    bool isUseCenter = static_cast<bool>(useCenter);
    Canvas* canvas = aniCanvas->GetCanvas();

    if (isUseCenter) {
        canvas->DrawPie(drawingRect, startAngle, sweepAngle);
    } else {
        canvas->DrawArc(drawingRect, startAngle, sweepAngle);
    }
    aniCanvas->NotifyDirty();
}

void AniCanvas::DrawPoint(ani_env* env, ani_object obj, ani_double x, ani_double y)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawPoint canvas is nullptr.");
        return;
    }
    aniCanvas->GetCanvas()->DrawPoint(Drawing::Point(x, y));
    aniCanvas->NotifyDirty();
}

bool GetPoints(ani_env* env, ani_array pointsObj, uint32_t size, Drawing::Point* point)
{
    ani_boolean isPointClass = false;
    for (uint32_t i = 0; i < size; i++) {
        ani_ref pointRef;
        if (ANI_OK != env->Array_Get(pointsObj, static_cast<ani_size>(i), &pointRef)) {
            ROSEN_LOGE("GetPoints get points failed");
            return false;
        }
        if (i == 0) {
            ani_class pointClass = AniGlobalClass::GetInstance().pointInterface;
            if (pointClass == nullptr) {
                ROSEN_LOGE("[ANI] can't find class %{public}s", ANI_CLASS_POINT_NAME);
                return false;
            }
            env->Object_InstanceOf(static_cast<ani_object>(pointRef), pointClass, &isPointClass);
        }
        if (!isPointClass || GetPointFromPointObj(env, static_cast<ani_object>(pointRef), point[i]) != ANI_OK) {
            ROSEN_LOGE("GetPoints points are invalid");
            return false;
        }
    }
    return true;
}

void AniCanvas::DrawPoints(ani_env* env, ani_object obj, ani_array pointsObj, ani_object aniPointMode)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawPoints canvas is nullptr.");
        return;
    }
    ani_size aniLength;
    if (ANI_OK != env->Array_GetLength(pointsObj, &aniLength) || aniLength == 0) {
        ROSEN_LOGE("AniCanvas::DrawPoints points are invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawPoints incorrect type points.");
        return;
    }
    uint32_t size = static_cast<uint32_t>(aniLength);
    if (size > MAX_ELEMENTSIZE) {
        ROSEN_LOGE("AniCanvas::DrawPoints size exceeds the upper limit");
        return;
    }
    Drawing::Point* points = new(std::nothrow) Point[size];
    if (points == nullptr) {
        ROSEN_LOGE("AniCanvas::DrawPoints points size is %{public}u", size);
        return;
    }

    if (!GetPoints(env, pointsObj, size, points)) {
        delete [] points;
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawPoints get points failed.");
        return;
    }
    if (!IsUndefined(env, aniPointMode)) {
        ani_int pointMode;
        if (ANI_OK != env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(aniPointMode), &pointMode)) {
            delete [] points;
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect type pointMode.");
            return;
        }
        if (CheckInt32OutOfRange(pointMode, 0, static_cast<int32_t>(PointMode::POLYGON_POINTMODE))) {
            delete [] points;
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "pointMode out of range.");
            return;
        }
        aniCanvas->GetCanvas()->DrawPoints(static_cast<PointMode>(pointMode), size, points);
        aniCanvas->NotifyDirty();
        delete [] points;
        return;
    }
    aniCanvas->GetCanvas()->DrawPoints(PointMode::POINTS_POINTMODE, size, points);
    aniCanvas->NotifyDirty();
    delete [] points;
}

void AniCanvas::DrawRegion(ani_env* env, ani_object obj, ani_object regionObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawRegion canvas is nullptr.");
        return;
    }
    auto aniRegion = GetNativeFromObj<AniRegion>(env, regionObj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::DrawRegion region is nullptr.");
        return;
    }
    aniCanvas->GetCanvas()->DrawRegion(*aniRegion->GetRegion());
    aniCanvas->NotifyDirty();
}

void AniCanvas::AttachBrush(ani_env* env, ani_object obj, ani_object brushObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }

    auto aniBrush = GetNativeFromObj<AniBrush>(env, brushObj, AniGlobalField::GetInstance().brushNativeObj);
    if (aniBrush == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->AttachBrush(*aniBrush->GetBrush());
}

void AniCanvas::AttachPen(ani_env* env, ani_object obj, ani_object penObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }

    auto aniPen = GetNativeFromObj<AniPen>(env, penObj, AniGlobalField::GetInstance().penNativeObj);
    if (aniPen == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    if (aniPen->GetPen() == nullptr) {
        ROSEN_LOGE("AniCanvas::AttachPen pen is nullptr");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->AttachPen(*aniPen->GetPen());
}

void AniCanvas::DetachBrush(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->DetachBrush();
}

void AniCanvas::DetachPen(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->DetachPen();
}

ani_int AniCanvas::Save(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return -1;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    return canvas->Save();
}


ani_long AniCanvas::SaveLayer(ani_env* env, ani_object obj, ani_object rectObj, ani_object brushObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return -1;
    }
    Canvas* canvas = aniCanvas->GetCanvas();

    if (IsUndefined(env, rectObj)) {
        ani_long ret = canvas->GetSaveCount();
        canvas->SaveLayer(SaveLayerOps());
        return ret;
    }

    Drawing::Rect drawingRect;
    Drawing::Rect* drawingRectPtr = GetRectFromAniRectObj(env, rectObj, drawingRect) ?  &drawingRect : nullptr;

    if (IsUndefined(env, brushObj)) {
        ani_long ret = canvas->GetSaveCount();
        canvas->SaveLayer(SaveLayerOps(drawingRectPtr, nullptr));
        return ret;
    }

    Drawing::AniBrush* aniBrush = GetNativeFromObj<AniBrush>(env, brushObj,
        AniGlobalField::GetInstance().brushNativeObj);
    Drawing::Brush* drawingBrushPtr = aniBrush && aniBrush->GetBrush() ? aniBrush->GetBrush().get() : nullptr;

    ani_long ret = canvas->GetSaveCount();
    SaveLayerOps saveLayerOps = SaveLayerOps(drawingRectPtr, drawingBrushPtr);
    canvas->SaveLayer(saveLayerOps);
    return ret;
}

void AniCanvas::Clear(ani_env* env, ani_object obj, ani_object objColor)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::Clear canvas is nullptr.");
        return;
    }

    Drawing::ColorQuad color;
    if (!GetColorQuadFromColorObj(env, objColor, color)) {
        ROSEN_LOGE("AniCanvas::Clear failed cause by colorObj");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::Clear incorrect type color.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->Clear(color);
    aniCanvas->NotifyDirty();
}

void AniCanvas::ClearWithOption(ani_env* env, ani_object obj, ani_object objColor)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::ClearWithOption canvas is nullptr.");
        return;
    }

    Drawing::ColorQuad color;
    if (!GetColorQuadFromParam(env, objColor, color)) {
        ROSEN_LOGE("AniCanvas::ClearWithOption failed cause by colorObj");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::ClearWithOption incorrect type color.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->Clear(color);
    aniCanvas->NotifyDirty();
}

void AniCanvas::Restore(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->Restore();
}

ani_int AniCanvas::GetSaveCount(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return -1;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    return canvas->GetSaveCount();
}

void AniCanvas::RestoreToCount(ani_env* env, ani_object obj, ani_int count)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr || count < 0) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::RestoreToCount canvas is nullptr.");
        return;
    }

    aniCanvas->GetCanvas()->RestoreToCount(count);
}

ani_int AniCanvas::GetWidth(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::GetWidth canvas is nullptr.");
        return -1;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    return canvas->GetWidth();
}

ani_int AniCanvas::GetHeight(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::GetHeight canvas is nullptr.");
        return -1;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    return canvas->GetHeight();
}

ani_object AniCanvas::GetLocalClipBounds(ani_env* env, ani_object obj)
{
    ani_object aniObj = CreateAniUndefined(env);
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::GetLocalClipBounds canvas is nullptr.");
        return aniObj;
    }
    
    Drawing::Rect rect = aniCanvas->GetCanvas()->GetLocalClipBounds();
    CreateRectObj(env, rect, aniObj);
    return aniObj;
}

ani_object AniCanvas::GetTotalMatrix(ani_env* env, ani_object obj)
{
    ani_object aniObj = CreateAniUndefined(env);
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::GetTotalMatrix canvas is nullptr.");
        return aniObj;
    }

    Drawing::Matrix matrix = aniCanvas->GetCanvas()->GetTotalMatrix();
    std::shared_ptr<Drawing::Matrix> matrixPtr = std::make_shared<Drawing::Matrix>(matrix);
    AniMatrix* aniMatrix = new AniMatrix(matrixPtr);
    aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().matrix,
        AniGlobalMethod::GetInstance().matrixCtor);
    if (ANI_OK != env->Object_SetField_Long(aniObj,
        AniGlobalField::GetInstance().matrixNativeObj, reinterpret_cast<ani_long>(aniMatrix))) {
        ROSEN_LOGE("AniCanvas::GetTotalMatrix failed cause by Object_SetField_Long");
        delete aniMatrix;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

void AniCanvas::Scale(ani_env* env, ani_object obj, ani_double sx, ani_double sy)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::Scale canvas is nullptr.");
        return;
    }
    aniCanvas->GetCanvas()->Scale(sx, sy);
}

void AniCanvas::Skew(ani_env* env, ani_object obj, ani_double sx, ani_double sy)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::Skew canvas is nullptr.");
        return;
    }
    aniCanvas->GetCanvas()->Shear(sx, sy);
}

void AniCanvas::Rotate(ani_env* env, ani_object obj, ani_double degrees, ani_double sx, ani_double sy)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::Rotate canvas is nullptr.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->Rotate(degrees, sx, sy);
}

void AniCanvas::ConcatMatrix(ani_env* env, ani_object obj, ani_object matrixObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ConcatMatrix canvas is nullptr.");
        return;
    }
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, matrixObj, AniGlobalField::GetInstance().matrixNativeObj);
    if (aniMatrix == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ConcatMatrix matrix is nullptr.");
        return;
    }
    if (aniMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("AniCanvas::ConcatMatrix matrix is nullptr");
        return;
    }

    aniCanvas->GetCanvas()->ConcatMatrix(*aniMatrix->GetMatrix());
}

void AniCanvas::Translate(ani_env* env, ani_object obj, ani_double dx, ani_double dy)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::Translate canvas is nullptr.");
        return;
    }
    aniCanvas->GetCanvas()->Translate(dx, dy);
}

void AniCanvas::ClipPath(ani_env* env, ani_object obj, ani_object pathObj, ani_object clipOpObj, ani_object aaObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ClipPath canvas is nullptr.");
        return;
    }
    auto aniPath = GetNativeFromObj<AniPath>(env, pathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ClipPath path is nullptr.");
        return;
    }

    Drawing::Canvas* canvas = aniCanvas->GetCanvas();
    bool doAntiAlias = false;
    if (!IsUndefined(env, aaObj)) {
        ani_boolean aniDoAntiAlias;
        ani_status result = env->Object_CallMethod_Boolean(
            aaObj, AniGlobalMethod::GetInstance().booleanGet, &aniDoAntiAlias);
        if (result != ANI_OK) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipPath incorrect type antiAlias.");
            return;
        }
        doAntiAlias = static_cast<bool>(aniDoAntiAlias);
    }

    if (!IsUndefined(env, clipOpObj)) {
        ani_int clipOp;
        if (ANI_OK != env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(clipOpObj), &clipOp)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipPath incorrect type clipOp.");
            return;
        }
        if (CheckInt32OutOfRange(clipOp, 0, static_cast<int32_t>(ClipOp::INTERSECT))) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipPath clipOp is out of range.");
            return;
        }
        canvas->ClipPath(*aniPath->GetPath(), static_cast<ClipOp>(clipOp), doAntiAlias);
        return;
    }
    canvas->ClipPath(*aniPath->GetPath(), ClipOp::INTERSECT, doAntiAlias);
}

void AniCanvas::ClipRect(ani_env* env, ani_object obj, ani_object rectObj, ani_object clipOpObj, ani_object aaObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ClipRect canvas is nullptr.");
        return;
    }

    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, rectObj, drawingRect)) {
        ROSEN_LOGE("AniCanvas::ClipRect get drawingRect failed");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ClipRect incorrect type rect.");
        return;
    }

    Drawing::Canvas* canvas = aniCanvas->GetCanvas();
    bool doAntiAlias = false;
    if (!IsUndefined(env, aaObj)) {
        ani_boolean aniDoAntiAlias;
        ani_status result = env->Object_CallMethod_Boolean(
            aaObj, AniGlobalMethod::GetInstance().booleanGet, &aniDoAntiAlias);
        if (result != ANI_OK) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipRect incorrect type antiAlias.");
            return;
        }
        doAntiAlias = static_cast<bool>(aniDoAntiAlias);
    }

    if (!IsUndefined(env, clipOpObj)) {
        ani_int clipOp;
        if (ANI_OK != env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(clipOpObj), &clipOp)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipRect incorrect type clipOp.");
            return;
        }
        if (CheckInt32OutOfRange(clipOp, 0, static_cast<int32_t>(ClipOp::INTERSECT))) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipRect clipOp is out of range.");
            return;
        }
        canvas->ClipRect(drawingRect, static_cast<ClipOp>(clipOp), doAntiAlias);
        return;
    }
    canvas->ClipRect(drawingRect, ClipOp::INTERSECT, doAntiAlias);
}

void AniCanvas::ClipRegion(ani_env* env, ani_object obj, ani_object regionObj, ani_object clipOpObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ClipRegion canvas is nullptr.");
        return;
    }
    auto aniRegion = GetNativeFromObj<AniRegion>(env, regionObj, AniGlobalField::GetInstance().regionNativeObj);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ClipRegion canvas is nullptr.");
        return;
    }

    Drawing::Canvas* canvas = aniCanvas->GetCanvas();
    if (!IsUndefined(env, clipOpObj)) {
        ani_int clipOp;
        if (ANI_OK != env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(clipOpObj), &clipOp)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipRegion incorrect type clipOp.");
            return;
        }
        if (CheckInt32OutOfRange(clipOp, 0, static_cast<int32_t>(ClipOp::INTERSECT))) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipRegion clipOp is out of range.");
            return;
        }
        canvas->ClipRegion(*aniRegion->GetRegion(), static_cast<ClipOp>(clipOp));
        return;
    }
    canvas->ClipRegion(*aniRegion->GetRegion());
}

void AniCanvas::ClipRoundRect(ani_env* env, ani_object obj, ani_object roundRectObj,
    ani_object clipOpObj, ani_object aaObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ClipRoundRect canvas is nullptr.");
        return;
    }
    auto aniRoundRect = GetNativeFromObj<AniRoundRect>(env, roundRectObj,
        AniGlobalField::GetInstance().roundRectNativeObj);
    if (aniRoundRect == nullptr || aniRoundRect->GetRoundRect() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::ClipRoundRect roundRect is nullptr.");
        return;
    }

    Drawing::Canvas* canvas = aniCanvas->GetCanvas();
    bool doAntiAlias = false;
    if (!IsUndefined(env, aaObj)) {
        ani_boolean aniDoAntiAlias;
        ani_status result = env->Object_CallMethod_Boolean(
            aaObj, AniGlobalMethod::GetInstance().booleanGet, &aniDoAntiAlias);
        if (result != ANI_OK) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipRoundRect incorrect type antiAlias.");
            return;
        }
        doAntiAlias = static_cast<bool>(aniDoAntiAlias);
    }

    if (!IsUndefined(env, clipOpObj)) {
        ani_int clipOp;
        if (ANI_OK != env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(clipOpObj), &clipOp)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipRoundRect incorrect type clipOp.");
            return;
        }
        if (CheckInt32OutOfRange(clipOp, 0, static_cast<int32_t>(ClipOp::INTERSECT))) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "AniCanvas::ClipRoundRect clipOp is out of range.");
            return;
        }
        canvas->ClipRoundRect(*aniRoundRect->GetRoundRect(), static_cast<ClipOp>(clipOp), doAntiAlias);
        return;
    }
    canvas->ClipRoundRect(*aniRoundRect->GetRoundRect(), ClipOp::INTERSECT, doAntiAlias);
}

ani_boolean AniCanvas::IsClipEmpty(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::IsClipEmpty canvas is nullptr.");
        return ANI_FALSE;
    }

    Drawing::Canvas* canvas = aniCanvas->GetCanvas();
    bool result = canvas->IsClipEmpty();
    return static_cast<ani_boolean>(result);
}

void AniCanvas::SetMatrix(ani_env* env, ani_object obj, ani_object matrixObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::SetMatrix canvas is nullptr.");
        return;
    }
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, matrixObj, AniGlobalField::GetInstance().matrixNativeObj);
    if (aniMatrix == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::SetMatrix matrix is nullptr.");
        return;
    }
    if (aniMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("AniCanvas::SetMatrix matrix is nullptr");
        return;
    }

    aniCanvas->GetCanvas()->SetMatrix(*aniMatrix->GetMatrix());
}

void AniCanvas::ResetMatrix(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniCanvas::ResetMatrix canvas is nullptr.");
        return;
    }

    aniCanvas->GetCanvas()->ResetMatrix();
}

ani_boolean AniCanvas::QuickRejectPath(ani_env* env, ani_object obj, ani_object pathObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::QuickRejectPath canvas is nullptr.");
        return ANI_FALSE;
    }

    auto aniPath = GetNativeFromObj<AniPath>(env, pathObj, AniGlobalField::GetInstance().pathNativeObj);
    if (aniPath == nullptr || aniPath->GetPath() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::QuickRejectPath path is nullptr.");
        return ANI_FALSE;
    }
    Drawing::Canvas* canvas = aniCanvas->GetCanvas();
    bool result = canvas->QuickReject(*aniPath->GetPath());
    return static_cast<ani_boolean>(result);
}

ani_boolean AniCanvas::QuickRejectRect(ani_env* env, ani_object obj, ani_object rectObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::QuickRejectRect canvas is nullptr.");
        return ANI_FALSE;
    }

    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, rectObj, drawingRect)) {
        ROSEN_LOGE("AniCanvas::QuickRejectRect get drawingRect failed");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniCanvas::QuickRejectRect incorrect type rect.");
        return ANI_FALSE;
    }
    Drawing::Canvas* canvas = aniCanvas->GetCanvas();
    bool result = canvas->QuickReject(drawingRect);
    return static_cast<ani_boolean>(result);
}

Canvas* AniCanvas::GetCanvas()
{
    return m_canvas;
}

ani_object AniCanvas::CreateAniCanvas(ani_env* env, Canvas* canvas)
{
    if (canvas == nullptr) {
        ROSEN_LOGE("CreateAniCanvas failed, canvas is nullptr!");
        return CreateAniUndefined(env);
    }

    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    ani_class aniClass;
    if (env->FindClass(ANI_CLASS_CANVAS_NAME, &aniClass) != ANI_OK) {
        ROSEN_LOGE("CreateAniCanvas FindClass failed");
        return CreateAniUndefined(env);
    }
    ani_method aniConstructor;
    if (env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniConstructor) != ANI_OK) {
        ROSEN_LOGE("CreateAniCanvas Class_FindMethod failed");
        return CreateAniUndefined(env);
    }
    ani_object aniObj;
    if (env->Object_New(aniClass, aniConstructor, &aniObj, aniRef) != ANI_OK) {
        ROSEN_LOGE("CreateAniCanvas Object_New failed");
        return CreateAniUndefined(env);
    }
    auto aniCanvas = new AniCanvas(canvas);
    if (ANI_OK != env->Object_SetFieldByName_Long(aniObj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniCanvas))) {
        ROSEN_LOGE("aniCanvas failed cause by Object_SetFieldByName_Long");
        delete aniCanvas;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

ani_object AniCanvas::CanvasTransferStatic(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniCanvas::CanvasTransferStatic failed to unwrap");
        return CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniCanvas::CanvasTransferStatic unwrapResult is null");
        return CreateAniUndefined(env);
    }
    auto jsCanvas = reinterpret_cast<JsCanvas*>(unwrapResult);
    if (jsCanvas->GetCanvasPtr() == nullptr) {
        ROSEN_LOGE("AniCanvas::CanvasTransferStatic jsCanvas is null");
        return CreateAniUndefined(env);
    }

#ifdef ROSEN_OHOS
    if (jsCanvas->GetPixelMap() == nullptr) {
        ROSEN_LOGE("AniCanvas::CanvasTransferStatic pixelMap is null");
        return CreateAniUndefined(env);
    }
#endif
    auto aniCanvas = new AniCanvas(jsCanvas->GetCanvasPtr(), jsCanvas->GetOwned());
#ifdef ROSEN_OHOS
    aniCanvas->mPixelMap_ = jsCanvas->GetPixelMap();
#endif
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    ani_object aniCanvasObj = CreateAniObject(env, AniGlobalClass::GetInstance().canvas,
        AniGlobalMethod::GetInstance().canvasCtor, aniRef);
    if (ANI_OK != env->Object_SetField_Long(
        aniCanvasObj, AniGlobalField::GetInstance().canvasNativeObj, reinterpret_cast<ani_long>(aniCanvas))) {
        ROSEN_LOGE("AniCanvas::CanvasTransferStatic failed create aniBrush");
        delete aniCanvas;
        return CreateAniUndefined(env);
    }
    return aniCanvasObj;
}

ani_long AniCanvas::GetCanvasAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, input, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ROSEN_LOGE("AniCanvas::GetCanvasAddr aniCanvas is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniCanvas->GetCanvas());
}

ani_long AniCanvas::GetPixelMapAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
#ifdef ROSEN_OHOS
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, input, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ROSEN_LOGE("AniCanvas::GetPixelMapAddr aniCanvas is null");
        return 0;
    }
    if (aniCanvas->mPixelMap_ == nullptr) {
        ROSEN_LOGE("AniCanvas::GetPixelMapAddr pixelMap is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniCanvas->GetPixelMapPtrAddr());
#endif
    return 0;
}

ani_boolean AniCanvas::GetCanvasOwned(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, input, AniGlobalField::GetInstance().canvasNativeObj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        ROSEN_LOGE("AniCanvas::GetPixelMapAddr aniCanvas is null");
        return false;
    }
    return aniCanvas->owned_;
}

#ifdef ROSEN_OHOS
std::shared_ptr<Media::PixelMap>* AniCanvas::GetPixelMapPtrAddr()
{
    return &mPixelMap_;
}
#endif

AniCanvas::~AniCanvas()
{
    if (owned_) {
        delete m_canvas;
    }
    m_canvas = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen
