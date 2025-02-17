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

#ifndef STABILITY_ROUND_RECT_TEST_H
#define STABILITY_ROUND_RECT_TEST_H
#include <map>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_round_rect.h>

#include "test_base.h"
#include "test_common.h"

typedef enum {
    SETTING_FUNCTION_ROUND_RECT_SET_CORNER = 0,
    SETTING_FUNCTION_ROUND_RECT_MAX
} SettingFunctionRoundRect;

typedef enum {
    OPERATION_FUNCTION_ROUND_RECT_GET_CORNER = 0,
    OPERATION_FUNCTION_ROUND_RECT_MAX
} OperationFunctionRoundRect;

// RoundRectCreate-RoundRectDestroy循环
class StabilityRoundRectCreate : public TestBase {
public:
    StabilityRoundRectCreate()
    {
        fileName_ = "StabilityRoundRectCreate";
    }
    ~StabilityRoundRectCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// RoundRectCreate-相关配置接口全调用-RoundRectDestroy循环
class StabilityRoundRectAll : public TestBase {
public:
    StabilityRoundRectAll()
    {
        fileName_ = "StabilityRoundRectAll";
    }
    ~StabilityRoundRectAll() override {};

private:
    TestRend rand_;

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void RoundRectSetCornerTest(OH_Drawing_RoundRect* roundRect);
    using HandlerFunc = std::function<void(OH_Drawing_RoundRect* roundRect)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { SETTING_FUNCTION_ROUND_RECT_SET_CORNER,
            [this](OH_Drawing_RoundRect* roundRect) { return RoundRectSetCornerTest(roundRect); } },
    };
};

// RoundRect相关操作接口随机循环调用
class StabilityRoundRectRandomFunc : public TestBase {
public:
    StabilityRoundRectRandomFunc()
    {
        fileName_ = "StabilityRoundRectRandomFunc";
    }
    ~StabilityRoundRectRandomFunc() override {};

private:
    TestRend rand_;

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void RoundRectGetCornerTest(OH_Drawing_RoundRect* roundRect);
    using HandlerFunc = std::function<void(OH_Drawing_RoundRect* roundRect)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_ROUND_RECT_GET_CORNER,
            [this](OH_Drawing_RoundRect* roundRect) { return RoundRectGetCornerTest(roundRect); } },
    };
};

#endif // STABILITY_ROUND_RECT_TEST_H