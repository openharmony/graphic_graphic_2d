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
#ifndef ALPHA_GRADIENTS_H
#define ALPHA_GRADIENTS_H
#include "test_base.h"
#include "test_common.h"
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

typedef uint32_t SkColor;
typedef unsigned int U8CPU;

class AlphaGradients : public TestBase {
public:
    AlphaGradients();
    ~AlphaGradients() override;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void draw_grad(OH_Drawing_Canvas *canvas, DrawRect &r, uint32_t c0, uint32_t c1, bool doPreMul);
};
#endif // ALPHA_GRADIENTS_H