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

#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_rect.h>

#include "../test_base.h"

#ifndef STROKE_RECT_SHADER_H
#define STROKE_RECT_SHADER_H

enum { K_W = 690, K_H = 300 };
class StrokeRectShader : public TestBase {
public:
    StrokeRectShader()
    {
        bitmapWidth_ = K_W;
        bitmapHeight_ = K_H;
    }
    ~StrokeRectShader() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

#endif // STROKE_RECT_SHADER
