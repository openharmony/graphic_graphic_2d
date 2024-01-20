
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

#include <mutex>
#include "js_canvas.h"
#include "../brush_napi/js_brush.h"
#include "../pen_napi/js_pen.h"
#include "../path_napi/js_path.h"
#include "../text_blob_napi/js_text_blob.h"
#include "../js_drawing_utils.h"
#include "native_value.h"
#ifdef ROSEN_OHOS
#include "pixel_map.h"
#include "pixel_map_napi.h"
#endif
#include "draw/canvas.h"
#include "image/image.h"
#include "draw/path.h"
#include "text/text.h"
#include "text/text_blob.h"
#include "utils/point.h"
#include "utils/sampling_options.h"
#include "utils/rect.h"

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

std::shared_ptr<Drawing::Image> ExtractDrawingImage(
    std::shared_ptr<Media::PixelMap> pixelMap)
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
        reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowBytes());
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
}
#endif

namespace Drawing {
thread_local napi_ref JsCanvas::constructor_ = nullptr;
static std::mutex g_constructorInitMutex;

Canvas* g_drawingCanvas = nullptr;
const std::string CLASS_NAME = "Canvas";
napi_value JsCanvas::Constructor(napi_env env, napi_callback_info info)
{
    if (g_drawingCanvas == nullptr) {
        ROSEN_LOGE("Drawing_napi: m_canvas is nullptr");
        return nullptr;
    }

    napi_value jsThis = nullptr;
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: failed to napi_get_cb_info");
        return nullptr;
    }
    if (argc == 0) {
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
            return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
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
        DECLARE_NAPI_FUNCTION("attachPen", JsCanvas::AttachPen),
        DECLARE_NAPI_FUNCTION("attachBrush", JsCanvas::AttachBrush),
        DECLARE_NAPI_FUNCTION("detachPen", JsCanvas::DetachPen),
        DECLARE_NAPI_FUNCTION("detachBrush", JsCanvas::DetachBrush),
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

napi_value JsCanvas::CreateJsCanvas(napi_env env, Canvas* canvas, float width, float height)
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
    Rect rect(0, 0, width, height);
    canvas->ClipRect(rect);
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
        ROSEN_LOGE("Drawing_napi: OnDrawRect canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("Drawing_napi: OnDrawRect Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("Drawing_napi: OnDrawRect Argv[0] is invalid");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_value tempValue = nullptr;
    double left = 0.0f;
    double top = 0.0f;
    double right = 0.0f;
    double bottom = 0.0f;
    napi_get_named_property(env, argv[0], "left", &tempValue);
    napi_get_value_double(env, tempValue, &left);
    napi_get_named_property(env, argv[0], "right", &tempValue);
    napi_get_value_double(env, tempValue, &right);
    napi_get_named_property(env, argv[0], "top", &tempValue);
    napi_get_value_double(env, tempValue, &top);
    napi_get_named_property(env, argv[0], "bottom", &tempValue);
    napi_get_value_double(env, tempValue, &bottom);

    Drawing::Rect drawingRect = Drawing::Rect(left, top, right, bottom);
    m_canvas->DrawRect(drawingRect);
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
        ROSEN_LOGE("Drawing_napi: OnDrawRect canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("Drawing_napi: OnDrawCircle Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    double x = 0.0f;
    double y = 0.0f;
    double radius = 0.0f;
    ConvertFromJsValue(env, argv[0], x);
    ConvertFromJsValue(env, argv[ARGC_ONE], y);
    ConvertFromJsValue(env, argv[ARGC_TWO], radius);

    Drawing::Point centerPt = Drawing::Point(x, y);
    m_canvas->DrawCircle(centerPt, radius);
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
        ROSEN_LOGE("Drawing_napi: OnDrawRect canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("Drawing_napi: OnDrawImage Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    PixelMapNapi* pixelMapNapi = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&pixelMapNapi));
    if (pixelMapNapi == nullptr) {
        ROSEN_LOGE("Drawing_napi: canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    if (pixelMapNapi->GetPixelNapiInner() == nullptr) {
        ROSEN_LOGE("Drawing_napi: pixelmap GetPixelNapiInner is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixelMapNapi->GetPixelNapiInner());
    if (image == nullptr) {
        ROSEN_LOGE("Drawing_napi: Drawing image is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    double px = 0.0f;
    double py = 0.0f;
    ConvertFromJsValue(env, argv[ARGC_ONE], px);
    ConvertFromJsValue(env, argv[ARGC_TWO], py);

    m_canvas->DrawImage(*image, px, py, Drawing::SamplingOptions());
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
        ROSEN_LOGE("Drawing_napi: OnDrawRect canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE || argc > ARGC_TWO) {
        ROSEN_LOGE("Drawing_napi: OnDrawColor Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    uint32_t alpha;
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argv[0], "alpha", &tempValue);
    napi_get_value_uint32(env, tempValue, &alpha);
    napi_get_named_property(env, argv[0], "red", &tempValue);
    napi_get_value_uint32(env, tempValue, &red);
    napi_get_named_property(env, argv[0], "green", &tempValue);
    napi_get_value_uint32(env, tempValue, &green);
    napi_get_named_property(env, argv[0], "blue", &tempValue);
    napi_get_value_uint32(env, tempValue, &blue);
    auto color = Color::ColorQuadSetARGB(alpha, red, green, blue);

    if (argc == ARGC_ONE) {
        m_canvas->DrawColor(color);
    } else {
        uint32_t jsMode;
        ConvertFromJsValue(env, argv[1], jsMode);
        m_canvas->DrawColor(color, BlendMode(jsMode));
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
        ROSEN_LOGE("Drawing_napi: OnDrawRect canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("Drawing_napi: OnDrawPoint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }
    double px = 0.0f;
    double py = 0.0f;
    ConvertFromJsValue(env, argv[0], px);
    ConvertFromJsValue(env, argv[1], py);

    m_canvas->DrawPoint(Point(px, py));
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
        ROSEN_LOGE("Drawing_napi: OnDrawRect canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("Drawing_napi: OnDrawPath Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    JsPath* jsPath = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsPath));
    if (jsPath == nullptr) {
        ROSEN_LOGE("Drawing_napi: jsPath is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    if (jsPath->GetPath() == nullptr) {
        ROSEN_LOGE("Drawing_napi: jsPath GetPath is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    m_canvas->DrawPath(*jsPath->GetPath());
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
        ROSEN_LOGE("Drawing_napi: OnDrawLine canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FOUR) {
        ROSEN_LOGE("Drawing_napi: OnDrawLine Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    double startPx = 0.0f;
    double startPy = 0.0f;
    double endPx = 0.0f;
    double endPy = 0.0f;
    ConvertFromJsValue(env, argv[0], startPx);
    ConvertFromJsValue(env, argv[ARGC_ONE], startPy);
    ConvertFromJsValue(env, argv[ARGC_TWO], endPx);
    ConvertFromJsValue(env, argv[ARGC_THREE], endPy);

    m_canvas->DrawLine(Point(startPx, startPy), Point(endPx, endPy));
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
        ROSEN_LOGE("Drawing_napi: OnDrawText canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsCanvas::OnDrawText Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    JsTextBlob* jsTextBlob = nullptr;
    double x = 0.0f;
    double y = 0.0f;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsTextBlob));
    ConvertFromJsValue(env, argv[ARGC_ONE], x);
    ConvertFromJsValue(env, argv[ARGC_TWO], y);
    if (jsTextBlob == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawText jsTextBlob is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }
    m_canvas->DrawTextBlob(jsTextBlob->GetTextBlob().get(), x, y);
    return NapiGetUndefined(env);
}

napi_value JsCanvas::AttachPen(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    Canvas* canvas = me->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("Drawing_napi: AttachPen canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsCanvas::AttachPen Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    JsPen* jsPen = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsPen));
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachPen jsPen is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }
    if (jsPen->GetPen() == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachPen pen is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }
    canvas->AttachPen(*jsPen->GetPen());
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
        ROSEN_LOGE("Drawing_napi: AttachBrush canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsCanvas::AttachBrush Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    JsBrush* jsBrush = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsBrush));
    if (jsBrush == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachBrush jsBrush is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }
    if (jsBrush->GetBrush() == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachBrush brush is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }
    canvas->AttachBrush(*jsBrush->GetBrush());
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
        ROSEN_LOGE("Drawing_napi: DetachPen canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    canvas->DetachPen();
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
        ROSEN_LOGE("Drawing_napi: DetachBrush canvas is null");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    canvas->DetachBrush();
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
} // namespace Drawing
} // namespace OHOS::Rosen
