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

#ifndef INTERFACE_SAMPLE_OPTION_TEST_H
#define INTERFACE_SAMPLE_OPTION_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_sampling_options.h>

#include "test_base.h"
#include "test_common.h"

class SamplingOptionsCreate : public TestBase {
public:
    SamplingOptionsCreate(int type, OH_Drawing_FilterMode filterMode, OH_Drawing_MipmapMode mipmapMode)
        : TestBase(type), filterMode(filterMode), mipmapMode(mipmapMode)
    {
        fileName_ = "SamplingOptionsCreate";
    };
    ~SamplingOptionsCreate() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    OH_Drawing_FilterMode filterMode;
    OH_Drawing_MipmapMode mipmapMode;
};

#endif // INTERFACE_SAMPLE_OPTION_TEST_H