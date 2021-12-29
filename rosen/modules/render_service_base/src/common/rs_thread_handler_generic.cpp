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

#include "rs_thread_handler_generic.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<RSThreadHandler> RSThreadHandler::Create()
{
    return std::make_unique<RSThreadHandlerGeneric>();
}

RSThreadHandlerGeneric::RSThreadHandlerGeneric()
{
    looper_ = ThreadLooperImpl::GetThreadInstance();
}

class GenericMessageHandler final : public ThreadLooperMessage {
public:
    explicit GenericMessageHandler(const RSTaskMessage::RSTask task) : task_(task) {}

    ~GenericMessageHandler() override = default;

    void Process(int param) const override
    {
        task_();
    }

private:
    RSTaskMessage::RSTask task_ = nullptr;
};

class GenericThreadMessage final : public RSTaskMessage {
public:
    explicit GenericThreadMessage(const RSTaskMessage::RSTask task) : messageHandler_(new GenericMessageHandler(task))
    {}
    ~GenericThreadMessage() = default;

    const std::shared_ptr<GenericMessageHandler>& GetMessageHandler() const
    {
        return messageHandler_;
    }

private:
    std::shared_ptr<GenericMessageHandler> messageHandler_;
};

RSTaskHandle RSThreadHandlerGeneric::CreateTask(const RSTaskMessage::RSTask task) const
{
    return std::make_shared<GenericThreadMessage>(task);
}

void RSThreadHandlerGeneric::PostTask(const RSTaskHandle taskHandle, int param)
{
    if (!looper_) {
        return;
    }
    auto* msg = static_cast<GenericThreadMessage*>(taskHandle.get());
    if (msg != nullptr) {
        looper_->PostMessage(msg->GetMessageHandler(), param);
    }
}

void RSThreadHandlerGeneric::PostTaskDelay(const RSTaskHandle taskHandle, int64_t nsecs, int param)
{
    if (!looper_) {
        return;
    }
    auto* msg = static_cast<GenericThreadMessage*>(taskHandle.get());
    if (msg != nullptr) {
        looper_->PostMessage(nsecs, msg->GetMessageHandler(), param);
    }
}

void RSThreadHandlerGeneric::CancelTask(RSTaskHandle taskHandle)
{
    if (!looper_) {
        return;
    }
    auto* msg = static_cast<GenericThreadMessage*>(taskHandle.get());
    if (msg != nullptr) {
        looper_->RemoveMessages(msg->GetMessageHandler());
    }
}

RSTaskHandle RSThreadHandler::StaticCreateTask(const RSTaskMessage::RSTask task)
{
    return std::make_shared<GenericThreadMessage>(task);
}
} // namespace Rosen
} // namespace OHOS
