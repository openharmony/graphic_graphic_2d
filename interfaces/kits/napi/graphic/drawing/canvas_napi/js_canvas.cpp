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

#ifdef USE_M133_SKIA
#include "src/base/SkUTF.h"
#else
#include "src/utils/SkUTF.h"
#endif

#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
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
#include "font_napi/js_font.h"
#include "lattice_napi/js_lattice.h"
#include "matrix_napi/js_matrix.h"
#include "pen_napi/js_pen.h"
#include "path_napi/js_path.h"
#include "region_napi/js_region.h"
#include "sampling_options_napi/js_sampling_options.h"
#include "text_blob_napi/js_text_blob.h"
#include "roundRect_napi/js_roundrect.h"
#include "js_drawing_utils.h"
#include "utils/performanceCaculate.h"
#if defined(OHOS_PLATFORM) || defined(ROSEN_ARKUI_X)
#include "pipeline/rs_recording_canvas.h"
#endif

namespace OHOS::Rosen {
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
using namespace Media;
namespace {
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
        ROSEN_LOGE("Drawing_napi::textPoint or indices is nullptr");
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
    if (m_canvas->GetDrawingType() != Drawing::DrawingType::RECORDING) {
        std::shared_ptr<Drawing::Image> image = Drawing::ExtractDrawingImage(pixelMap);
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
const std::string CLASS_NAME = "Canvas";

static const napi_property_descriptor g_properties[] = {
    DECLARE_NAPI_FUNCTION("clear", JsCanvas::Clear),
    DECLARE_NAPI_FUNCTION("drawArc", JsCanvas::DrawArc),
    DECLARE_NAPI_FUNCTION("drawArcWithCenter", JsCanvas::DrawArcWithCenter),
    DECLARE_NAPI_FUNCTION("drawRect", JsCanvas::DrawRect),
    DECLARE_NAPI_FUNCTION("drawCircle", JsCanvas::DrawCircle),
    DECLARE_NAPI_FUNCTION("drawImage", JsCanvas::DrawImage),
    DECLARE_NAPI_FUNCTION("drawImageLattice", JsCanvas::DrawImageLattice),
    DECLARE_NAPI_FUNCTION("drawImageNine", JsCanvas::DrawImageNine),
    DECLARE_NAPI_FUNCTION("drawImageRect", JsCanvas::DrawImageRect),
    DECLARE_NAPI_FUNCTION("drawImageRectWithSrc", JsCanvas::DrawImageRectWithSrc),
    DECLARE_NAPI_FUNCTION("drawColor", JsCanvas::DrawColor),
    DECLARE_NAPI_FUNCTION("drawOval", JsCanvas::DrawOval),
    DECLARE_NAPI_FUNCTION("drawPoint", JsCanvas::DrawPoint),
    DECLARE_NAPI_FUNCTION("drawPoints", JsCanvas::DrawPoints),
    DECLARE_NAPI_FUNCTION("drawPath", JsCanvas::DrawPath),
    DECLARE_NAPI_FUNCTION("drawLine", JsCanvas::DrawLine),
    DECLARE_NAPI_FUNCTION("drawTextBlob", JsCanvas::DrawText),
    DECLARE_NAPI_FUNCTION("drawSingleCharacter", JsCanvas::DrawSingleCharacter),
    DECLARE_NAPI_FUNCTION("drawSingleCharacterWithFeatures", JsCanvas::DrawSingleCharacterWithFeatures),
    DECLARE_NAPI_FUNCTION("getTotalMatrix", JsCanvas::GetTotalMatrix),
    DECLARE_NAPI_FUNCTION("getLocalClipBounds", JsCanvas::GetLocalClipBounds),
    DECLARE_NAPI_FUNCTION("drawPixelMapMesh", JsCanvas::DrawPixelMapMesh),
    DECLARE_NAPI_FUNCTION("drawRegion", JsCanvas::DrawRegion),
    DECLARE_NAPI_FUNCTION("drawShadow", JsCanvas::DrawShadow),
    DECLARE_NAPI_FUNCTION("drawBackground", JsCanvas::DrawBackground),
    DECLARE_NAPI_FUNCTION("drawRoundRect", JsCanvas::DrawRoundRect),
    DECLARE_NAPI_FUNCTION("drawNestedRoundRect", JsCanvas::DrawNestedRoundRect),
    DECLARE_NAPI_FUNCTION("attachPen", JsCanvas::AttachPen),
    DECLARE_NAPI_FUNCTION("attachBrush", JsCanvas::AttachBrush),
    DECLARE_NAPI_FUNCTION("detachPen", JsCanvas::DetachPen),
    DECLARE_NAPI_FUNCTION("detachBrush", JsCanvas::DetachBrush),
    DECLARE_NAPI_FUNCTION("skew", JsCanvas::Skew),
    DECLARE_NAPI_FUNCTION("rotate", JsCanvas::Rotate),
    DECLARE_NAPI_FUNCTION("getSaveCount", JsCanvas::GetSaveCount),
    DECLARE_NAPI_FUNCTION("getWidth", JsCanvas::GetWidth),
    DECLARE_NAPI_FUNCTION("getHeight", JsCanvas::GetHeight),
    DECLARE_NAPI_FUNCTION("save", JsCanvas::Save),
    DECLARE_NAPI_FUNCTION("saveLayer", JsCanvas::SaveLayer),
    DECLARE_NAPI_FUNCTION("restore", JsCanvas::Restore),
    DECLARE_NAPI_FUNCTION("restoreToCount", JsCanvas::RestoreToCount),
    DECLARE_NAPI_FUNCTION("scale", JsCanvas::Scale),
    DECLARE_NAPI_FUNCTION("clipPath", JsCanvas::ClipPath),
    DECLARE_NAPI_FUNCTION("clipRegion", JsCanvas::ClipRegion),
    DECLARE_NAPI_FUNCTION("clipRect", JsCanvas::ClipRect),
    DECLARE_NAPI_FUNCTION("concatMatrix", JsCanvas::ConcatMatrix),
    DECLARE_NAPI_FUNCTION("clipRoundRect", JsCanvas::ClipRoundRect),
    DECLARE_NAPI_FUNCTION("setMatrix", JsCanvas::SetMatrix),
    DECLARE_NAPI_FUNCTION("resetMatrix", JsCanvas::ResetMatrix),
    DECLARE_NAPI_FUNCTION("translate", JsCanvas::Translate),
    DECLARE_NAPI_FUNCTION("isClipEmpty", JsCanvas::IsClipEmpty),
    DECLARE_NAPI_FUNCTION("quickRejectPath", JsCanvas::QuickRejectPath),
    DECLARE_NAPI_FUNCTION("quickRejectRect", JsCanvas::QuickRejectRect),
};

napi_value JsCanvas::Constructor(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_START_CACULATE;
    napi_value jsThis = nullptr;
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: failed to napi_get_cb_info");
        return nullptr;
    }

#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi);

    if (pixelMapNapi->GetPixelNapiInner() == nullptr) {
        return nullptr;
    }

    Bitmap bitmap;
    if (!ExtracetDrawingBitmap(pixelMapNapi->GetPixelNapiInner(), bitmap)) {
        return nullptr;
    }

    Canvas* canvas = new Canvas();
    canvas->Bind(bitmap);
    JsCanvas *jsCanvas = new JsCanvas(canvas, true);
    jsCanvas->mPixelMap_ = pixelMapNapi->GetPixelNapiInner();
    status = napi_wrap(env, jsThis, jsCanvas, JsCanvas::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsCanvas;
        ROSEN_LOGE("Drawing_napi: Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
#else
    return nullptr;
#endif
}

bool JsCanvas::CreateConstructor(napi_env env)
{
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(g_properties) / sizeof(g_properties[0]), g_properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: CreateConstructor Failed, define class fail");
        return false;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: CreateConstructor Failed, create reference fail");
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
            if (!CreateConstructor(env)) {
                ROSEN_LOGE("Drawing_napi: CreateConstructor Failed");
                return nullptr;
            }
        }
    }

    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: CreateJsCanvas napi_get_reference_value failed");
        return nullptr;
    }

