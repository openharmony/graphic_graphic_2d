/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include "common/rs_vector4.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "mask_napi.h"
#include "mask/include/pixel_map_mask_para.h"
#include "pixel_map.h"
#include "ui_effect_napi_utils.h"

#ifdef IMAGE_NAPI_ENABLE
#include "pixel_map_napi.h"
#endif

namespace OHOS {
namespace Rosen {

namespace {
bool ParsePixelMap(napi_env env, napi_value argv, std::shared_ptr<Media::PixelMap>& pixelMap)
{
#ifdef IMAGE_NAPI_ENABLE
    napi_value constructor = nullptr;
    napi_value global = nullptr;
    bool isInstance = false;
    napi_status ret = napi_invalid_arg;

    napi_get_global(env, &global);

    ret = napi_get_named_property(env, global, "PixelMap", &constructor);
    if (ret != napi_ok) {
        MASK_LOG_E("Get PixelMapNapi property failed!");
        return false;
    }

    napi_valuetype res = napi_undefined;
    ret = napi_typeof(env, argv, &res);
    if (ret == napi_ok && res == napi_object) {
        pixelMap = Media::PixelMapNapi::GetPixelMap(env, argv);
        return true;
    }

    ret = napi_instanceof(env, argv, constructor, &isInstance);
    if (ret == napi_ok && isInstance) {
        pixelMap = Media::PixelMapNapi::GetPixelMap(env, argv);
        return true;
    }

    MASK_LOG_E("InValued type! ret: %{public}d, isInstance: %{public}d, res: %{public}d", ret, isInstance, res);
#else
    MASK_LOG_E("ImageNapi disabled, parse pixel map failed");
#endif
    return false;
}
}

using namespace UIEffect;

static const std::string CLASS_NAME = "Mask";

MaskNapi::MaskNapi() {}

MaskNapi::~MaskNapi() {}

thread_local napi_ref MaskNapi::sConstructor_ = nullptr;

napi_value MaskNapi::Init(napi_env env, napi_value exports)
{
    RegisterMaskParaUnmarshallingCallback();
    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createRippleMask", CreateRippleMask),
        DECLARE_NAPI_STATIC_FUNCTION("createRadialGradientMask", CreateRadialGradientMask),
        DECLARE_NAPI_STATIC_FUNCTION("createPixelMapMask", CreatePixelMapMask),
        DECLARE_NAPI_STATIC_FUNCTION("createWaveGradientMask", CreateWaveGradientMask),
    };

    napi_value constructor = nullptr;

    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        UIEFFECT_ARRAY_SIZE(static_prop), static_prop, &constructor);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, MASK_LOG_E("MaskNapi Init define class fail"));

    status = napi_create_reference(env, constructor, 1, &sConstructor_);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, MASK_LOG_E("MaskNapi Init create reference fail"));

    napi_value global = nullptr;
    status = napi_get_global(env, &global);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, MASK_LOG_E("MaskNapi Init get global fail"));

    status = napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, MASK_LOG_E("MaskNapi Init set global named property fail"));

    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, MASK_LOG_E("MaskNapi Init set named property fail"));

    status = napi_define_properties(env, exports, UIEFFECT_ARRAY_SIZE(static_prop), static_prop);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, MASK_LOG_E("MaskNapi Init define properties fail"));

    return exports;
}

napi_value MaskNapi::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok, nullptr, MASK_LOG_E("MaskNapi Constructor parsing input fail"));

    MaskNapi *filterNapi = new(std::nothrow) MaskNapi();
    UIEFFECT_NAPI_CHECK_RET_D(filterNapi != nullptr, nullptr,
        MASK_LOG_E("MaskNapi Constructor filterNapi is nullptr"));
    status = napi_wrap(env, jsThis, filterNapi, MaskNapi::Destructor, nullptr, nullptr);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok, nullptr, filterNapi,
        MASK_LOG_E("MaskNapi Constructor wrap fail"));
    return jsThis;
}

void MaskNapi::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    MaskNapi *filterNapi = static_cast<MaskNapi*>(nativeObject);
    UIEFFECT_NAPI_CHECK_RET_VOID_D(filterNapi != nullptr, MASK_LOG_E("MaskNapi Destructor nativeObject is nullptr"));

    delete filterNapi;
    filterNapi = nullptr;
}

