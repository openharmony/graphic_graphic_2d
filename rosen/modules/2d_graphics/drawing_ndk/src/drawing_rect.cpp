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

#include "drawing_rect.h"

#include "utils/rect.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Rect* CastToRect(OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<Rect*>(cRect);
}

OH_Drawing_Rect* OH_Drawing_RectCreate(float left, float top, float right, float bottom)
{
    return (OH_Drawing_Rect*)new Rect(left, top, right, bottom);
}

void OH_Drawing_RectDestroy(OH_Drawing_Rect* cRect)
{
    delete CastToRect(cRect);
}
