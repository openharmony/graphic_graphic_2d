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

#include "sync_fence.h"

#include <libsync.h>
#include <unistd.h>
#include <errno.h>

#include "hilog/log.h"

namespace OHOS {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, 0xD001400, "SyncFence" };
constexpr int32_t INVALID_FD = -1;
}  // namespace

const sptr<SyncFence> SyncFence::INVALID_FENCE = sptr<SyncFence>(new SyncFence(INVALID_FD));

SyncFence::SyncFence(int32_t fenceFd) : fenceFd_(fenceFd)
{
    HiLog::Debug(LABEL, "%{public}s fenceFd: %{public}d", __func__, fenceFd_.Get());
}

SyncFence::~SyncFence()
{
}

int32_t SyncFence::Wait(uint32_t timeout)
{
    if (fenceFd_ < 0) {
        return 0;
    }

    int32_t err = sync_wait(fenceFd_, timeout);
    return err < 0 ? -errno : 0;
}

sptr<SyncFence> SyncFence::MergeFence(const std::string &name,
                const sptr<SyncFence>& fence1, const sptr<SyncFence>& fence2)
{
    int32_t newFenceFd = INVALID_FD;
    int32_t fenceFd1 = fence1->fenceFd_;
    int32_t fenceFd2 = fence2->fenceFd_;

    if (fenceFd1 >= 0 && fenceFd2 >= 0) {
        newFenceFd = sync_merge(name.c_str(), fenceFd1, fenceFd2);
    } else if (fenceFd1 >= 0) {
        newFenceFd = sync_merge(name.c_str(), fenceFd1, fenceFd1);
    } else if (fenceFd2 >= 0) {
        newFenceFd = sync_merge(name.c_str(), fenceFd2, fenceFd2);
    } else {
        return INVALID_FENCE;
    }

    if (newFenceFd == INVALID_FD) {
        int32_t err = -errno;
        HiLog::Error(LABEL, "sync_merge(%{public}s) failed, error: %{public}s (%{public}d)",
                     name.c_str(), strerror(-err), err);
        return INVALID_FENCE;
    }

    return sptr<SyncFence>(new SyncFence(newFenceFd));
}

int32_t SyncFence::Dup() const
{
    return ::dup(fenceFd_);
}

int32_t SyncFence::Get() const
{
    return fenceFd_;
}

} // namespace OHOS
