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
bool TextBundleConfigParser::IsMetaDataExistInEntryModule(const std::string& metaData)
{
    auto bundleMgr = GetSystemAbilityManager();
    if (bundleMgr == nullptr) {
        TEXT_LOGE("Bundle manager is nullptr");
        return false;
    }

    AppExecFwk::BundleInfo bundleInfo;
    ErrCode errCode = bundleMgr->GetBundleInfoForSelf(
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE) |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_METADATA),
        bundleInfo);
    if (FAILED(errCode)) {
        TEXT_LOGE("Get bundle info for self failed, errcode: %{public}x", errCode);
        return false;
    }

    auto infoIter = std::find_if(bundleInfo.hapModuleInfos.begin(), bundleInfo.hapModuleInfos.end(),
        [](const AppExecFwk::HapModuleInfo info) {
            return info.moduleType == AppExecFwk::ModuleType::ENTRY;
        });
    if (infoIter == bundleInfo.hapModuleInfos.end()) {
        TEXT_LOGD("Entry module not found");
        return false;
    }

    auto& dataList = infoIter->metadata;
    return std::find_if(dataList.begin(), dataList.end(),
        [&metaData](const AppExecFwk::Metadata& data) {
            return data.name == metaData;
        }) != dataList.end();
}

sptr<AppExecFwk::IBundleMgr> TextBundleConfigParser::GetSystemAbilityManager()
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        TEXT_LOGE("System ability manager is nullptr");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        TEXT_LOGE("Remote object is nullptr");
        return nullptr;
    }

    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}
#endif

bool TextBundleConfigParser::IsAdapterTextHeightEnabled()
{
#ifdef OHOS_TEXT_ENABLE
    static bool adapterTextHeight = []() {
        const std::string ADAPTER_TEXT_HEIGHT_META_DATA = "ohos.graphics2d.text.adapter_text_height";
        auto enabled = IsMetaDataExistInEntryModule(ADAPTER_TEXT_HEIGHT_META_DATA);
        TEXT_LOGD("Adapter text height enabled: %{public}d", enabled);
        return enabled;
    }();
    return adapterTextHeight;
#else
    return false;
#endif
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS