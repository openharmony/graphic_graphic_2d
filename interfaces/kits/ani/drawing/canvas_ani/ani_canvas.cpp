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
#include "brush_ani/ani_brush.h"
#include "effect/color_space.h"
#include "pixel_map_taihe_ani.h"
#include "image/image.h"
#include "pen_ani/ani_pen.h"
#include "sampling_options_ani/ani_sampling_options.h"

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
const char* ANI_CLASS_CANVAS_NAME = "L@ohos/graphics/drawing/drawing/Canvas;";
ani_status AniCanvas::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_CANVAS_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_CANVAS_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "drawRect", "L@ohos/graphics/common2D/common2D/Rect;:V",
            reinterpret_cast<void*>(DrawRectWithRect) },
        ani_native_function { "drawRect", "DDDD:V", reinterpret_cast<void*>(DrawRect) },
        ani_native_function { "drawImageRect", nullptr, reinterpret_cast<void*>(DrawImageRect) },
        ani_native_function { "drawPixelMapMesh", nullptr, reinterpret_cast<void*>(DrawPixelMapMesh) },
        ani_native_function { "attachPen", nullptr, reinterpret_cast<void*>(AttachPen) },
        ani_native_function { "attachBrush", nullptr, reinterpret_cast<void*>(AttachBrush) },
        ani_native_function { "detachPen", nullptr, reinterpret_cast<void*>(DetachPen) },
        ani_native_function { "detachBrush", nullptr, reinterpret_cast<void*>(DetachBrush) },
        ani_native_function { "save", nullptr, reinterpret_cast<void*>(Save) },
        ani_native_function { "saveLayer", nullptr, reinterpret_cast<void*>(SaveLayer) },
        ani_native_function { "restore", nullptr, reinterpret_cast<void*>(Restore) },
        ani_native_function { "getSaveCount", nullptr, reinterpret_cast<void*>(GetSaveCount) },
        ani_native_function { "rotate", nullptr, reinterpret_cast<void*>(Rotate) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
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
    if (pixelmapObj == nullptr) {
        AniCanvas *aniCanvas = new AniCanvas();
        if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniCanvas))) {
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

    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniCanvas))) {
        ROSEN_LOGE("AniCanvas::Constructor failed create aniCanvas");
        delete aniCanvas;
        return;
    }
#endif
}

void AniCanvas::DrawRect(ani_env* env, ani_object obj,
    ani_double left, ani_double top, ani_double right, ani_double bottom)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
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
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
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

