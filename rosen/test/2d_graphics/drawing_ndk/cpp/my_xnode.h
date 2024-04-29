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

#ifndef MY_XNODE_H
#define MY_XNODE_H
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arpa/nameser.h>
#include <bits/alltypes.h>
#include <native_window/external_window.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <cstdint>
#include <map>
#include <sys/mman.h>
#include <string>
#include "napi/native_api.h"

class MyXNode {
public:
    MyXNode() = default;
    ~MyXNode(){};
   
    static void Export(napi_env env, napi_value exports);
    static napi_value XNodeDraw(napi_env env, napi_callback_info info);
    static napi_value NapiFunction(napi_env env, napi_callback_info info);
    static napi_value NapiPerformance(napi_env env, napi_callback_info info);
    
    static void TestFunction(OH_Drawing_Canvas* canvas, std::string caseName);
    static uint32_t TestPerformance(OH_Drawing_Canvas* canvas, std::string caseName, uint32_t testCount);
};
#endif
