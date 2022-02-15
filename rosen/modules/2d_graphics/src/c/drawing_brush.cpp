/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "c/drawing_brush.h"

#include "draw/brush.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Brush* CastToBrush(OH_Drawing_Brush* cBrush)
{
    return reinterpret_cast<Brush*>(cBrush);
}

static const Brush& CastToBrush(const OH_Drawing_Brush& cBrush)
{
    return reinterpret_cast<const Brush&>(cBrush);
}

OH_Drawing_Brush* OH_Drawing_BrushCreate()
{
    return (OH_Drawing_Brush*)new Brush;
}

void OH_Drawing_BrushDestroy(OH_Drawing_Brush* cBrush)
{
    delete CastToBrush(cBrush);
}

bool OH_Drawing_BrushIsAntiAlias(const OH_Drawing_Brush* cBrush)
{
    return CastToBrush(*cBrush).IsAntiAlias();
}

void OH_Drawing_BrushSetAntiAlias(OH_Drawing_Brush* cBrush, bool aa)
{
    CastToBrush(cBrush)->SetAntiAlias(aa);
}

OH_Drawing_Color OH_Drawing_BrushGetColor(const OH_Drawing_Brush* cBrush)
{
    return CastToBrush(*cBrush).GetColor().CastToColorQuad();
}

void OH_Drawing_BrushSetColor(OH_Drawing_Brush* cBrush, OH_Drawing_Color c)
{
    CastToBrush(cBrush)->SetColor(c);
}
