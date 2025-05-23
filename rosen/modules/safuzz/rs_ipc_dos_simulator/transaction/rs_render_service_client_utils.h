/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SAFUZZ_RS_RENDER_SERVICE_CLIENT_UTILS_H
#define SAFUZZ_RS_RENDER_SERVICE_CLIENT_UTILS_H

#include "common/safuzz_log.h"
#include "ipc_callbacks/buffer_available_callback_stub.h"
#include "ipc_callbacks/buffer_clear_callback_stub.h"
#include "ipc_callbacks/hgm_config_change_callback_stub.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#endif
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_stub.h"
#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"
#include "ipc_callbacks/rs_self_drawing_node_rect_change_callback_stub.h"
#include "ipc_callbacks/rs_surface_buffer_callback_stub.h"
#include "ipc_callbacks/rs_surface_occlusion_change_callback_stub.h"
#include "ipc_callbacks/rs_uiextension_callback_stub.h"
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "ipc_callbacks/surface_capture_callback_stub.h"
#include "ipc_callbacks/rs_transaction_data_callback_stub.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
class CustomBufferAvailableCallback : public RSBufferAvailableCallbackStub {
public:
    explicit CustomBufferAvailableCallback(const BufferAvailableCallback &callback) : cb_(callback) {}
    ~CustomBufferAvailableCallback() override {};

    void OnBufferAvailable() override
    {
        if (cb_ != nullptr) {
            cb_();
        }
    }

private:
    BufferAvailableCallback cb_;
};

class CustomBufferClearCallback : public RSBufferClearCallbackStub {
public:
    explicit CustomBufferClearCallback(const BufferClearCallback &callback) : cb_(callback) {}
    ~CustomBufferClearCallback() override {};

    void OnBufferClear() override
    {
        if (cb_ != nullptr) {
            cb_();
        }
    }

private:
    BufferClearCallback cb_;
};

class CustomHgmConfigChangeCallback : public RSHgmConfigChangeCallbackStub {
public:
    explicit CustomHgmConfigChangeCallback(const HgmConfigChangeCallback& callback) : cb_(callback) {}
    ~CustomHgmConfigChangeCallback() override {};

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override
    {
        if (cb_ != nullptr) {
            cb_(configData);
        }
    }

    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override
    {
    }

    void OnHgmRefreshRateUpdate(int32_t refreshRate) override
    {
    }
private:
    HgmConfigChangeCallback cb_;
};

class CustomHgmRefreshRateModeChangeCallback : public RSHgmConfigChangeCallbackStub {
public:
    explicit CustomHgmRefreshRateModeChangeCallback(const HgmRefreshRateModeChangeCallback& callback) : cb_(callback) {}
    ~CustomHgmRefreshRateModeChangeCallback() override {};

    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override
    {
        if (cb_ != nullptr) {
            cb_(refreshRateMode);
        }
    }

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override
    {
    }

    void OnHgmRefreshRateUpdate(int32_t refreshRate) override
    {
    }
private:
    HgmRefreshRateModeChangeCallback cb_;
};

class CustomHgmRefreshRateUpdateCallback : public RSHgmConfigChangeCallbackStub {
public:
    explicit CustomHgmRefreshRateUpdateCallback(const HgmRefreshRateUpdateCallback& callback) : cb_(callback) {}
    ~CustomHgmRefreshRateUpdateCallback() override {};

    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override
    {
    }

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override
    {
    }

    void OnHgmRefreshRateUpdate(int32_t refreshRate) override
    {
        ROSEN_LOGD("CustomHgmRefreshRateUpdateCallback::OnHgmRefreshRateUpdate called");
        if (cb_ != nullptr) {
            cb_(refreshRate);
        }
    }

private:
    HgmRefreshRateUpdateCallback cb_;
};

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
class CustomPointerLuminanceChangeCallback : public RSPointerLuminanceChangeCallbackStub {
public:
    explicit CustomPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback) : cb_(callback) {}
    ~CustomPointerLuminanceChangeCallback() override {};
 
    void OnPointerLuminanceChanged(int32_t brightness) override
    {
        if (cb_ != nullptr) {
            cb_(brightness);
        }
    }
 
private:
    PointerLuminanceChangeCallback cb_;
};
#endif

class CustomFrameRateLinkerExpectedFpsUpdateCallback : public RSFrameRateLinkerExpectedFpsUpdateCallbackStub {
public:
    explicit CustomFrameRateLinkerExpectedFpsUpdateCallback(
        const FrameRateLinkerExpectedFpsUpdateCallback& callback) : cb_(callback) {}
    ~CustomFrameRateLinkerExpectedFpsUpdateCallback() override {};

