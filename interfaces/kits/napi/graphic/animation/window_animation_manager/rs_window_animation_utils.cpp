/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_window_animation_utils.h"

#include <js_runtime_utils.h>

#include "rs_window_animation_log.h"

#include "ui/rs_proxy_node.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;

napi_value RSWindowAnimationUtils::CreateJsWindowAnimationTarget(napi_env env,
    const sptr<RSWindowAnimationTarget>& target)
{
    WALOGD("Create!");
    if (target == nullptr) {
        WALOGD("Target is null!");
        return CreateNull(env);
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WALOGE("CreateJsWindowAnimationTarget failed to create object!");
        return CreateUndefined(env);
    }

    napi_finalize finalizeCallback = [](napi_env env, void* data, void* hint) {
        auto target = sptr<RSWindowAnimationTarget>(static_cast<RSWindowAnimationTarget*>(hint));
        target.GetRefPtr()->DecStrongRef(target.GetRefPtr());
    };
    target.GetRefPtr()->IncStrongRef(target.GetRefPtr());
    napi_wrap(env, objValue, &(target->surfaceNode_), finalizeCallback, target.GetRefPtr(), nullptr);
    if (auto proxyNode = RSBaseNode::ReinterpretCast<RSProxyNode>(target->surfaceNode_)) {
        // force proxy node to flush correct context alpha on next visit
        proxyNode->ResetContextVariableCache();
    }

    napi_set_named_property(env, objValue, "bundleName", CreateJsValue(env, target->bundleName_));
    napi_set_named_property(env, objValue, "abilityName", CreateJsValue(env, target->abilityName_));
    napi_set_named_property(env, objValue, "windowBounds", CreateJsRRect(env, target->windowBounds_));
    napi_set_named_property(env, objValue, "missionId", CreateJsValue(env, target->missionId_));
    napi_set_named_property(env, objValue, "windowId", CreateJsValue(env, target->windowId_));

    return objValue;
}

napi_value RSWindowAnimationUtils::CreateJsWindowAnimationTargetArray(napi_env env,
    const std::vector<sptr<RSWindowAnimationTarget>>& targets)
{
    WALOGD("Create!");
    napi_value array = nullptr;
    napi_create_array_with_length(env, targets.size(), &array);
    uint32_t index = 0;
    for (const auto& item : targets) {
        napi_set_element(env, array, index++, CreateJsWindowAnimationTarget(env, item));
    }
    return array;
}

napi_value RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(
    napi_env env, const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Create!");
    if (finishedCallback == nullptr) {
        WALOGE("Finished callback is null!");
        return CreateUndefined(env);
    }
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WALOGE("CreateJsWindowAnimationFinishedCallback failed to create object!");
        return CreateUndefined(env);
    }

    napi_value object = nullptr;
    napi_coerce_to_object(env, objValue, &object);
    if (object == nullptr) {
        WALOGE("CreateJsWindowAnimationFinishedCallback failed to convert object!");
        return CreateUndefined(env);
    }

    napi_finalize finalizeCallback = [](napi_env env, void* data, void* hint) {
        auto finishedCallback =
            sptr<RSIWindowAnimationFinishedCallback>(static_cast<RSIWindowAnimationFinishedCallback*>(data));
        finishedCallback.GetRefPtr()->DecStrongRef(finishedCallback.GetRefPtr());
    };
    finishedCallback.GetRefPtr()->IncStrongRef(finishedCallback.GetRefPtr());
    napi_wrap(env, object, finishedCallback.GetRefPtr(), finalizeCallback, nullptr, nullptr);

    napi_callback jsFinishedCallback = [](napi_env env, napi_callback_info info) -> napi_value {
        WALOGD("Native finished callback is called!");
        auto nativeFinishedCallback = CheckParamsAndGetThis<RSIWindowAnimationFinishedCallback>(env, info);
        if (nativeFinishedCallback == nullptr) {
            WALOGE("Finished callback is null!");
            return CreateUndefined(env);
        }

        nativeFinishedCallback->OnAnimationFinished();
        return CreateUndefined(env);
    };
    const char *moduleName = "RSWindowAnimationUtils";
    BindNativeFunction(env, object, "onAnimationFinish", moduleName, jsFinishedCallback);
    return objValue;
}

napi_value RSWindowAnimationUtils::CreateJsRRect(napi_env env, const RRect& rrect)
{
    WALOGD("Create!");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WALOGE("CreateJsRRect failed to create object!");
        return CreateUndefined(env);
    }

    napi_value object = nullptr;
    napi_coerce_to_object(env, objValue, &object);
    if (object == nullptr) {
        WALOGE("CreateJsRRect failed to convert object!");
        return CreateUndefined(env);
    }

    napi_set_named_property(env, object, "left", CreateJsValue(env, rrect.rect_.left_));
    napi_set_named_property(env, object, "top", CreateJsValue(env, rrect.rect_.top_));
    napi_set_named_property(env, object, "width", CreateJsValue(env, rrect.rect_.width_));
    napi_set_named_property(env, object, "height", CreateJsValue(env, rrect.rect_.height_));
    napi_set_named_property(env, object, "radius", CreateJsValue(env, rrect.radius_[0].x_));
    return objValue;
}

bool RSWindowAnimationUtils::IsSystemApp()
{
    uint64_t tokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(tokenId);
}

napi_value RSWindowAnimationUtils::CreateNull(napi_env env)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

napi_value RSWindowAnimationUtils::CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}
} // namespace Rosen
} // namespace OHOS