    if (canvas == nullptr) {
        ROSEN_LOGE("Drawing_napi: canvas is nullptr");
        return nullptr;
    }
    JsCanvas *jsCanvas = new JsCanvas(canvas);
    napi_create_object(env, &result);
    if (result == nullptr) {
        delete jsCanvas;
        ROSEN_LOGE("jsCanvas::CreateJsCanvas Create canvas object failed!");
        return nullptr;
    }
    status = napi_wrap(env, result, jsCanvas, JsCanvas::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsCanvas;
        ROSEN_LOGE("Drawing_napi: Failed to wrap native instance");
        return nullptr;
    }
    napi_define_properties(env, result, sizeof(g_properties) / sizeof(g_properties[0]), g_properties);
    return result;
}

void JsCanvas::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    DRAWING_PERFORMANCE_STOP_CACULATE;
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
            if (!CreateConstructor(env)) {
                ROSEN_LOGE("Drawing_napi: CreateConstructor Failed");
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
}

napi_value JsCanvas::Clear(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnClear(env, info) : nullptr;
}

napi_value JsCanvas::OnClear(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnClear canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    ColorQuad color;
    if (!ConvertFromAdaptHexJsColor(env, argv[ARGC_ZERO], color)) {
        ROSEN_LOGE("JsCanvas::OnClear Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
    }

    JS_CALL_DRAWING_FUNC(m_canvas->Clear(color));
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawShadow(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawShadow(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawShadow(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawShadow canvas is null.");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_SEVEN] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_SEVEN);
    JsPath* jsPath = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsPath);

    Point3 offset;
    Point3 lightPos;
    if (!ConvertFromJsPoint3d(env, argv[ARGC_ONE], offset) || !ConvertFromJsPoint3d(env, argv[ARGC_TWO], lightPos)) {
        ROSEN_LOGE("JsCanvas::OnDrawShadow argv[ARGC_ONE] or argv[ARGC_TWO] is invalid.");
        return nullptr;
    }

    double lightRadius = 0.0f;
    GET_DOUBLE_PARAM(ARGC_THREE, lightRadius);

    ColorQuad ambientColor;
    ColorQuad spotColor;
    if (!ConvertFromAdaptHexJsColor(env, argv[ARGC_FOUR], ambientColor) ||
        !ConvertFromAdaptHexJsColor(env, argv[ARGC_FIVE], spotColor)) {
        ROSEN_LOGE("JsCanvas::OnDrawShadow argv[ARGC_FOUR] or argv[ARGC_FIVE] is invalid.");
        return nullptr;
    }

    int32_t shadowFlag = 0;
    GET_ENUM_PARAM(ARGC_SIX, shadowFlag, 0, static_cast<int32_t>(ShadowFlags::ALL));
    OHOS::Rosen::Drawing::ShadowFlags shadowFlags = static_cast<OHOS::Rosen::Drawing::ShadowFlags>(shadowFlag);
    if (!ConvertFromJsShadowFlag(env, argv[ARGC_SIX], shadowFlags)) {
        ROSEN_LOGE("JsCanvas::OnDrawShadow argv[ARGC_SIX] is invalid.");
        return nullptr;
    }
    if (jsPath->GetPath() == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawShadow GetPath is nullptr.");
        return nullptr;
    }

    m_canvas->DrawShadow(*jsPath->GetPath(), offset, lightPos, lightRadius, ambientColor, spotColor,
        static_cast<ShadowFlags>(shadowFlag));
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawArc(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawArc(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawArc(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawArc canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    
    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    double startAngle = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, startAngle);
    double sweepAngle = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, sweepAngle);

    JS_CALL_DRAWING_FUNC(m_canvas->DrawArc(drawingRect, startAngle, sweepAngle));
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawArcWithCenter(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawArcWithCenter(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawArcWithCenter(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawArcWithCenter canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    double startAngle = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, startAngle);
    double sweepAngle = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, sweepAngle);
    bool jsUseCenter = false;
    GET_BOOLEAN_PARAM(ARGC_THREE, jsUseCenter);

    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    if (jsUseCenter) {
        JS_CALL_DRAWING_FUNC(m_canvas->DrawPie(drawingRect, startAngle, sweepAngle));
    } else {
        JS_CALL_DRAWING_FUNC(m_canvas->DrawArc(drawingRect, startAngle, sweepAngle));
    }
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawRect(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawRect(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawRect(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawRect canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_FOUR);
    Drawing::Rect drawingRect;
    if (argc == ARGC_ONE) {
        double ltrb[ARGC_FOUR] = {0};
        if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        }

        drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    } else if (argc == ARGC_FOUR) {
        double left = 0.0;
        GET_DOUBLE_PARAM(ARGC_ZERO, left);
        double top = 0.0;
        GET_DOUBLE_PARAM(ARGC_ONE, top);
        double right = 0.0;
        GET_DOUBLE_PARAM(ARGC_TWO, right);
        double bottom = 0.0;
        GET_DOUBLE_PARAM(ARGC_THREE, bottom);
        drawingRect = Drawing::Rect(left, top, right, bottom);
    } else {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    m_canvas->DrawRect(drawingRect);
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::QuickRejectPath(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnQuickRejectPath(env, info) : nullptr;
}

napi_value JsCanvas::OnQuickRejectPath(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnQuickRejectPath canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsPath* jsPath = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsPath);

    if (jsPath == nullptr || jsPath->GetPath() == nullptr) {
        ROSEN_LOGE("JsCanvas::OnQuickRejectPath path is nullptr");
        return nullptr;
    }

    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    return CreateJsValue(env, m_canvas->QuickReject(*jsPath->GetPath()));
}

napi_value JsCanvas::QuickRejectRect(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnQuickRejectRect(env, info) : nullptr;
}

napi_value JsCanvas::OnQuickRejectRect(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnQuickRejectRect canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    return CreateJsValue(env, m_canvas->QuickReject(drawingRect));
}

napi_value JsCanvas::DrawCircle(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawCircle(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawCircle(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawCircle canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);
    double radius = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, radius);

    Drawing::Point centerPt = Drawing::Point(x, y);
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    m_canvas->DrawCircle(centerPt, radius);
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawImage(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawImage(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawImage(napi_env env, napi_callback_info info)
{
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImage canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_THREE, ARGC_FOUR);

    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, py);
    PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi);

    auto pixel = pixelMapNapi->GetPixelNapiInner();
    if (pixel == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImage pixelmap GetPixelNapiInner is nullptr");
        return nullptr;
    }

    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixel);
    if (image == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImage image is nullptr");
        return nullptr;
    }

    if (argc == ARGC_THREE) {
        DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
        if (m_canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
            ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(m_canvas);
            Drawing::Rect src(0, 0, pixel->GetWidth(), pixel->GetHeight());
            Drawing::Rect dst(px, py, px + pixel->GetWidth(), py + pixel->GetHeight());
            canvas_->DrawPixelMapRect(pixel, src, dst, Drawing::SamplingOptions());
            return nullptr;
        }
        m_canvas->DrawImage(*image, px, py, Drawing::SamplingOptions());
    } else {
        JsSamplingOptions* jsSamplingOptions = nullptr;
        GET_UNWRAP_PARAM(ARGC_THREE, jsSamplingOptions);

        std::shared_ptr<SamplingOptions> samplingOptions = jsSamplingOptions->GetSamplingOptions();
        if (samplingOptions == nullptr) {
            ROSEN_LOGE("JsCanvas::OnDrawImage get samplingOptions is nullptr");
            return nullptr;
        }
        if (m_canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
            ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(m_canvas);
            Drawing::Rect src(0, 0, pixel->GetWidth(), pixel->GetHeight());
            Drawing::Rect dst(px, py, px + pixel->GetWidth(), py + pixel->GetHeight());
            canvas_->DrawPixelMapRect(pixel, src, dst, *samplingOptions.get());
            return nullptr;
        }
        DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
        m_canvas->DrawImage(*image, px, py, *samplingOptions.get());
    }

    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawColor(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawColor(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawColor(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawColor canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FIVE;
    napi_value argv[ARGC_FIVE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_FIVE);

    if (argc == ARGC_ONE || argc == ARGC_TWO) {
        ColorQuad color;
        if (!ConvertFromAdaptHexJsColor(env, argv[ARGC_ZERO], color)) {
            ROSEN_LOGE("JsCanvas::OnDrawColor Argv[0] is invalid");
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Parameter verification failed. The range of color channels must be [0, 255].");
        }
        if (argc == ARGC_ONE) {
            DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
            m_canvas->DrawColor(color);
        } else {
            int32_t jsMode = 0;
            GET_ENUM_PARAM(ARGC_ONE, jsMode, 0, static_cast<int32_t>(BlendMode::LUMINOSITY));
            DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
            m_canvas->DrawColor(color, static_cast<BlendMode>(jsMode));
        }
    } else if (argc == ARGC_FOUR || argc == ARGC_FIVE) {
        int32_t alpha = 0;
        GET_COLOR_PARAM(ARGC_ZERO, alpha);
        int32_t red = 0;
        GET_COLOR_PARAM(ARGC_ONE, red);
        int32_t green = 0;
        GET_COLOR_PARAM(ARGC_TWO, green);
        int32_t blue = 0;
        GET_COLOR_PARAM(ARGC_THREE, blue);
        auto color = Color::ColorQuadSetARGB(alpha, red, green, blue);
        if (argc == ARGC_FOUR) {
            DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
            m_canvas->DrawColor(color);
        } else {
            int32_t jsMode = 0;
            GET_ENUM_PARAM(ARGC_FOUR, jsMode, 0, static_cast<int32_t>(BlendMode::LUMINOSITY));
            DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
            m_canvas->DrawColor(color, static_cast<BlendMode>(jsMode));
        }
    } else {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawOval(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawOval(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawOval(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawOval canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    JS_CALL_DRAWING_FUNC(m_canvas->DrawOval(drawingRect));
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawPoint(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawPoint(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawPoint(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPoint canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, py);

    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    m_canvas->DrawPoint(Point(px, py));
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

static bool OnMakePoints(napi_env& env, Point* point, uint32_t size, napi_value& array)
{
    if (size > MAX_ELEMENTSIZE) {
        ROSEN_LOGE("JsTextBlob::OnMakePoints size exceeds the upper limit");
        return false;
    }
    for (uint32_t i = 0; i < size; i++) {
        napi_value tempNumber = nullptr;
        napi_get_element(env, array, i, &tempNumber);
        napi_value tempValue = nullptr;
        double pointX = 0.0;
        double pointY = 0.0;
        napi_get_named_property(env, tempNumber, "x", &tempValue);
        bool isPointXOk = ConvertFromJsValue(env, tempValue, pointX);
        napi_get_named_property(env, tempNumber, "y", &tempValue);
        bool isPointYOk = ConvertFromJsValue(env, tempValue, pointY);
        if (!(isPointXOk && isPointYOk)) {
            return false;
        }
        point[i] = Point(pointX, pointY);
    }
    return true;
}

napi_value JsCanvas::DrawPoints(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawPoints(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawPoints(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPoints canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_TWO);
    napi_value array = argv[ARGC_ZERO];
    uint32_t size = 0;
    if (napi_get_array_length(env, array, &size) != napi_ok || (size == 0)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect src array size.");
    }
    if (argc == ARGC_ONE) {
        Point* points = new(std::nothrow) Point[size];
        if (points == nullptr) {
            return nullptr;
        }
        if (!OnMakePoints(env, points, size, array)) {
            delete [] points;
            ROSEN_LOGE("JsCanvas::OnDrawPoints Argv[ARGC_ZERO] is invalid");
            return nullptr;
        }
        JS_CALL_DRAWING_FUNC(m_canvas->DrawPoints(PointMode::POINTS_POINTMODE, size, points));
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
        if (mPixelMap_ != nullptr) {
            mPixelMap_->MarkDirty();
        }
#endif
        delete [] points;
        return nullptr;
    }

    int32_t pointMode = 0;
    GET_ENUM_PARAM(ARGC_ONE, pointMode, 0, static_cast<int32_t>(PointMode::POLYGON_POINTMODE));
    Point* points = new(std::nothrow) Point[size];
    if (points == nullptr) {
        return nullptr;
    }
    if (!OnMakePoints(env, points, size, array)) {
        delete [] points;
        ROSEN_LOGE("JsCanvas::OnDrawPoints Argv[ARGC_ZERO] is invalid");
        return nullptr;
    }
    JS_CALL_DRAWING_FUNC(m_canvas->DrawPoints(static_cast<PointMode>(pointMode), size, points));
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    delete [] points;
    return nullptr;
}

napi_value JsCanvas::DrawPath(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawPath(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawPath(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPath canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsPath* jsPath = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsPath);

    if (jsPath->GetPath() == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPath path is nullptr");
        return nullptr;
    }

    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    m_canvas->DrawPath(*jsPath->GetPath());
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawLine(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawLine(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawLine(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawLine canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double startPx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, startPx);
    double startPy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, startPy);
    double endPx = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, endPx);
    double endPy = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, endPy);

    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    m_canvas->DrawLine(Point(startPx, startPy), Point(endPx, endPy));
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawText(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawText(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawText(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawText canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    JsTextBlob* jsTextBlob = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsTextBlob);
    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, y);

    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    m_canvas->DrawTextBlob(jsTextBlob->GetTextBlob().get(), x, y);
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawSingleCharacter(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawSingleCharacter(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawSingleCharacter(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawSingleCharacter canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);
    size_t len = 0;
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], nullptr, 0, &len) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }
    if (len == 0 || len > 4) { // 4 is the maximum length of a character encoded in UTF8.
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. Input parameter0 should be single character.");
    }
    char str[len + 1];
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], str, len + 1, &len) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }
    JsFont* jsFont = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, jsFont);
    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, y);
    std::shared_ptr<Font> font = jsFont->GetFont();
    if (font == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawSingleCharacter font is nullptr");
        return nullptr;
    }
    const char* currentStr = str;
    int32_t unicode = SkUTF::NextUTF8(&currentStr, currentStr + len);
    std::shared_ptr<Font> themeFont = MatchThemeFont(font, unicode);
    if (themeFont != nullptr) {
        font = themeFont;
    }
    size_t byteLen = currentStr - str;
    if (byteLen != len) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. Input parameter0 should be single character.");
    }
    m_canvas->DrawSingleCharacter(unicode, *font, x, y);
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawSingleCharacterWithFeatures(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawSingleCharacterWithFeatures(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawSingleCharacterWithFeatures(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FIVE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FIVE);

    size_t len = 0;
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], nullptr, 0, &len) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }
    if (len == 0 || len > 4) { // 4 is the maximum length of a character encoded in UTF8.
        return NapiThrowError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED,
            "Input parameter0 should be single character.");
    }
    std::vector<char> strBuffer(len + 1);
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], strBuffer.data(), len + 1, &len) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid parameter0 type.");
    }
    strBuffer[len] = 0;

    const char* currentPtr = strBuffer.data();
    const char* endPtr = strBuffer.data() + len;
    int32_t unicode = SkUTF::NextUTF8(&currentPtr, endPtr);
    size_t byteLen = currentPtr - strBuffer.data();
    if (byteLen != len) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED,
            "Parameter verification failed. Input parameter0 should be single character.");
    }

    JsFont* jsFont = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, jsFont);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, x);

    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, y);

    std::shared_ptr<Font> font = jsFont->GetFont();
    if (font == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawSingleCharacterWithFeatures font is nullptr");
        return nullptr;
    }

    std::shared_ptr<Font> themeFont = MatchThemeFont(font, unicode);
    if (themeFont != nullptr) {
        font = themeFont;
    }

    napi_value array = argv[ARGC_FOUR];
    uint32_t size = 0;
    if (napi_get_array_length(env, array, &size) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid src array");
    }

    std::shared_ptr<DrawingFontFeatures> featuresPtr = std::make_shared<DrawingFontFeatures>();
    MakeFontFeaturesFromJsArray(env, featuresPtr, size, array);
    m_canvas->DrawSingleCharacterWithFeatures(strBuffer.data(), *font, x, y, featuresPtr);
#ifdef ROSEN_OHOS
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawPixelMapMesh(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawPixelMapMesh(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawPixelMapMesh(napi_env env, napi_callback_info info)
{
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_SEVEN] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_SEVEN);

    PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi);

    if (pixelMapNapi->GetPixelNapiInner() == nullptr) {
        ROSEN_LOGE("Drawing_napi::pixelMap pixelmap getPixelNapiInner is nullptr");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = pixelMapNapi->GetPixelNapiInner();

    int32_t column = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_ONE, column);
    int32_t row = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_TWO, row);
    int32_t vertOffset = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_FOUR, vertOffset);
    int32_t colorOffset = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_SIX, colorOffset);

    if (column == 0 || row == 0) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh column or row is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid column or row params.");
    }

    napi_value verticesArray = argv[ARGC_THREE];
    uint32_t verticesSize = 0;
    napi_get_array_length(env, verticesArray, &verticesSize);
    int64_t tempVerticesSize = ((column + 1) * (row + 1) + vertOffset) * 2; // x and y two coordinates
    if (verticesSize != tempVerticesSize) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh vertices are invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter3 type.");
    }

    auto vertices = new (std::nothrow) float[verticesSize];
    if (!vertices) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh create array with size of vertices failed");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Size of vertices exceed memory limit.");
    }
    
    for (uint32_t i = 0; i < verticesSize; i++) {
        napi_value tempVertex = nullptr;
        napi_get_element(env, verticesArray, i, &tempVertex);
        double vertex = 0.f;
        if (napi_get_value_double(env, tempVertex, &vertex) != napi_ok) {
            ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh vertex is invalid");
            delete []vertices;
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect DrawPixelMapMesh parameter vertex type.");
        }
        vertices[i] = vertex;
    }
    float* verticesMesh = verticesSize ? (vertices + vertOffset * 2) : nullptr; // offset two coordinates

    napi_value colorsArray = argv[ARGC_FIVE];
    uint32_t colorsSize = 0;
    napi_get_array_length(env, colorsArray, &colorsSize);
    int64_t tempColorsSize = (column + 1) * (row + 1) + colorOffset;

    if (colorsSize != 0 && colorsSize != tempColorsSize) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh colors are invalid");
        delete []vertices;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter5 type.");
    }

    if (colorsSize == 0) {
        DrawingPixelMapMesh(pixelMap, column, row, verticesMesh, nullptr, m_canvas);
        if (mPixelMap_ != nullptr) {
            mPixelMap_->MarkDirty();
        }
        delete []vertices;
        return nullptr;
    }

    auto colors = new (std::nothrow) uint32_t[colorsSize];
    if (!colors) {
        ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh create array with size of colors failed");
        delete []vertices;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Size of colors exceed memory limit.");
    }
    for (uint32_t i = 0; i < colorsSize; i++) {
        napi_value tempColor = nullptr;
        napi_get_element(env, colorsArray, i, &tempColor);
        uint32_t color = 0;
        if (napi_get_value_uint32(env, tempColor, &color) != napi_ok) {
            ROSEN_LOGE("JsCanvas::OnDrawPixelMapMesh color is invalid");
            delete []vertices;
            delete []colors;
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect DrawPixelMapMesh parameter color type.");
        }
        colors[i] = color;
    }
    uint32_t* colorsMesh = colors + colorOffset;

    DrawingPixelMapMesh(pixelMap, column, row, verticesMesh, colorsMesh, m_canvas);
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
    delete []vertices;
    delete []colors;
    return nullptr;
