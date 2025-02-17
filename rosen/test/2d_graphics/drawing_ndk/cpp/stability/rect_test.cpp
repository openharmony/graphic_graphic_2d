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

#include "rect_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_rect.h>

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_RECT_0100
@Description:RectCreate-RectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_RectCreate-OH_Drawing_RectDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRectCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityRectCreate OnTestStability");
    TestRend rand = TestRend();
    float sMin = 0;     // 0 for test
    float sMax = 100;   // 100 for test
    float endMin = 200; // 200 for test
    float endMax = 300; // 300 for test
    float left = rand.nextRangeF(sMin, sMax);
    float top = rand.nextRangeF(sMin, sMax);
    float right = rand.nextRangeF(endMin, endMax);
    float bottom = rand.nextRangeF(endMin, endMax);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
        OH_Drawing_RectDestroy(rect);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_RECT_0200
@Description:RectCopy-RectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_RectCopy-OH_Drawing_RectDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRectCopy::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityRectCopy OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 200, 200);        // 0, 0, 200,200 for test
        OH_Drawing_Rect* rectDst = OH_Drawing_RectCreate(200, 200, 500, 500); // 200, 200, 500, 500 for test
        OH_Drawing_RectCopy(rect, rectDst);
        OH_Drawing_RectDestroy(rect);
        OH_Drawing_RectDestroy(rectDst);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_RECT_0300
@Description:RectCreate-相关配置接口全调用-RectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、RectCreate-相关配置接口全调用-RectDestroy循环
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRectAll::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityRectAll OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        index_ = i;
        uint32_t funcIndexMax = SETTING_FUNCTION_RECT_SET_DESTROY;
        for (int j = 0; j <= funcIndexMax; j++) {
            handlers_[j](rect_);
        }
    }
}

void StabilityRectAll::RectCreateTest(OH_Drawing_Rect* rect)
{
    float sMin = 0;
    float sMax = 100;   // 100 for test
    float endMin = 200; // 200 for test
    float endMax = 300; // 300 for test
    float left = rand_.nextRangeF(sMin, sMax);
    float top = rand_.nextRangeF(sMin, sMax);
    float right = rand_.nextRangeF(endMin, endMax);
    float bottom = rand_.nextRangeF(endMin, endMax);
    rect = OH_Drawing_RectCreate(left, top, right, bottom);
}

void StabilityRectAll::RectSetLeftTest(OH_Drawing_Rect* rect)
{
    float leftPos = 560; // 560 for test
    OH_Drawing_RectSetLeft(rect, leftPos);
}

void StabilityRectAll::RectSetTopTest(OH_Drawing_Rect* rect)
{
    float topPos = 560; // 560 for test
    OH_Drawing_RectSetTop(rect, topPos);
}

void StabilityRectAll::RectSetRightTest(OH_Drawing_Rect* rect)
{
    float rightPos = 580; // 580 for test
    OH_Drawing_RectSetRight(rect, rightPos);
}

void StabilityRectAll::RectSetBottomTest(OH_Drawing_Rect* rect)
{
    float bottomPos = 600; // 600 for test
    OH_Drawing_RectSetBottom(rect, bottomPos);
}

void StabilityRectAll::RectCopyTest(OH_Drawing_Rect* rect)
{
    OH_Drawing_RectCopy(rect, rect);
}

void StabilityRectAll::RectDestroyTest(OH_Drawing_Rect* rect)
{
    OH_Drawing_RectDestroy(rect);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_RECT_0400
@Description:Rect相关操作接口随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Rect相关操作接口随机循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRectRandom::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityRectRandom OnTestStability");
    TestRend rand = TestRend();
    for (int i = 0; i < testCount_; i++) {
        uint32_t funcIndexMax = OPERATION_FUNCTION_RECT_GET_WIDTH;
        uint32_t index = rand.nextRangeU(0, funcIndexMax);
        handlers_[index](rect_);
    }
}

void StabilityRectRandom::RectIntersectTest(OH_Drawing_Rect* rect)
{
    bool rectIsIntersect = OH_Drawing_RectIntersect(rect, other_);
}

void StabilityRectRandom::RectJoinTest(OH_Drawing_Rect* rect)
{
    bool rectIsJoin = OH_Drawing_RectJoin(rect, other_);
}

void StabilityRectRandom::RectGetLeftTest(OH_Drawing_Rect* rect)
{
    float getLeft = OH_Drawing_RectGetLeft(rect);
}

void StabilityRectRandom::RectGetTopTest(OH_Drawing_Rect* rect)
{
    float getTop = OH_Drawing_RectGetTop(rect);
}

void StabilityRectRandom::RectGetRightTest(OH_Drawing_Rect* rect)
{
    float getRight = OH_Drawing_RectGetRight(rect);
}

void StabilityRectRandom::RectGetBottomTest(OH_Drawing_Rect* rect)
{
    float getBottom = OH_Drawing_RectGetBottom(rect);
}

void StabilityRectRandom::RectGetHeightTest(OH_Drawing_Rect* rect)
{
    float getHeight = OH_Drawing_RectGetHeight(rect);
}

void StabilityRectRandom::RectGetWidthTest(OH_Drawing_Rect* rect)
{
    float getWidth = OH_Drawing_RectGetWidth(rect);
}