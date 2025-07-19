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

#include "rslayer_cmd.h"
#include "rsrenderlayer_cmd.h"

namespace OHOS {
namespace Rosen {

#define DECLARE_RSLAYER_CMD_CREATE(CMD_NAME)                                          \
    std::shared_ptr<RSRenderLayerCmd> RSLayer##CMD_NAME##Cmd::CreateRenderLayerCmd() const    \
    {                                                                                              \
        auto renderProperty = GetRsRenderLayerProperty();                                                 \
        auto renderLayerCmd = std::make_shared<RSRenderLayer##CMD_NAME##Cmd>(renderProperty); \
        return renderLayerCmd;                                                                     \
    }

#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE, DELTA_OP, THRESHOLD_TYPE) \
    RSLayer##CMD_NAME##Cmd::RSLayer##CMD_NAME##Cmd(const std::shared_ptr<RSLayerProperty>& property)    \
        : RSLayerCmd(property, RSLayerCmdType::CMD_TYPE)                                   \
    {                                                                                                            \
        rsLayerProperty_->SetThresholdType();                                              \
    }                                                                                                            \
    RSLayerCmdType RSLayer##CMD_NAME##Cmd::GetRsLayerCmdType() const                                          \
    {                                                                                                            \
        return RSLayerCmdType::CMD_TYPE;                                                                    \
    }                                                                                                            \
    DECLARE_RSLAYER_CMD_CREATE(CMD_NAME)

#include "rslayer_cmd_def.in"

#undef DECLARE_RSLAYER_CMD
} // namespace Rosen
} // namespace OHOS