bool ParseRippleMask(
    napi_env env, napi_value* argv, const std::shared_ptr<RippleMaskPara>& rippleMask, const size_t& realArgc)
{
    if (!rippleMask || realArgc < NUM_3) {
        return false;
    }
    uint32_t parseTimes = 0;
    double point[NUM_2] = { 0 };
    if (ConvertFromJsPoint(env, argv[NUM_0], point, NUM_2)) {
        Vector2f center(static_cast<float>(point[NUM_0]), static_cast<float>(point[NUM_1]));
        rippleMask->SetCenter(center);
        parseTimes++;
    }
    double val = 0;
    if (ParseJsDoubleValue(env, argv[NUM_1], val)) {
        rippleMask->SetRadius(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, argv[NUM_2], val)) {
        rippleMask->SetWidth(static_cast<float>(val));
        parseTimes++;
    }
    if (realArgc == NUM_4 && ParseJsDoubleValue(env, argv[NUM_3], val)) {
        rippleMask->SetWidthCenterOffset(static_cast<float>(val));
        parseTimes++;
    }
    return (parseTimes == realArgc);
}

bool ParseWaveGradientMask(
    napi_env env, napi_value* argv, const std::shared_ptr<WaveGradientMaskPara>& waveGradientMask, size_t& realArgc)
{
    if (!waveGradientMask) {
        MASK_LOG_E("ParseWaveGradientMask: waveGradientMask is nullptr");
        return false;
    }
    int parseTimes = 0;
    double point[NUM_2] = { 0.0 };
    if (ConvertFromJsPoint(env, argv[NUM_0], point, NUM_2)) {
        Vector2f center(static_cast<float>(point[NUM_0]), static_cast<float>(point[NUM_1]));
        waveGradientMask->SetWaveCenter(center);
        parseTimes++;
    }
    double val = 0.0;
    if (ParseJsDoubleValue(env, argv[NUM_1], val)) {
        waveGradientMask->SetWaveWidth(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, argv[NUM_2], val)) {
        waveGradientMask->SetPropagationRadius(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, argv[NUM_3], val)) {
        waveGradientMask->SetBlurRadius(static_cast<float>(val));
        parseTimes++;
    }
    if (realArgc == NUM_5 && ParseJsDoubleValue(env, argv[NUM_4], val)) {
        waveGradientMask->SetTurbulenceStrength(static_cast<float>(val));
        parseTimes++;
    }

    MASK_LOG_I("ParseWaveGradientMask parseTimes: %{public}d, realArgc: %{public}zu", parseTimes, realArgc);
    return (parseTimes == realArgc);
}

bool ParseValueArray(napi_env env, napi_value valuesArray, std::vector<float>& colors, std::vector<float>& positions)
{
    bool isArray = false;
    napi_is_array(env, valuesArray, &isArray);
    if (!isArray) {
        return false;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, valuesArray, &length) != napi_ok) {
        return false;
    }
    colors.reserve(length);
    positions.reserve(length);
    double val = 0.0;
    for (uint32_t i = 0; i < length; i++) {
        napi_value tmpArray = nullptr;
        if (napi_get_element(env, valuesArray, i, &tmpArray) != napi_ok || !tmpArray) {
            return false;
        }
        uint32_t tmpLen = 0;
        if (napi_get_array_length(env, tmpArray, &tmpLen) != napi_ok || tmpLen != NUM_2) {
            return false;
        }

        if (!ConvertDoubleValueFromJsElement(env, tmpArray, NUM_0, val)) {
            return false;
        }
        colors.emplace_back(static_cast<float>(val));

        if (!ConvertDoubleValueFromJsElement(env, tmpArray, NUM_1, val)) {
            return false;
        }
        positions.emplace_back(static_cast<float>(val));
    }
    return true;
}

