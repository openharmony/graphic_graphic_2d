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

#include "feature/hyper_graphic_manager/hgm_rp_context.h"

#include "hgm_xml_parser.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* HGM_CONFIG_PATH = "/sys_prod/etc/graphic/hgm_policy_config.xml";
}

HgmRPContext::HgmRPContext()
{
    InitHgmConfig();

    hgmRPEnergy_ = std::make_shared<HgmRPEnergy>();
    convertFrameRateFunc_ = [this](const RSPropertyUnit unit, float velocity, int32_t area, int32_t length) -> int32_t {
        return rpFrameRatePolicy_.GetExpectedFrameRate(unit, velocity, area, length);
    };
}

int32_t HgmContext::InitHgmConfig(std::unordered_map<std::string, std::string>& sourceTuningConfig,
    std::unordered_map<std::string, std::string>& solidLayerConfig, std::vector<std::string>& appBufferList)
{
    auto parser = std::make_unique<RPHgmXMLParser>();
    if (parser->LoadConfiguration(HGM_CONFIG_PATH) != EXEC_SUCCESS) {
        HGM_LOGW("HgmRPContext failed to load hgm xml configuration file");
        return XML_FILE_LOAD_FAIL;
    }
    sourceTuningConfig = parser->GetSourceTuningConfig();
    solidLayerConfig = parser->GetSolidLayerConfig();
    appBufferList = parser->GetAppBufferList();

    return EXEC_SUCCESS;
}

void HgmRPContext::SetServiceToProcessInfo(sptr<HgmServiceToProcessInfo> serviceToProcessInfo,
    uint32_t *pendingScreenRefreshRate, uint64_t *pendingConstraintRelativeTime)
{
    if (serviceToProcessInfo == nullptr) {
        HGM_LOGW("HgmRPContext serviceToProcessInfo is null");
        return;
    }

    RS_TRACE_NAME_FMT("HgmRPContext::SetServiceToProcessInfo %d, %s",
        serviceToProcessInfo->pendingScreenRefreshRate, serviceToProcessInfo->hgmDataChangeTypes.to_string().c_str());

    *pendingScreenRefreshRate = serviceToProcessInfo->pendingScreenRefreshRate;
    *pendingConstraintRelativeTime = serviceToProcessInfo->pendingConstraintRelativeTime;

    if (serviceToProcessInfo->hgmDataChangeTypes.test(HgmDataChangeType::ADAPTIVE_VSYNC)) {
        isAdaptive_ = serviceToProcessInfo->isAdaptive;
        gameNodeName_ = serviceToProcessInfo->gameNodeName;
    }

    if (serviceToProcessInfo->hgmDataChangeTypes.test(HgmDataChangeType::HGM_CONFIG_DATA)) {
        ltpoEnabled_ = serviceToProcessInfo->ltpoEnabled;
        isDelayMode_ = serviceToProcessInfo->isDelayMode;
        pipelineOffsetPulseNum_ = serviceToProcessInfo->pipelineOffsetPulseNum;
        rpFrameRatePolicy_.HgmConfigUpdateCallback(serviceToProcessInfo->rpHgmConfigData);
        hgmRPEnergy_->HgmConfigUpdateCallback(serviceToProcessInfo->rpHgmConfigData);
    }
    hgmRPEnergy_->SetTouchState(serviceToProcessInfo->isPowerIdle);
}
} // namespace Rosen
} // namespace OHOS