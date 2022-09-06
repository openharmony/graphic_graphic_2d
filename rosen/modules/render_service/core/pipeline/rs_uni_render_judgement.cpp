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
namespace {
const std::string CONFIG_PATH = "/etc/";
const std::string UNIRENDER_CONFIG_FILE_NAME = "unirender.config";
const std::string UNI_RENDER_DISABLED_TAG = "DISABLED";
const std::string UNI_RENDER_ENABLED_FOR_ALL_TAG = "ENABLED_FOR_ALL";
const std::string UNI_RENDER_DYNAMIC_SWITCH_TAG = "DYNAMIC_SWITCH";
}

// used by render server
UniRenderEnabledType RSUniRenderJudgement::GetUniRenderEnabledType()
{
    return uniRenderEnabledType_;
}

bool RSUniRenderJudgement::IsUniRender()
{
    return RSUniRenderJudgement::GetUniRenderEnabledType() != UniRenderEnabledType::UNI_RENDER_DISABLED;
}

void RSUniRenderJudgement::InitUniRenderConfig()
{
    InitUniRenderWithConfigFile();
    RS_LOGD("Init RenderService UniRender Type:%d", uniRenderEnabledType_);
}

std::ifstream& RSUniRenderJudgement::SafeGetLine(std::ifstream &configFile, std::string &line)
{
    std::string myline;
    std::getline(configFile, myline);
    if (myline.size() && myline[myline.size() - 1] == '\r') {
        line = myline.substr(0, myline.size() - 1);
    } else {
        line = myline;
    }
    return configFile;
}

void RSUniRenderJudgement::InitUniRenderWithConfigFile()
{
    // open config file
    std::string configFilePath = CONFIG_PATH + UNIRENDER_CONFIG_FILE_NAME;
    std::ifstream configFile = std::ifstream(configFilePath.c_str());
    std::string line;
    // first line, init uniRenderEnabledType_
    if (!configFile.is_open() || !SafeGetLine(configFile, line) || line.empty()) { // default case
#ifdef RS_ENABLE_UNI_RENDER
        uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DYNAMIC_SWITCH;
#else
        uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
#endif
    } else if (line == UNI_RENDER_DISABLED_TAG) {
        uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    } else if (line == UNI_RENDER_ENABLED_FOR_ALL_TAG) {
        uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    } else if (line == UNI_RENDER_DYNAMIC_SWITCH_TAG) {
        uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DYNAMIC_SWITCH;
    }
    configFile.close();
}
} // namespace Rosen
} // namespace OHOS
