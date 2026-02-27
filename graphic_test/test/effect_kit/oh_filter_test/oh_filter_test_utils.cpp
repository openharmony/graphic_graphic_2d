/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "oh_filter_test_utils.h"
#include "pixelmap_native_impl.h"
#include "rs_graphic_test_img.h"

namespace OHOS {
namespace Rosen {
OH_PixelmapNative* CreateTestPixelMap(const std::string path)
{
    std::shared_ptr<Media::PixelMap> pixelMap = DecodePixelMap(path, Media::AllocatorType::SHARE_MEM_ALLOC);
    return new OH_PixelmapNative(pixelMap);
}

OH_Filter* CreateFilter(OH_PixelmapNative* pixelMapNative)
{
    OH_Filter* filter = nullptr;
    OH_Filter_CreateEffect(pixelMapNative, &filter);
    return filter;
}

}  // namespace Rosen
}  // namespace OHOS