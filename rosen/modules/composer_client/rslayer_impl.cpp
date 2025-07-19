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

#include "rslayer_impl.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<RSTransactionHandler> RSLayerImpl::GetRSTransaction() const
{
    // adapt transaction
}

bool RSLayerImpl::AddCommand(std::unique_ptr<RSCommand>& command,
    FollowType followType, LayerId layerId) const
{
    auto transaction = GetRSTransaction();
    if (transaction != nullptr) {
        transaction->AddCommand(command, followType, layerId);
    }
    return true;
}

void RSLayerImpl::AddRsLayerCmd(const std::shared_ptr<RslayerCmd> layerCmd)
{
    {
        std::unique_lock<std::recursive_mutex> lock(propertyMutex_);
        if (!layerCmd || commands_.count(layerCmd->GetPropertyId())) {
            return;
        }
        layerCmd->AttachToLayer(shared_from_this());
        commands_.emplace(layerCmd->GetPropertyId(), layerCmd);
        commandsTypeMap_.emplace((int16_t)layerCmd->GetRsLayerCmdType(), layerCmd);
    }
    if (layerCmd->GetRsLayerCmdType() == RSLayerCmdType::NODE_MODIFIER) {
        return;
    }
    CheckLayerCmdType(layerCmd->GetRsLayerCmdType());
    std::unique_ptr<RSCommand> command = std::make_unique<RSAddLayerCmd>(GetId(), layerCmd->CreateRenderModifier());
    AddCommand(command, GetFollowType(), GetId());
}

template<typename RSLayerCmdName, typename PropertyName, typename T>
void RSLayerImpl::SetRsLayerCmd(RSLayerCmdType RSLayerCmdType, T value)
{
    std::unique_lockstd::recursive_mutex lock(propertyMutex_);
    auto iter = rsLayerCmds_.find(RSLayerCmdType);
    if (iter != rsLayerCmds_.end()) {
        auto property = std::static_pointer_cast(iter->second->GetProperty());
        if (property == nullptr) {
            ROSEN_LOGE("RSLayer::SetRsLayerCmd: failed to set property, property is null!");
            return;
        }
        property->Set(value);
        return;
    }
    auto property = std::make_shared<PropertyName>(value);
    auto rsLayerCmd = std::make_shared<RSLayerCmdName>(property);
    rsLayerCmds_.emplace(RSLayerCmdType, rsLayerCmd);
    AddRsLayerCmd(rsLayerCmd);
}

void RSLayerImpl::SetAlpha(const GraphicLayerAlpha &alpha)
{
    if (alpha == layerAlpha_) {
        return;
    }
    layerAlpha_ = alpha;
    SetRsLayerCmd<RslayerAlphaCmd, RSLayerProperty>(RSLayerCmdType::ALPHA, alpha);
}

} // namespace Rosen
} // namespace OHOS
