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
#include "draw/sdf_shaper_impl.h"
#include <unordered_map>

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static SDFShapeImpl* CastToSDFShape(OH_Drawing_SDF* cShape)
{
    return reinterpret_cast<SDFShapeImpl*>(cShape);
}

OH_Drawing_SDF* OH_Drawing_SDFCreate()
{
    return (OH_Drawing_SDF*)new SDFShapeImpl
}

void OH_Drawing_SDFDestroy(OH_Drawing_SDF* cShape)
{
   delete CastToSDFShape(cShape);
}

void OH_Drawing_SDFBuildShader(OH_Drawing_SDF* cShape)
{
    SDFShapeImpl* shape = CastToSDFShape(cShaper);
    shape->BuildShader();
}
