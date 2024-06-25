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
#include "egl_bundle_mgr_helper.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "wrapper_log.h"

namespace OHOS {
namespace AppExecFwk {
EGLBundleMgrHelper::EGLBundleMgrHelper() {}

EGLBundleMgrHelper::~EGLBundleMgrHelper()
{
    if (bundleMgr_ != nullptr && bundleMgr_->AsObject() != nullptr && deathRecipient_ != nullptr) {
        bundleMgr_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    }
}

sptr<IBundleMgr> EGLBundleMgrHelper::Connect()
{
    WLOGD("Call EGLBundleMgrHelper::Connect");
    std::lock_guard<std::mutex> lock(mutex_);
    if (bundleMgr_ == nullptr) {
        sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            WLOGE("Failed to get system ability manager.");
            return nullptr;
        }
        sptr<IRemoteObject> remoteObject_ = systemAbilityManager->GetSystemAbility(
            BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (remoteObject_ == nullptr || (bundleMgr_ = iface_cast<IBundleMgr>(remoteObject_)) == nullptr) {
            WLOGE("Failed to get bundle mgr service remote object");
            return nullptr;
        }
        std::weak_ptr<EGLBundleMgrHelper> weakPtr = shared_from_this();
        auto deathCallback = [weakPtr](const wptr<IRemoteObject> &object) {
            auto sharedPtr = weakPtr.lock();
            if (sharedPtr == nullptr) {
                WLOGE("Bundle helper instance is nullptr");
                return;
            }
            sharedPtr->OnDeath();
        };
        deathRecipient_ = new(std::nothrow) EGLBundleMgrServiceDeathRecipient(deathCallback);
        if (deathRecipient_ == nullptr) {
            WLOGE("Failed to create death recipient ptr deathRecipient_!");
            return nullptr;
        }
        if (bundleMgr_->AsObject() != nullptr) {
            bundleMgr_->AsObject()->AddDeathRecipient(deathRecipient_);
        }
    }
    return bundleMgr_;
}

void EGLBundleMgrHelper::OnDeath()
{
    WLOGD("Call EGLBundleMgrHelper::OnDeath");
    std::lock_guard<std::mutex> lock(mutex_);
    if (bundleMgr_ == nullptr || bundleMgr_->AsObject() == nullptr) {
        WLOGE("bundleMgr_ is nullptr!");
        return;
    }
    bundleMgr_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    bundleMgr_ = nullptr;
}

ErrCode EGLBundleMgrHelper::GetBundleInfoForSelf(AppExecFwk::GetBundleInfoFlag flags, BundleInfo &bundleInfo)
{
    WLOGD("Call EGLBundleMgrHealper::GetBundleInfoForSelf.");
    auto bundleMgr_ = Connect();
    if (bundleMgr_ == nullptr) {
        WLOGE("Failed to connect.");
        return -1;
    }
    return bundleMgr_->GetBundleInfoForSelf(static_cast<int32_t>(flags), bundleInfo);
}

EGLBundleMgrServiceDeathRecipient::EGLBundleMgrServiceDeathRecipient(
    const std::function<void(const wptr<IRemoteObject>& object)>& deathCallback)
    : deathCallback_(deathCallback) {}

void EGLBundleMgrServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& object)
{
    if (deathCallback_ != nullptr) {
        deathCallback_(object);
    }
}

}
}