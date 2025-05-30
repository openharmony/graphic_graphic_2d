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
#ifndef CIRCULAR_ARCS_H
#define CIRCULAR_ARCS_H
#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"

class CircularArcStrokeMatrix : public TestBase {
public:
    CircularArcStrokeMatrix();
    ~CircularArcStrokeMatrix() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;

private:
    std::vector<OH_Drawing_Matrix*> CreateMatrices(float coordinate);
    void ApplyRotations(std::vector<OH_Drawing_Matrix*>& matrices, int baseMatrixCnt, float coordinate);
};
#endif // CIRCULAR_ARCS_H