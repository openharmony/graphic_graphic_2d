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

#include "js_canvas.h"

#include <mutex>

#ifdef ROSEN_OHOS
#include "pixel_map.h"
#include "pixel_map_napi.h"
#endif
#include "native_value.h"
#include "draw/canvas.h"
#include "draw/path.h"
#include "image/image.h"
#include "render/rs_pixel_map_shader.h"
#include "text/text.h"
#include "text/text_blob.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "utils/sampling_options.h"
#include "utils/vertices.h"

#include "brush_napi/js_brush.h"
#include "pen_napi/js_pen.h"
#include "path_napi/js_path.h"
#include "sampling_options_napi/js_sampling_options.h"
#include "text_blob_napi/js_text_blob.h"
#include "js_drawing_utils.h"

namespace OHOS::Rosen {
#ifdef ROSEN_OHOS
using namespace Media;
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
    bitmap.SetInfo(drawingImageInfo);
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
    if (m_canvas->GetDrawingType() != Drawing::DrawingType::RECORDING) {
        std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMap);
        if (image == nullptr) {
            ROSEN_LOGE("Drawing_napi::DrawingPixelMapMesh image is nullptr");
            return;
        }
        auto shader = Drawing::ShaderEffect::CreateImageShader(*image,
            Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
        m_canvas->GetMutableBrush().SetShaderEffect(shader);
    } else {
        auto shader = Drawing::ShaderEffect::CreateExtendShader(std::make_shared<RSPixelMapShader>(pixelMap,
            Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix()));
        m_canvas->GetMutableBrush().SetShaderEffect(shader);
    }

    JS_CALL_DRAWING_FUNC(
        m_canvas->DrawVertices(*builder.Detach(), Drawing::BlendMode::MODULATE));
}
}
#endif

namespace Drawing {
thread_local napi_ref JsCanvas::constructor_ = nullptr;
static std::mutex g_constructorInitMutex;

Canvas* g_drawingCanvas = nullptr;
const std::string CLASS_NAME = "Canvas";
napi_value JsCanvas::Constructor(napi_env env, napi_callback_info info)
{
    napi_value jsThis = nullptr;
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: failed to napi_get_cb_info");
        return nullptr;
    }
    if (argc == 0) {
        if (g_drawingCanvas == nullptr) {
            ROSEN_LOGE("Drawing_napi: m_canvas is nullptr");
            return nullptr;
        }
        JsCanvas *jsCanvas = new(std::nothrow) JsCanvas(g_drawingCanvas);
        status = napi_wrap(env, jsThis, jsCanvas, JsCanvas::Destructor, nullptr, nullptr);
        if (status != napi_ok) {
            delete jsCanvas;
            ROSEN_LOGE("Drawing_napi: Failed to wrap native instance");
            return nullptr;
        }
    } else {
#ifdef ROSEN_OHOS
        PixelMapNapi* pixelMapNapi = nullptr;
        napi_unwrap(env, argv[0], reinterpret_cast<void**>(&pixelMapNapi));
        if (pixelMapNapi == nullptr) {
            return nullptr;
        }

        if (pixelMapNapi != nullptr && pixelMapNapi->GetPixelNapiInner() == nullptr) {
            return nullptr;
        }

        Bitmap bitmap;
        if (!ExtracetDrawingBitmap(pixelMapNapi->GetPixelNapiInner(), bitmap)) {
            return nullptr;
        }

        Canvas* canvas = new Canvas();
        canvas->Bind(bitmap);
        JsCanvas *jsCanvas = new(std::nothrow) JsCanvas(canvas, true);
        status = napi_wrap(env, jsThis, jsCanvas, JsCanvas::Destructor, nullptr, nullptr);
        if (status != napi_ok) {
            delete jsCanvas;
            ROSEN_LOGE("Drawing_napi: Failed to wrap native instance");
            return nullptr;
        }
#else
        return nullptr;
#endif
    }
    return jsThis;
}

bool JsCanvas::DeclareFuncAndCreateConstructor(napi_env env)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("drawRect", JsCanvas::DrawRect),
        DECLARE_NAPI_FUNCTION("drawCircle", JsCanvas::DrawCircle),
        DECLARE_NAPI_FUNCTION("drawImage", JsCanvas::DrawImage),
        DECLARE_NAPI_FUNCTION("drawColor", JsCanvas::DrawColor),
        DECLARE_NAPI_FUNCTION("drawPoint", JsCanvas::DrawPoint),
        DECLARE_NAPI_FUNCTION("drawPath", JsCanvas::DrawPath),
        DECLARE_NAPI_FUNCTION("drawLine", JsCanvas::DrawLine),
        DECLARE_NAPI_FUNCTION("drawTextBlob", JsCanvas::DrawText),
        DECLARE_NAPI_FUNCTION("drawPixelMapMesh", JsCanvas::DrawPixelMapMesh),
        DECLARE_NAPI_FUNCTION("attachPen", JsCanvas::AttachPen),
        DECLARE_NAPI_FUNCTION("attachBrush", JsCanvas::AttachBrush),
        DECLARE_NAPI_FUNCTION("detachPen", JsCanvas::DetachPen),
        DECLARE_NAPI_FUNCTION("detachBrush", JsCanvas::DetachBrush),
        DECLARE_NAPI_FUNCTION("skew", JsCanvas::Skew),
        DECLARE_NAPI_FUNCTION("rotate", JsCanvas::Rotate),
        DECLARE_NAPI_FUNCTION("getSaveCount", JsCanvas::GetSaveCount),
        DECLARE_NAPI_FUNCTION("save", JsCanvas::Save),
        DECLARE_NAPI_FUNCTION("restore", JsCanvas::Restore),
        DECLARE_NAPI_FUNCTION("restoreToCount", JsCanvas::RestoreToCount),
        DECLARE_NAPI_FUNCTION("scale", JsCanvas::Scale),
        DECLARE_NAPI_FUNCTION("clipPath", JsCanvas::ClipPath),
        DECLARE_NAPI_FUNCTION("clipRect", JsCanvas::ClipRect),
        DECLARE_NAPI_FUNCTION("translate", JsCanvas::Translate),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: DeclareFuncAndCreateConstructor Failed, define class fail");
        return false;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: DeclareFuncAndCreateConstructor Failed, create reference fail");
        return false;
    }
    return true;
}

napi_value JsCanvas::CreateJsCanvas(napi_env env, Canvas* canvas)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_constructorInitMutex);
        if (!constructor_) {
            if (!DeclareFuncAndCreateConstructor(env)) {
                ROSEN_LOGE("Drawing_napi: DeclareFuncAndCreateConstructor Failed");
                return nullptr;
            }
        }
    }

    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: CreateJsCanvas napi_get_reference_value failed");
        return nullptr;
    }
    g_drawingCanvas = canvas;
    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: New instance could not be obtained");
        return nullptr;
    }
    return result;
}

void JsCanvas::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsCanvas *napi = reinterpret_cast<JsCanvas *>(nativeObject);
        delete napi;
    }
}

napi_value JsCanvas::Init(napi_env env, napi_value exportObj)
{
    {
        std::lock_guard<std::mutex> lock(g_constructorInitMutex);
        if (!constructor_) {
            if (!DeclareFuncAndCreateConstructor(env)) {
                ROSEN_LOGE("Drawing_napi: DeclareFuncAndCreateConstructor Failed");
                return nullptr;
            }
        }
    }

    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: napi_get_reference_value failed");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to set constructor");
        return nullptr;
    }
    return exportObj;
}

JsCanvas::~JsCanvas()
{
    if (owned_) {
        delete m_canvas;
    }
    m_canvas = nullptr;
    g_drawingCanvas = nullptr;
}

