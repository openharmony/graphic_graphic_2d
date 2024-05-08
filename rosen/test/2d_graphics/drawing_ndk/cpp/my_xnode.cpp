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

#include "my_xnode.h"
#include <bits/alltypes.h>
#include <chrono>
#include <fcntl.h>
#include <memory>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_text_typography.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include "common/log_common.h"
#include "napi/native_api.h"
#include "testcasefactory.h"
#include "test_common.h"

napi_value MyXNode::NapiFunction(napi_env env, napi_callback_info info)
{
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiFunction: env or info is null");
        return nullptr;
    }

    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiFunction: napi_get_cb_info fail");
        return nullptr;
    }
    void* contex;
    napi_unwrap(env, argv[0], &contex);
    OH_Drawing_Canvas* canvas = reinterpret_cast<OH_Drawing_Canvas*>(contex);
    if (canvas == nullptr) {
        DRAWING_LOGE("NapiFunction: MyXNode get canvas fail");
        return nullptr;
    }

    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[1], caseName)) {
        DRAWING_LOGE("NapiFunction: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("NapiFunction: caseName = %{public}s", caseName.c_str());
    
    TestFunction(canvas, caseName);
    return nullptr;
}

napi_value MyXNode::NapiPerformance(napi_env env, napi_callback_info info)
{
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiFunction: env or info is null");
        return nullptr;
    }

    size_t argc = 3;
    napi_value argv[3] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiFunction: napi_get_cb_info fail");
        return nullptr;
    }
    void* contex;
    napi_unwrap(env, argv[0], &contex);
    OH_Drawing_Canvas* canvas = reinterpret_cast<OH_Drawing_Canvas*>(contex);
    if (canvas == nullptr) {
        DRAWING_LOGE("NapiFunction: get canvas fail");
        return nullptr;
    }

    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[1], caseName)) { // 2:cassname
        DRAWING_LOGE("NapiFunction: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("NapiFunction: caseName = %{public}s", caseName.c_str());
    
    uint32_t testCount;
    if (!ConvertIntFromJsValue(env, argv[2], testCount)) { // 2:count
        DRAWING_LOGE("NapiDrawPattern: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("NapiFunction: testCount = %{public}u", testCount);
    
    uint32_t time = TestPerformance(canvas, caseName, testCount);
    napi_value value = nullptr;
    (void)napi_create_int32(env, time, &value);
    return value;
}

void MyXNode::TestFunction(OH_Drawing_Canvas* canvas, std::string caseName)
{
    DRAWING_LOGI("MyXNode TestFunction start %{public}s", caseName.c_str());
    auto testCase = TestCaseFactory::GetFunctionCpuCase(caseName);
    if (testCase == nullptr) {
        DRAWING_LOGE("failed to get testcase");
        return;
    }
    OH_Drawing_CanvasSave(canvas);
    testCase->ClipCanvasToDrawSize(canvas);
    testCase->TestFunctionGpu(canvas);
    OH_Drawing_CanvasRestore(canvas);
}

uint32_t MyXNode::TestPerformance(OH_Drawing_Canvas* canvas, std::string caseName, uint32_t testCount)
{
    auto testCase = TestCaseFactory::GetPerformanceCpuCase(caseName);
    if (testCase == nullptr) {
        DRAWING_LOGE("failed to get testcase");
        return 0;
    }
    
    testCase->SetTestCount(testCount);
    testCase->TestPerformanceGpu(canvas);
    return testCase->GetTime();
}

void MyXNode::Export(napi_env env, napi_value exports)
{
    DRAWING_LOGI("MyXNode napi init");
    if ((env == nullptr) || (exports == nullptr)) {
        DRAWING_LOGE("Export: env or exports is null");
        return;
    }

    napi_property_descriptor desc[] = {
        {"TestFunctional", nullptr, MyXNode::NapiFunction, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"TestPerformance", nullptr, MyXNode::NapiPerformance, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc) != napi_ok) {
        DRAWING_LOGE("Export: napi_define_properties failed");
    }
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    MyXNode::Export(env, exports);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "myxnode",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) { napi_module_register(&demoModule); }
