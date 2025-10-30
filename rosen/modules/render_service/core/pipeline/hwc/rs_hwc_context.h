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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_HWC_CONTEXT_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_HWC_CONTEXT_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "hgm_command.h"

namespace OHOS::Rosen {
class RSHWCContext {
public:
    RSHWCContext(const std::unordered_map<std::string, std::string>& sourceTuningConfig,
        const std::unordered_map<std::string, std::string>& solidLayerConfig)
        : hwcSourceTuningConfig_(sourceTuningConfig), hwcSolidLayerConfig_(solidLayerConfig) {};
    ~RSHWCContext() = default;

    RSHWCContext(const RSHWCContext&) = delete;
    RSHWCContext(const RSHWCContext&&) = delete;
    RSHWCContext& operator=(const RSHWCContext&) = delete;
    RSHWCContext& operator=(const RSHWCContext&&) = delete;

    // use in temporary scheme with background alpha
    void CheckPackageChange(const std::vector<std::string>& pkgs);

    std::unordered_map<std::string, std::string>& GetMutableSourceTuningConfig()
    {
        return sourceTuningConfig_;
    }
    std::unordered_map<std::string, std::string>& GetMutableSolidLayerConfig()
    {
        return solidLayerConfig_;
    }

private:
    std::unordered_map<std::string, std::string> sourceTuningConfig_;
    std::unordered_map<std::string, std::string> solidLayerConfig_;
    std::unordered_map<std::string, std::string> hwcSourceTuningConfig_;
    std::unordered_map<std::string, std::string> hwcSolidLayerConfig_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_HWC_CONTEXT_H