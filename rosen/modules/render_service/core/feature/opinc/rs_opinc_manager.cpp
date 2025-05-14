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

#include "rs_opinc_manager.h"
#include "feature_cfg/graphic_feature_param_manager.h"

namespace OHOS {
namespace Rosen {

RSOpincManager& RSOpincManager::Instance()
{
    static RSOpincManager instance;
    return instance;
}

void RSOpincManager::ReadOPIncCcmParam()
{
    isOPIncOn_ = OPIncParam::IsOPIncEnable();
    RS_LOGI("RSOpincManager::ReadOPIncCcmParam isOPIncOn_=%{public}d", isOPIncOn_);
    DrawableV2::RSOpincDrawCache::SetAutoCacheEnable(isOPIncOn_);
}
}
}