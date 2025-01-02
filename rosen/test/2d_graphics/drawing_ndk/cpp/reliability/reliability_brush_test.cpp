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

#include "reliability_brush_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_image_filter.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_shadow_layer.h>
#include <thread>

#include "test_common.h"

#include "common/log_common.h"
// 两个线程
static void threadFunction(OH_Drawing_Canvas* canvas)
{
    int drawCount = 100;
    for (int i = 0; i < drawCount; i++) {
        DrawingFunc(canvas);
    }
}

/*
@Description:detach brush后，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_CanvasAttachBrush
    4、OH_Drawing_CanvasDetachBrush
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityDetachBrush::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityDetachBrush OnTestReliability");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDetachBrush(canvas);
    DrawingFunc(canvas);
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:attach brush后调用destroy接口，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_BrushCreate、OH_Drawing_CanvasAttachBrush
    4、OH_Drawing_BrushDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityAttachBrush::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityAttachBrush OnTestReliability");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_BrushDestroy(brush);
    DrawingFunc(canvas);
}

/*
@Description:brush setShaderEffect后销毁ShaderEffect，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_BrushCreate、OH_Drawing_BrushSetShaderEffect，OH_Drawing_CanvasAttachBrush
    4、OH_Drawing_ShaderEffectDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityBrushSetShaderEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetShaderEffect OnTestReliability");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(DRAW_COLORGREEN);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    DrawingFunc(canvas);
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:brush setShadowLayer后销毁ShadowLayer，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_BrushCreate、OH_Drawing_ShadowLayer，OH_Drawing_CanvasAttachBrush
    4、OH_Drawing_BrushSetShadowLayerDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityBrushSetShadowLayer::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetShadowLayer OnTestReliability");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_ShadowLayer* shadowLayer = OH_Drawing_ShadowLayerCreate(4.0, 3, 3, 0xFFFFFFFF);
    OH_Drawing_BrushSetShadowLayer(brush, shadowLayer);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_ShadowLayerDestroy(shadowLayer);
    DrawingFunc(canvas);
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:brush setFilter后销毁Filter，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_BrushCreate、OH_Drawing_BrushSetFilter，OH_Drawing_CanvasAttachBrush
    4、OH_Drawing_BrushSetFilterDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityBrushSetFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetFilter OnTestReliability");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_FilterDestroy(filter);
    DrawingFunc(canvas);
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:brush setFilter后销毁ImageFilter，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_BrushCreate、OH_Drawing_BrushSetFilter，OH_Drawing_CanvasAttachBrush
    4、OH_Drawing_ImageFilterDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityBrushSetImageFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetImageFilter OnTestReliability");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_BrushSetFilter(brush, filter);
    float sigmaX = 10;
    float sigmaY = 10;
    OH_Drawing_ImageFilter* imageFilter =
        OH_Drawing_ImageFilterCreateBlur(sigmaX, sigmaY, OH_Drawing_TileMode::CLAMP, nullptr);
    OH_Drawing_FilterSetImageFilter(filter, imageFilter);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_ImageFilterDestroy(imageFilter);
    DrawingFunc(canvas);
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:brush setFilter后销毁MaskFilter，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_BrushCreate、OH_Drawing_BrushSetFilter，OH_Drawing_CanvasAttachBrush
    4、OH_Drawing_MaskFilterDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityBrushSetMaskFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetMaskFilter OnTestReliability");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    float sigma = 10;
    bool respectCTM = true;
    OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::SOLID, sigma, respectCTM);
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_MaskFilterDestroy(maskFilter);
    DrawingFunc(canvas);
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:brush setFilter后销毁ColorFilter，再调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind
    3、OH_Drawing_BrushCreate、OH_Drawing_BrushSetFilter，OH_Drawing_CanvasAttachBrush
    4、OH_Drawing_ColorFilterDestroy
    5、draw相关接口全调用
    6、释放内存
@Result:
    图片与基线一致
 */
void ReliabilityBrushSetColorFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetColorFilter OnTestReliability");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_ColorFilter* colorFilter =
        OH_Drawing_ColorFilterCreateBlendMode(DRAW_COLORGREEN, OH_Drawing_BlendMode::BLEND_MODE_CLEAR);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    DrawingFunc(canvas);
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:draw过程中，detach brush
@Step:
    1、启线程1创建brush
    2、启线程2 OH_Drawing_BrushCreate，传入线程1 brush
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建brush
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityDetachBrushJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityDetachBrushJoinThread OnTestReliability");
    int drawCount = 100;
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    std::thread subThread(threadFunction, canvas);
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDetachBrush(canvas);
    }
    subThread.join();
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:draw过程中，brush调用destroy接口
@Step:
    1、启线程1创建brush
    2、启线程2 OH_Drawing_BrushCreate，传入线程1 brush
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建brush
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityAttachBrushJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityAttachBrushJoinThread OnTestReliability");
    int drawCount = 100;
    std::thread subThread(threadFunction, canvas);
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_BrushDestroy(brush);
    }
    subThread.join();
}

