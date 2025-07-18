/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cj_ui_effect_log.h"
#include "cj_visual_effect.h"
#include "effect/include/brightness_blender.h"

namespace OHOS {
namespace Rosen {
using namespace UIEffect;
CJVisualEffect::CJVisualEffect()
{
    m_EffectObj = std::make_shared<VisualEffect>();
    UIEFFECT_LOG_D("CJVisualEffect create VisualEffect success.");
}
} // namespace Rosen
} // namespace OHOS