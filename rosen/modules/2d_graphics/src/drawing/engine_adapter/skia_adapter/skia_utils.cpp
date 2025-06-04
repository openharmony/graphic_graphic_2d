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

#ifndef USE_M133_SKIA
#include "include/gpu/vk/GrVkGraphicCoreTrace.h"
#endif
#include "utils/graphic_coretrace.h"
#include "skia_utils.h"

#ifdef USE_M133_SKIA
#include "src/base/SkUtils.h"
#else
#include "src/core/SkUtils.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

void SkiaUtils::SetVmaCacheStatus(bool flag)
{
#ifndef USE_M133_SKIA
    SkSetVmaCacheFlag(flag);
#endif
}

void SkiaUtils::RecordCoreTrace(int functionType)
{
#if defined(SK_VULKAN) && defined(SKIA_DFX_FOR_GPURESOURCE_CORETRACE)
    GraphicCoreTrace::RecordCoreTrace(functionType);
#endif
}

void SkiaUtils::RecordCoreTrace(int functionType, uint64_t nodeId)
{
#if defined(SK_VULKAN) && defined(SKIA_DFX_FOR_GPURESOURCE_CORETRACE)
    GraphicCoreTrace::RecordCoreTrace(functionType, nodeId);
#endif
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS