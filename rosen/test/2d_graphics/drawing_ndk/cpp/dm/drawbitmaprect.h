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
#ifndef DRAWBITMAPRECT_H
#define DRAWBITMAPRECT_H
#include "test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

typedef float Scalar;
struct Rect {
    Scalar fLeft;   // !< smaller x-axis bounds
    Scalar fTop;    // !< smaller y-axis bounds
    Scalar fRight;  // !< larger x-axis bounds
    Scalar fBottom; // !< larger y-axis bounds
};

class DrawBitmapRect2 : public TestBase {
  public:
    DrawBitmapRect2();
    ~DrawBitmapRect2() override;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAWBITMAPRECT_H