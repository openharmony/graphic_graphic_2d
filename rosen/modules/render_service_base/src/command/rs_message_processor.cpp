/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "command/rs_message_processor.h"

#include "command/rs_command.h"

namespace OHOS {
namespace Rosen {
RSMessageProcessor& RSMessageProcessor::Instance()
{
    static RSMessageProcessor processor;
    return processor;
}

RSMessageProcessor::~RSMessageProcessor() {}

void RSMessageProcessor::AddUIMessage(std::shared_ptr<RSCommand>& command)
{
    commands_.push(std::move(command));
}

void RSMessageProcessor::AddUIMessage(std::shared_ptr<RSCommand>&& command)
{
    commands_.push(std::move(command));
}

void RSMessageProcessor::CommitUIMsg(std::queue<std::shared_ptr<RSCommand>>& commands)
{
    commands = std::move(commands_);
}
} // namespace Rosen
} // namespace OHOS
