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

#include "canvas_draw_text_blob_text.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_text_typography.h>

#include "performance.h"
#include "test_common.h"
/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0900
@Description:
    1、超大字符串绘制性能-OH_Drawing_CanvasDrawTextBlob
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、构造字符串
    5、DrawTextBlob 偏移到下一行（部分重叠），重复调用1000次
    6、仅attach pen，重复操作5
    7、仅attach brush，重复操作5
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceCanvasDrawTextBlobMax::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PerformanceCanvasDrawTextBlobMax OnTestPerformance OH_Drawing_CanvasDrawTextBlob");

    if (!canvas) {
        DRAWING_LOGE("PerformanceCanvasDrawTextBlobMax OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    std::string text = "WT";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 500); // 500 for test
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
    int drawCount = 90;
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, 5, 500); // 5 500 for test
    }
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0900
@Description:
    1、超长字符串绘制性能-OH_Drawing_CanvasDrawTextBlob
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、构造字符串
    5、DrawTextBlob 偏移到下一行（部分重叠），重复调用1000次
    6、仅attach pen，重复操作5
    7、仅attach brush，重复操作5
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceCanvasDrawTextBlobLong::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PerformanceCanvasDrawTextBlobLong OnTestPerformance OH_Drawing_CanvasDrawTextBlob");

    if (!canvas) {
        DRAWING_LOGE("PerformanceCanvasDrawTextBlobLong OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    // 2.OH_Drawing_CanvasDrawTextBlob
    std::string text = "In the heart of the bustling city stood an old bookstore, its facade weathered by time but its "
                       "spirit undimmed.";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 14); // 14 for test
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, 5, i * 10); // 5 10 for test
    }
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}