#else
    return nullptr;
#endif
}

napi_value JsCanvas::DrawRegion(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawRegion(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawRegion(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawRegion canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsRegion* jsRegion = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsRegion);
    if (jsRegion->GetRegion() == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawRegion region is nullptr");
        return nullptr;
    }
    JS_CALL_DRAWING_FUNC(m_canvas->DrawRegion(*jsRegion->GetRegion()));
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawBackground(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawBackground(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawBackground(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawBackground canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsBrush* jsBrush = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsBrush);
    if (jsBrush->GetBrush() == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawBackground brush is nullptr");
        return nullptr;
    }

    m_canvas->DrawBackground(*jsBrush->GetBrush());
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawRoundRect(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawRoundRect(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawRoundRect(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawRoundRect canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsRoundRect* jsRoundRect = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsRoundRect);

    m_canvas->DrawRoundRect(jsRoundRect->GetRoundRect());
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawNestedRoundRect(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawNestedRoundRect(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawNestedRoundRect(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawNestedRoundRect canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    JsRoundRect* jsOuter = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsOuter);

    JsRoundRect* jsInner = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, jsInner);

    m_canvas->DrawNestedRoundRect(jsOuter->GetRoundRect(), jsInner->GetRoundRect());
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::GetTotalMatrix(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnGetTotalMatrix(env, info) : nullptr;
}

napi_value JsCanvas::OnGetTotalMatrix(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::GetTotalMatrix canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Matrix matrix = m_canvas->GetTotalMatrix();
    std::shared_ptr<Matrix> matrixPtr = std::make_shared<Matrix>(matrix);

    return JsMatrix::CreateJsMatrix(env, matrixPtr);
}

napi_value JsCanvas::AttachPen(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    Canvas* canvas = me->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachPen canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsPen* jsPen = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsPen);

    if (jsPen->GetPen() == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachPen pen is nullptr");
        return nullptr;
    }
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    canvas->AttachPen(*jsPen->GetPen());
    return nullptr;
}

napi_value JsCanvas::AttachBrush(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    Canvas* canvas = me->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachBrush canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsBrush* jsBrush = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsBrush);

    if (jsBrush->GetBrush() == nullptr) {
        ROSEN_LOGE("JsCanvas::AttachBrush brush is nullptr");
        return nullptr;
    }
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    canvas->AttachBrush(*jsBrush->GetBrush());
    return nullptr;
}

napi_value JsCanvas::DetachPen(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    Canvas* canvas = me->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::DetachPen canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    canvas->DetachPen();
    return nullptr;
}

napi_value JsCanvas::DetachBrush(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    if (me == nullptr) {
        return nullptr;
    }
    Canvas* canvas = me->GetCanvas();
    if (canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::DetachBrush canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    canvas->DetachBrush();
    return nullptr;
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

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double sx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, sx);
    double sy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, sy);

    m_canvas->Shear(sx, sy);
    return nullptr;
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

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double degree = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, degree);
    double sx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, sx);
    double sy = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, sy);

    m_canvas->Rotate(degree, sx, sy);
    return nullptr;
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
    return CreateJsNumber(env, m_canvas->GetSaveCount());
}

napi_value JsCanvas::GetWidth(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnGetWidth(env, info) : nullptr;
}

napi_value JsCanvas::OnGetWidth(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnGetWidth canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    return CreateJsNumber(env, m_canvas->GetWidth());
}

napi_value JsCanvas::GetHeight(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnGetHeight(env, info) : nullptr;
}

napi_value JsCanvas::OnGetHeight(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnGetHeight canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    return CreateJsNumber(env, m_canvas->GetHeight());
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
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_THREE);

    JsPath* jsPath = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsPath);

    Path* path = jsPath->GetPath();
    if (path == nullptr) {
        ROSEN_LOGE("JsCanvas::OnClipPath path is nullptr");
        return nullptr;
    }
    if (argc == ARGC_ONE) {
        m_canvas->ClipPath(*path);
        return nullptr;
    }

    int32_t jsClipOp = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_ONE, jsClipOp);

    if (argc == ARGC_TWO) {
        m_canvas->ClipPath(*path, static_cast<ClipOp>(jsClipOp));
        return nullptr;
    }

    bool jsDoAntiAlias = false;
    GET_BOOLEAN_PARAM(ARGC_TWO, jsDoAntiAlias);

    m_canvas->ClipPath(*path, static_cast<ClipOp>(jsClipOp), jsDoAntiAlias);
    return nullptr;
}

