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

#include "c/drawing_sdf_shaper.h"
#include "draw/sdf_shaper_base.h"
#include <unordered_map>

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static SDFShapeBase* CastToSdfShape(OH_Drawing_Sdf* cShape)
{
    return reinterpret_cast<SDFShapeBase*>(cShape);
}

OH_Drawing_Sdf* OH_Drawing_SdfCreate()
{
    return (OH_Drawing_Sdf*)new SDFShapeBase;
}

void OH_Drawing_SdfDestroy(OH_Drawing_Sdf* cShape)
{
    delete CastToSdfShape(cShape);
}

void OH_Drawing_SdfBuildShader(OH_Drawing_Sdf* cShape)
{
    SDFShapeBase* shape = CastToSdfShape(cShaper);
    shape->BuildShader();
}
