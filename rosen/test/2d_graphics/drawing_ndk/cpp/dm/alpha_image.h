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
#ifndef ALPHA_IMAGE_H
#define ALPHA_IMAGE_H
#include "test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_rect.h>

class AlphaImage : public TestBase {
public:
    AlphaImage();
    ~AlphaImage() override;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    float kSize = 96;
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Filter *filter = OH_Drawing_FilterCreate();
};

class AlphaImageAlphaTint : public TestBase {
public:
    AlphaImageAlphaTint();
    ~AlphaImageAlphaTint() override;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // AA_RECT_MODES_H