/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef FUNCTION_FONT_MEASURE_TEXT_TEST_H
#define FUNCTION_FONT_MEASURE_TEXT_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include "test_base.h"
#include "test_common.h"

/*
 * OH_Drawing_FontMeasureText接口正常入参验证,第五个参数传空
 * OH_Drawing_FontMeasureText接口正常入参验证,第四个参数枚举值各种情况
 * OH_Drawing_FontMeasureText接口正常入参验证,第五个参数边界框设置小于文本大小
 * OH_Drawing_FontMeasureText接口正常入参验证，text传入特殊字符
 * OH_Drawing_FontMeasureText接口正常入参验证，text传入中文
 */
class FunctionFontMeasureText : public TestBase {
public:
    FunctionFontMeasureText()
    {
        fileName_ = "FunctionFontMeasureText";
    }
    ~FunctionFontMeasureText() override{};

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // FUNCTION_FONT_MEASURE_TEXT_TEST_H