bool ParseRadialGradientMask(
    napi_env env, napi_value *argv, std::shared_ptr<RadialGradientMaskPara> mask, const size_t& realArgc)
{
    if (!mask || realArgc != NUM_4) {
        return false;
    }
    int parseTimes = 0;
    double point[NUM_2] = { 0 };
    if (ConvertFromJsPoint(env, argv[NUM_0], point, NUM_2)) {
        Vector2f center(static_cast<float>(point[NUM_0]), static_cast<float>(point[NUM_1]));
        mask->SetCenter(center);
        parseTimes++;
    }
    double val;
    if (ParseJsDoubleValue(env, argv[NUM_1], val)) {
        mask->SetRadiusX(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(env, argv[NUM_2], val)) {
        mask->SetRadiusY(static_cast<float>(val));
        parseTimes++;
    }
    std::vector<float> colors;
    std::vector<float> positions;
    if (ParseValueArray(env, argv[NUM_3], colors, positions)) {
        mask->SetColors(colors);
        mask->SetPositions(positions);
        parseTimes++;
    }
    return (parseTimes == NUM_4);
}

napi_value MaskNapi::Create(napi_env env, std::shared_ptr<MaskPara> maskPara)
{
    // Create New Object
    napi_value maskObj = nullptr;
    napi_status status = napi_create_object(env, &maskObj);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && maskObj != nullptr, nullptr,
        MASK_LOG_E("MaskNapi CreateMask fail, create new object fail."));
    // Create Mask and set para
    Mask* mask = new (std::nothrow) Mask();
    UIEFFECT_NAPI_CHECK_RET_D(mask != nullptr, nullptr,
        MASK_LOG_E("MaskNapi CreateMask fail, mask is nullptr."));
    mask->SetMaskPara(maskPara);
    // Wrap Object(js) with Mask(native)
    status = napi_wrap(env, maskObj, mask,
        [](napi_env env, void* data, void* hint) {
            Mask* maskObj = (Mask*)data;
            delete maskObj;
            maskObj = nullptr;
        },
        nullptr, nullptr);
    UIEFFECT_NAPI_CHECK_RET_DELETE_POINTER(status == napi_ok && maskObj != nullptr, nullptr, mask,
        MASK_LOG_E("MaskNapi CreateMask wrap fail"));
    return maskObj;
}

napi_value MaskNapi::CreateWaveGradientMask(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        MASK_LOG_E("MaskNapi CreateWaveGradientMask failed, is not system app");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "MaskNapi CreateWaveGradientMask failed, is not system app");
        return nullptr;
    }
    const size_t requireMinArgc = NUM_4;
    const size_t requireMaxArgc = NUM_5;
    size_t realArgc = NUM_5;
    napi_value argv[requireMaxArgc];
    napi_value thisVar = nullptr;
    napi_status status;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && requireMinArgc <= realArgc && realArgc <= requireMaxArgc, nullptr,
        MASK_LOG_E("MaskNapi CreateWaveGradientMask parsing input fail."));
    
    auto maskPara = std::make_shared<WaveGradientMaskPara>();
    UIEFFECT_NAPI_CHECK_RET_D(ParseWaveGradientMask(env, argv, maskPara, realArgc), nullptr,
        MASK_LOG_E("MaskNapi CreateWaveGradientMask parsing mask input fail."));
    MASK_LOG_I("MaskNapi CreateWaveGradientMask parsing mask success.");
    return Create(env, maskPara);
}

napi_value MaskNapi::CreateRippleMask(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        MASK_LOG_E("MaskNapi CreateRippleMask failed, is not system app");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "MaskNapi CreateRippleMask failed, is not system app");
        return nullptr;
    }
    const size_t requireMinArgc = NUM_3;
    const size_t requireMaxArgc = NUM_4;
    size_t realArgc = NUM_4;
    napi_value argv[requireMaxArgc];
    napi_value thisVar = nullptr;
    napi_status status;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && requireMinArgc <= realArgc && realArgc <= requireMaxArgc, nullptr,
        MASK_LOG_E("MaskNapi CreateRippleMask parsing input fail."));

    auto maskPara = std::make_shared<RippleMaskPara>();
    UIEFFECT_NAPI_CHECK_RET_D(ParseRippleMask(env, argv, maskPara, realArgc), nullptr,
        MASK_LOG_E("MaskNapi CreateRippleMask parsing mask input fail."));
    return Create(env, maskPara);
}

