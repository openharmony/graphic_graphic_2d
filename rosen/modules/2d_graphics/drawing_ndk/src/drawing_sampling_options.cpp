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

#include "drawing_sampling_options.h"
#include "utils/sampling_options.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static SamplingOptions* CastToSamplingOptions(OH_Drawing_SamplingOptions* cSamplingOptions)
{
    return reinterpret_cast<SamplingOptions*>(cSamplingOptions);
}

OH_Drawing_SamplingOptions* OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode fm, OH_Drawing_MipmapMode mm)
{
    return (OH_Drawing_SamplingOptions*)new SamplingOptions(static_cast<FilterMode>(fm), static_cast<MipmapMode>(mm));
}

void OH_Drawing_SamplingOptionsDestroy(OH_Drawing_SamplingOptions* cSamplingOptions)
{
    delete CastToSamplingOptions(cSamplingOptions);
}