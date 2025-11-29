/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RS_HETERO_PICK_COLOR_MANAGER_H
#define RS_HETERO_PICK_COLOR_MANAGER_H

#include "image/image.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"
#include "feature/colorpicker/rs_color_picker_manager.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

namespace OHOS {
namespace Rosen {
class RSHeteroPickColorManager {
public:
    static RSHeteroPickColorManager& Instance();
    RSHeteroPickColorManager() = default;
    ~RSHeteroPickColorManager() = default;

    bool GetColor(const std::function<void(Drawing::ColorQuad&)>& updateColor,
        Drawing::Surface* surface, std::shared_ptr<Drawing::Image>& image);
};

} // namespace Rosen
} // namespace OHOS
#endif // RS_HETERO_PICK_COLOR_MANAGER_H