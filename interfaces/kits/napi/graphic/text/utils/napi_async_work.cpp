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

#include "napi_async_work.h"
#include "napi_common.h"

namespace OHOS::Rosen {
ContextBase::~ContextBase()
{
    TEXT_LOGD("Entry");
    TEXT_CHECK(env != nullptr, return);
    TEXT_CHECK(work == nullptr, status = napi_delete_async_work(env, work));
    TEXT_ERROR_CHECK(status == napi_ok, return, "Failed to delete async work, status:%{public}d",
        static_cast<int>(status));
    TEXT_CHECK(callbackRef == nullptr, status = napi_delete_reference(env, callbackRef));
    TEXT_ERROR_CHECK(status == napi_ok, return, "Failed to delete callback reference, status:%{public}d",
        static_cast<int>(status));
    TEXT_CHECK(selfRef == nullptr, status = napi_delete_reference(env, selfRef));
    TEXT_ERROR_CHECK(status == napi_ok, return, "Failed to delete self reference, status:%{public}d",
        static_cast<int>(status));

    env = nullptr;
    callbackRef = nullptr;
    selfRef = nullptr;
}

void ContextBase::GetCbInfo(napi_env envi, napi_callback_info info, NapiCbInfoParser parser, bool sync)
{
    env = envi;
    size_t argc = ARGC_MAX;
    napi_value argv[ARGC_MAX] = {nullptr};
    status = napi_get_cb_info(env, info, &argc, argv, &self, nullptr);
    NAPI_CHECK_ARGS(this, status == napi_ok, status, TextErrorCode::ERROR, return,
        "Failed to get callback info, status:%d", static_cast<int>(status));
    NAPI_CHECK_ARGS(this, argc <= ARGC_MAX, status, TextErrorCode::ERROR_INVALID_PARAM, return,
        "Too many arguments %zu", argc);
    NAPI_CHECK_ARGS(this, self, status, TextErrorCode::ERROR_INVALID_PARAM, return,
        "There is no JavaScript for this parameter");

    status = napi_create_reference(env, self, 1, &selfRef);
    NAPI_CHECK_ARGS(this, status == napi_ok, status, TextErrorCode::ERROR, return,
        "Failed to create reference, status:%d", static_cast<int>(status));

    status = napi_unwrap(env, self, &native);
    NAPI_CHECK_ARGS(this, status == napi_ok, status, TextErrorCode::ERROR, return,
        "Failed to unwrap self, status:%d", static_cast<int>(status));

    if (!sync && (argc > 0)) {
        // get the last arguments :: <callback>
        size_t index = argc - 1;
        napi_valuetype type = napi_undefined;
        napi_status typeStatus = napi_typeof(env, argv[index], &type);
        if ((typeStatus == napi_ok) && (type == napi_function)) {
            status = napi_create_reference(env, argv[index], 1, &callbackRef);
            NAPI_CHECK_ARGS(this, status == napi_ok, status, TextErrorCode::ERROR, return,
                "Failed to get callback ref, status:%d", static_cast<int>(status));
            argc = index;
            TEXT_LOGD("Async callback, no promise");
        } else {
            TEXT_LOGD("No callback, async promise");
        }
    }

    if (parser) {
        parser(argc, argv);
    } else {
        NAPI_CHECK_ARGS(this, argc == 0, status, TextErrorCode::ERROR_INVALID_PARAM, return,
            "Required no arguments");
    }
}

napi_value NapiAsyncWork::Enqueue(napi_env env, sptr<ContextBase> contextBase, const std::string& name,
                                  NapiAsyncExecute execute, NapiAsyncComplete complete)
{
    TEXT_ERROR_CHECK(contextBase, return nullptr, "Context is null");
    contextBase->execute = std::move(execute);
    contextBase->complete = std::move(complete);
    napi_value promise = nullptr;
    napi_status stat = napi_invalid_arg;
    if (contextBase->callbackRef == nullptr) {
        stat = napi_create_promise(contextBase->env, &contextBase->deferred, &promise);
        NAPI_CHECK_ARGS(contextBase, stat == napi_ok, stat, TextErrorCode::ERROR, return nullptr,
            "Failed to create promise, stat:%d", static_cast<int>(stat));
    } else {
        stat = napi_get_undefined(contextBase->env, &promise);
        NAPI_CHECK_ARGS(contextBase, stat == napi_ok, stat, TextErrorCode::ERROR, return nullptr,
            "Failed to get undefined, stat:%d", static_cast<int>(stat));
    }

    napi_value resource = nullptr;
    stat = napi_create_string_utf8(contextBase->env, name.c_str(), NAPI_AUTO_LENGTH, &resource);
    NAPI_CHECK_ARGS(contextBase, stat == napi_ok, stat, TextErrorCode::ERROR, return promise,
        "Failed to create string, stat:%d", static_cast<int>(stat));
    stat = napi_create_async_work(
        contextBase->env, nullptr, resource,
        [](napi_env env, void* data) {
            TEXT_ERROR_CHECK(data, return, "Data is null");
            sptr<ContextBase> contextBase(static_cast<ContextBase *>(data));
            if (contextBase && contextBase->execute && contextBase->status == napi_ok) {
                contextBase->execute();
            }
        },
        [](napi_env env, napi_status status, void* data) {
            TEXT_ERROR_CHECK(data, return, "Data is null");
            sptr<ContextBase> contextBase(static_cast<ContextBase *>(data));
            TEXT_ERROR_CHECK(contextBase, return, "Context is null");
            if ((status != napi_ok) && contextBase && (contextBase->status == napi_ok)) {
                contextBase->status = status;
            }
            if (contextBase && (contextBase->complete) && (status == napi_ok) && (contextBase->status == napi_ok)) {
                contextBase->complete(contextBase->output);
            }
            GenerateOutput(contextBase);
        },
        reinterpret_cast<void*>(contextBase.GetRefPtr()), &contextBase->work);
    NAPI_CHECK_ARGS(contextBase, stat == napi_ok, stat, TextErrorCode::ERROR, return promise,
        "Failed to create async work, stat:%d", static_cast<int>(stat));

    stat = napi_queue_async_work_with_qos(contextBase->env, contextBase->work, napi_qos_user_initiated);
    NAPI_CHECK_ARGS(contextBase, stat == napi_ok, stat, TextErrorCode::ERROR, return promise,
        "Failed to queue async work, stat:%d", static_cast<int>(stat));
    return promise;
}

void NapiAsyncWork::GenerateOutput(sptr<ContextBase> contextBase)
{
    TEXT_ERROR_CHECK(contextBase, return, "Context is null");
    napi_value result[RESULT_ALL] = {nullptr};
    napi_status stat = napi_invalid_arg;
    if (contextBase->status == napi_ok) {
        stat = napi_get_undefined(contextBase->env, &result[RESULT_ERROR]);
        TEXT_CHECK(stat == napi_ok, TEXT_LOGD("Failed to assign result, stat:%d", static_cast<int>(stat)));
        if (contextBase->output == nullptr) {
            stat = napi_get_undefined(contextBase->env, &contextBase->output);
            TEXT_CHECK(stat == napi_ok, TEXT_LOGD("Failed to assign output, stat:%d", static_cast<int>(stat)));
        }
        result[RESULT_DATA] = contextBase->output;
    } else {
        napi_value message = nullptr;
        napi_value code = nullptr;
        stat = napi_create_string_utf8(contextBase->env, contextBase->errMessage.c_str(), NAPI_AUTO_LENGTH, &message);
        TEXT_CHECK(stat == napi_ok, TEXT_LOGD("Failed to create string, stat:%d", static_cast<int>(stat)));
        stat = napi_create_error(contextBase->env, nullptr, message, &result[RESULT_ERROR]);
        TEXT_CHECK(stat == napi_ok, TEXT_LOGD("Failed to create error, stat:%d", static_cast<int>(stat)));
        stat = napi_create_int32(contextBase->env, contextBase->errCode, &code);
        TEXT_CHECK(stat == napi_ok, TEXT_LOGD("Failed to assign errCode, stat:%d", static_cast<int>(stat)));
        stat = napi_set_named_property(contextBase->env, result[RESULT_ERROR], "code", code);
        TEXT_CHECK(stat == napi_ok, TEXT_LOGD("Failed to set named property, stat:%d", static_cast<int>(stat)));
        stat = napi_get_undefined(contextBase->env, &result[RESULT_DATA]);
        TEXT_CHECK(stat == napi_ok, TEXT_LOGD("Failed to assign data, stat:%d", static_cast<int>(stat)));
    }
    if (contextBase->deferred != nullptr) {
        if (contextBase->status == napi_ok) {
            stat = napi_resolve_deferred(contextBase->env, contextBase->deferred, result[RESULT_DATA]);
            TEXT_CHECK(stat == napi_ok, TEXT_LOGE("Failed to resolve a deferred promise, stat:%d",
                static_cast<int>(stat)));
        } else {
            stat = napi_reject_deferred(contextBase->env, contextBase->deferred, result[RESULT_ERROR]);
            TEXT_CHECK(stat == napi_ok, TEXT_LOGE("Failed to reject a deferred promise, stat:%d",
                static_cast<int>(stat)));
        }
    } else {
        napi_value callback = nullptr;
        stat = napi_get_reference_value(contextBase->env, contextBase->callbackRef, &callback);
        TEXT_CHECK(stat == napi_ok, TEXT_LOGE("Failed to get reference value, stat:%d",
            static_cast<int>(stat)));
        napi_value callbackResult = nullptr;
        stat = napi_call_function(contextBase->env, nullptr, callback, RESULT_ALL, result, &callbackResult);
        TEXT_CHECK(stat == napi_ok, TEXT_LOGE("Failed to call callback function, stat:%d", static_cast<int>(stat)));
    }
    contextBase->execute = nullptr;
    contextBase->complete = nullptr;
}
}