napi_value JsCanvas::ClipRegion(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnClipRegion(env, info) : nullptr;
}

napi_value JsCanvas::OnClipRegion(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnClipRegion m_canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_TWO);

    JsRegion* jsRegion = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsRegion);

    Region* region = jsRegion->GetRegion();
    if (region == nullptr) {
        ROSEN_LOGE("JsCanvas::OnClipRegion region is nullptr");
        return nullptr;
    }
    if (argc == ARGC_ONE) {
        m_canvas->ClipRegion(*region);
        return nullptr;
    }

    int32_t jsClipOp = 0;
    GET_ENUM_PARAM(ARGC_ONE, jsClipOp, 0, static_cast<int32_t>(ClipOp::INTERSECT));

    m_canvas->ClipRegion(*region, static_cast<ClipOp>(jsClipOp));
    return nullptr;
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

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);

    m_canvas->Translate(dx, dy);
    return nullptr;
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
    return CreateJsNumber(env, m_canvas->Save());
}

napi_value JsCanvas::SaveLayer(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnSaveLayer(env, info) : nullptr;
}

napi_value JsCanvas::OnSaveLayer(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnSaveLayer canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ZERO, ARGC_TWO);

    uint32_t ret = 0;
    if (argc == ARGC_ZERO) {
        ret = m_canvas->GetSaveCount();
        m_canvas->SaveLayer(SaveLayerOps());
        return CreateJsNumber(env, ret);
    }

    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, argv[ARGC_ZERO], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect OnSaveLayer parameter0 type.");
    }
    Drawing::Rect* drawingRectPtr = nullptr;
    double ltrb[ARGC_FOUR] = {0};
    if (valueType == napi_object) {
        if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
        }
        drawingRectPtr = new Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    }

    if (argc == ARGC_ONE) {
        ret = m_canvas->GetSaveCount();
        m_canvas->SaveLayer(SaveLayerOps(drawingRectPtr, nullptr));
        if (drawingRectPtr != nullptr) {
            delete drawingRectPtr;
        }
        return CreateJsNumber(env, ret);
    }

    if (napi_typeof(env, argv[ARGC_ONE], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        if (drawingRectPtr != nullptr) {
            delete drawingRectPtr;
        }
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect OnSaveLayer parameter1 type.");
    }
    Drawing::Brush* drawingBrushPtr = nullptr;
    if (valueType == napi_object) {
        JsBrush* jsBrush = nullptr;
        napi_status status = napi_unwrap(env, argv[ARGC_ONE], reinterpret_cast<void**>(&jsBrush));
        if (status != napi_ok || jsBrush == nullptr) {
            if (drawingRectPtr != nullptr) {
                delete drawingRectPtr;
            }
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                std::string("Incorrect ") + __FUNCTION__ + " parameter" + std::to_string(ARGC_ONE) + " type.");
        }
        drawingBrushPtr = jsBrush->GetBrush();
    }
    ret = m_canvas->GetSaveCount();
    SaveLayerOps saveLayerOps = SaveLayerOps(drawingRectPtr, drawingBrushPtr);
    m_canvas->SaveLayer(saveLayerOps);
    if (drawingRectPtr != nullptr) {
        delete drawingRectPtr;
    }
    return CreateJsNumber(env, ret);
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

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t count = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_ZERO, count);

    m_canvas->RestoreToCount(count);
    return nullptr;
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
    return nullptr;
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
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_THREE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    if (argc == ARGC_ONE) {
        m_canvas->ClipRect(drawingRect);
        return nullptr;
    }

    int32_t clipOpInt = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_ONE, clipOpInt);

    if (argc == ARGC_TWO) {
        m_canvas->ClipRect(drawingRect, static_cast<ClipOp>(clipOpInt));
        return nullptr;
    }

    bool doAntiAlias = false;
    GET_BOOLEAN_PARAM(ARGC_TWO, doAntiAlias);

    m_canvas->ClipRect(drawingRect, static_cast<ClipOp>(clipOpInt), doAntiAlias);
    return nullptr;
}

