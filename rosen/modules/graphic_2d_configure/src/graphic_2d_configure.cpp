/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "graphic_2d_configure.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unistd.h>

#include "platform/common/rs_log.h"
#include "vsync_generator.h"

namespace OHOS::Rosen {

Graphic2dConfigure& Graphic2dConfigure::Instance()
{
    static Graphic2dConfigure instance;
    instance.Init();
    return instance;
}

Graphic2dConfigure::Graphic2dConfigure()
{
    RS_LOGD("Construction of Graphic2dConfigure");
}

void Graphic2dConfigure::Init()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [this] () {
        if (InitXmlConfig() != EXEC_SUCCESS) {
            RS_LOGE("Graphic2dConfigure falied to parse");
            return;
        } else {
            RS_LOGD("Graphic2dConfigure successed to parse");
        }
    });
}

int32_t Graphic2dConfigure::InitXmlConfig()
{
    RS_LOGD("Graphic2dConfigure is parsing xml configuration");
    if (!mParser_) {
        mParser_ = std::make_unique<Graphic2dXmlParser>();
    }

    if (mParser_->LoadConfiguration(CONFIG_FILE_PRODUCT) != EXEC_SUCCESS) {
        RS_LOGD("Graphic2dConfigure failed to load prod xml configuration file");
    }
    if (mParser_->Parse() != EXEC_SUCCESS) {
        RS_LOGD("Graphic2dConfigure failed to parse prod xml configuration");
    }

    if (!mConfigData_) {
        mConfigData_ = mParser_->GetParsedData();
    }

    return EXEC_SUCCESS;
}

void Graphic2dConfigure::parserTest()
{
    auto &graphic2dConfig = mConfigData_->graphic2dConfig_;
    for (const auto &[key, value] : graphic2dConfig) {
        RS_LOGI("Graphic2dConfigure graphic2dConfig_ key: %{public}s value: %{public}s",
                key.c_str(),
                value.c_str());
    }
}
}
