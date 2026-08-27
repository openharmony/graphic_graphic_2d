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
 
#include "display_engine/rs_display_effect_swing_control.h"
#include "pipeline/rs_surface_render_node.h"
 
namespace OHOS {
namespace Rosen {
RSDisplayEffectSwingControl& RSDisplayEffectSwingControl::Get()
{
    static RSDisplayEffectSwingControl instance;
    return instance;
}
 
RSDisplayEffectSwingControl::~RSDisplayEffectSwingControl()
{
    // destructor
}
 
void RSDisplayEffectSwingControl::SetSwingEnabled(ScreenId screenId, bool enable)
{
    // Update swing status.
}
 
bool RSDisplayEffectSwingControl::IsSwingRegistered(ScreenId screenId)
{
    return false;
}
 
SwingData RSDisplayEffectSwingControl::GetSwingData(ScreenId screenId)
{
    return SwingData{};
}
} // namespace Rosen
} // namespace OHOS