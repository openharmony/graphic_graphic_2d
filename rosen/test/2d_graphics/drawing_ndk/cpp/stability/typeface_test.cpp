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
#include "typeface_test.h"

#include <fstream>
#include <iostream>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_memory_stream.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_shadow_layer.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_typeface.h>
#include <native_drawing/drawing_types.h>

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TYPEFACE_0100
@Description:TypefaceCreateDefault-TypefaceDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_TypefaceCreateDefault-OH_Drawing_TypefaceDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityTypefaceCreateDefault::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityTypefaceCreateDefault OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Typeface* typeFace = OH_Drawing_TypefaceCreateDefault();
        OH_Drawing_TypefaceDestroy(typeFace);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TYPEFACE_0200
@Description:TypefaceCreateFromFile-TypefaceDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_TypefaceCreateFromFile-OH_Drawing_TypefaceDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityTypefaceCreateFromFile::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityTypefaceCreateFromFile OnTestStability");
    const char path[] = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Typeface* typeFace = OH_Drawing_TypefaceCreateFromFile(path, rand.nextULessThan(sizeof(path)));
        OH_Drawing_TypefaceDestroy(typeFace);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TYPEFACE_0300
@Description:TypefaceCreateFromStream-TypefaceDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、调用OH_Drawing_MemoryStreamCreate创建MemoryStream
    3、OH_Drawing_TypefaceCreateFromStream-OH_Drawing_TypefaceDestroy循环调用1000次
    4、执行结束后等待3分钟
    5、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityTypefaceCreateFromStream::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityTypefaceCreateFromStream OnTestStability");
    const char* path = "/system/fonts/HarmonyOS_Sans_TC.ttf";
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        DRAWING_LOGE("StabilityTypefaceCreateFromStream file open failed!!!! %{public}s", path);
        return;
    }
    ifs.seekg(0, std::ios::end);
    if (!ifs.good()) {
        ifs.close();
        DRAWING_LOGE("StabilityTypefaceCreateFromStream file seek failed!!!!");
        return;
    }

    uint64_t size = ifs.tellg();

    if (!ifs.good()) {
        ifs.close();
        DRAWING_LOGE("StabilityTypefaceCreateFromStream file seek 2 failed!!!!");
        return;
    }
    ifs.seekg(ifs.beg);
    if (!ifs.good()) {
        ifs.close();
        DRAWING_LOGE("StabilityTypefaceCreateFromStream file seek 2 failed!!!!");
        return;
    }
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
    ifs.read(buffer.get(), size);
    if (!ifs.good()) {
        ifs.close();
        DRAWING_LOGE("StabilityTypefaceCreateFromStream  file read failed!!!!");
        return;
    }

    ifs.close();
    const uint8_t* data = reinterpret_cast<uint8_t*>(buffer.get());
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_MemoryStream* memoryStream = OH_Drawing_MemoryStreamCreate(data, size, true);
        OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromStream(memoryStream, 0);
        OH_Drawing_TypefaceDestroy(typeface);
    }
}