napi_value JsCanvas::DrawRect(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawRect(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawRect(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawRect canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsCanvas::OnDrawRect Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("JsCanvas::OnDrawRect Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    napi_value tempValue = nullptr;
    double left = 0.0;
    double top = 0.0;
    double right = 0.0;
    double bottom = 0.0;
    napi_get_named_property(env, argv[0], "left", &tempValue);
    bool isLeftOk = ConvertFromJsValue(env, tempValue, left);
    napi_get_named_property(env, argv[0], "right", &tempValue);
    bool isRightOk = ConvertFromJsValue(env, tempValue, right);
    napi_get_named_property(env, argv[0], "top", &tempValue);
    bool isTopOk = ConvertFromJsValue(env, tempValue, top);
    napi_get_named_property(env, argv[0], "bottom", &tempValue);
    bool isBottomOk = ConvertFromJsValue(env, tempValue, bottom);
    if (!(isLeftOk && isRightOk && isTopOk && isBottomOk)) {
        ROSEN_LOGE("JsCanvas::OnDrawRect Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    Drawing::Rect drawingRect = Drawing::Rect(left, top, right, bottom);
    JS_CALL_DRAWING_FUNC(m_canvas->DrawRect(drawingRect));

    return NapiGetUndefined(env);
}

napi_value JsCanvas::DrawCircle(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawCircle(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawCircle(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawCircle canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsCanvas::OnDrawCircle Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double x = 0.0;
    double y = 0.0;
    double radius = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], x) && ConvertFromJsValue(env, argv[ARGC_ONE], y) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], radius))) {
        ROSEN_LOGE("JsCanvas::OnDrawCircle Argv is invalid");
        return NapiGetUndefined(env);
    }

    Drawing::Point centerPt = Drawing::Point(x, y);
    JS_CALL_DRAWING_FUNC(
        m_canvas->DrawCircle(centerPt, radius));
    return NapiGetUndefined(env);
}

napi_value JsCanvas::DrawImage(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawImage(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawImage(napi_env env, napi_callback_info info)
{
#ifdef ROSEN_OHOS
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImage canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE || argc > ARGC_FOUR) {
        ROSEN_LOGE("JsCanvas::OnDrawImage Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    PixelMapNapi* pixelMapNapi = nullptr;
    double px = 0.0;
    double py = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&pixelMapNapi));
    if (pixelMapNapi == nullptr ||
        !(ConvertFromJsValue(env, argv[ARGC_ONE], px) && ConvertFromJsValue(env, argv[ARGC_TWO], py))) {
        ROSEN_LOGE("JsCanvas::OnDrawImage Argv is invalid");
        return NapiGetUndefined(env);
    }

    if (pixelMapNapi->GetPixelNapiInner() == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImage pixelmap GetPixelNapiInner is nullptr");
        return NapiGetUndefined(env);
    }

    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMapNapi->GetPixelNapiInner());
    if (image == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImage image is nullptr");
        return NapiGetUndefined(env);
    }

    if (argc == ARGC_THREE) {
        JS_CALL_DRAWING_FUNC(m_canvas->DrawImage(*image, px, py, Drawing::SamplingOptions()));
    } else {
        JsSamplingOptions* jsSamplingOptions = nullptr;
        napi_unwrap(env, argv[ARGC_THREE], reinterpret_cast<void **>(&jsSamplingOptions));
        if (jsSamplingOptions == nullptr) {
            ROSEN_LOGE("JsCanvas::OnDrawImage jsSamplingOptions is nullptr");
            return NapiGetUndefined(env);
        }

        std::shared_ptr<SamplingOptions> samplingOptions = jsSamplingOptions->GetSamplingOptions();
        if (samplingOptions == nullptr) {
            ROSEN_LOGE("JsCanvas::OnDrawImage get samplingOptions is nullptr");
            return NapiGetUndefined(env);
        }
        JS_CALL_DRAWING_FUNC(m_canvas->DrawImage(*image, px, py, *samplingOptions.get()));
    }

#endif
    return NapiGetUndefined(env);
}

napi_value JsCanvas::DrawColor(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawColor(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawColor(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawColor canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE || argc > ARGC_TWO) {
        ROSEN_LOGE("JsCanvas::OnDrawColor Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value tempValue = nullptr;
    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    napi_get_named_property(env, argv[0], "alpha", &tempValue);
    bool isAlphaOk = ConvertClampFromJsValue(env, tempValue, alpha, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[0], "red", &tempValue);
    bool isRedOk = ConvertClampFromJsValue(env, tempValue, red, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[0], "green", &tempValue);
    bool isGreenOk = ConvertClampFromJsValue(env, tempValue, green, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[0], "blue", &tempValue);
    bool isBlueOk = ConvertClampFromJsValue(env, tempValue, blue, 0, Color::RGB_MAX);
    if (!(isAlphaOk && isRedOk && isGreenOk && isBlueOk)) {
        ROSEN_LOGE("JsCanvas::OnDrawColor Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    auto color = Color::ColorQuadSetARGB(alpha, red, green, blue);
    if (argc == ARGC_ONE) {
        JS_CALL_DRAWING_FUNC(m_canvas->DrawColor(color));
    } else {
        uint32_t jsMode = 0;
        if (!ConvertFromJsValue(env, argv[1], jsMode)) {
            ROSEN_LOGE("JsCanvas::OnDrawColor Argv[1] is invalid");
            return NapiGetUndefined(env);
        }
        JS_CALL_DRAWING_FUNC(m_canvas->DrawColor(color, BlendMode(jsMode)));
    }
    return NapiGetUndefined(env);
}

napi_value JsCanvas::DrawPoint(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawPoint(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawPoint(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPoint canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsCanvas::OnDrawPoint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double px = 0.0;
    double py = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], px) && ConvertFromJsValue(env, argv[1], py))) {
        ROSEN_LOGE("JsCanvas::OnDrawPoint Argv is invalid");
        return NapiGetUndefined(env);
    }

    JS_CALL_DRAWING_FUNC(m_canvas->DrawPoint(Point(px, py)));
    return NapiGetUndefined(env);
}

napi_value JsCanvas::DrawPath(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawPath(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawPath(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPath canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsCanvas::OnDrawPath Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsPath* jsPath = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsPath));
    if (jsPath == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPath jsPath is nullptr");
        return NapiGetUndefined(env);
    }

    if (jsPath->GetPath() == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPath path is nullptr");
        return NapiGetUndefined(env);
    }

    JS_CALL_DRAWING_FUNC(m_canvas->DrawPath(*jsPath->GetPath()));
    return NapiGetUndefined(env);
}

napi_value JsCanvas::DrawLine(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawLine(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawLine(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawLine canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FOUR) {
        ROSEN_LOGE("JsCanvas::OnDrawLine Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double startPx = 0.0;
    double startPy = 0.0;
    double endPx = 0.0;
    double endPy = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], startPx) && ConvertFromJsValue(env, argv[ARGC_ONE], startPy) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], endPx) && ConvertFromJsValue(env, argv[ARGC_THREE], endPy))) {
        ROSEN_LOGE("JsCanvas::OnDrawLine Argv is invalid");
        return NapiGetUndefined(env);
    }

    JS_CALL_DRAWING_FUNC(m_canvas->DrawLine(Point(startPx, startPy), Point(endPx, endPy)));
    return NapiGetUndefined(env);
}

napi_value JsCanvas::DrawText(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawText(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawText(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawText canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsCanvas::OnDrawText Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsTextBlob* jsTextBlob = nullptr;
    double x = 0.0;
    double y = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsTextBlob));
    if (jsTextBlob == nullptr ||
        !(ConvertFromJsValue(env, argv[ARGC_ONE], x) && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        ROSEN_LOGE("JsCanvas::OnDrawText Argv is invalid");
        return NapiGetUndefined(env);
    }

    JS_CALL_DRAWING_FUNC(m_canvas->DrawTextBlob(jsTextBlob->GetTextBlob().get(), x, y));
    return NapiGetUndefined(env);
}

napi_value JsCanvas::DrawPixelMapMesh(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawPixelMapMesh(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawPixelMapMesh(napi_env env, napi_callback_info info)
{
#ifdef ROSEN_OHOS
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_SEVEN;
    napi_value argv[ARGC_SEVEN] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc != ARGC_SEVEN) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    PixelMapNapi* pixelMapNapi = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&pixelMapNapi));
    if (pixelMapNapi == nullptr) {
        ROSEN_LOGE("Drawing_napi::pixelMap pixelMapNapi is nullptr");
        return nullptr;
    }

    if (pixelMapNapi->GetPixelNapiInner() == nullptr) {
        ROSEN_LOGE("Drawing_napi::pixelMap pixelmap getPixelNapiInner is nullptr");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = pixelMapNapi->GetPixelNapiInner();

    uint32_t column = 0;
    uint32_t row = 0;
    uint32_t vertOffset = 0;
    uint32_t colorOffset = 0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ONE], column) && ConvertFromJsValue(env, argv[ARGC_TWO], row) &&
        ConvertFromJsValue(env, argv[ARGC_FOUR], vertOffset) && ConvertFromJsValue(env, argv[ARGC_SIX], colorOffset))) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh Argv is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid uint32 params.");
    }
    if (column == 0 || row == 0) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh column or row is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid column or row params.");
    }

    napi_value verticesArray = argv[ARGC_THREE];
    uint32_t verticesSize = 0;
    napi_get_array_length(env, verticesArray, &verticesSize);
    uint64_t tempVerticesSize = ((column + 1) * (row + 1) + vertOffset) * 2; // x and y two coordinates
    if (verticesSize != tempVerticesSize) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh vertices are invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid vertices params.");
    }

    auto vertices = new float[verticesSize];
    for (uint32_t i = 0; i < verticesSize; i++) {
        napi_value tempVertex = nullptr;
        napi_get_element(env, verticesArray, i, &tempVertex);
        double vertex = 0.f;
        bool isVertexOk = ConvertFromJsValue(env, tempVertex, vertex);
        if (!isVertexOk) {
            ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh vertex is invalid");
            delete []vertices;
            return NapiGetUndefined(env);
        }
        vertices[i] = vertex;
    }
    float* verticesMesh = verticesSize ? (vertices + vertOffset * 2) : nullptr; // offset two coordinates

    napi_value colorsArray = argv[ARGC_FIVE];
    uint32_t colorsSize = 0;
    napi_get_array_length(env, colorsArray, &colorsSize);
    uint64_t tempColorsSize = (column + 1) * (row + 1) + colorOffset;

    if (colorsSize != 0 && colorsSize != tempColorsSize) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh colors are invalid");
        delete []vertices;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid colors params.");
    }

    if (colorsSize == 0) {
        DrawingPixelMapMesh(pixelMap, column, row, verticesMesh, nullptr, m_canvas);
        delete []vertices;
        return NapiGetUndefined(env);
    }

    auto colors = new uint32_t[colorsSize];
    for (uint32_t i = 0; i < colorsSize; i++) {
        napi_value tempColor = nullptr;
        napi_get_element(env, colorsArray, i, &tempColor);
        uint32_t color = 0;
        bool isColorOk = ConvertFromJsValue(env, tempColor, color);
        if (!isColorOk) {
            ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh color is invalid");
            delete []vertices;
            delete []colors;
            return NapiGetUndefined(env);
        }
        colors[i] = color;
    }
    uint32_t* colorsMesh = colors + colorOffset;

    DrawingPixelMapMesh(pixelMap, column, row, verticesMesh, colorsMesh, m_canvas);
    delete []vertices;
    delete []colors;
    return NapiGetUndefined(env);