napi_value JsCanvas::ClipRoundRect(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnClipRoundRect(env, info) : nullptr;
}

napi_value JsCanvas::OnClipRoundRect(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnClipRoundRect m_canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_THREE);

    JsRoundRect* jsRoundRect = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsRoundRect);
    if (jsRoundRect == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawRegion jsRoundRect is nullptr");
        return nullptr;
    }

    if (argc == ARGC_ONE) {
        m_canvas->ClipRoundRect(jsRoundRect->GetRoundRect());
        return nullptr;
    }

    int32_t clipOpInt = 0;
    GET_ENUM_PARAM(ARGC_ONE, clipOpInt, 0, static_cast<int32_t>(ClipOp::INTERSECT));

    if (argc == ARGC_TWO) {
        m_canvas->ClipRoundRect(jsRoundRect->GetRoundRect(), static_cast<ClipOp>(clipOpInt));
        return nullptr;
    }

    bool doAntiAlias = false;
    GET_BOOLEAN_PARAM(ARGC_TWO, doAntiAlias);

    m_canvas->ClipRoundRect(jsRoundRect->GetRoundRect(), static_cast<ClipOp>(clipOpInt), doAntiAlias);
    return nullptr;
}

napi_value JsCanvas::SetMatrix(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnSetMatrix(env, info) : nullptr;
}

