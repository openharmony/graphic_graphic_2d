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
#include "modules/skparagraph/include/TextGlobalConfig.h"

#include <utility>

#ifdef ENABLE_OHOS_ENHANCE
#include "application_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "hap_module_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "utils/text_log.h"
#endif

namespace OHOS {
namespace Rosen {
namespace SPText {
#ifdef ENABLE_OHOS_ENHANCE
const std::string ADAPTER_TEXT_HEIGHT_META_DATA = "ohos.graphics2d.text.adapter_text_height";
const std::string DISABLE_SPACING_FOR_CONTROL_CHAR_META_DATA = "ohos.graphics2d.text.disable_spacing_for_control_char";
const size_t VERSION_DIVISOR = 100;

namespace {
bool GetMetaDataValue(const std::string& metaData,
    const AppExecFwk::BundleInfo& bundleInfo, std::string& value)
{
    for (const auto& info : bundleInfo.hapModuleInfos) {
        for (const auto& data : info.metadata) {
            if (data.name == metaData) {
                value = data.value;
                return true;
            }
        }
    }
    return false;
}

skia::textlayout::GlobalOptimizationSwitchState ParseSwitchState(const std::string& value)
{
    if (value == "enable") {
        return skia::textlayout::GlobalOptimizationSwitchState::ENABLE;
    }
    if (value == "disable") {
        return skia::textlayout::GlobalOptimizationSwitchState::DISABLE;
    }
    return skia::textlayout::GlobalOptimizationSwitchState::UNSET;
}
} // namespace

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

bool TextBundleConfigParser::IsDisableSpacingForControlChar() const
{
    return initStatus_ && disableSpacingForControlChar_;
}

bool TextBundleConfigParser::IsAdapterTextHeightEnabled() const
{
    return initStatus_ && adapterTextHeightEnable_;
}

bool TextBundleConfigParser::IsTargetApiVersion(size_t targetVersion) const
{
    return initStatus_ && bundleApiVersion_ >= targetVersion;
}

#ifdef ENABLE_OHOS_ENHANCE
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
    disableSpacingForControlChar_ =
        IsMetaDataExistInModule(DISABLE_SPACING_FOR_CONTROL_CHAR_META_DATA, bundleInfo);
    TEXT_LOGI("Disable spacing for control char %{public}d", disableSpacingForControlChar_);
    InitOptimizationSwitches(bundleInfo);
}

void TextBundleConfigParser::InitOptimizationSwitches(const AppExecFwk::BundleInfo& bundleInfo)
{
    static constexpr std::pair<skia::textlayout::TextOptimizationSwitchId, const char*> kSwitchMetaKeys[] = {
        { skia::textlayout::TextOptimizationSwitchId::TRAILING_SPACE_OPTIMIZED,
            "ohos.graphics2d.text.trailing_space_optimized" },
        { skia::textlayout::TextOptimizationSwitchId::ENABLE_AUTO_SPACE,
            "ohos.graphics2d.text.auto_space" },
        { skia::textlayout::TextOptimizationSwitchId::COMPRESS_HEAD_PUNCTUATION,
            "ohos.graphics2d.text.compress_head_punctuation" },
        { skia::textlayout::TextOptimizationSwitchId::INCLUDE_FONT_PADDING,
            "ohos.graphics2d.text.include_font_padding" },
        { skia::textlayout::TextOptimizationSwitchId::FALLBACK_LINE_SPACING,
            "ohos.graphics2d.text.fallback_line_spacing" },
        { skia::textlayout::TextOptimizationSwitchId::ORPHAN_CHAR_OPTIMIZATION,
            "ohos.graphics2d.text.orphan_char_optimization" },
    };
    for (const auto& [id, key] : kSwitchMetaKeys) {
        std::string value;
        auto state = skia::textlayout::GlobalOptimizationSwitchState::UNSET;
        if (GetMetaDataValue(key, bundleInfo, value)) {
            state = ParseSwitchState(value);
        }
        skia::textlayout::TextGlobalConfig::SetOptimizationSwitch(id, state);
    }
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
#ifdef ENABLE_OHOS_ENHANCE
    InitTextBundleConfig();
#else
    InitTextBundleFailed();
#endif
    skia::textlayout::TextGlobalConfig::SetTargetVersion(bundleApiVersion_);
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS