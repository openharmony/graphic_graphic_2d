/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_app_state_listener.h"
#include "pipeline/rs_main_thread.h"

namespace OHOS::Rosen {
RSAppStateListener::RSAppStateListener()
{
}

RSAppStateListener::~RSAppStateListener()
{
}

void RSAppStateListener::OnTrim(Memory::SystemMemoryLevel level)
{
    RSMainThread::Instance()->HandleOnTrim(level);
}

void RSAppStateListener::ForceReclaim(int32_t pid, int32_t uid)
{
    Memory::SystemMemoryLevel level = Memory::SystemMemoryLevel::MEMORY_LEVEL_CRITICAL;
    RSMainThread::Instance()->HandleOnTrim(level);
}
} // namespace OHOS::Rosen