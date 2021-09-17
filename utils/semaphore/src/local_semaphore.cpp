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

#include "local_semaphore.h"

namespace OHOS {
LocalSemaphore::LocalSemaphore()
{
    sem_init(&sem, false, 0);
}

LocalSemaphore::LocalSemaphore(int count)
{
    sem_init(&sem, false, count);
}

LocalSemaphore::~LocalSemaphore()
{
    sem_destroy(&sem);
}

void LocalSemaphore::Inc()
{
    sem_post(&sem);
}

void LocalSemaphore::Dec()
{
    sem_wait(&sem);
}
} // namespace OHOS