    void OnFrameRateLinkerExpectedFpsUpdate(pid_t dstPid, int32_t expectedFps) override
    {
        ROSEN_LOGD("CustomFrameRateLinkerExpectedFpsUpdateCallback::OnFrameRateLinkerExpectedFpsUpdate called,"
            " pid=%{public}d, fps=%{public}d", dstPid, expectedFps);
        if (cb_ != nullptr) {
            cb_(dstPid, expectedFps);
        }
    }

private:
    FrameRateLinkerExpectedFpsUpdateCallback cb_;
};

class CustomOcclusionChangeCallback : public RSOcclusionChangeCallbackStub {
public:
    explicit CustomOcclusionChangeCallback(const OcclusionChangeCallback &callback) : cb_(callback) {}
    ~CustomOcclusionChangeCallback() override {};

    void OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData) override
    {
        if (cb_ != nullptr) {
            cb_(occlusionData);
        }
    }

private:
    OcclusionChangeCallback cb_;
};

class SurfaceBufferCallbackDirector : public RSSurfaceBufferCallbackStub {
public:
    explicit SurfaceBufferCallbackDirector(RSRenderServiceClient* client) : client_(client) {}
    ~SurfaceBufferCallbackDirector() noexcept override = default;
    void OnFinish(const FinishCallbackRet& ret) override
    {
        client_->TriggerOnFinish(ret);
    }

    void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) override
    {
        client_->TriggerOnAfterAcquireBuffer(ret);
    }

private:
    RSRenderServiceClient* client_;
};

class CustomSurfaceOcclusionChangeCallback : public RSSurfaceOcclusionChangeCallbackStub {
public:
    explicit CustomSurfaceOcclusionChangeCallback(const SurfaceOcclusionChangeCallback &callback) : cb_(callback) {}
    ~CustomSurfaceOcclusionChangeCallback() override {};

    void OnSurfaceOcclusionVisibleChanged(float visibleAreaRatio) override
    {
        if (cb_ != nullptr) {
            cb_(visibleAreaRatio);
        }
    }

private:
    SurfaceOcclusionChangeCallback cb_;
};

class CustomUIExtensionCallback : public RSUIExtensionCallbackStub {
public:
    explicit CustomUIExtensionCallback(const UIExtensionCallback &callback) : cb_(callback) {}
    ~CustomUIExtensionCallback() override {};

    void OnUIExtension(std::shared_ptr<RSUIExtensionData> uiExtensionData, uint64_t userId) override
    {
        if (cb_ != nullptr) {
            cb_(uiExtensionData, userId);
        }
    }

private:
    UIExtensionCallback cb_;
};

class CustomScreenChangeCallback : public RSScreenChangeCallbackStub {
public:
    explicit CustomScreenChangeCallback(const ScreenChangeCallback &callback) : cb_(callback) {}
    ~CustomScreenChangeCallback() override {};

    void OnScreenChanged(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason) override
    {
        if (cb_ != nullptr) {
            cb_(id, event, reason);
        }
    }

private:
    ScreenChangeCallback cb_;
};

class SurfaceCaptureCallbackDirector : public RSSurfaceCaptureCallbackStub {
public:
    explicit SurfaceCaptureCallbackDirector(RSRenderServiceClient* client) : client_(client) {}
    ~SurfaceCaptureCallbackDirector() override {};
    void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig, Media::PixelMap* pixelmap) override
    {
        std::shared_ptr<Media::PixelMap> surfaceCapture(pixelmap);
        client_->TriggerSurfaceCaptureCallback(id, captureConfig, surfaceCapture);
    };

private:
    RSRenderServiceClient* client_;
};

class TransactionDataCallbackDirector : public RSTransactionDataCallbackStub {
public:
    explicit TransactionDataCallbackDirector (std::shared_ptr<RSRenderServiceClient> client) : client_(client) {}
    ~TransactionDataCallbackDirector() noexcept override = default;
    void OnAfterProcess(int32_t pid, uint64_t timeStamp) override
    {
        if (auto shared_client = client_.lock()) {
            shared_client->TriggerTransactionDataCallbackAndErase(pid, timeStamp);
        } else {
            RS_LOGD("OnAfterProcess: TriggerTransactionDataCallbackAndErase, timeStamp: %{public}"
                PRIu64 " pid: %{public}d", timeStamp, pid);
        }
    }

private:
    std::weak_ptr<RSRenderServiceClient> client_;
};

class CustomSelfDrawingNodeRectChangeCallback : public RSSelfDrawingNodeRectChangeCallbackStub {
public:
    explicit CustomSelfDrawingNodeRectChangeCallback(const SelfDrawingNodeRectChangeCallback& callback) : cb_(callback)
    {}
    ~CustomSelfDrawingNodeRectChangeCallback() override {};

    void OnSelfDrawingNodeRectChange(std::shared_ptr<RSSelfDrawingNodeRectData> rectData) override
    {
        if (cb_ != nullptr) {
            cb_(rectData);
        }
    }

private:
    SelfDrawingNodeRectChangeCallback cb_;
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RS_RENDER_SERVICE_CLIENT_UTILS_H
