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

#ifndef COMPOSER_CLIENT_RS_LAYER_PROPERTY_ALPHA_CMD_H
#define COMPOSER_CLIENT_RS_LAYER_PROPERTY_ALPHA_CMD_H

#include "rslayer_cmd.h"

namespace OHOS {
namespace Rosen {

/**
 * @class RslayerAlphaCmd
 * @brief The class for alpha RslayerCmd.
 */
class RSC_EXPORT RslayerAlphaCmd : public RslayerCmd {
public:
    RslayerAlphaCmd() = default;
    ~RslayerAlphaCmd() = default;

    static inline constexpr auto Type = RSLayerCmdType::ALPHA;

    RSLayerCmdType GetRsLayerCmdType() const override
    {
        return Type;
    }
};

} // namespace Rosen
} // namespace OHOS

#endif // COMPOSER_CLIENT_RS_LAYER_PROPERTY_ALPHA_CMD_H