napi_value JsCanvas::OnSetMatrix(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnSetMatrix canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsMatrix* jsMatrix = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsMatrix);

    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("JsCanvas::OnSetMatrix matrix is nullptr");
        return nullptr;
    }

    JS_CALL_DRAWING_FUNC(m_canvas->SetMatrix(*jsMatrix->GetMatrix()));
    return nullptr;
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

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double sx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, sx);
    double sy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, sy);

    m_canvas->Scale(sx, sy);
    return nullptr;
}

napi_value JsCanvas::ConcatMatrix(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnConcatMatrix(env, info) : nullptr;
}

napi_value JsCanvas::OnConcatMatrix(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnConcatMatrix canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsMatrix* jsMatrix = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsMatrix);

    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("JsCanvas::OnConcatMatrix matrix is nullptr");
        return nullptr;
    }

    JS_CALL_DRAWING_FUNC(m_canvas->ConcatMatrix(*jsMatrix->GetMatrix()));
    return nullptr;
}

napi_value JsCanvas::IsClipEmpty(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnIsClipEmpty(env, info) : nullptr;
}

napi_value JsCanvas::OnIsClipEmpty(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnIsClipEmpty canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsValue(env, m_canvas->IsClipEmpty());
}

napi_value JsCanvas::GetLocalClipBounds(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnGetLocalClipBounds(env, info) : nullptr;
}

