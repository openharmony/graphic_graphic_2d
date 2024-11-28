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

#include "pen_exception_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_error_code.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_image_filter.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_shadow_layer.h>
#include <native_drawing/drawing_text_blob.h>
#include <thread>

#include "test_common.h"

#include "common/log_common.h"
/*
@Description:detach pen后，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_CanvasAttachPen
    4、OH_Drawing_CanvasDetachPen
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityDetachPen::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityDetachPen OnTestReliability");
    // 3.OH_Drawing_CanvasAttachPen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    // 10, pen width for test
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 4.OH_Drawing_CanvasDetachPen
    OH_Drawing_CanvasDetachPen(canvas);
    // 5.draw相关接口全调用
    DrawingFunc(canvas);
    // 6.释放内存
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:attach pen后调用destroy接口，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_PenCreate、OH_Drawing_CanvasAttachPen
    4、OH_Drawing_PenDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityDestroyPen::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityDestroyPen OnTestReliability");
    // 3.OH_Drawing_CanvasAttachPen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    // 10, pen width for test
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 4.OH_Drawing_PenDestroy
    OH_Drawing_PenDestroy(pen);
    // 5.draw相关接口全调用
    DrawingFunc(canvas);
}
/*
@Description:pen setShaderEffect后销毁ShaderEffect，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_PenCreate、OH_Drawing_PenSetShaderEffect，OH_Drawing_CanvasAttachPen
    4、OH_Drawing_ShaderEffectDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilitySetShaderEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilitySetShaderEffect OnTestReliability");
    // 3.OH_Drawing_PenSetShaderEffect&&OH_Drawing_CanvasAttachPen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFF0000);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 4.OH_Drawing_ShaderEffectDestroy
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    // 5.draw相关接口全调用
    DrawingFunc(canvas);
    // 6.释放内存
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:brush setShadowLayer后销毁ShadowLayer，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_PenCreate、OH_Drawing_ShadowLayer，OH_Drawing_CanvasAttachPen
    4、OH_Drawing_PenSetShadowLayerDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilitySetShadowLayer::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilitySetShadowLayer OnTestReliability");
    // 3.OH_Drawing_PenSetShaderEffect&&OH_Drawing_ShadowLayer
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    TestRend rand;
    // 1,10,-5,10,-5,10 for test
    OH_Drawing_ShadowLayer* shadowLayer = OH_Drawing_ShadowLayerCreate(
        rand.nextRangeF(1, 10), rand.nextRangeF(-5, 10), rand.nextRangeF(-5, 10), rand.nextULessThan(0xffffffff));
    OH_Drawing_PenSetShadowLayer(pen, shadowLayer);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 4.OH_Drawing_ShadowLayerDestroy()
    OH_Drawing_ShadowLayerDestroy(shadowLayer);
    // 5.draw相关接口全调用
    DrawingFunc(canvas);
    // 6.释放内存
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:pen SetPathEffect后销毁PathEffect，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_PenCreate、OH_Drawing_PenSetShadowLayer，OH_Drawing_CanvasAttachPen
    4、OH_Drawing_PenSetPathEffectDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilitySetPathEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilitySetPathEffect OnTestReliability");
    // 3.OH_Drawing_PenSetPathEffect
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    TestRend rand;
    // 1,30 for test
    float intervals = rand.nextRangeF(1, 30);
    // 10,1,10 for test
    OH_Drawing_PathEffect* pathEffect =
        OH_Drawing_CreateDashPathEffect(&intervals, rand.nextULessThan(10), rand.nextRangeF(1, 10));
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 4.OH_Drawing_PathEffectDestroy
    OH_Drawing_PathEffectDestroy(pathEffect);
    // 5.draw相关接口全调用
    DrawingFunc(canvas);
    // 6.释放内存
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:pen setFilter后销毁Filter，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_PenCreate、OH_Drawing_PenSetFilter，OH_Drawing_CanvasAttachPen
    4、OH_Drawing_PenSetFilterDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilitySetFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilitySetFilter OnTestReliability");
    // 3.OH_Drawing_PenSetFilter
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 4.OH_Drawing_PathEffectDestroy
    OH_Drawing_FilterDestroy(filter);
    // 5.draw相关接口全调用
    DrawingFunc(canvas);
    // 6.释放内存
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:pen setFilter后销毁ImageFilter，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_PenCreate、OH_Drawing_PenSetFilter，OH_Drawing_CanvasAttachPen
    4、OH_Drawing_ImageFilterDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityImageFilterDestroy::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityImageFilterDestroy OnTestReliability");
    // 3.H_Drawing_ImageFilterCreateBlur()
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    // 10,20 for test
    OH_Drawing_ImageFilter* imageFilter = OH_Drawing_ImageFilterCreateBlur(10, 20, CLAMP, nullptr);
    // 10,20 for test
    OH_Drawing_ImageFilter* imageFilter1 = OH_Drawing_ImageFilterCreateBlur(10, 20, CLAMP, imageFilter);
    OH_Drawing_FilterSetImageFilter(filter, imageFilter1);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 4.OH_Drawing_ImageFilterDestroy
    OH_Drawing_ImageFilterDestroy(imageFilter);
    OH_Drawing_ImageFilterDestroy(imageFilter1);
    // 5.draw相关接口全调用
    DrawingFunc(canvas);
    // 6.释放内存
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:pen setFilter后销毁MaskFilter，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_PenCreate、OH_Drawing_PenSetFilter，OH_Drawing_CanvasAttachPen
    4、OH_Drawing_MaskFilterDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityMaskFilterDestroy::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityMaskFilterDestroy OnTestReliability");
    // 3.创建maskFilter，再把maskFilter设置给filter,filter再设置给pen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    float sigma = 10.f; // 10.f 高斯模糊的标准偏差
    OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(NORMAL, sigma, true);
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 4.OH_Drawing_MaskFilterDestroy
    OH_Drawing_MaskFilterDestroy(maskFilter);
    // 5.draw相关接口全调用
    DrawingFunc(canvas);
    // 6.释放内存
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:pen setFilter后销毁ColorFilter，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_PenCreate、OH_Drawing_PenSetFilter，OH_Drawing_CanvasAttachPen
    4、OH_Drawing_ColorFilterDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityColorFilterDestroy::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityColorFilterDestroy OnTestReliability");
    // 3.创建maskFilter，再把maskFilter设置给filter,filter再设置给pen
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter =
        OH_Drawing_ColorFilterCreateBlendMode(DRAW_COLORBLUE, OH_Drawing_BlendMode::BLEND_MODE_CLEAR);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // 4.OH_Drawing_ColorFilterDestroy
    OH_Drawing_ColorFilterDestroy(colorFilter);
    // 5.draw相关接口全调用
    DrawingFunc(canvas);
    // 6.释放内存
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_PenDestroy(pen);
}

static void threadFunction(OH_Drawing_Canvas* canvas)
{
    // 20, test count
    for (int i = 0; i < 20; i++) {
        DrawingFunc(canvas);
    }
}
/*
@Description:draw过程中，detach pen
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ImageFilterCreateBlur，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityThreadDetachPen::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityThreadDetachPen OnTestReliability");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    std::thread t(threadFunction, canvas);
    // 300, test count
    for (int i = 0; i < 300; i++) {
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDetachPen(canvas);
    }
    t.join();
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:draw过程中，pen调用destroy接口
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ImageFilterCreateBlur，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityThreadDestroyPen::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityThreadDestroyPen OnTestReliability");
    std::thread t(threadFunction, canvas);
    // 300, test count
    for (int i = 0; i < 300; i++) {
        OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_PenDestroy(pen);
    }
    t.join();
}
/*
@Description:draw过程中，pen 销毁ShaderEffect
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ShaderEffectCreateColorShader，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityThreadDestroyShaderEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityThreadDestroyShaderEffect OnTestReliability");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    std::thread t(threadFunction, canvas);
    // 300, test count
    for (int i = 0; i < 300; i++) {
        OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(0xFFFF0000);
        OH_Drawing_PenSetShaderEffect(pen, shaderEffect);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
    }
    t.join();
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:draw过程中，pen 销毁ShadowLayer
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ShadowLayerCreate，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityThreadDestroyShadowLayer::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityThreadDestroyShadowLayer OnTestReliability");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    TestRend rand;
    std::thread t(threadFunction, canvas);
    int drawCount = 300;
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_ShadowLayer* shadowLayer = OH_Drawing_ShadowLayerCreate(
            rand.nextRangeF(1, 10), rand.nextRangeF(-5, 10), rand.nextRangeF(-5, 10), rand.nextULessThan(0xffffffff));
        OH_Drawing_PenSetShadowLayer(pen, shadowLayer);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_ShadowLayerDestroy(shadowLayer);
    }
    t.join();
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:draw过程中，pen 销毁PathEffect
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_CreateDashPathEffect，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityThreadDestroyPathEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityThreadDestroyPathEffect OnTestReliability");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    TestRend rand;
    std::thread t(threadFunction, canvas);
    // 300, test count
    for (int i = 0; i < 300; i++) {
        // 1,30, for test
        float intervals = rand.nextRangeF(1, 30);
        // 1,10, for test
        OH_Drawing_PathEffect* pathEffect =
            OH_Drawing_CreateDashPathEffect(&intervals, rand.nextULessThan(10), rand.nextRangeF(1, 10));
        OH_Drawing_PenSetPathEffect(pen, pathEffect);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_PathEffectDestroy(pathEffect);
    }
    t.join();
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:draw过程中，pen 销毁Filter
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ImageFilterCreateBlur，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityThreadDestroyFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityThreadDestroyFilter OnTestReliability");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    std::thread t(threadFunction, canvas);
    // 300, test count
    for (int i = 0; i < 300; i++) {
        OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
        OH_Drawing_PenSetFilter(pen, filter);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_FilterDestroy(filter);
    }
    t.join();
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:draw过程中，pen 销毁ImageFilter
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ImageFilterCreateBlur，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityThreadDestroyImageFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityThreadDestroyImageFilter OnTestReliability");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    std::thread t(threadFunction, canvas);
    // 300, test count
    for (int i = 0; i < 300; i++) {
        // 10,20, for test
        OH_Drawing_ImageFilter* imageFilter = OH_Drawing_ImageFilterCreateBlur(10, 20, CLAMP, nullptr);
        // 10,20, for test
        OH_Drawing_ImageFilter* imageFilter1 = OH_Drawing_ImageFilterCreateBlur(10, 20, CLAMP, imageFilter);
        OH_Drawing_FilterSetImageFilter(filter, imageFilter1);
        OH_Drawing_PenSetFilter(pen, filter);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_ImageFilterDestroy(imageFilter);
        OH_Drawing_ImageFilterDestroy(imageFilter1);
    }
    t.join();
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:draw过程中，pen 销毁MaskFilter
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_MaskFilterCreateBlur，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityThreadDestroyMaskFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityThreadDestroyMaskFilter OnTestReliability");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    float sigma = 10.f; // 10.f 高斯模糊的标准偏差
    std::thread t(threadFunction, canvas);
    // 500, test count
    for (int i = 0; i < 500; i++) {
        OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(NORMAL, sigma, true);
        OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
        OH_Drawing_PenSetFilter(pen, filter);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_MaskFilterDestroy(maskFilter);
    }
    t.join();
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_PenDestroy(pen);
}
/*
@Description:draw过程中，pen 销毁ColorFilter
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ColorFilterCreateBlendMode，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityThreadDestroyColorFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityThreadDestroyColorFilter OnTestReliability");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    std::thread t(threadFunction, canvas);
    // 500, test count
    for (int i = 0; i < 500; i++) {
        OH_Drawing_ColorFilter* colorFilter =
            OH_Drawing_ColorFilterCreateBlendMode(DRAW_COLORBLUE, OH_Drawing_BlendMode::BLEND_MODE_CLEAR);
        OH_Drawing_FilterSetColorFilter(filter, colorFilter);
        OH_Drawing_PenSetFilter(pen, filter);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
    t.join();
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_PenDestroy(pen);
}