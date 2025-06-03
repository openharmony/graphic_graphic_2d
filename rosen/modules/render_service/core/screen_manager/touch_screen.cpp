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

#include "touch_screen.h"

#include "platform/common/rs_log.h"
#ifdef TP_FEATURE_ENABLE
#include "v1_1/itp_interfaces.h"
#endif

namespace OHOS {
namespace Rosen {
TouchScreen::TouchScreen() = default;
TouchScreen::~TouchScreen() = default;

#ifdef TP_FEATURE_ENABLE
int32_t TouchScreen::SetFeatureConfig(int32_t feature, const char *config)
{
    int32_t code = -1;
    auto client = OHOS::HDI::Tp::V1_1::ITpInterfaces::Get(false);
    if (client == nullptr) {
        RS_LOGW("TouchScreen %{public}s: client failed.", __func__);
        return code;
    }
    if (client->HwSetFeatureConfig(feature, config, code) < 0) {
        RS_LOGW("TouchScreen %{public}s: HwSetFeatureConfig failed.", __func__);
    }
    return code;
}

int32_t TouchScreen::SetAftConfig(const char *config)
{
    int32_t code = -1;
    auto client = OHOS::HDI::Tp::V1_1::ITpInterfaces::Get(false);
    if (client == nullptr) {
        RS_LOGW("TouchScreen %{public}s: client failed.", __func__);
        return code;
    }
    if (client->HwTsSetAftConfig(config, code) < 0) {
        RS_LOGW("TouchScreen %{public}s: HwTsSetAftConfig failed.", __func__);
    }
    return code;
}
#endif
} // namespace Rosen
} // namespace OHOS
