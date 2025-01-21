/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#include "text_bundle_config_parser.h"

#ifdef OHOS_TEXT_ENABLE
#include "application_info.h"
#include "hap_module_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "utils/text_log.h"
#endif

namespace OHOS {
namespace Rosen {
namespace SPText {
#ifdef OHOS_TEXT_ENABLE
const std::string ADAPTER_TEXT_HEIGHT_META_DATA = "ohos.graphics2d.text.adapter_text_height";
const size_t VERSION_DIVISOR = 100;
#endif

bool TextBundleConfigParser::IsMetaDataExistInModule(const std::string& metaData)
{
#ifdef OHOS_TEXT_ENABLE
    for (const auto& info : bundleInfo_.hapModuleInfos) {
        for (const auto& data : info.metadata) {
            if (data.name == metaData) {
                return true;
            }
        }
    }
#endif
    return false;
}

#ifdef OHOS_TEXT_ENABLE
sptr<AppExecFwk::IBundleMgr> TextBundleConfigParser::GetSystemAbilityManager()
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        TEXT_LOGE("Failed to get System ability manager");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        TEXT_LOGE("Failed to get remote object");
        return nullptr;
    }

    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}
#endif

bool TextBundleConfigParser::IsAdapterTextHeightEnabled()
{
#ifdef OHOS_TEXT_ENABLE
    return initStatus_ && adapterTextHeightEnable_;
#else
    return false;
#endif
}

bool TextBundleConfigParser::IsTargetApiVersion(size_t targetVersion)
{
    return initStatus_ && targetApiVersionResult_ >= targetVersion;
}

#ifdef OHOS_TEXT_ENABLE
void TextBundleConfigParser::InitTextBundleConfig()
{
    initStatus_ = true;
    targetApiVersionResult_ = bundleInfo_.targetVersion % VERSION_DIVISOR;
    adapterTextHeightEnable_ = IsMetaDataExistInModule(ADAPTER_TEXT_HEIGHT_META_DATA);
    TEXT_LOGI("Adapter text height enabled: %{public}d", adapterTextHeightEnable_);
}
#endif

void TextBundleConfigParser::InitTextBundleFailed()
{
    initStatus_ = false;
    adapterTextHeightEnable_ = false;
    targetApiVersionResult_ = std::numeric_limits<uint32_t>::max();
}

void TextBundleConfigParser::InitBundleInfo()
{
#ifdef OHOS_TEXT_ENABLE
    sptr<AppExecFwk::IBundleMgr> bundleMgr = GetSystemAbilityManager();
    if (bundleMgr == nullptr) {
        TEXT_LOGE("Failed to Get bundle manager");
        InitTextBundleFailed();
        return;
    }

    ErrCode errCode = bundleMgr->GetBundleInfoForSelf(
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE) |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_METADATA),
        bundleInfo_);
    if (FAILED(errCode)) {
        TEXT_LOGE("Failed to Get bundle info, errcode: %{public}x", errCode);
        InitTextBundleFailed();
        return;
    }

    InitTextBundleConfig();
#else
    InitTextBundleFailed();
#endif
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS