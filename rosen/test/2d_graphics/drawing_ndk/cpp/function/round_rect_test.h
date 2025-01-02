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

#ifndef FUNCTION_ROUND_RECT_TEST_H
#define FUNCTION_ROUND_RECT_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

/*
 * OH_Drawing_RoundRectOffset接口绘制图形一半在画布内，一半在画布外
 * OH_Drawing_RoundRectOffset接口绘制图形全部在画布外
 */
class FunctionRoundRectOffset : public TestBase {
public:
    FunctionRoundRectOffset()
    {
        fileName_ = "FunctionRoundRectOffset";
    }
    ~FunctionRoundRectOffset() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

/*
 * OH_Drawing_RoundRectCreate接口异常入参验证传入正方形，参数2,3设置超过正方形边长一半
 * OH_Drawing_RoundRectCreate接口正常入参验证传入长方形，参数2,3设置20,20
 * OH_Drawing_RoundRectCreate接口异常入参验证传入长方形，参数2,3设置超过最短边长一半
 * OH_Drawing_RoundRectCreate接口正常入参验证传入长方形，参数2,3设置20,80
 * OH_Drawing_RoundRectCreate接口绘制图形一半在画布内，一半在画布外
 * OH_Drawing_RoundRectCreate接口绘制图形全部在画布外
 */
class FunctionRoundRectCreate : public TestBase {
public:
    FunctionRoundRectCreate()
    {
        fileName_ = "FunctionRoundRectCreate";
    }
    ~FunctionRoundRectCreate() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

#endif // FUNCTION_ROUND_RECT_TEST_H
