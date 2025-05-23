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

#ifndef OHOS_ROSEN_JS_REGION_H
#define OHOS_ROSEN_JS_REGION_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "utils/region.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsRegion final {
public:
    explicit JsRegion(std::shared_ptr<Region> region) : m_region(region) {};
    ~JsRegion() {};

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);

    static napi_value GetBoundaryPath(napi_env env, napi_callback_info info);
    static napi_value GetBounds(napi_env env, napi_callback_info info);
    static napi_value IsComplex(napi_env env, napi_callback_info info);
    static napi_value IsEmpty(napi_env env, napi_callback_info info);
    static napi_value IsEqual(napi_env env, napi_callback_info info);
    static napi_value IsPointContained(napi_env env, napi_callback_info info);
    static napi_value IsRegionContained(napi_env env, napi_callback_info info);
    static napi_value Offset(napi_env env, napi_callback_info info);
    static napi_value Op(napi_env env, napi_callback_info info);
    static napi_value QuickReject(napi_env env, napi_callback_info info);
    static napi_value QuickRejectRegion(napi_env env, napi_callback_info info);
    static napi_value SetEmpty(napi_env env, napi_callback_info info);
    static napi_value SetRect(napi_env env, napi_callback_info info);
    static napi_value SetRegion(napi_env env, napi_callback_info info);
    static napi_value SetPath(napi_env env, napi_callback_info info);

    Region* GetRegion();

private:
    napi_value OnGetBoundaryPath(napi_env env, napi_callback_info info);
    napi_value OnGetBounds(napi_env env, napi_callback_info info);
    napi_value OnIsComplex(napi_env env, napi_callback_info info);
    napi_value OnIsEmpty(napi_env env, napi_callback_info info);
    napi_value OnIsEqual(napi_env env, napi_callback_info info);
    napi_value OnIsPointContained(napi_env env, napi_callback_info info);
    napi_value OnIsRegionContained(napi_env env, napi_callback_info info);
    napi_value OnOffset(napi_env env, napi_callback_info info);
    napi_value OnOp(napi_env env, napi_callback_info info);
    napi_value OnQuickReject(napi_env env, napi_callback_info info);
    napi_value OnQuickRejectRegion(napi_env env, napi_callback_info info);
    napi_value OnSetEmpty(napi_env env, napi_callback_info info);
    napi_value OnSetRect(napi_env env, napi_callback_info info);
    napi_value OnSetRegion(napi_env env, napi_callback_info info);
    napi_value OnSetPath(napi_env env, napi_callback_info info);

    static thread_local napi_ref constructor_;
    std::shared_ptr<Region> m_region = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_REGION_H
