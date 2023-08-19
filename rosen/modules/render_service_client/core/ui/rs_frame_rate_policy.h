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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_FRAME_RATE_POLICY_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_FRAME_RATE_POLICY_H

#include <memory>
#include "transaction/rs_hgm_config_data.h"

namespace OHOS {
namespace Rosen {
class RSFrameRatePolicy {
public:
    static RSFrameRatePolicy* GetInstance();

    void RegisterHgmConfigChangeCallback();

    int GetPreferredFps(const std::string& scene, float speed);

private:
    RSFrameRatePolicy();
    ~RSFrameRatePolicy();
    RSFrameRatePolicy(const RSFrameRatePolicy&) = delete;
    RSFrameRatePolicy(const RSFrameRatePolicy&&) = delete;
    RSFrameRatePolicy &operator = (const RSFrameRatePolicy&) = delete;
    RSFrameRatePolicy &operator = (const RSFrameRatePolicy&&) = delete;

    void HgmConfigChangeCallback(std::shared_ptr<RSHgmConfigData> configData);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_FRAME_RATE_POLICY_H
