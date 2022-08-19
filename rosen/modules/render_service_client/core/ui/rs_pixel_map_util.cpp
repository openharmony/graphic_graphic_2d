/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ui/rs_pixel_map_util.h"

#include "pixel_map_rosen_utils.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
bool RSPixelMapUtil::UploadToGpuIfNeeded(
    std::shared_ptr<Media::PixelMap> pixmap, GrContext* context, bool buildMips, bool limitToMaxTextureSize)
{
    if (RSSystemProperties::GetUniRenderEnabled()) {
        return true;
    }
    return Media::PixelMapRosenUtils::UploadToGpu(pixmap, context, buildMips, limitToMaxTextureSize);
}
} // namespace Rosen
} // namespace OHOS
