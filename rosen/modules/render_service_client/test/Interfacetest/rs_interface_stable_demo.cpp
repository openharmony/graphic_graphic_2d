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

#include <iostream>
#include "rs_interface_client_frame_test.h"
#include "platform/ohos/rs_irender_service_connection_ipc_interface_code.h"
#include "foundation/window/window_manager/interfaces/innerkits/wm/window.h"
using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

class CustomizedSurfaceCapture : public SurfaceCaptureCallback {
public:
    CustomizedSurfaceCapture() {}

    ~CustomizedSurfaceCapture() override {}

    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override {}

    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
}; // class CustomizedSurfaceCapture

class TestSurfaceBufferCallback : public SurfaceBufferCallback {
public:
    void OnFinish(const FinishCallbackRet& ret) {}
    void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) {}
};

int main()
{
    InterfaceClientFrameTest::GetInstance().SetExecFun(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE),
            [](InterfaceClientFrameTest &testFrame) {
                auto& rsInterefaces = RSInterfaces::GetInstance();
                std::shared_ptr<SurfaceCaptureCallback> callback = std::make_shared<CustomizedSurfaceCapture>();
                RSSurfaceCaptureConfig captureConfig;
                rsInterefaces.TakeSurfaceCapture(testFrame.GetRootSurfaceNode(), callback, captureConfig);
                rsInterefaces.TakeSurfaceCapture(testFrame.GetRootSurfaceNode(), nullptr, captureConfig);

                rsInterefaces.TakeSurfaceCaptureWithBlur(testFrame.GetRootSurfaceNode(), callback, captureConfig);

                rsInterefaces.TakeUICaptureInRange(testFrame.GetRootSurfaceNode(),
                    testFrame.GetRootCanvasNode(), 0, callback, 1.0, 1.0, true);
                rsInterefaces.TakeUICaptureInRange(nullptr, nullptr, 1, callback, 1.0, 1.0, true);

                rsInterefaces.TakeSurfaceCaptureSoloNodeList(testFrame.GetRootCanvasNode());
                rsInterefaces.TakeSurfaceCaptureSoloNodeList(nullptr);

                rsInterefaces.TakeSelfSurfaceCapture(testFrame.GetRootSurfaceNode(), callback);
                rsInterefaces.TakeSelfSurfaceCapture(nullptr, callback);
                rsInterefaces.TakeSelfSurfaceCapture(nullptr, nullptr);

                rsInterefaces.TakeSurfaceCaptureWithAllWindows(nullptr, callback, captureConfig, true);
                
                rsInterefaces.SetHwcNodeBounds(testFrame.GetRootCanvasNode()->GetId(), 0, 0, 0, 0);
                rsInterefaces.SetHwcNodeBounds(0, 0, 0, 0, 0);

                rsInterefaces.SetWindowContainer(testFrame.GetRootCanvasNode()->GetId(), 0);
                rsInterefaces.SetWindowContainer(testFrame.GetRootCanvasNode()->GetId(), 1);

                rsInterefaces.ClearUifirstCache(0);
                rsInterefaces.ClearUifirstCache(testFrame.GetRootCanvasNode()->GetId());

                std::vector<int32_t> pidList = {getpid()};
                rsInterefaces.DropFrameByPid(pidList);
                rsInterefaces.DropFrameByPid({});

                rsInterefaces.FreezeScreen(nullptr, true);
                rsInterefaces.FreezeScreen(nullptr, false);

                auto screenId = rsInterefaces.GetDefaultScreenId();
                rsInterefaces.SetScreenFrameGravity(screenId, 0);
                rsInterefaces.SetScreenFrameGravity(-1, 0);

                auto callbackSurface = std::make_shared<TestSurfaceBufferCallback>();
                rsInterefaces.RegisterSurfaceBufferCallback(getpid(), 0, nullptr);
                rsInterefaces.RegisterSurfaceBufferCallback(getpid(), 0, nullptr);
                rsInterefaces.RegisterSurfaceBufferCallback(getpid(), 1, callbackSurface);

                rsInterefaces.UnregisterSurfaceBufferCallback(0, 1);
                rsInterefaces.UnregisterSurfaceBufferCallback(getpid(), 1);

                HdrStatus hdrStatus;
                rsInterefaces.GetScreenHDRStatus(screenId, hdrStatus);
                rsInterefaces.GetScreenHDRStatus(0, hdrStatus);

                rsInterefaces.SetLayerTopForHWC(testFrame.GetRootCanvasNode()->GetId(), 0, 0);
                rsInterefaces.SetLayerTopForHWC(0, 0, 0);

                rsInterefaces.SetAncoForceDoDirect(true);
                rsInterefaces.SetAncoForceDoDirect(false);

                FocusAppInfo info;
                rsInterefaces.SetFocusAppInfo(info);
        }
    );

    InterfaceClientFrameTest::GetInstance().Run();
    return EXIT_SUCCESS;
}