napi_value JsCanvas::OnGetLocalClipBounds(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnGetLocalClipBounds canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    return GetRectAndConvertToJsValue(env, m_canvas->GetLocalClipBounds());
}

Canvas* JsCanvas::GetCanvas()
{
    return m_canvas;
}

void JsCanvas::ResetCanvas()
{
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

napi_value JsCanvas::DrawImageNine(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawImageNine(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawImageNine(napi_env env, napi_callback_info info)
{
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImageNine canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi); // arg #0: pixelmap/image
    auto pixel = pixelMapNapi->GetPixelNapiInner();
    if (pixel == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImageNine pixelmap GetPixelNapiInner is nullptr");
        return nullptr;
    }

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb, ARGC_FOUR)) { // arg #1: centerRect
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect rect src parameter type. The type of left, top, right and bottom must be number.");
    }
    Drawing::RectI centerRectI = Drawing::RectI(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    if (!ConvertFromJsRect(env, argv[ARGC_TWO], ltrb, ARGC_FOUR)) { // arg #2: dstRect
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect rect dst parameter type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect dstRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    int32_t filterMode = 0;
    GET_ENUM_PARAM(ARGC_THREE, filterMode, 0, static_cast<int32_t>(FilterMode::LINEAR));

    if (m_canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(m_canvas);
        canvas_->DrawPixelMapNine(pixel, centerRectI, dstRect, static_cast<FilterMode>(filterMode));
        return nullptr;
    }
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixel);
    JS_CALL_DRAWING_FUNC(m_canvas->DrawImageNine(image.get(),
        centerRectI, dstRect, static_cast<FilterMode>(filterMode), nullptr));
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawImageLattice(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawImageLattice(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawImageLattice(napi_env env, napi_callback_info info)
{
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImageLattice canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi); // arg #0: pixelmap/image
    auto pixel = pixelMapNapi->GetPixelNapiInner();
    if (pixel == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImageLattice pixelmap GetPixelNapiInner is nullptr");
        return nullptr;
    }

    JsLattice* jsLattice = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, jsLattice);
    std::shared_ptr<Lattice> lattice = jsLattice->GetLattice();
    if (lattice == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImageLattice lattice is nullptr");
        return nullptr;
    }

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_TWO], ltrb, ARGC_FOUR)) { // arg #2: dstRect
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect rect dst parameter type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect dstRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    int32_t filterMode = 0;
    GET_ENUM_PARAM(ARGC_THREE, filterMode, 0, static_cast<int32_t>(FilterMode::LINEAR));

    if (m_canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(m_canvas);
        canvas_->DrawPixelMapLattice(pixel, *lattice, dstRect, static_cast<FilterMode>(filterMode));
        return nullptr;
    }
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixel);
    JS_CALL_DRAWING_FUNC(m_canvas->DrawImageLattice(image.get(),
        *lattice, dstRect, static_cast<FilterMode>(filterMode)));
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawImageRect(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawImageRect(env, info) : nullptr;
}

napi_value JsCanvas::OnDrawImageRect(napi_env env, napi_callback_info info)
{
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImageRect canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_TWO, ARGC_THREE);

    PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi); // arg #0: pixelmap/image
    auto pixel = pixelMapNapi->GetPixelNapiInner();
    if (pixel == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImageRect pixelmap GetPixelNapiInner is nullptr");
        return nullptr;
    }

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb, ARGC_FOUR)) { // arg #1: dstRect
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect rect dst parameter type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect dstRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    if (argc == ARGC_TWO) { // without (optional) arg #2: samplingOptions
        DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
        if (m_canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
            ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(m_canvas);
            Drawing::Rect srcRect(0, 0, pixel->GetWidth(), pixel->GetHeight());
            canvas_->DrawPixelMapRect(pixel, srcRect, dstRect, Drawing::SamplingOptions(),
                SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
            return nullptr;
        }
        std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixel);
        if (image == nullptr) {
            ROSEN_LOGE("JsCanvas::OnDrawImageRect image is nullptr");
            return nullptr;
        }
        m_canvas->DrawImageRect(*image, dstRect, Drawing::SamplingOptions());
    } else {
        JsSamplingOptions* jsSamplingOptions = nullptr;
        GET_UNWRAP_PARAM(ARGC_TWO, jsSamplingOptions); // (optional) arg #2: samplingOptions
        std::shared_ptr<SamplingOptions> samplingOptions = jsSamplingOptions->GetSamplingOptions();
        if (samplingOptions == nullptr) {
            ROSEN_LOGE("JsCanvas::OnDrawImageRect get samplingOptions is nullptr");
            return nullptr;
        }
        if (m_canvas->GetDrawingType() == Drawing::DrawingType::RECORDING) {
            ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(m_canvas);
            Drawing::Rect srcRect(0, 0, pixel->GetWidth(), pixel->GetHeight());
            canvas_->DrawPixelMapRect(pixel, srcRect, dstRect, *samplingOptions.get(),
                SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
            return nullptr;
        }
        DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
        std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixel);
        if (image == nullptr) {
            ROSEN_LOGE("JsCanvas::OnDrawImageRect image is nullptr");
            return nullptr;
        }
        m_canvas->DrawImageRect(*image, dstRect, *samplingOptions.get());
    }
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
#endif
    return nullptr;
}