napi_value MaskNapi::CreateRadialGradientMask(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        MASK_LOG_E("MaskNapi CreateRadialGradientMask failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "MaskNapi CreateRadialGradientMask failed, is not system app");
        return nullptr;
    }
    const size_t requireArgc = NUM_4;
    size_t realArgc = NUM_4;
    napi_value argv[requireArgc];
    napi_value thisVar = nullptr;
    napi_status status;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && realArgc == requireArgc, nullptr,
        MASK_LOG_E("MaskNapi CreateRadialGradientMask parsing input fail."));
    
    auto maskPara = std::make_shared<RadialGradientMaskPara>();
    UIEFFECT_NAPI_CHECK_RET_D(ParseRadialGradientMask(env, argv, maskPara, realArgc), nullptr,
        MASK_LOG_E("MaskNapi CreateRadialGradientMask parsing mask input fail."));
    return Create(env, maskPara);
}

static void ClampVector4f(Vector4f& v, float minx, float maxn)
{
    for (auto& data : v.data_) {
        data = std::clamp(data, minx, maxn);
    }
}

static bool ParsePixelMapMask(napi_env env, napi_value* argv, size_t realArgc, std::shared_ptr<PixelMapMaskPara> para)
{
    if (!para) {
        return false;
    }

    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    if (!ParsePixelMap(env, argv[NUM_0], pixelMap)) {
        MASK_LOG_E("ParsePixelMapMask parse pixelMap failed");
        return false;
    }
    para->SetPixelMap(pixelMap);

    Vector4f src;
    if (!ParseJsLTRBRect(env, argv[NUM_1], src)) {
        MASK_LOG_E("ParsePixelMapMask parse src failed");
        return false;
    }
    ClampVector4f(src, 0.f, 1.f);
    para->SetSrc(src);

    Vector4f dst;
    if (!ParseJsLTRBRect(env, argv[NUM_2], dst)) {
        MASK_LOG_E("ParsePixelMapMask parse dst failed");
        return false;
    }
    ClampVector4f(dst, 0.f, 1.f);
    para->SetDst(dst);

    if (realArgc >= NUM_4) {
        Vector4f fillColor;
        if (ParseJsRGBAColor(env, argv[NUM_3], fillColor)) {
            ClampVector4f(fillColor, 0.f, 1.f);
            para->SetFillColor(fillColor);
        }
    }
    return true;
}

napi_value MaskNapi::CreatePixelMapMask(napi_env env, napi_callback_info info)
{
    if (!UIEffectNapiUtils::IsSystemApp()) {
        MASK_LOG_E("MaskNapi CreatePixelMapMask failed");
        napi_throw_error(env, std::to_string(ERR_NOT_SYSTEM_APP).c_str(),
            "MaskNapi CreatePixelMapMask failed, is not system app");
        return nullptr;
    }
    static const size_t maxArgc = NUM_4;
    static const size_t minArgc = NUM_3;
    size_t realArgc = maxArgc;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value argv[maxArgc];
    napi_value thisVar = nullptr;
    UIEFFECT_JS_ARGS(env, info, status, realArgc, argv, thisVar);
    UIEFFECT_NAPI_CHECK_RET_D(status == napi_ok && minArgc <= realArgc && realArgc <= maxArgc, nullptr,
        MASK_LOG_E("MaskNapi CreatePixelMapMask parsing input fail."));
    auto para = std::make_shared<PixelMapMaskPara>();
    if (!ParsePixelMapMask(env, argv, realArgc, para)) {
        MASK_LOG_E("MaskNapi CreatePixelMapMask parse param failed");
        return nullptr;
    }

    if (!para->IsValid()) {
        MASK_LOG_E("MaskNapi CreatePixelMapMask pixel map mask param invalid");
        return nullptr;
    }

    return Create(env, para);
}

void MaskNapi::RegisterMaskParaUnmarshallingCallback()
{
    PixelMapMaskPara::RegisterUnmarshallingCallback();
    RadialGradientMaskPara::RegisterUnmarshallingCallback();
}

}  // namespace Rosen
}  // namespace OHOS
