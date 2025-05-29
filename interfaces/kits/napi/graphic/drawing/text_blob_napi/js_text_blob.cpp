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

#include "js_text_blob.h"

#include "native_value.h"

#include "js_drawing_utils.h"
#include "font_napi/js_font.h"

namespace OHOS::Rosen {
namespace Drawing {
const std::string CLASS_NAME = "TextBlob";
thread_local napi_ref JsTextBlob::constructor_ = nullptr;
std::shared_ptr<TextBlob> drawingTextBlob;
static constexpr size_t CHAR16_SIZE = 2;
napi_value JsTextBlob::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("bounds", JsTextBlob::Bounds),
        DECLARE_NAPI_STATIC_FUNCTION("makeFromString", JsTextBlob::MakeFromString),
        DECLARE_NAPI_STATIC_FUNCTION("makeFromRunBuffer", JsTextBlob::MakeFromRunBuffer),
        DECLARE_NAPI_STATIC_FUNCTION("makeFromPosText", JsTextBlob::MakeFromPosText),
        DECLARE_NAPI_FUNCTION("uniqueID", JsTextBlob::UniqueID),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to define TextBlob class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to set constructor");
        return nullptr;
    }

    status = napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties);
    if (status != napi_ok) {
        ROSEN_LOGE("Drawing_napi: Failed to define static function");
        return nullptr;
    }
    return exportObj;
}

JsTextBlob::~JsTextBlob()
{
    m_textBlob = nullptr;
}

napi_value JsTextBlob::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    JsTextBlob *jsTextBlob = new JsTextBlob(env, drawingTextBlob);

    status = napi_wrap(env, jsThis, jsTextBlob,
                       JsTextBlob::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsTextBlob;
        ROSEN_LOGE("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsTextBlob::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsTextBlob *napi = reinterpret_cast<JsTextBlob *>(nativeObject);
        delete napi;
    }
}

napi_value JsTextBlob::MakeFromRunBuffer(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_TWO, ARGC_THREE);

    napi_value array = argv[ARGC_ZERO];
    uint32_t size = 0;
    if (napi_get_array_length(env, array, &size) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }

    JsFont* jsFont = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, jsFont);

    std::shared_ptr<Font> font = jsFont->GetFont();
    if (font == nullptr) {
        ROSEN_LOGE("JsTextBlob::MakeFromRunBuffer font is nullptr");
        return nullptr;
    }
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont;
    }

    TextBlobBuilder::RunBuffer runBuffer;
    std::shared_ptr<TextBlobBuilder> textBlobBuilder = std::make_shared<TextBlobBuilder>();
    if (argc == ARGC_TWO) {
        runBuffer = textBlobBuilder->AllocRunPos(*font, size);
    } else {
        Rect drawingRect;
        napi_valuetype isRectNullptr;
        if (!OnMakeDrawingRect(env, argv[ARGC_TWO], drawingRect, isRectNullptr)) {
            ROSEN_LOGE("JsTextBlob::MakeFromRunBuffer Argv[2] is invalid");
            return nullptr;
        }
        runBuffer = textBlobBuilder->AllocRunPos(*font, size, isRectNullptr == napi_null ? nullptr : &drawingRect);
    }
    if (!OnMakeRunBuffer(env, runBuffer, size, array)) {
        ROSEN_LOGE("JsTextBlob::MakeFromRunBuffer Argv[0] is invalid");
        return nullptr;
    }

    std::shared_ptr<TextBlob> textBlob = textBlobBuilder->Make();
    if (textBlob == nullptr) {
        ROSEN_LOGE("JsTextBlob::MakeFromRunBuffer textBlob is nullptr");
        return nullptr;
    }
    return JsTextBlob::CreateJsTextBlob(env, textBlob);
}

bool JsTextBlob::OnMakeDrawingRect(napi_env& env, napi_value& argv, Rect& drawingRect, napi_valuetype& isRectNullptr)
{
    napi_typeof(env, argv, &isRectNullptr);
    if (isRectNullptr != napi_null) {
        double ltrb[ARGC_FOUR] = {0};
        if (!ConvertFromJsRect(env, argv, ltrb, ARGC_FOUR)) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect parameter2 type. The type of left, top, right and bottom must be number.");
        }

        drawingRect.SetLeft(ltrb[ARGC_ZERO]);
        drawingRect.SetTop(ltrb[ARGC_ONE]);
        drawingRect.SetRight(ltrb[ARGC_TWO]);
        drawingRect.SetBottom(ltrb[ARGC_THREE]);
    }
    return true;
}