napi_value JsCanvas::DrawImageRectWithSrc(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnDrawImageRectWithSrc(env, info) : nullptr;
}

#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
static napi_value OnDrawingImageRectWithSrc(napi_env env, napi_value* argv, size_t argc, Canvas& canvas,
                                            const std::shared_ptr<Media::PixelMap> pixel,
                                            const Rect& srcRect, const Rect& dstRect)
{
    if (argc == ARGC_THREE) { // without optional arg #3 (samplingOptions) and arg #4 (constraint):
        DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
        if (canvas.GetDrawingType() == Drawing::DrawingType::RECORDING) {
            ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(&canvas);
            canvas_->DrawPixelMapRect(pixel, srcRect, dstRect, Drawing::SamplingOptions());
            return nullptr;
        }
        std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixel);
        if (image == nullptr) {
            ROSEN_LOGE("JsCanvas::OnDrawImageRectWithSrc image is nullptr");
            return nullptr;
        }
        canvas.DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions());
    } else if (argc == ARGC_FOUR) { // without optional arg #4 (constraint):
        JsSamplingOptions* jsSamplingOptions = nullptr;
        GET_UNWRAP_PARAM(ARGC_THREE, jsSamplingOptions);
        std::shared_ptr<SamplingOptions> samplingOptions = jsSamplingOptions->GetSamplingOptions();
        if (samplingOptions == nullptr) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect samplingOptions parameter.");
        }
        if (canvas.GetDrawingType() == Drawing::DrawingType::RECORDING) {
            ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(&canvas);
            canvas_->DrawPixelMapRect(pixel, srcRect, dstRect, *samplingOptions.get());
            return nullptr;
        }
        DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
        std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixel);
        if (image == nullptr) {
            ROSEN_LOGE("JsCanvas::OnDrawImageRectWithSrc image is nullptr");
            return nullptr;
        }
        canvas.DrawImageRect(*image, srcRect, dstRect, *samplingOptions.get());
    } else if (argc == ARGC_FIVE) {  // with optional arg #3 (samplingOptions) and arg #4 (constraint):
        JsSamplingOptions* jsSamplingOptions = nullptr;
        GET_UNWRAP_PARAM(ARGC_THREE, jsSamplingOptions);
        std::shared_ptr<SamplingOptions> samplingOptions = jsSamplingOptions->GetSamplingOptions();
        if (samplingOptions == nullptr) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect samplingOptions parameter.");
        }
        int32_t constraint = 0;
        GET_ENUM_PARAM(ARGC_FOUR,
            constraint,
            static_cast<int32_t>(SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT),
            static_cast<int32_t>(SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT));
        if (canvas.GetDrawingType() == Drawing::DrawingType::RECORDING) {
            ExtendRecordingCanvas* canvas_ = reinterpret_cast<ExtendRecordingCanvas*>(&canvas);
            canvas_->DrawPixelMapRect(pixel, srcRect, dstRect,
                *samplingOptions.get(), static_cast<SrcRectConstraint>(constraint));
            return nullptr;
        }
        DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
        std::shared_ptr<Drawing::Image> image = ExtractDrawingImage(pixel);
        if (image == nullptr) {
            ROSEN_LOGE("JsCanvas::OnDrawImageRectWithSrc image is nullptr");
            return nullptr;
        }
        canvas.DrawImageRect(*image, srcRect, dstRect,
            *samplingOptions.get(), static_cast<SrcRectConstraint>(constraint));
    } else { // argc > 5:
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "More than 5 parameters are not supported");
    }
    return nullptr;
}
#endif

napi_value JsCanvas::OnDrawImageRectWithSrc(napi_env env, napi_callback_info info)
{
#if defined(ROSEN_OHOS) || defined(ROSEN_ARKUI_X)
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImageRectWithSrc canvas is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FIVE;
    napi_value argv[ARGC_FIVE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_THREE, ARGC_FIVE);

    PixelMapNapi* pixelMapNapi = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, pixelMapNapi); // arg #0: pixelmap/image
    auto pixel = pixelMapNapi->GetPixelNapiInner();
    if (pixel == nullptr) {
        ROSEN_LOGE("JsCanvas::OnDrawImageRectWithSrc pixelmap GetPixelNapiInner is nullptr");
        return nullptr;
    }

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb, ARGC_FOUR)) { // arg #1: srcRect
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect rect src parameter type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect srcRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    if (!ConvertFromJsRect(env, argv[ARGC_TWO], ltrb, ARGC_FOUR)) { // arg #2: dstRect
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect rect dst parameter type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect dstRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    napi_value result = OnDrawingImageRectWithSrc(env, argv, argc, *m_canvas, pixel, srcRect, dstRect);
    if (mPixelMap_ != nullptr) {
        mPixelMap_->MarkDirty();
    }
    return result;
#else
    return nullptr;
#endif
}

napi_value JsCanvas::ResetMatrix(napi_env env, napi_callback_info info)
{
    JsCanvas* me = CheckParamsAndGetThis<JsCanvas>(env, info);
    return (me != nullptr) ? me->OnResetMatrix(env, info) : nullptr;
}

napi_value JsCanvas::OnResetMatrix(napi_env env, napi_callback_info info)
{
    if (m_canvas == nullptr) {
        ROSEN_LOGE("JsCanvas::OnResetMatrix m_canvas is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    m_canvas->ResetMatrix();
    return nullptr;
}

} // namespace Drawing
} // namespace OHOS::Rosen
