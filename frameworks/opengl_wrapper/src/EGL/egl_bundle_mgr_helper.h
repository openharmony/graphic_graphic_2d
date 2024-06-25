/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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
#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_BUNDLE_MGR_HELPER_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_BUNDLE_MGR_HELPER_H

#include <singleton.h>

#include "bundle_mgr_interface.h"

namespace OHOS {
namespace AppExecFwk {
class EGLBundleMgrHelper : public std::enable_shared_from_this<EGLBundleMgrHelper> {
public:
    DISALLOW_COPY_AND_MOVE(EGLBundleMgrHelper);

    ErrCode GetBundleInfoForSelf(AppExecFwk::GetBundleInfoFlag flags, BundleInfo& bundleInfo);

private:
    sptr<IBundleMgr> Connect();

    void OnDeath();

private:
    DECLARE_DELAYED_SINGLETON(EGLBundleMgrHelper);

    std::mutex mutex_;
    sptr<IBundleMgr> bundleMgr_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
};

class EGLBundleMgrServiceDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit EGLBundleMgrServiceDeathRecipient(
        const std::function<void(const wptr<IRemoteObject>& object)>& deathCallback);
    DISALLOW_COPY_AND_MOVE(EGLBundleMgrServiceDeathRecipient);
    virtual ~EGLBundleMgrServiceDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject>& object) override;

private:
    std::function<void (const wptr<IRemoteObject>& object)> deathCallback_;
};

}
}

#endif // FRAMEWORKS_OPENGL_WRAPPER_EGL_BUNDLE_MGR_HELPER_H