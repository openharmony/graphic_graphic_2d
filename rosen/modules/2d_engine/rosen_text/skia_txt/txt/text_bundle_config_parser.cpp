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
#ifdef USE_M133_SKIA
#include "modules/skparagraph/include/TextGlobalConfig.h"
#endif

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

bool TextBundleConfigParser::IsMetaDataExistInModule(const std::string& metaData,
    const AppExecFwk::BundleInfo& bundleInfo)
{
    for (const auto& info : bundleInfo.hapModuleInfos) {
        for (const auto& data : info.metadata) {
            if (data.name == metaData) {
                return true;
            }
        }
    }
    return false;
}

bool TextBundleConfigParser::GetBundleInfo(AppExecFwk::BundleInfo& bundleInfo)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        TEXT_LOGE("Failed to get system ability manager");
        return false;
    }

    sptr<IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        TEXT_LOGE("Failed to get remote object");
        return false;
    }

    sptr<AppExecFwk::IBundleMgr> bundleMgr =
        iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (bundleMgr == nullptr) {
        TEXT_LOGE("Failed to get bundle manager");
        return false;
    }
    ErrCode errCode = bundleMgr->GetBundleInfoForSelf(
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE) |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_METADATA),
        bundleInfo);
    if (errCode != ERR_OK) {
        TEXT_LOGE("Failed to get bundle info, errcode: %{public}x", errCode);
        return false;
    }
    return true;
}
#endif

bool TextBundleConfigParser::IsAdapterTextHeightEnabled() const
{
    return initStatus_ && adapterTextHeightEnable_;
}

bool TextBundleConfigParser::IsTargetApiVersion(size_t targetVersion) const
{
    return initStatus_ && bundleApiVersion_ >= targetVersion;
}

#ifdef OHOS_TEXT_ENABLE
void TextBundleConfigParser::InitTextBundleConfig()
{
    AppExecFwk::BundleInfo bundleInfo;
    if (!GetBundleInfo(bundleInfo)) {
        InitTextBundleFailed();
        return;
    }

    initStatus_ = true;
    bundleApiVersion_ = bundleInfo.targetVersion % VERSION_DIVISOR;
    adapterTextHeightEnable_ = IsMetaDataExistInModule(ADAPTER_TEXT_HEIGHT_META_DATA, bundleInfo);
    TEXT_LOGI("Adapter text height enabled %{public}d", adapterTextHeightEnable_);
}
#endif

void TextBundleConfigParser::InitTextBundleFailed()
{
    initStatus_ = false;
    adapterTextHeightEnable_ = false;
    bundleApiVersion_ = std::numeric_limits<uint32_t>::max();
}

void TextBundleConfigParser::InitBundleInfo()
{
#ifdef OHOS_TEXT_ENABLE
    InitTextBundleConfig();
#else
    InitTextBundleFailed();
#endif

#ifdef USE_M133_SKIA
    skia::textlayout::TextGlobalConfig::SetTargetVersion(bundleApiVersion_);
#endif
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS