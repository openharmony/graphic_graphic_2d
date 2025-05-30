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

#include "rs_upload_resource_thread.h"

#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"
#include "src/core/SkImagePriv.h"
#include "src/image/SkImage_Base.h"

namespace OHOS::Rosen {

void UploadTextureWithSkia(bool paraUpload, const sk_sp<SkImage>& imageUp,
    const std::shared_ptr<Media::PixelMap>& pixelMapUp, uint64_t uniqueId)
{
    auto& instance = RSUploadResourceThread::Instance();
    if (paraUpload && instance.IsEnable() && imageUp &&
        (instance.ImageSupportParallelUpload(imageUp->width(), imageUp->height()))) {
        std::function<void()> uploadTexturetask =
            [image = imageUp, pixelMap = pixelMapUp, count = instance.GetFrameCount()]() -> void {
            auto& instance = RSUploadResourceThread::Instance();
            auto grContext = instance.GetShareGrContext().get();
            if (grContext && image && pixelMap && instance.TaskIsValid(count)) {
                RS_TRACE_NAME_FMT("parallel upload texture w%d h%d", image->width(), image->height());
                SkImage_pinAsTexture(image.get(), grContext);
#ifdef RS_ENABLE_VK
                if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
                    OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
                    RSUploadResourceThread::Instance().GetShareGrContext()->submit();
                }
#endif // RS_ENABLE_VK
            }
        };
        RSUploadResourceThread::Instance().PostTask(uploadTexturetask, std::to_string(uniqueId));
    }
}
}