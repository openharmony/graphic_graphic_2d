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
#include "../js_drawing_utils.h"
#include "native_value.h"
#include "../font_napi/js_font.h"
#include "../js_common.h"

namespace OHOS::Rosen {
namespace Drawing {
const std::string CLASS_NAME = "TextBlob";
thread_local napi_ref JsTextBlob::constructor_ = nullptr;
std::shared_ptr<TextBlob> drawingTextBlob;
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
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO || argc > ARGC_THREE) {
        ROSEN_LOGE("[NAPI]Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_value array = argv[0];
    uint32_t size = 0;
    napi_get_array_length(env, array, &size);

    void* pointerResult = nullptr;
    napi_unwrap(env, argv[1], &pointerResult);
    auto jsFont = static_cast<JsFont*>(pointerResult);
    if (jsFont == nullptr) {
        ROSEN_LOGE("[NAPI]Failed to get font from js object");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }
    std::shared_ptr<Font> fontPtr = jsFont->GetFont();
    if (fontPtr == nullptr) {
        ROSEN_LOGE("[NAPI]fontPtr is nullptr");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    TextBlobBuilder::RunBuffer runBuffer;
    std::shared_ptr<TextBlobBuilder> textBlobBuilder = std::make_shared<TextBlobBuilder>();
    if (argc == ARGC_TWO) {
        runBuffer = textBlobBuilder->AllocRunPos(*fontPtr, size);
    } else {
        Rect drawingRect;
        napi_valuetype isRectNullptr;
        OnMakeDrawingRect(env, argv[ARGC_TWO], drawingRect, isRectNullptr);
        runBuffer = textBlobBuilder->AllocRunPos(*fontPtr, size, isRectNullptr == napi_null ? nullptr : &drawingRect);
    }
    OnMakeRunBuffer(env, runBuffer, size, array);

    std::shared_ptr<TextBlob> textBlob = textBlobBuilder->Make();
    if (textBlob == nullptr) {
        ROSEN_LOGE("MakeFromRunBuffer failed");
        return nullptr;
    }
    napi_value jsTextBlobObj = JsTextBlob::CreateJsTextBlob(env, textBlob);
    if (jsTextBlobObj == nullptr) {
        ROSEN_LOGE("jsTextBlobObj is null");
        return nullptr;
    }
    return jsTextBlobObj;
}

void JsTextBlob::OnMakeDrawingRect(napi_env& env, napi_value& argv, Rect& drawingRect, napi_valuetype& isRectNullptr)
{
    napi_typeof(env, argv, &isRectNullptr);
    if (isRectNullptr != napi_null) {
        napi_value tempValue = nullptr;
        double left = 0.0f;
        double top = 0.0f;
        double right = 0.0f;
        double bottom = 0.0f;
        napi_get_named_property(env, argv, "left", &tempValue);
        napi_get_value_double(env, tempValue, &left);
        napi_get_named_property(env, argv, "right", &tempValue);
        napi_get_value_double(env, tempValue, &right);
        napi_get_named_property(env, argv, "top", &tempValue);
        napi_get_value_double(env, tempValue, &top);
        napi_get_named_property(env, argv, "bottom", &tempValue);
        napi_get_value_double(env, tempValue, &bottom);
        drawingRect.SetLeft(left);
        drawingRect.SetRight(right);
        drawingRect.SetTop(top);
        drawingRect.SetBottom(bottom);
    }
}

void JsTextBlob::OnMakeRunBuffer(napi_env& env, TextBlobBuilder::RunBuffer& runBuffer, uint32_t size, napi_value& array)
{
    for (uint32_t i = 0; i < size; i++) {
        napi_value tempRunBuffer = nullptr;
        napi_get_element(env, array, i, &tempRunBuffer);
        napi_value tempValue = nullptr;
        uint32_t glyph = 0;
        double positionX = 0.0f;
        double positionY = 0.0f;
        napi_get_named_property(env, tempRunBuffer, "glyph", &tempValue);
        napi_get_value_uint32(env, tempValue, &glyph);
        napi_get_named_property(env, tempRunBuffer, "positionX", &tempValue);
        napi_get_value_double(env, tempValue, &positionX);
        napi_get_named_property(env, tempRunBuffer, "positionY", &tempValue);
        napi_get_value_double(env, tempValue, &positionY);
        runBuffer.glyphs[i] = (uint16_t)glyph;
        runBuffer.pos[2 * i] = positionX; // 2: double
        runBuffer.pos[2 * i + 1] = positionY; // 2: double
    }
}

napi_value JsTextBlob::MakeFromString(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO || argc > ARGC_THREE) {
        ROSEN_LOGE("[NAPI]Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    std::string text = "";
    ConvertFromJsValue(env, argv[0], text);

    void* pointerResult = nullptr;
    napi_unwrap(env, argv[1], &pointerResult);
    auto jsFont = static_cast<JsFont*>(pointerResult);
    if (jsFont == nullptr) {
        ROSEN_LOGE("[NAPI]Failed to get font from js object");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }
    std::shared_ptr<Font> fontPtr = jsFont->GetFont();
    if (fontPtr == nullptr) {
        ROSEN_LOGE("[NAPI]fontPtr is nullptr");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    std::shared_ptr<TextBlob> textBlob;
    if (argc == ARGC_TWO) {
        textBlob = TextBlob::MakeFromString(text.c_str(), *fontPtr);
    } else {
        TextEncoding TextEncoding = TextEncoding::UTF8;
        ConvertFromJsTextEncoding(env, TextEncoding, argv[ARGC_TWO]);
        textBlob = TextBlob::MakeFromString(text.c_str(), *fontPtr, TextEncoding);
    }

    if (textBlob == nullptr) {
        ROSEN_LOGE("ConvertFromJsTextEncoding failed");
        return nullptr;
    }
    napi_value jsTextBlobObj = JsTextBlob::CreateJsTextBlob(env, textBlob);
    if (jsTextBlobObj == nullptr) {
        ROSEN_LOGE("jsTextBlobObj is null");
        return nullptr;
    }
    return jsTextBlobObj;
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
            ROSEN_LOGE("Drawing_napi: New instance could not be obtained");
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
        ROSEN_LOGE("[NAPI]textBlob is null!");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    std::shared_ptr<Rect> rect = m_textBlob->Bounds();

    if (!rect) {
        ROSEN_LOGE("[NAPI]textBlob->Bounds failed ");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }
    return GetRectAndConvertToJsValue(env, rect);
}

std::shared_ptr<TextBlob> JsTextBlob::GetTextBlob()
{
    return m_textBlob;
}
} // namespace Drawing
} // namespace OHOS::Rosen