#else
    return nullptr;
#endif
}

napi_value JsCanvas::AttachPen(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    Canvas* canvas = me->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachPen canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsCanvas::AttachPen Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsPen* jsPen = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsPen));
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachPen jsPen is nullptr");
        return NapiGetUndefined(env);
    }
    if (jsPen->GetPen() == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachPen pen is nullptr");
        return NapiGetUndefined(env);
    }
    JS_CALL_DRAWING_FUNC(canvas->AttachPen(*jsPen->GetPen()));
    return NapiGetUndefined(env);
}

napi_value JsCanvas::AttachBrush(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    Canvas* canvas = me->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachBrush canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsCanvas::AttachBrush Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsBrush* jsBrush = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsBrush));
    if (jsBrush == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachBrush jsBrush is nullptr");
        return NapiGetUndefined(env);
    }
    if (jsBrush->GetBrush() == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachBrush brush is nullptr");
        return NapiGetUndefined(env);
    }
    JS_CALL_DRAWING_FUNC(canvas->AttachBrush(*jsBrush->GetBrush()));
    return NapiGetUndefined(env);
}

napi_value JsCanvas::DetachPen(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    Canvas* canvas = me->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::DetachPen canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    JS_CALL_DRAWING_FUNC(canvas->DetachPen());
    return NapiGetUndefined(env);
}

