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

#ifndef INTERFACE_PEN_TEST_H
#define INTERFACE_PEN_TEST_H
#include <bits/alltypes.h>
#include <multimedia/player_framework/native_avscreen_capture_base.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_sampling_options.h>

#include "napi/native_api.h"
#include "test_base.h"
#include "test_common.h"

class PenReset : public TestBase {
public:
    explicit PenReset(int type) : TestBase(type)
    {
        fileName_ = "PenReset";
    };
    ~PenReset() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // INTERFACE_PEN_TEST_H
