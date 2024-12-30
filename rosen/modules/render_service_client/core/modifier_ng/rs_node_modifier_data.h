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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_NODE_MODIFIER_DATA_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_NODE_MODIFIER_DATA_H

#include <array>
#include <memory>

#include "modifier_ng/rs_modifier_manager.h"

// class RSModifierManager;
// class RSBackgroundModifierManager;
// class RSContentModifierManager;
// class RSForegroundModifierManager;
// class RSOverlayModifierManager;
// class RSAppearanceModifierManager;

namespace OHOS::Rosen {
namespace ModifierNG {
enum class RSModifierCategory : uint8_t {
    Geometry = 0,
    Background,
    Content,
    Foreground,
    Overlay,
    Appearance,
    COUNT,
};

class RSC_EXPORT RSNodeModifierData {
public:
    RSNodeModifierData();
    ~RSNodeModifierData() {}

    std::array<std::shared_ptr<RSModifierManagerNG>, static_cast<size_t>(RSModifierCategory::COUNT)> managers_ = {
        nullptr
    };
};
} // namespace ModifierNG
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_NODE_MODIFIER_DATA_H