napi_value JsCanvas::DetachBrush(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    Canvas* canvas = me->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::DetachBrush canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    JS_CALL_DRAWING_FUNC(canvas->DetachBrush());
    return NapiGetUndefined(env);
}

napi_value JsCanvas::Skew(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnSkew(env, info) : nullptr;
}

napi_value JsCanvas::OnSkew(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnSkew m_canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc != ARGC_TWO) {
        ROSEN_LOGE("JsCanvas::OnSkew Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double sx = 0.0;
    double sy = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], sx) && ConvertFromJsValue(env, argv[ARGC_ONE], sy))) {
        ROSEN_LOGE("JsCanvas::OnSkew Argv is invalid");
        return NapiGetUndefined(env);
    }

    m_canvas->Shear(sx, sy);
    return NapiGetUndefined(env);
}

napi_value JsCanvas::Rotate(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnRotate(env, info) : nullptr;
}

napi_value JsCanvas::OnRotate(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnRotate m_canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc != ARGC_THREE) {
        ROSEN_LOGE("JsCanvas::OnRotate argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double degree = 0.0;
    double sx = 0.0;
    double sy = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], degree) && ConvertFromJsValue(env, argv[ARGC_ONE], sx) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], sy))) {
        ROSEN_LOGE("JsCanvas::OnRotate argv is invalid");
        return NapiGetUndefined(env);
    }

    m_canvas->Rotate(degree, sx, sy);
    return NapiGetUndefined(env);
}

