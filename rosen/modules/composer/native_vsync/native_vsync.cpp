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
#include "external_vsync.h"
#include "transaction/rs_interfaces.h"

#include <string>
#include <memory>

#ifndef weak_alias
    #define weak_alias(old, new) \
        extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))
#endif

struct NativeVSync {
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver_;
};

static NativeVSync* OHNativeVSyncToNativeVSync(OHNativeVSync* ohNativeVSync)
{
    return reinterpret_cast<NativeVSync*>(ohNativeVSync);
}

static OHNativeVSync* NativeVSyncToOHNativeVSync(NativeVSync* nativeVSync)
{
    return reinterpret_cast<OHNativeVSync*>(nativeVSync);
}

OHNativeVSync* CreateOHNativeVSync(const char* name, unsigned int length)
{
    if (name == nullptr) {
        return nullptr;
    }
    std::string vsyncName(name, length);
    NativeVSync* nativeVSync = new NativeVSync;
    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver = rsClient.CreateVSyncReceiver(vsyncName);
    receiver->Init();
    nativeVSync->receiver_ = receiver;
    return NativeVSyncToOHNativeVSync(nativeVSync);
}

void DestroyOHNativeVSync(OHNativeVSync *nativeVSync)
{
    if (nativeVSync == nullptr) {
        return;
    }

    delete OHNativeVSyncToNativeVSync(nativeVSync);
}

int OHNativeVSyncRequestFrame(OHNativeVSync *ohNativeVSync, OHNativeVSyncFrameCallback callback, void* data)
{
    NativeVSync* nativeVSync = OHNativeVSyncToNativeVSync(ohNativeVSync);
    if (nativeVSync == nullptr || nativeVSync->receiver_ == nullptr) {
        return -1;
    }
    OHOS::Rosen::VSyncReceiver::FrameCallback frameCallback = {
        .userData_ = data,
        .callback_ = callback,
    };
    return nativeVSync->receiver_->RequestNextVSync(frameCallback);
}

weak_alias(CreateOHNativeVSync, OH_NativeVSync_CreateOHNativeVSync);
weak_alias(DestroyOHNativeVSync, OH_NativeVSync_DestroyOHNativeVSync);
weak_alias(OHNativeVSyncRequestFrame, OH_NativeVSync_OHNativeVSyncRequestFrame);
