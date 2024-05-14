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

#include "js_common.h"
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

    JsTextBlob *jsTextBlob = new(std::nothrow) JsTextBlob(env, drawingTextBlob);

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
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);
    CHECK_EACH_PARAM(ARGC_ONE, napi_object);

    napi_value array = argv[0];
    uint32_t size = 0;
    napi_get_array_length(env, array, &size);

    void* pointerResult = nullptr;
    napi_unwrap(env, argv[1], &pointerResult);
    auto jsFont = static_cast<JsFont*>(pointerResult);
    if (jsFont == nullptr) {
        ROSEN_LOGE("JsTextBlob::MakeFromRunBuffer jsFont is nullptr");
        return nullptr;
    }
    std::shared_ptr<Font> font = jsFont->GetFont();
    if (font == nullptr) {
        ROSEN_LOGE("JsTextBlob::MakeFromRunBuffer font is nullptr");
        return nullptr;
    }

    TextBlobBuilder::RunBuffer runBuffer;
    std::shared_ptr<TextBlobBuilder> textBlobBuilder = std::make_shared<TextBlobBuilder>();
    if (argc == ARGC_TWO) {
        runBuffer = textBlobBuilder->AllocRunPos(*font, size);
    } else {
        CHECK_EACH_PARAM(ARGC_TWO, napi_object);
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
    CHECK_EACH_PARAM(ARGC_ZERO, napi_string);
    CHECK_EACH_PARAM(ARGC_ONE, napi_object);

    void* pointerResult = nullptr;
    napi_unwrap(env, argv[1], &pointerResult);
    auto jsFont = static_cast<JsFont*>(pointerResult);
    if (jsFont == nullptr) {
        ROSEN_LOGE("JsTextBlob::MakeFromString jsFont is nullptr");
        return nullptr;
    }
    std::shared_ptr<Font> font = jsFont->GetFont();
    if (font == nullptr) {
        ROSEN_LOGE("JsTextBlob::MakeFromString font is nullptr");
        return nullptr;
    }

    // Chinese characters need to be encoded with UTF16
    size_t len = 0;
    if (napi_get_value_string_utf16(env, argv[0], nullptr, 0, &len) != napi_ok) {
        return nullptr;
    }
    char16_t buffer[len + 1];
    if (napi_get_value_string_utf16(env, argv[0], buffer, len + 1, &len) != napi_ok) {
        return nullptr;
    }
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromText(buffer, CHAR16_SIZE * len, *font, TextEncoding::UTF16);

    if (textBlob == nullptr) {
        ROSEN_LOGE("JsTextBlob::MakeFromString textBlob is nullptr");
        return nullptr;
    }
    napi_value jsTextBlob = JsTextBlob::CreateJsTextBlob(env, textBlob);
    if (jsTextBlob == nullptr) {
        ROSEN_LOGE("JsTextBlob::MakeFromString jsTextBlob is nullptr");
        return nullptr;
    }
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
    return GetRectAndConvertToJsValue(env, rect);
}

std::shared_ptr<TextBlob> JsTextBlob::GetTextBlob()
{
    return m_textBlob;
}
} // namespace Drawing
} // namespace OHOS::Rosen