napi_value JsCanvas::GetSaveCount(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnGetSaveCount(env, info) : nullptr;
}

napi_value JsCanvas::OnGetSaveCount(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnGetSaveCount canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = 0;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc != 0) {
        ROSEN_LOGE("JsCanvas::OnGetSaveCount argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    return CreateJsNumber(env, m_canvas->GetSaveCount());
}

napi_value JsCanvas::ClipPath(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnClipPath(env, info) : nullptr;
}

napi_value JsCanvas::OnClipPath(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnClipPath m_canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE || argc > ARGC_THREE) {
        ROSEN_LOGE("JsCanvas::OnClipPath argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsPath* jsPath = nullptr;
    napi_unwrap(env, argv[ARGC_ZERO], reinterpret_cast<void **>(&jsPath));
    if (jsPath == nullptr) {
        ROSEN_LOGE("JsCanvas::OnClipPath jsPath is nullptr");
        return NapiGetUndefined(env);
    }
    Path* path = jsPath->GetPath();
    if (path == nullptr) {
        ROSEN_LOGE("JsCanvas::OnClipPath path is nullptr");
        return NapiGetUndefined(env);
    }
    if (argc == ARGC_ONE) {
        m_canvas->ClipPath(*path);
        return NapiGetUndefined(env);
    }
    uint32_t jsClipOp = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], jsClipOp)) {
        ROSEN_LOGE("JsCanvas::OnClipPath argv[1] is error");
        return NapiGetUndefined(env);
    }
    if (argc == ARGC_TWO) {
        m_canvas->ClipPath(*path, static_cast<ClipOp>(jsClipOp));
        return NapiGetUndefined(env);
    }
    bool jsDoAntiAlias = false;
    if (!ConvertFromJsValue(env, argv[ARGC_TWO], jsDoAntiAlias)) {
        ROSEN_LOGE("JsCanvas::OnClipPath argv[2] is error");
        return NapiGetUndefined(env);
    }
    m_canvas->ClipPath(*path, static_cast<ClipOp>(jsClipOp), jsDoAntiAlias);
    return NapiGetUndefined(env);
}

napi_value JsCanvas::Translate(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnTranslate(env, info) : nullptr;
}

napi_value JsCanvas::OnTranslate(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnTranslate m_canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsCanvas::OnTranslate argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double dx = 0.0;
    double dy = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], dx) && ConvertFromJsValue(env, argv[ARGC_ONE], dy))) {
        ROSEN_LOGE("JsCanvas::OnTranslate argv is invalid");
        return NapiGetUndefined(env);
    }
    m_canvas->Translate(dx, dy);
    return NapiGetUndefined(env);
}

napi_value JsCanvas::Save(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnSave(env, info) : nullptr;
}

napi_value JsCanvas::OnSave(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnSave canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    auto saveCount = m_canvas->Save();
    return CreateJsNumber(env, saveCount);
}

napi_value JsCanvas::RestoreToCount(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnRestoreToCount(env, info) : nullptr;
}

napi_value JsCanvas::OnRestoreToCount(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnRestoreToCount canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsCanvas::OnRestoreToCount argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    uint32_t count = 0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], count))) {
        ROSEN_LOGE("JsCanvas::OnRestoreToCount argv is invalid");
        return NapiGetUndefined(env);
    }
    m_canvas->RestoreToCount(count);
    return NapiGetUndefined(env);
}

