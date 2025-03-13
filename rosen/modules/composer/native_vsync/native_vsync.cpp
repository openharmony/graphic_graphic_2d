/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include <vsync_receiver.h>
#include "transaction/rs_interfaces.h"
#include "feature/hyper_graphic_manager/rs_frame_rate_linker.h"
#include "vsync_log.h"
#include "native_vsync.h"

using namespace OHOS;

namespace {
struct NativeVSync {
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver_;
    std::shared_ptr<OHOS::Rosen::RSFrameRateLinker> frameRateLinker_;
};
}
static NativeVSync* OH_NativeVSync_OHNativeVSyncToNativeVSync(OH_NativeVSync* ohNativeVSync)
{
    return reinterpret_cast<NativeVSync*>(ohNativeVSync);
}

static OH_NativeVSync* OH_NativeVSync_NativeVSyncToOHNativeVSync(NativeVSync* nativeVSync)
{
    return reinterpret_cast<OH_NativeVSync*>(nativeVSync);
}

std::shared_ptr<OHOS::Rosen::VSyncReceiver> CreateAndInitVSyncReceiver(
    const std::string& vsyncName,
    uint64_t windowID = 0,
    bool isAssociatedWindow = false,
    NativeVSync* nativeVSync = nullptr)
{
    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver;
    if (isAssociatedWindow) {
        nativeVSync->frameRateLinker_ = OHOS::Rosen::RSFrameRateLinker::Create();
        receiver = rsClient.CreateVSyncReceiver(
            vsyncName, nativeVSync->frameRateLinker_->GetId(), nullptr, windowID, true);
    } else {
        receiver = rsClient.CreateVSyncReceiver(vsyncName);
    }
    if (receiver == nullptr) {
        VLOGE("Create VSyncReceiver failed");
        return nullptr;
    }
    int ret = receiver->Init();
    if (ret != 0) {
        VLOGE("VSyncReceiver Init failed, ret:%{public}d", ret);
        return nullptr;
    }
    return receiver;
}

OH_NativeVSync* OH_NativeVSync_Create(const char* name, unsigned int length)
{
    if (name == nullptr) {
        VLOGE("name is nullptr, please check");
        return nullptr;
    }
    std::string vsyncName(name, length);
    NativeVSync* nativeVSync = new NativeVSync;
    auto receiver = CreateAndInitVSyncReceiver(vsyncName, 0, true, nativeVSync);
    if (receiver == nullptr) {
        VLOGE("receiver is nullptr, please check");
        delete nativeVSync;
        return nullptr;
    }
    nativeVSync->receiver_ = receiver;
    return OH_NativeVSync_NativeVSyncToOHNativeVSync(nativeVSync);
}

OH_NativeVSync* OH_NativeVSync_Create_ForAssociatedWindow(uint64_t windowID, const char* name, unsigned int length)
{
    if (name == nullptr) {
        VLOGE("name is nullptr, please check");
        return nullptr;
    }
    std::string vsyncName(name, length);
    NativeVSync* nativeVSync = new NativeVSync;
    auto receiver = CreateAndInitVSyncReceiver(vsyncName, windowID, true, nativeVSync);
    if (receiver == nullptr) {
        VLOGE("receiver is nullptr, please check");
        delete nativeVSync;
        return nullptr;
    }
    nativeVSync->receiver_ = receiver;
    return OH_NativeVSync_NativeVSyncToOHNativeVSync(nativeVSync);
}

void OH_NativeVSync_Destroy(OH_NativeVSync *nativeVSync)
{
    if (nativeVSync == nullptr) {
        VLOGE("parameter is nullptr, please check");
        return;
    }

    delete OH_NativeVSync_OHNativeVSyncToNativeVSync(nativeVSync);
    nativeVSync = nullptr;
}

int OH_NativeVSync_RequestFrame(OH_NativeVSync *ohNativeVSync, OH_NativeVSync_FrameCallback callback, void* data)
{
    NativeVSync* nativeVSync = OH_NativeVSync_OHNativeVSyncToNativeVSync(ohNativeVSync);
    if (nativeVSync == nullptr || nativeVSync->receiver_ == nullptr || callback == nullptr) {
        VLOGE("parameter is nullptr, please check");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    OHOS::Rosen::VSyncReceiver::FrameCallback frameCallback = {
        .userData_ = data,
        .callback_ = callback,
    };
    return nativeVSync->receiver_->RequestNextVSync(frameCallback);
}

int OH_NativeVSync_RequestFrameWithMultiCallback(
    OH_NativeVSync *ohNativeVSync, OH_NativeVSync_FrameCallback callback, void* data)
{
    NativeVSync* nativeVSync = OH_NativeVSync_OHNativeVSyncToNativeVSync(ohNativeVSync);
    if (nativeVSync == nullptr || nativeVSync->receiver_ == nullptr || callback == nullptr) {
        VLOGE("parameter is nullptr, please check");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    OHOS::Rosen::VSyncReceiver::FrameCallback frameCallback = {
        .userData_ = data,
        .callback_ = callback,
    };
    return nativeVSync->receiver_->RequestNextVSyncWithMultiCallback(frameCallback);
}

int OH_NativeVSync_GetPeriod(OH_NativeVSync* nativeVsync, long long* period)
{
    NativeVSync* nativeVSync = OH_NativeVSync_OHNativeVSyncToNativeVSync(nativeVsync);
    if (nativeVSync == nullptr || nativeVSync->receiver_ == nullptr || period == nullptr) {
        VLOGE("parameter is nullptr, please check");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    return nativeVSync->receiver_->GetVSyncPeriod(*reinterpret_cast<int64_t*>(period));
}

int OH_NativeVSync_DVSyncSwitch(OH_NativeVSync* ohNativeVSync, bool enable)
{
    NativeVSync* nativeVSync = OH_NativeVSync_OHNativeVSyncToNativeVSync(ohNativeVSync);
    if (nativeVSync == nullptr || nativeVSync->receiver_ == nullptr) {
        VLOGE("parameter is nullptr, please check");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    return nativeVSync->receiver_->SetNativeDVSyncSwitch(enable);
}