bool JsTextBlob::OnMakeRunBuffer(napi_env& env, TextBlobBuilder::RunBuffer& runBuffer, uint32_t size, napi_value& array)
{
    if (size > MAX_ELEMENTSIZE) {
        ROSEN_LOGE("JsTextBlob::OnMakeRunBuffer size exceeds the upper limit");
        return false;
    }
    for (uint32_t i = 0; i < size; i++) {
        napi_value tempRunBuffer = nullptr;
        napi_get_element(env, array, i, &tempRunBuffer);
        napi_value tempValue = nullptr;
        uint32_t glyph = 0;
        double positionX = 0.0;
        double positionY = 0.0;
        napi_get_named_property(env, tempRunBuffer, "glyph", &tempValue);
        bool isGlyphOk = ConvertFromJsValue(env, tempValue, glyph);
        napi_get_named_property(env, tempRunBuffer, "positionX", &tempValue);
        bool isPositionXOk = ConvertFromJsValue(env, tempValue, positionX);
        napi_get_named_property(env, tempRunBuffer, "positionY", &tempValue);
        bool isPositionYOk = ConvertFromJsValue(env, tempValue, positionY);
        if (!(isGlyphOk && isPositionXOk && isPositionYOk)) {
            return false;
        }

        runBuffer.glyphs[i] = (uint16_t)glyph;
        runBuffer.pos[2 * i] = positionX; // 2: double
        runBuffer.pos[2 * i + 1] = positionY; // 2: double
    }
    return true;
}

napi_value JsTextBlob::MakeFromString(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_TWO, ARGC_THREE);

    JsFont* jsFont = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, jsFont);

    std::shared_ptr<Font> font = jsFont->GetFont();
    if (font == nullptr) {
        ROSEN_LOGE("JsTextBlob::MakeFromString font is nullptr");
        return nullptr;
    }
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont;
    }

    // Chinese characters need to be encoded with UTF16
    size_t len = 0;
    if (napi_get_value_string_utf16(env, argv[ARGC_ZERO], nullptr, 0, &len) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }
    char16_t* buffer = new(std::nothrow) char16_t[len + 1];
    if (!buffer) {
        ROSEN_LOGE("JsTextBlob::MakeFromString fail to create buffer");
        return nullptr;
    }
    if (napi_get_value_string_utf16(env, argv[ARGC_ZERO], buffer, len + 1, &len) != napi_ok) {
        delete[] buffer;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromText(buffer, CHAR16_SIZE * len, *font, TextEncoding::UTF16);

    if (textBlob == nullptr) {
        delete[] buffer;
        ROSEN_LOGE("JsTextBlob::MakeFromString textBlob is nullptr");
        return nullptr;
    }
    napi_value jsTextBlob = JsTextBlob::CreateJsTextBlob(env, textBlob);
    if (jsTextBlob == nullptr) {
        delete[] buffer;
        ROSEN_LOGE("JsTextBlob::MakeFromString jsTextBlob is nullptr");
        return nullptr;
    }
    delete[] buffer;
    return jsTextBlob;
}

napi_value JsTextBlob::UniqueID(napi_env env, napi_callback_info info)
{
    JsTextBlob* me = CheckParamsAndGetThis<JsTextBlob>(env, info);
    return (me != nullptr) ? me->OnUniqueID(env, info) : nullptr;
}

