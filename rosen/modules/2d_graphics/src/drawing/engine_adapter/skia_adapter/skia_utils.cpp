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

#include "include/gpu/vk/GrVkGraphicCoreTrace.h"
#include "utils/graphic_coretrace.h"
#include "skia_utils.h"

#include "src/core/SkUtils.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void SkiaUtils::SetVmaCacheStatus(bool flag)
{
    SkSetVmaCacheFlag(flag);
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