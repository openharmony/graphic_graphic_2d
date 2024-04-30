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

#ifndef GRADIENTSGM_H
#define GRADIENTSGM_H
#include <bits/alltypes.h>
#include <ctime>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>

#include "../test_base.h"
#include "test_common.h"

#include "common/log_common.h"

typedef float Scalar;
typedef uint32_t Color;
#define SCALAR_HALF 0.5f // default
#define SCALAR1 1.0f     // default
#define SCALAR_AVE(a, b) (((a) + (b)) * SCALAR_HALF)

template<typename T, size_t N>
char (&SkArrayCountHelper(T (&array)[N]))[N];
#define SK_ARRAY_COUNT(array) (sizeof(SkArrayCountHelper(array)))

class Gradients : public TestBase {
    inline Scalar IntToScalar(int x)
    {
        return static_cast<Scalar>(x);
    };

public:
    Gradients();
    ~Gradients() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    //  渐变的起点和终点坐标
    OH_Drawing_Point* firstPoint = OH_Drawing_PointCreate(0, 0); // 0, 0  创建Point
    OH_Drawing_Point* secondPoint = OH_Drawing_PointCreate(100.0, 100.0);//100.0, 100.0 创建点
};
#endif // GRADIENTSGM_H