napi_value JsTextBlob::OnUniqueID(napi_env env, napi_callback_info info)
{
    if (m_textBlob == nullptr) {
        ROSEN_LOGE("JsTextBlob::OnUniqueID textBlob is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    return CreateJsNumber(env, m_textBlob->UniqueID());
}

static bool MakePoints(napi_env& env, Point* point, uint32_t size, napi_value& array)
{
    napi_status status = napi_invalid_arg;
    for (uint32_t i = 0; i < size; i++) {
        napi_value tempNumber = nullptr;
        status = napi_get_element(env, array, i, &tempNumber);
        if (status != napi_ok) {
            return false;
        }
        napi_value tempValue = nullptr;
        status = napi_get_named_property(env, tempNumber, "x", &tempValue);
        if (status != napi_ok) {
            return false;
        }
        double pointX = 0.0;
        const bool isPointXOk = ConvertFromJsValue(env, tempValue, pointX);
        status = napi_get_named_property(env, tempNumber, "y", &tempValue);
        if (status != napi_ok) {
            return false;
        }
        double pointY = 0.0;
        const bool isPointYOk = ConvertFromJsValue(env, tempValue, pointY);
        if (!(isPointXOk && isPointYOk)) {
            return false;
        }
        point[i] = Point(pointX, pointY);
    }
    return true;
}

static napi_value getJsTextBlob(const char* buffer, size_t bufferLen, const Point points[],
                                const std::shared_ptr<Font>& font, napi_env env)
{
    std::shared_ptr<TextBlob> textBlob =
        TextBlob::MakeFromPosText(buffer, bufferLen, points, *font, TextEncoding::UTF8);
    if (textBlob == nullptr) {
        ROSEN_LOGE("getJsTextBlob: textBlob is nullptr");
        return nullptr;
    }
    napi_value jsTextBlob = JsTextBlob::CreateJsTextBlob(env, textBlob);
    if (jsTextBlob == nullptr) {
        ROSEN_LOGE("getJsTextBlob: jsTextBlob is nullptr");
        return nullptr;
    }
    return jsTextBlob;
}

napi_value JsTextBlob::MakeFromPosText(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    uint32_t len = 0;
    GET_UINT32_PARAM(ARGC_ONE, len);

    size_t bufferLen = 0;
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], nullptr, 0, &bufferLen) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }

    char* buffer = new(std::nothrow) char[bufferLen + 1];
    if (!buffer) {
        ROSEN_LOGE("JsTextBlob::MakeFromPosText: failed to create buffer");
        return nullptr;
    }
    if (napi_get_value_string_utf8(env, argv[ARGC_ZERO], buffer, bufferLen + 1, &bufferLen) != napi_ok) {
        delete[] buffer;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect Argv[0] type.");
    }

    napi_value array = argv[ARGC_TWO];
    uint32_t pointsSize = 0;
    if (napi_get_array_length(env, array, &pointsSize) != napi_ok) {
        delete[] buffer;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect Argv[2].");
    }
    if (pointsSize == 0 || bufferLen == 0) {
        delete[] buffer;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Argv[0] is empty.");
    }
    if (len != pointsSize) {
        delete[] buffer;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "string length does not match points array length.");
    }

    JsFont* jsFont = nullptr;
    if ((napi_unwrap(env, argv[ARGC_THREE], reinterpret_cast<void**>(&jsFont)) != napi_ok) || jsFont == nullptr) {
        delete[] buffer;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            std::string("Incorrect ") + __FUNCTION__ + " parameter" + std::to_string(ARGC_THREE) + " type.");
    }
    std::shared_ptr<Font> font = jsFont->GetFont();
    if (font == nullptr) {
        delete[] buffer;
        ROSEN_LOGE("JsTextBlob::MakeFromPosText: font is nullptr");
        return nullptr;
    }
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont;
    }

    Point* points = new(std::nothrow) Point[pointsSize];
    if (!points) {
        delete[] buffer;
        ROSEN_LOGE("JsTextBlob::MakeFromPosText: failed to create Point");
        return nullptr;
    }
    if (!MakePoints(env, points, pointsSize, array)) {
        delete[] buffer;
        delete[] points;
        ROSEN_LOGE("JsTextBlob::MakeFromPosText: Argv[2] is invalid");
        return nullptr;
    }
    napi_value jsTextBlob = getJsTextBlob(buffer, bufferLen, points, font, env);
    delete[] buffer;
    delete[] points;
    return jsTextBlob;
}

napi_value JsTextBlob::CreateJsTextBlob(napi_env env, const std::shared_ptr<TextBlob> textBlob)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        drawingTextBlob = textBlob;
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        if (status == napi_ok) {
            return result;
        } else {
            ROSEN_LOGE("JsTextBlob::CreateJsTextBlob New instance could not be obtained");
        }
    }
    return result;
}

napi_value JsTextBlob::Bounds(napi_env env, napi_callback_info info)
{
    JsTextBlob* me = CheckParamsAndGetThis<JsTextBlob>(env, info);
    return (me != nullptr) ? me->OnBounds(env, info) : nullptr;
}

napi_value JsTextBlob::OnBounds(napi_env env, napi_callback_info info)
{
    if (m_textBlob == nullptr) {
        ROSEN_LOGE("JsTextBlob::OnBounds textBlob is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::shared_ptr<Rect> rect = m_textBlob->Bounds();

    if (!rect) {
        ROSEN_LOGE("JsTextBlob::OnBounds rect is nullptr");
        return nullptr;
    }
    return GetRectAndConvertToJsValue(env, *rect);
}

std::shared_ptr<TextBlob> JsTextBlob::GetTextBlob()
{
    return m_textBlob;
}
} // namespace Drawing
} // namespace OHOS::Rosen