napi_value JsCanvas::Restore(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnRestore(env, info) : nullptr;
}

napi_value JsCanvas::OnRestore(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnRestore m_canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    m_canvas->Restore();
    return NapiGetUndefined(env);
}

napi_value JsCanvas::ClipRect(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnClipRect(env, info) : nullptr;
}

napi_value JsCanvas::OnClipRect(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnClipRect m_canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE || argc > ARGC_THREE) {
        ROSEN_LOGE("JsCanvas::OnClipRect argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[ARGC_ZERO] == nullptr || napi_typeof(env, argv[ARGC_ZERO], &valueType) != napi_ok ||
        valueType != napi_object) {
        ROSEN_LOGE("JsCanvas::OnClipRect argv[0] is invalid");
        return NapiGetUndefined(env);
    }
    napi_value tempValue = nullptr;
    double left = 0.0;
    double top = 0.0;
    double right = 0.0;
    double bottom = 0.0;
    napi_get_named_property(env, argv[ARGC_ZERO], "left", &tempValue);
    bool isLeftOk = ConvertFromJsValue(env, tempValue, left);
    napi_get_named_property(env, argv[ARGC_ZERO], "right", &tempValue);
    bool isRightOk = ConvertFromJsValue(env, tempValue, right);
    napi_get_named_property(env, argv[ARGC_ZERO], "top", &tempValue);
    bool isTopOk = ConvertFromJsValue(env, tempValue, top);
    napi_get_named_property(env, argv[ARGC_ZERO], "bottom", &tempValue);
    bool isBottomOk = ConvertFromJsValue(env, tempValue, bottom);
    if (!(isLeftOk && isRightOk && isTopOk && isBottomOk)) {
        ROSEN_LOGE("JsCanvas::OnClipRect argv[0] is invalid");
        return NapiGetUndefined(env);
    }
    Drawing::Rect drawingRect = Drawing::Rect(left, top, right, bottom);

    if (argc == ARGC_ONE) {
        m_canvas->ClipRect(drawingRect);
        return NapiGetUndefined(env);
    }
    uint32_t clipOpInt = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], clipOpInt)) {
        ROSEN_LOGE("JsCanvas::OnClipRect argv[1] is error");
        return NapiGetUndefined(env);
    }
    if (argc == ARGC_TWO) {
        m_canvas->ClipRect(drawingRect, static_cast<ClipOp>(clipOpInt));
        return NapiGetUndefined(env);
    }
    bool doAntiAlias = false;
    if (!ConvertFromJsValue(env, argv[ARGC_TWO], doAntiAlias)) {
        ROSEN_LOGE("JsCanvas::OnClipRect argv[2] is error");
        return NapiGetUndefined(env);
    }
    m_canvas->ClipRect(drawingRect, static_cast<ClipOp>(clipOpInt), doAntiAlias);
    return NapiGetUndefined(env);
}

napi_value JsCanvas::Scale(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnScale(env, info) : nullptr;
}

napi_value JsCanvas::OnScale(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnScale canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsCanvas::OnScale argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double sx = 0.0;
    double sy = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], sx) && ConvertFromJsValue(env, argv[1], sy))) {
        ROSEN_LOGE("JsCanvas::OnScale argv is invalid");
        return NapiGetUndefined(env);
    }

    m_canvas->Scale(sx, sy);
    return NapiGetUndefined(env);
}

Canvas* JsCanvas::GetCanvas()
{
    return m_canvas;
}

void JsCanvas::ResetCanvas()
{
    g_drawingCanvas = nullptr;
    m_canvas = nullptr;
}

void JsCanvas::ClipCanvas(float width, float height)
{
    if (m_canvas) {
        Rect rect(0, 0, width, height);
        JS_CALL_DRAWING_FUNC(m_canvas->ClipRect(rect));
    }
}

void JsCanvas::SaveCanvas()
{
    if (m_canvas) {
        JS_CALL_DRAWING_FUNC(m_canvas->Save());
    }
}

void JsCanvas::RestoreCanvas()
{
    if (m_canvas) {
        JS_CALL_DRAWING_FUNC(m_canvas->Restore());
    }
}
} // namespace Drawing
} // namespace OHOS::Rosen