/*
@Description:draw过程中，brush 销毁ShaderEffect
@Step:
    1、启线程1创建shaderEffect
    2、启线程2 OH_Drawing_ShaderEffectCreateColorShader，传入线程1 shaderEffect
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建shaderEffect
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityBrushSetShaderEffectJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetShaderEffectJoinThread OnTestReliability");
    int drawCount = 100;
    std::thread subThread(threadFunction, canvas);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(DRAW_COLORGREEN);
        OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
    }
    subThread.join();
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:draw过程中，brush 销毁ShadowLayer
@Step:
    1、启线程1创建shadowLayer
    2、启线程2 OH_Drawing_ShadowLayerCreate，传入线程1 shadowLayer
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建shadowLayer
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityBrushSetShadowLayerJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetShadowLayerJoinThread OnTestReliability");
    int drawCount = 100;
    std::thread subThread(threadFunction, canvas);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_ShadowLayer* shadowLayer = OH_Drawing_ShadowLayerCreate(4.0, 3, 3, 0xFFFFFFFF);
        OH_Drawing_BrushSetShadowLayer(brush, shadowLayer);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_ShadowLayerDestroy(shadowLayer);
    }
    subThread.join();
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:draw过程中，brush 销毁Filter
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ImageFilterCreateBlur，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityBrushSetFilterJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetFilterJoinThread OnTestReliability");
    int drawCount = 100;
    std::thread subThread(threadFunction, canvas);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
        OH_Drawing_BrushSetFilter(brush, filter);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_FilterDestroy(filter);
    }
    subThread.join();
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:draw过程中，brush 销毁ImageFilter
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ImageFilterCreateBlur，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityBrushSetImageFilterJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetImageFilterJoinThread OnTestReliability");
    int drawCount = 100;
    std::thread subThread(threadFunction, canvas);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
        OH_Drawing_BrushSetFilter(brush, filter);
        float sigmaX = 10;
        float sigmaY = 10;
        OH_Drawing_ImageFilter* imageFilter =
            OH_Drawing_ImageFilterCreateBlur(sigmaX, sigmaY, OH_Drawing_TileMode::CLAMP, nullptr);
        OH_Drawing_FilterSetImageFilter(filter, imageFilter);
        OH_Drawing_BrushSetFilter(brush, filter);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_FilterDestroy(filter);
        OH_Drawing_ImageFilterDestroy(imageFilter);
    }
    subThread.join();
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:draw过程中，brush 销毁MaskFilter
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_MaskFilterCreateBlur，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityBrushSetMaskFilterJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetMaskFilterJoinThread OnTestReliability");
    int drawCount = 100;
    std::thread subThread(threadFunction, canvas);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
        float sigma = 10;
        bool respectCTM = true;
        OH_Drawing_MaskFilter* maskFilter =
            OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::SOLID, sigma, respectCTM);
        OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
        OH_Drawing_BrushSetFilter(brush, filter);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_FilterDestroy(filter);
        OH_Drawing_MaskFilterDestroy(maskFilter);
    }
    subThread.join();
    OH_Drawing_BrushDestroy(brush);
}

/*
@Description:draw过程中，brush 销毁ColorFilter
@Step:
    1、启线程1创建Filter
    2、启线程2 OH_Drawing_ColorFilterCreateBlendMode，传入线程1 Filter
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Filter
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityBrushSetColorFilterJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityBrushSetColorFilterJoinThread OnTestReliability");
    int drawCount = 100;
    std::thread subThread(threadFunction, canvas);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
        OH_Drawing_ColorFilter* colorFilter =
            OH_Drawing_ColorFilterCreateBlendMode(DRAW_COLORGREEN, OH_Drawing_BlendMode::BLEND_MODE_CLEAR);
        OH_Drawing_FilterSetColorFilter(filter, colorFilter);
        OH_Drawing_BrushSetFilter(brush, filter);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_FilterDestroy(filter);
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
    subThread.join();
    OH_Drawing_BrushDestroy(brush);
}