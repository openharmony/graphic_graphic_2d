/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_ISURFACE_BUFFER_CALLBACK_H
#define ROSEN_RENDER_SERVICE_BASE_ISURFACE_BUFFER_CALLBACK_H

#include <vector>
#include <iremote_broker.h>

#ifdef ROSEN_OHOS
#include "sync_fence.h"
#endif
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
struct FinishCallbackRet {
    uint64_t uid;
    std::vector<uint32_t> surfaceBufferIds;
    std::vector<uint8_t> isRenderedFlags;
#ifdef ROSEN_OHOS
    std::vector<sptr<SyncFence>> releaseFences;
#endif
    bool isUniRender;
};

struct AfterAcquireBufferRet {
    uint64_t uid;
    bool isUniRender;
};

class RSISurfaceBufferCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.SurfaceBufferListener");

    RSISurfaceBufferCallback() = default;
    virtual ~RSISurfaceBufferCallback() noexcept = default;

    // Uid and BufferQueue are a one-to-one mapping relationship. This API is used
    // when an application has multiple XComponent components, and it notifies ArkUI
    // which BufferQueue's Buffer can be released after it has been consumed.
    virtual void OnFinish(const FinishCallbackRet& ret) = 0;

    // We send a callback to Arkui after Acquire-Buffer succeeds.
    virtual void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) = 0;
};

struct DefaultSurfaceBufferCallbackFuncs {
    std::function<void(const FinishCallbackRet&)> OnFinish;
    std::function<void(const AfterAcquireBufferRet&)> OnAfterAcquireBuffer;
};

class RSB_EXPORT RSDefaultSurfaceBufferCallback : public RSISurfaceBufferCallback {
public:
    RSDefaultSurfaceBufferCallback(DefaultSurfaceBufferCallbackFuncs funcs);
    ~RSDefaultSurfaceBufferCallback() noexcept override = default;

    void OnFinish(const FinishCallbackRet& ret) override;
    void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) override;
    sptr<IRemoteObject> AsObject() override;
private:
    std::function<void(const FinishCallbackRet&)> finishCallback_;
    std::function<void(const AfterAcquireBufferRet&)> afterAcquireBufferCallback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_ISURFACE_BUFFER_CALLBACK_H
