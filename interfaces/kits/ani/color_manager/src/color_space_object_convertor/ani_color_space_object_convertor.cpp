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
#include "ani_color_space_object_convertor.h"

#include "ani_color_space_manager.h"
#include "ani_color_space_utils.h"

namespace OHOS {
namespace ColorManager {
std::shared_ptr<ColorSpace> GetColorSpaceByAniObject(ani_env *env, ani_object object)
{
    if (object == nullptr) {
        ACMLOGE("[ANI]GetColorSpaceByAniObject::object is nullptr");
        return nullptr;
    }
    AniColorSpaceManager* aniColorSpaceManager =  AniColorSpaceManager::unwrap(env, object);
    if (aniColorSpaceManager == nullptr) {
        ACMLOGE("[ANI]GetColorSpaceByAniObject::ani color space manager is nullptr");
        return nullptr;
    }
    return aniColorSpaceManager->GetColorSpaceToken();
}
}  // namespace ColorManager
}  // namespace OHOS
