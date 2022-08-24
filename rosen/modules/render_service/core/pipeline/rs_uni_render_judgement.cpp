/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_uni_render_judgement.h"

#include <fstream>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
const std::string CONFIG_PATH = "/etc/";
const std::string UNIRENDER_CONFIG_FILE_NAME = "unirender.config";

bool StartsWith(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

// used by render server
UniRenderEnabledType RSUniRenderJudgement::GetUniRenderEnabledType()
{
    return uniRenderEnabledType_;
}

const std::set<std::string>& RSUniRenderJudgement::GetUniRenderEnabledList()
{
    return uniRenderBlockList_;
}

bool RSUniRenderJudgement::IsUniRender()
{
    return RSUniRenderJudgement::GetUniRenderEnabledType() != UniRenderEnabledType::UNI_RENDER_DISABLED;
}

bool RSUniRenderJudgement::QueryClientEnabled(const std::string &bundleName)
{
    switch (uniRenderEnabledType_) {
        case UniRenderEnabledType::UNI_RENDER_PARTIALLY_ENABLED:
            return uniRenderBlockList_.find(bundleName) == uniRenderBlockList_.end();
        case UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL:
            return true;
        case UniRenderEnabledType::UNI_RENDER_DISABLED:
        default:
            return false;
    }
}

void RSUniRenderJudgement::InitUniRenderConfig()
{
    InitUniRenderWithConfigFile();
    RS_LOGD("Init RenderService UniRender Type:%d", uniRenderEnabledType_);
}

void RSUniRenderJudgement::InitUniRenderWithConfigFile()
{
    // open config file
    std::string configFilePath = CONFIG_PATH + UNIRENDER_CONFIG_FILE_NAME;
    std::ifstream configFile = std::ifstream(configFilePath.c_str());
    std::string line;
    // first line, init uniRenderEnabledType_
    if (!configFile.is_open() || !std::getline(configFile, line) || line == "") {
        uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
        configFile.close();
        return;
    }
    auto iter = uniRenderConfigMap_.find(line);
    if (iter == uniRenderConfigMap_.end()) {
        uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
        configFile.close();
        return;
    }
    uniRenderEnabledType_ = iter->second;
    if (uniRenderEnabledType_ == UniRenderEnabledType::UNI_RENDER_PARTIALLY_ENABLED) {
        while (std::getline(configFile, line)) {
            if (line == "" || StartsWith(line, "//")) {
                continue;
            }
            uniRenderBlockList_.insert(line);
        }
        if (uniRenderBlockList_.empty()) {
            uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
        }
    }
    configFile.close();
}
} // namespace Rosen
} // namespace OHOS
