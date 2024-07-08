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

#include "js_lattice.h"
#include "js_drawing_utils.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {
const std::string CLASS_NAME = "Lattice";
thread_local napi_ref JsLattice::constructor_ = nullptr;
napi_value JsLattice::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createImageLattice", JsLattice::CreateImageLattice),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsLattice::Init failed to define lattice class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsLattice::Init failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsLattice::Init failed to set constructor");
        return nullptr;
    }

    status = napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties);
    if (status != napi_ok) {
        ROSEN_LOGE("JsLattice::Init failed to define static function");
        return nullptr;
    }
    return exportObj;
}

void JsLattice::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsLattice>(static_cast<JsLattice*>(data));
}

JsLattice::~JsLattice()
{
    if (m_lattice != nullptr) {
        if (m_lattice->fXDivs != nullptr) {
            delete[] m_lattice->fXDivs;
        }
        if (m_lattice->fXDivs != nullptr) {
            delete[] m_lattice->fYDivs;
        }
        if (m_lattice->fRectTypes != nullptr) {
            delete[] m_lattice->fRectTypes;
        }
        if (m_lattice->fColors != nullptr) {
            delete[] m_lattice->fColors;
        }
        m_lattice = nullptr;
    }
}

napi_value JsLattice::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsLattice::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsLattice *jsLattice = new(std::nothrow) JsLattice();
    if (jsLattice == nullptr) {
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsLattice, JsLattice::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsLattice;
        ROSEN_LOGE("JsLattice::Constructor failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsLattice::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsLattice *napi = reinterpret_cast<JsLattice *>(nativeObject);
        delete napi;
    }
}

bool GetLatticeDividers(napi_env env, napi_value dividersArray, uint32_t count, int** dividers)
{
    uint32_t dividersSize = 0;
    napi_get_array_length(env, dividersArray, &dividersSize);
    if (dividersSize != count || dividersSize > 5) { // 5: max value
        ROSEN_LOGE("JsLattice::CreateImageLattice dividers are invalid");
        return false;
    }
    if (dividersSize != 0) {
        auto divs = new int[dividersSize];
        for (uint32_t i = 0; i < dividersSize; i++) {
            napi_value tempDiv = nullptr;
            napi_get_element(env, dividersArray, i, &tempDiv);
            int div = 0;
            if (napi_get_value_int32(env, tempDiv, &div) != napi_ok) {
                ROSEN_LOGE("JsLattice::CreateImageLattice divider is invalid");
                delete []divs;
                return false;
            }
            divs[i] = div;
        }
        *dividers = divs;
    }
    return true;
}

bool GetLatticeRectTypes(napi_env env, napi_value rectTypesArray, uint32_t count,
    Lattice::RectType** latticeRectTypes)
{
    uint32_t rectTypesSize = 0;
    napi_get_array_length(env, rectTypesArray, &rectTypesSize);
    if ((rectTypesSize != 0 && rectTypesSize != count) || rectTypesSize > 36) { // 36: max value
        ROSEN_LOGE("JsLattice::CreateImageLattice rectTypes are invalid");
        return false;
    }
    if (rectTypesSize != 0) {
        auto rectTypes = new Lattice::RectType[rectTypesSize];
        for (uint32_t i = 0; i < rectTypesSize; i++) {
            napi_value tempType = nullptr;
            napi_get_element(env, rectTypesArray, i, &tempType);
            int rectType = 0;
            if (napi_get_value_int32(env, tempType, &rectType) != napi_ok ||
                rectType < 0 || rectType > 2) { // 2: FIXEDCOLOR
                ROSEN_LOGE("JsLattice::CreateImageLattice rectType is invalid");
                delete []rectTypes;
                return false;
            }
            rectTypes[i] = static_cast<Lattice::RectType>(rectType);
        }
        *latticeRectTypes = rectTypes;
    }
    return true;
}

bool GetLatticeColors(napi_env env, napi_value colorsArray, uint32_t count, Color** latticeColors)
{
    uint32_t colorsSize = 0;
    napi_get_array_length(env, colorsArray, &colorsSize);
    if ((colorsSize != 0 && colorsSize != count) || colorsSize > 36) { // 36: max value
        ROSEN_LOGE("JsLattice::CreateImageLattice colors are invalid");
        return false;
    }
    if (colorsSize != 0) {
        auto colors = new Color[colorsSize];
        for (uint32_t i = 0; i < colorsSize; i++) {
            napi_value tempColor = nullptr;
            napi_get_element(env, colorsArray, i, &tempColor);
            Drawing::Color drawingColor;
            int32_t argb[ARGC_FOUR] = {0};
            if (!ConvertFromJsColor(env, tempColor, argb, ARGC_FOUR)) {
                ROSEN_LOGE("JsLattice::CreateImageLattice colors is invalid");
                delete []colors;
                return false;
            }
            drawingColor = Color::ColorQuadSetARGB(
                argb[ARGC_ZERO], argb[ARGC_ONE], argb[ARGC_TWO], argb[ARGC_THREE]);
            colors[i] = drawingColor;
        }
        *latticeColors = colors;
    }
    return true;
}

napi_value JsLattice::CreateImageLattice(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_SEVEN;
    napi_value argv[ARGC_SEVEN] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_FOUR, ARGC_SEVEN);

    uint32_t xCount = 0;
    GET_UINT32_PARAM(ARGC_TWO, xCount);
    uint32_t yCount = 0;
    GET_UINT32_PARAM(ARGC_THREE, yCount);

    int* xDividers = nullptr;
    if (!GetLatticeDividers(env, argv[ARGC_ZERO], xCount, &xDividers)) {
        ROSEN_LOGE("JsLattice::CreateImageLattice xDividers are invalid");
        delete xDividers;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
    }
    int* yDividers = nullptr;
    if (!GetLatticeDividers(env, argv[ARGC_ONE], yCount, &yDividers)) {
        ROSEN_LOGE("JsLattice::CreateImageLattice yDividers are invalid");
        delete xDividers;
        delete yDividers;
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter1 type.");
    }
    Lattice lat;
    lat.fXDivs = xDividers;
    lat.fYDivs = yDividers;
    lat.fXCount = xCount;
    lat.fYCount = yCount;
    lat.fBounds = nullptr;
    lat.fRectTypes = nullptr;
    lat.fColors = nullptr;

    if (argc >= ARGC_FIVE) {
        napi_valuetype valueType = napi_undefined;
        if (napi_typeof(env, argv[ARGC_FOUR], &valueType) != napi_ok ||
            (valueType != napi_null && valueType != napi_object)) {
            delete xDividers;
            delete yDividers;
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect CreateImageLattice parameter5 type.");
        }
        int32_t ltrb[ARGC_FOUR] = {0};
        if (valueType == napi_object) {
            if (!ConvertFromJsIRect(env, argv[ARGC_FOUR], ltrb, ARGC_FOUR)) {
                delete xDividers;
                delete yDividers;
                return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                    "Incorrect parameter5 type. The type of left, top, right and bottom must be number.");
            }
            lat.fBounds = new Drawing::RectI(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
        }
    }

    if (argc >= ARGC_SIX) {
        int count = (xCount + 1) * (yCount + 1); // 1: grid size need + 1
        Lattice::RectType* latticeRectTypes = nullptr;
        if (!GetLatticeRectTypes(env, argv[ARGC_FIVE], count, &latticeRectTypes)) {
            delete xDividers;
            delete yDividers;
            delete latticeRectTypes;
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter6 type.");
        }
        lat.fRectTypes = latticeRectTypes;

        if (argc == ARGC_SEVEN) {
            Color* latticeColors = nullptr;
            if (!GetLatticeColors(env, argv[ARGC_SIX], count, &latticeColors)) {
                delete xDividers;
                delete yDividers;
                delete latticeRectTypes;
                delete latticeColors;
                return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter7 type.");
            }
            lat.fColors = latticeColors;
        }
    }
    return JsLattice::Create(env, std::make_shared<Lattice>(lat));
}

napi_value JsLattice::Create(napi_env env, std::shared_ptr<Lattice> lattice)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || lattice == nullptr) {
        ROSEN_LOGE("JsLattice::Create object is null!");
        return nullptr;
    }

    std::unique_ptr<JsLattice> jsLattice = std::make_unique<JsLattice>(lattice);
    napi_wrap(env, objValue, jsLattice.release(), JsLattice::Finalizer, nullptr, nullptr);

    if (objValue == nullptr) {
        ROSEN_LOGE("JsLattice::Create object value is null!");
        return nullptr;
    }
    return objValue;
}

std::shared_ptr<Lattice> JsLattice::GetLattice()
{
    return m_lattice;
}
} // namespace Drawing
} // namespace OHOS::Rosen
