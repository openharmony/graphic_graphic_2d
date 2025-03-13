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

#include "js_region.h"
#include "draw/path.h"
#include "utils/region.h"
#include "path_napi/js_path.h"
#include "native_value.h"
#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsRegion::constructor_ = nullptr;
const std::string CLASS_NAME = "Region";
napi_value JsRegion::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("isPointContained", JsRegion::IsPointContained),
        DECLARE_NAPI_FUNCTION("isRegionContained", JsRegion::IsRegionContained),
        DECLARE_NAPI_FUNCTION("op", JsRegion::Op),
        DECLARE_NAPI_FUNCTION("quickReject", JsRegion::QuickReject),
        DECLARE_NAPI_FUNCTION("setRect", JsRegion::SetRect),
        DECLARE_NAPI_FUNCTION("setPath", JsRegion::SetPath),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRegion::Init Failed to define Region class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRegion::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRegion::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsRegion::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRegion::Constructor failed to napi_get_cb_info");
        return nullptr;
    }
    auto region = std::make_shared<Region>();
    JsRegion* jsRegion = new JsRegion(region);
    status = napi_wrap(env, jsThis, jsRegion,
                       JsRegion::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRegion;
        ROSEN_LOGE("JsRegion::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsRegion::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsRegion* napi = reinterpret_cast<JsRegion*>(nativeObject);
        delete napi;
    }
}

napi_value JsRegion::IsPointContained(napi_env env, napi_callback_info info)
{
    JsRegion* me = CheckParamsAndGetThis<JsRegion>(env, info);
    return (me != nullptr) ? me->OnIsPointContained(env, info) : nullptr;
}

napi_value JsRegion::OnIsPointContained(napi_env env, napi_callback_info info)
{
    if (m_region == nullptr) {
        ROSEN_LOGE("JsRegion::OnIsPointContained region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    int32_t x = 0;
    GET_INT32_PARAM(ARGC_ZERO, x);
    int32_t y = 0;
    GET_INT32_PARAM(ARGC_ONE, y);

    return CreateJsValue(env, m_region->Contains(x, y));
}

napi_value JsRegion::IsRegionContained(napi_env env, napi_callback_info info)
{
    JsRegion* me = CheckParamsAndGetThis<JsRegion>(env, info);
    return (me != nullptr) ? me->OnIsRegionContained(env, info) : nullptr;
}

napi_value JsRegion::OnIsRegionContained(napi_env env, napi_callback_info info)
{
    if (m_region == nullptr) {
        ROSEN_LOGE("JsRegion::OnIsRegionContained region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsRegion* other = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, other);
    if (other->GetRegion() == nullptr) {
        ROSEN_LOGE("JsRegion::OnIsRegionContained other is nullptr");
        return nullptr;
    }

    return CreateJsValue(env, m_region->IsRegionContained(*other->GetRegion()));
}

napi_value JsRegion::Op(napi_env env, napi_callback_info info)
{
    JsRegion* me = CheckParamsAndGetThis<JsRegion>(env, info);
    return (me != nullptr) ? me->OnOp(env, info) : nullptr;
}

napi_value JsRegion::OnOp(napi_env env, napi_callback_info info)
{
    if (m_region == nullptr) {
        ROSEN_LOGE("JsRegion::OnOp region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    JsRegion* region = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, region);
    if (region->GetRegion() == nullptr) {
        ROSEN_LOGE("JsRegion::OnOp region is nullptr");
        return nullptr;
    }

    int32_t regionOp = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_ONE, regionOp);

    RegionOp op = static_cast<RegionOp>(regionOp);
    if (op < RegionOp::DIFFERENCE || op > RegionOp::REPLACE) {
        ROSEN_LOGE("JsRegion::OnOp regionOp is error param");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsValue(env, m_region->Op(*region->GetRegion(), op));
}

napi_value JsRegion::QuickReject(napi_env env, napi_callback_info info)
{
    JsRegion* me = CheckParamsAndGetThis<JsRegion>(env, info);
    return (me != nullptr) ? me->OnQuickReject(env, info) : nullptr;
}

napi_value JsRegion::OnQuickReject(napi_env env, napi_callback_info info)
{
    if (m_region == nullptr) {
        ROSEN_LOGE("JsRegion::OnQuickReject region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    int32_t left = 0;
    GET_INT32_PARAM(ARGC_ZERO, left);
    int32_t top = 0;
    GET_INT32_PARAM(ARGC_ONE, top);
    int32_t right = 0;
    GET_INT32_PARAM(ARGC_TWO, right);
    int32_t bottom = 0;
    GET_INT32_PARAM(ARGC_THREE, bottom);
    Drawing::RectI rectI = Drawing::RectI(left, top, right, bottom);

    return CreateJsValue(env, m_region->QuickReject(rectI));
}

napi_value JsRegion::SetRect(napi_env env, napi_callback_info info)
{
    JsRegion* me = CheckParamsAndGetThis<JsRegion>(env, info);
    return (me != nullptr) ? me->OnSetRect(env, info) : nullptr;
}

napi_value JsRegion::OnSetRect(napi_env env, napi_callback_info info)
{
    if (m_region == nullptr) {
        ROSEN_LOGE("JsRegion::OnSetRect region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    int32_t left = 0;
    GET_INT32_PARAM(ARGC_ZERO, left);
    int32_t top = 0;
    GET_INT32_PARAM(ARGC_ONE, top);
    int32_t right = 0;
    GET_INT32_PARAM(ARGC_TWO, right);
    int32_t bottom = 0;
    GET_INT32_PARAM(ARGC_THREE, bottom);
    Drawing::RectI rectI = Drawing::RectI(left, top, right, bottom);

    return CreateJsValue(env, m_region->SetRect(rectI));
}

napi_value JsRegion::SetPath(napi_env env, napi_callback_info info)
{
    JsRegion* me = CheckParamsAndGetThis<JsRegion>(env, info);
    return (me != nullptr) ? me->OnSetPath(env, info) : nullptr;
}

napi_value JsRegion::OnSetPath(napi_env env, napi_callback_info info)
{
    if (m_region == nullptr) {
        ROSEN_LOGE("JsRegion::OnSetPath region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    JsPath* jsPath = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsPath);
    if (jsPath->GetPath() == nullptr) {
        ROSEN_LOGE("JsRegion::OnSetPath jsPath is nullptr");
        return nullptr;
    }
    JsRegion* jsClip = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, jsClip);
    if (jsClip->GetRegion() == nullptr) {
        ROSEN_LOGE("JsRegion::OnSetPath jsClip is nullptr");
        return nullptr;
    }

    return CreateJsValue(env, m_region->SetPath(*jsPath->GetPath(), *jsClip->GetRegion()));
}

Region* JsRegion::GetRegion()
{
    return m_region.get();
}
} // namespace Drawing
} // namespace OHOS::Rosen
