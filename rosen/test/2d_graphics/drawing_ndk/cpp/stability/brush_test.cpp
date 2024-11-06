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
#include "brush_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_shadow_layer.h>

#include "test_common.h"

#include "common/log_common.h"

void StabilityBrushCreate::BrushSetAntiAliasTest(OH_Drawing_Brush* brush)
{
    // 2 for test,rand true or false
    OH_Drawing_BrushSetAntiAlias(brush, rand_.nextU() % 2 == 0);
}

void StabilityBrushCreate::BrushSetColorTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushSetColor(brush, rand_.nextU());
}

void StabilityBrushCreate::BrushSetAlphaTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushSetAlpha(brush, rand_.nextULessThan(0xFF));
}

void StabilityBrushCreate::BrushSetFilterTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushSetFilter(brush, filter_);
}

void StabilityBrushCreate::BrushSetShaderEffectTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushSetShaderEffect(brush, effect_);
}

void StabilityBrushCreate::BrushSetShadowLayerTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushSetShadowLayer(brush, layer_);
}

void StabilityBrushCreate::BrushSetSetBlendModeTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushSetBlendMode(brush, DrawGetEnum(BLEND_MODE_CLEAR, BLEND_MODE_LUMINOSITY, rand_.nextU()));
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BRUSH_0100
@Description:BrushCreate-相关配置接口全调用-BrushDestroy循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、执行自动化用例：
    OH_Drawing_BrushCreate-OH_Drawing_BrushDestroy 循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityBrushCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
        for (int j = 0; j < SETTING_FUNCTION_BRUSH_MAX; j++) {
            handlers_[j](brush);
        }
        OH_Drawing_BrushDestroy(brush);
    }
};

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BRUSH_0200
@Description:BrushCopy-BrushDestroy循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、执行自动化用例：
    OH_Drawing_BrushCreate，OH_Drawing_BrushCopy-OH_Drawing_BrushDestroy 循环调用1000次，OH_Drawing_BrushDestroy
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityBrushCopy::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Brush* brush = OH_Drawing_BrushCopy(brush1);
        OH_Drawing_BrushDestroy(brush);
    }
    OH_Drawing_BrushDestroy(brush1);
};

void StabilityBrushRandInvoke::BrushIsAntiAliasTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushIsAntiAlias(brush);
}

void StabilityBrushRandInvoke::BrushGetColorTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushGetColor(brush);
}

void StabilityBrushRandInvoke::BrushGetAlphaTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushGetAlpha(brush);
}

void StabilityBrushRandInvoke::BrushGetFilterTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_BrushGetFilter(brush, filter);
    OH_Drawing_FilterDestroy(filter);
}

void StabilityBrushRandInvoke::BrushResetTest(OH_Drawing_Brush* brush)
{
    OH_Drawing_BrushReset(brush);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BRUSH_0300
@Description:Brush相关操作随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Brush相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityBrushRandInvoke::OnTestStability(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < testCount_; i++) {
        uint32_t index = rand_.nextRangeU(0, OPERATION_FUNCTION_BRUSH_MAX - 1);
        handlers_[index](brush);
    }
    OH_Drawing_BrushDestroy(brush);
};