/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "c/drawing_matrix.h"

#include "utils/matrix.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Matrix* CastToMatrix(OH_Drawing_Matrix* cMatrix)
{
    return reinterpret_cast<Matrix*>(cMatrix);
}

OH_Drawing_Matrix* OH_Drawing_MatrixCreate()
{
    return (OH_Drawing_Matrix*)new Matrix();
}

void OH_Drawing_MatrixSetMatrix(OH_Drawing_Matrix* cMatrix, float scaleX, float skewX, float transX,
    float skewY, float scaleY, float transY, float persp0, float persp1, float persp2)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->SetMatrix(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
}

void OH_Drawing_MatrixDestroy(OH_Drawing_Matrix* cMatrix)
{
    delete CastToMatrix(cMatrix);
}