void AniCanvas::DrawImageRect(ani_env* env, ani_object obj,
    ani_object pixelmapObj, ani_object rectObj, ani_object samplingOptionsObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
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
    ani_boolean isUndefined;
    env->Reference_IsUndefined(samplingOptionsObj, &isUndefined);
    if (!isUndefined) {
        aniSamplingOptions = GetNativeFromObj<AniSamplingOptions>(env, samplingOptionsObj);
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
#endif

void AniCanvas::DrawPixelMapMesh(ani_env* env, ani_object obj,
    ani_object pixelmapObj, ani_double aniMeshWidth, ani_double aniMeshHeight,
    ani_object verticesObj, ani_double aniVertOffset, ani_object colorsObj, ani_double aniColorOffset)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
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
    int32_t colorOffset = aniColorOffset;
    if (column == 0 || row == 0) {
        ROSEN_LOGE("AniCanvas::DrawPixelMapMesh column or row is invalid");
        AniThrowError(env, "Invalid column or row params.");
        return;
    }

    ani_double aniLength;
    if (ANI_OK != env->Object_GetPropertyByName_Double(verticesObj, "length", &aniLength)) {
        AniThrowError(env, "Invalid params.");
        return;
    }

    uint32_t verticesSize = aniLength;
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
    for (uint32_t i = 0; i < verticesSize; i++) {
        ani_double vertex;
        ani_ref vertexRef;
        if (ANI_OK !=  env->Object_CallMethodByName_Ref(
            verticesObj, "$_get", "I:Lstd/core/Object;", &vertexRef, (ani_int)i) ||
            ANI_OK !=  env->Object_CallMethodByName_Double(
                static_cast<ani_object>(vertexRef), "unboxed", ":D", &vertex)) {
            delete []vertices;
            AniThrowError(env, "Incorrect DrawPixelMapMesh parameter vertex type.");
            return;
        }
        vertices[i] = vertex;
    }

    float* verticesMesh = verticesSize ? (vertices + vertOffset * 2) : nullptr; // offset two coordinates

    if (ANI_OK != env->Object_GetPropertyByName_Double(colorsObj, "length", &aniLength)) {
        delete []vertices;
        AniThrowError(env, "Invalid params.");
        return;
    }
    uint32_t colorsSize = aniLength;
    int64_t tempColorsSize = (column + 1) * (row + 1) + colorOffset;
    if (colorsSize != 0 && colorsSize != tempColorsSize) {
        ROSEN_LOGE("AniCanvas::DrawPixelMapMesh colors are invalid");
        delete []vertices;
        AniThrowError(env, "Incorrect parameter5 type");
        return;
    }

    if (colorsSize == 0) {
        DrawingPixelMapMesh(pixelMap, column, row, verticesMesh, nullptr, aniCanvas->GetCanvas());
        aniCanvas->NotifyDirty();
        delete []vertices;
        return;
    }

    auto colors = new (std::nothrow) uint32_t[colorsSize];
    if (!colors) {
        ROSEN_LOGE("AniCanvas::DrawPixelMapMesh create array with size of colors failed");
        delete []vertices;
        AniThrowError(env, "Size of colors exceed memory limit.");
        return;
    }
    for (uint32_t i = 0; i < colorsSize; i++) {
        ani_double color;
        ani_ref colorRef;
        if (ANI_OK !=  env->Object_CallMethodByName_Ref(
            colorsObj, "$_get", "I:Lstd/core/Object;", &colorRef, (ani_int)i) ||
            ANI_OK !=  env->Object_CallMethodByName_Double(
                static_cast<ani_object>(colorRef), "unboxed", ":D", &color)) {
            delete []vertices;
            delete []colors;
            AniThrowError(env, "Incorrect DrawPixelMapMesh parameter color type.");
            return;
        }
        colors[i] = color;
    }
    uint32_t* colorsMesh = colors + colorOffset;
    DrawingPixelMapMesh(pixelMap, column, row, verticesMesh, colorsMesh, aniCanvas->GetCanvas());
    aniCanvas->NotifyDirty();
    delete []vertices;
    delete []colors;
#endif
}

void AniCanvas::AttachBrush(ani_env* env, ani_object obj, ani_object brushObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }

    auto aniBrush = GetNativeFromObj<AniBrush>(env, brushObj);
    if (aniBrush == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->AttachBrush(aniBrush->GetBrush());
}

void AniCanvas::AttachPen(ani_env* env, ani_object obj, ani_object penObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }

    auto aniPen = GetNativeFromObj<AniPen>(env, penObj);
    if (aniPen == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->AttachPen(aniPen->GetPen());
}

void AniCanvas::DetachBrush(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->DetachBrush();
}

void AniCanvas::DetachPen(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->DetachPen();
}

ani_double AniCanvas::Save(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return -1;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    return canvas->Save();
}


ani_double AniCanvas::SaveLayer(ani_env* env, ani_object obj, ani_object rectObj, ani_object brushObj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return -1;
    }
    Canvas* canvas = aniCanvas->GetCanvas();

    ani_boolean rectIsUndefined = ANI_TRUE;
    env->Reference_IsUndefined(rectObj, &rectIsUndefined);
    if (rectIsUndefined) {
        ani_double ret = canvas->GetSaveCount();
        canvas->SaveLayer(SaveLayerOps());
        return ret;
    }

    ani_boolean brushIsUndefined = ANI_TRUE;
    env->Reference_IsUndefined(brushObj, &brushIsUndefined);

    Drawing::Rect drawingRect;
    Drawing::Rect* drawingRectPtr = GetRectFromAniRectObj(env, rectObj, drawingRect) ?  &drawingRect : nullptr;

    if (brushIsUndefined) {
        ani_double ret = canvas->GetSaveCount();
        canvas->SaveLayer(SaveLayerOps(drawingRectPtr, nullptr));
        return ret;
    }

    Drawing::AniBrush* aniBrush = GetNativeFromObj<AniBrush>(env, brushObj);
    Drawing::Brush* drawingBrushPtr = aniBrush ? &aniBrush->GetBrush() : nullptr;

    ani_double ret = canvas->GetSaveCount();
    SaveLayerOps saveLayerOps = SaveLayerOps(drawingRectPtr, drawingBrushPtr);
    canvas->SaveLayer(saveLayerOps);
    return ret;
}

void AniCanvas::Restore(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->Restore();
}

ani_double AniCanvas::GetSaveCount(ani_env* env, ani_object obj)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return -1;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    return canvas->GetSaveCount();
}

void AniCanvas::Rotate(ani_env* env, ani_object obj, ani_double degrees, ani_double sx, ani_double sy)
{
    auto aniCanvas = GetNativeFromObj<AniCanvas>(env, obj);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        AniThrowError(env, "Invalid params.");
        return;
    }
    Canvas* canvas = aniCanvas->GetCanvas();
    canvas->Rotate(degrees, sx, sy);
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

    auto aniCanvas = new AniCanvas(canvas);
    ani_object aniObj = CreateAniObject(env, ANI_CLASS_CANVAS_NAME, nullptr, nullptr);
    if (ANI_OK != env->Object_SetFieldByName_Long(aniObj,
        NATIVE_OBJ, reinterpret_cast<ani_long>(aniCanvas))) {
        ROSEN_LOGE("aniCanvas failed cause by Object_SetFieldByName_Long");
        delete aniCanvas;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

AniCanvas::~AniCanvas()
{
    if (owned_) {
        delete m_canvas;
    }
    m_canvas = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen
