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

#include <bits/alltypes.h>
#include <chrono>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_text_typography.h>
#include "common/log_common.h"
#include "my_xnode.h"
#include "napi/native_api.h"
#include "napi/native_api.h"
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <unordered_map>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_rect.h>
#include "testcasefactory.h"


bool ConvertStringFromJsValue(napi_env env, napi_value jsValue, std::string &value)
{
    size_t len = 0;
    if (napi_get_value_string_utf8(env, jsValue, nullptr, 0, &len) != napi_ok) {
        return false;
    }
    auto buffer = std::make_unique<char[]>(len + 1);
    size_t strLength = 0;
    if (napi_get_value_string_utf8(env, jsValue, buffer.get(), len + 1, &strLength) == napi_ok) {
        value = buffer.get();
        return true;
    }
    return false;
}

bool ConvertStringFromIntValue(napi_env env, napi_value jsValue, uint32_t &value)
{
    return napi_get_value_uint32(env, jsValue, &value) == napi_ok;
}

napi_value MyXNode::XNodeDraw(napi_env env, napi_callback_info info)
{
    DRAWING_LOGE("MyXNode XNodeDraw::NapiSetTestCount");

    DRAWING_LOGI("NapiSetTestCount");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }
    void* contex;
    napi_unwrap(env, argv[0], &contex);
    OH_Drawing_Canvas* canvas = reinterpret_cast<OH_Drawing_Canvas*>(contex);
    if (canvas == nullptr) {
        DRAWING_LOGE("XNodeDraw get canvas fail");
        return nullptr;
    }
    
    // 创建一个画刷Brush对象，Brush对象用于形状的填充
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    //绘制矩形
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
    
    return nullptr;
}


napi_value MyXNode::NapiFunction(napi_env env, napi_callback_info info)
{
    DRAWING_LOGE("MyXNode XNodeDraw::NapiFunction");

    DRAWING_LOGI("NapiFunction");
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
        DRAWING_LOGE("NapiFunction: XNodeDraw get canvas fail");
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
    DRAWING_LOGE("MyXNode XNodeDraw::NapiFunction");

    DRAWING_LOGI("NapiFunction");
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
        DRAWING_LOGE("NapiFunction: XNodeDraw get canvas fail");
        return nullptr;
    }

    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[1], caseName)) { // 2:cassname
        DRAWING_LOGE("NapiFunction: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("NapiFunction: caseName = %{public}s", caseName.c_str());
    
    uint32_t testCount;
    if (!ConvertStringFromIntValue(env, argv[2], testCount)) { // 2:count
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
    DRAWING_LOGE("MyXNode TestFunction start %{public}s", caseName.c_str());
    auto testCase = TestCaseFactory::GetFunctionCpuCase(caseName);
    if (testCase == nullptr) {
        DRAWING_LOGE("failed to get testcase");
        return;
    }
    testCase->TestFunctionGpu(canvas);
}

uint32_t MyXNode::TestPerformance(OH_Drawing_Canvas* canvas, std::string caseName, uint32_t testCount)
{
    DRAWING_LOGE("MyXNode TestPerformance start");
    auto testCase = TestCaseFactory::GetPerformanceCpuCase(caseName);
    if (testCase == nullptr) {
        DRAWING_LOGE("failed to get testcase");
        return 0;
    }
    
    testCase->SetTestCount(testCount);
    testCase->TestPerformanceGpu(canvas);
    DRAWING_LOGE("MyXNode TestPerformance end");
    return testCase->GetTime();
}

void MyXNode::Export(napi_env env, napi_value exports)
{
    DRAWING_LOGE("MyXNode MyXNode napi init");
    if ((env == nullptr) || (exports == nullptr)) {
        DRAWING_LOGE("Export: env or exports is null");
        return;
    }

    napi_property_descriptor desc[] = {
        {"draw", nullptr, MyXNode::XNodeDraw, nullptr, nullptr, nullptr, napi_default, nullptr},
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
    DRAWING_LOGE("MyXNode myxnode napi init");
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
