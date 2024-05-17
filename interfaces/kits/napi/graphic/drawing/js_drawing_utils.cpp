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

#include "js_drawing_utils.h"

#ifdef ROSEN_OHOS
#include <parameters.h>
#endif

#include "draw/color.h"

namespace OHOS::Rosen {

#ifdef ROSEN_OHOS
bool JsDrawingTestUtils::closeDrawingTest_ =
    std::atoi((OHOS::system::GetParameter("persist.sys.graphic.drawing.test", "0").c_str())) != 1;
#else
bool JsDrawingTestUtils::closeDrawingTest_ = true;
#endif
namespace Drawing {
void BindNativeFunction(napi_env env, napi_value object, const char* name, const char* moduleName, napi_callback func)
{
    std::string fullName;
    if (moduleName) {
        fullName = moduleName;
        fullName += '.';
    }
    fullName += name;
    napi_value funcValue = nullptr;
    napi_create_function(env, fullName.c_str(), fullName.size(), func, nullptr, &funcValue);
    napi_set_named_property(env, object, fullName.c_str(), funcValue);
}

napi_value CreateJsError(napi_env env, int32_t errCode, const std::string& message)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, errCode), CreateJsValue(env, message), &result);
    return result;
}

bool ConvertFromJsTextEncoding(napi_env env, TextEncoding& textEncoding, napi_value nativeType)
{
    napi_value type = nativeType;
    if (type == nullptr) {
        return false;
    }
    uint32_t resultValue = 0;
    napi_get_value_uint32(env, type, &resultValue);

    switch (resultValue) {
        case 0: // 0: TextEncoding::UTF8
            textEncoding = TextEncoding::UTF8;
            break;
        case 1: // 1: TextEncoding::UTF16
            textEncoding = TextEncoding::UTF16;
            break;
        case 2: // 2: TextEncoding::UTF32
            textEncoding = TextEncoding::UTF32;
            break;
        case 3: // 3: TextEncoding::GLYPH_ID
            textEncoding = TextEncoding::GLYPH_ID;
            break;
        default: // default: TextEncoding::UTF8
            textEncoding = TextEncoding::UTF8;
            break;
    }
    return true;
}

napi_value NapiThrowError(napi_env env, DrawingErrorCode err, const std::string& message)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(err), message));
    return nullptr;
}

static const char* ARGB_STRING[4] = {"alpha", "red", "green", "blue"};
static const char* LTRB_STRING[4] = {"left", "top", "right", "bottom"};

bool ConvertFromJsColor(napi_env env, napi_value jsValue, int32_t* argb, size_t size)
{
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        int32_t* curChannel = argb + idx;
        napi_get_named_property(env, jsValue, ARGB_STRING[idx], &tempValue);
        if (napi_get_value_int32(env, tempValue, curChannel) != napi_ok ||
            *curChannel < 0 || *curChannel > Color::RGB_MAX) {
            return false;
        }
    }
    return true;
}

bool ConvertFromJsRect(napi_env env, napi_value jsValue, double* ltrb, size_t size)
{
    napi_value tempValue = nullptr;
    for (size_t idx = 0; idx < size; idx++) {
        double* curEdge = ltrb + idx;
        napi_get_named_property(env, jsValue, LTRB_STRING[idx], &tempValue);
        if (napi_get_value_double(env, tempValue, curEdge) != napi_ok) {
            return false;
        }
    }
    return true;
}
} // namespace Drawing
} // namespace OHOS::Rosen
