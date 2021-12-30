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

#include "rs_thread_looper_generic.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<RSThreadLooper> RSThreadLooper::Create()
{
    return std::make_unique<RSThreadLooperGeneric>();
}

RSThreadLooperGeneric::RSThreadLooperGeneric()
{
    looper_ = ThreadLooperImpl::CreateThreadInstance();
}

void RSThreadLooperGeneric::ProcessOneMessage(int timeoutMillis)
{
    looper_->ProcessOneMessage(timeoutMillis);
}

void RSThreadLooperGeneric::ProcessAllMessages(int timeoutMillis)
{
    looper_->ProcessAllMessages(timeoutMillis);
}

void RSThreadLooperGeneric::WakeUp()
{
    looper_->WakeUp();
}
} // namespace Rosen
} // namespace OHOS
