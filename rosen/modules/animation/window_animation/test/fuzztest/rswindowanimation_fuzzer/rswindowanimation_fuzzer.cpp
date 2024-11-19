/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "rswindowanimation_fuzzer.h"

#include <securec.h>

#include "rs_window_animation_finished_callback_proxy.h"
#include "rs_window_animation_finished_callback.h"
#include "rs_window_animation_stub.h"
#include "rs_iwindow_animation_finished_callback.h"
#include "rs_window_animation_target.h"
#include "rs_iwindow_animation_controller.h"
#include "rs_window_animation_proxy.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
    using namespace Rosen;
    namespace {
        constexpr size_t STR_LEN = 10;
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
    }
    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    /*
    * get a string from g_data
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = GetData<char>();
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }

    class MockIRemoteObject : public IRemoteObject {
    public:
        MockIRemoteObject() : IRemoteObject {u"MockIRemoteObject"}
        {
        }

        ~MockIRemoteObject()
        {
        }

        int32_t GetObjectRefCount()
        {
            return 0;
        }

        int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
        {
            return sendRequestResult_;
        }

        bool IsProxyObject() const
        {
            return true;
        }

        bool CheckObjectLegality() const
        {
            return true;
        }

        bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
        {
            return true;
        }

        bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
        {
            return true;
        }

        sptr<IRemoteBroker> AsInterface()
        {
            return nullptr;
        }

        int Dump(int fd, const std::vector<std::u16string> &args)
        {
            return 0;
        }

        int sendRequestResult_ = 0;
        int count_ = 0;
    };

    class RSWindowAnimationFinishedCallbackStubMock : public RSWindowAnimationFinishedCallbackStub {
    public:
        RSWindowAnimationFinishedCallbackStubMock() = default;
        virtual ~RSWindowAnimationFinishedCallbackStubMock() = default;

        void OnAnimationFinished() override {};
    };

    class RSWindowAnimationStubMock : public RSWindowAnimationStub {
    public:
        RSWindowAnimationStubMock() = default;
        virtual ~RSWindowAnimationStubMock() = default;

        void OnStartApp(StartingAppType type, const sptr<RSWindowAnimationTarget>& startingWindowTarget,
            const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

        void OnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
            const sptr<RSWindowAnimationTarget>& toWindowTarget,
            const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

        void OnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
            const sptr<RSWindowAnimationTarget>& toWindowTarget,
            const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

        void OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
            const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

        void OnMinimizeAllWindow(std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget,
            const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

        void OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
            const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

        void OnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

        void OnWindowAnimationTargetsUpdate(const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
            const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets) override {};

        void OnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget) override {};
    };

    void FinishedCallbackFuzzTest()
    {
        // test
        sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);
        finishedCallback->OnAnimationFinished();
    }

    void FinishedCallbackProxyFuzzTest()
    {
        // test
        sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);
        sptr<RSWindowAnimationFinishedCallbackProxy> finishedCallbackProxy =
            new RSWindowAnimationFinishedCallbackProxy(finishedCallback);
        finishedCallbackProxy->OnAnimationFinished();
    }

    void FinishedCallbackStubFuzzTest()
    {
        // test
        uint32_t code = GetData<uint32_t>();

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        auto finishedCallbackStub = std::make_shared<RSWindowAnimationFinishedCallbackStubMock>();
        int res = finishedCallbackStub->OnRemoteRequest(code, data, reply, option);

        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationFinishedCallback::GetDescriptor());
        res = finishedCallbackStub->OnRemoteRequest(code, data1, reply, option);
        res = finishedCallbackStub->OnRemoteRequest(
            RSIWindowAnimationFinishedCallback::ON_ANIMATION_FINISHED, data1, reply, option);
        res = finishedCallbackStub->OnRemoteRequest(-1, data1, reply, option);
    }

    void RSWindowAnimationProxy1FuzzTest()
    {
        // get data
        StartingAppType type = GetData<StartingAppType>();
        sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);
        // test
        sptr<RSWindowAnimationProxy> rSWindowAnimationProxy = new RSWindowAnimationProxy(nullptr);
        rSWindowAnimationProxy->OnStartApp(type, nullptr, finishedCallback);
        rSWindowAnimationProxy->OnAppTransition(nullptr, nullptr, finishedCallback);
        rSWindowAnimationProxy->OnAppBackTransition(nullptr, nullptr, finishedCallback);
        rSWindowAnimationProxy->OnMinimizeWindow(nullptr, finishedCallback);
        std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget;
        rSWindowAnimationProxy->OnMinimizeAllWindow(minimizingWindowsTarget, finishedCallback);
        rSWindowAnimationProxy->OnCloseWindow(nullptr, finishedCallback);
        rSWindowAnimationProxy->OnScreenUnlock(finishedCallback);
        std::vector<sptr<RSWindowAnimationTarget>> floatingWindowTargets;
        rSWindowAnimationProxy->OnWindowAnimationTargetsUpdate(nullptr, floatingWindowTargets);
        rSWindowAnimationProxy->OnWallpaperUpdate(nullptr);
    }

    void RSWindowAnimationProxy2FuzzTest()
    {
        // get data
        StartingAppType type = GetData<StartingAppType>();
        std::string bundleName = GetStringFromData(STR_LEN);
        std::string abilityName = GetStringFromData(STR_LEN);
        uint32_t windowId = GetData<uint32_t>();
        uint64_t displayId = GetData<uint64_t>();
        int32_t missionId = GetData<int32_t>();
        RSSurfaceNodeConfig config;
        auto animationSurfaceNode = RSSurfaceNode::Create(config, true);
        sptr<RSWindowAnimationTarget> windowAnimationTarget = new RSWindowAnimationTarget();
        windowAnimationTarget->bundleName_ = bundleName;
        windowAnimationTarget->abilityName_ = abilityName;
        windowAnimationTarget->windowBounds_ = RRect();
        windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
        windowAnimationTarget->windowId_ = windowId;
        windowAnimationTarget->displayId_ = displayId;
        windowAnimationTarget->missionId_ = missionId;
        sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);

        // test
        sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
        sptr<RSWindowAnimationProxy> rSWindowAnimationProxy = new RSWindowAnimationProxy(remoteMocker);
        rSWindowAnimationProxy->OnStartApp(type, windowAnimationTarget, finishedCallback);
        rSWindowAnimationProxy->OnAppTransition(windowAnimationTarget, windowAnimationTarget, finishedCallback);
        rSWindowAnimationProxy->OnAppBackTransition(windowAnimationTarget, windowAnimationTarget, finishedCallback);
        rSWindowAnimationProxy->OnMinimizeWindow(windowAnimationTarget, finishedCallback);
        std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget;
        minimizingWindowsTarget.push_back(windowAnimationTarget);
        rSWindowAnimationProxy->OnMinimizeAllWindow(minimizingWindowsTarget, finishedCallback);
        rSWindowAnimationProxy->OnCloseWindow(windowAnimationTarget, finishedCallback);
        rSWindowAnimationProxy->OnScreenUnlock(finishedCallback);
        rSWindowAnimationProxy->OnWindowAnimationTargetsUpdate(windowAnimationTarget, minimizingWindowsTarget);
        rSWindowAnimationProxy->OnWallpaperUpdate(windowAnimationTarget);
        finishedCallback = nullptr;
    }

    void RSWindowAnimationStubFuzzTest()
    {
        // test
        uint32_t code = GetData<uint32_t>();

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(code, data, reply, option);

        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationFinishedCallback::GetDescriptor());
        res = windowAnimationStub->OnRemoteRequest(code, data, reply, option);
    }

    void RSWindowAnimationStubStartAppFuzzTest()
    {
        // test
        StartingAppType type = GetData<StartingAppType>();
        RSSurfaceNodeConfig config;
        auto animationSurfaceNode = RSSurfaceNode::Create(config, true);
        auto windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
        windowAnimationTarget->bundleName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->abilityName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->windowBounds_ = RRect();
        windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
        windowAnimationTarget->windowId_ = GetData<uint32_t>();
        windowAnimationTarget->displayId_ = GetData<uint64_t>();
        windowAnimationTarget->missionId_ = GetData<int32_t>();
        auto callback = new RSWindowAnimationFinishedCallback(nullptr);

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_START_APP, data, reply, option);
        
        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data1.WriteInt32(type);
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_START_APP, data1, reply, option);

        MessageParcel data2;
        data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data2.WriteInt32(type);
        data2.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_START_APP, data2, reply, option);

        MessageParcel data3;
        data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data3.WriteInt32(type);
        data3.WriteParcelable(windowAnimationTarget.get());
        data3.WriteRemoteObject(callback->AsObject());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_START_APP, data3, reply, option);
        callback = nullptr;
    }

    void RSWindowAnimationStubAppTransitionFuzzTest()
    {
        // test
        RSSurfaceNodeConfig config;
        auto animationSurfaceNode = RSSurfaceNode::Create(config, true);
        auto windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
        windowAnimationTarget->bundleName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->abilityName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->windowBounds_ = RRect();
        windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
        windowAnimationTarget->windowId_ = GetData<uint32_t>();
        windowAnimationTarget->displayId_ = GetData<uint64_t>();
        windowAnimationTarget->missionId_ = GetData<int32_t>();
        auto callback = new RSWindowAnimationFinishedCallback(nullptr);

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_APP_TRANSITION, data, reply, option);
        
        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data1.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_APP_TRANSITION, data1, reply, option);

        MessageParcel data2;
        data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data2.WriteParcelable(windowAnimationTarget.get());
        data2.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_APP_TRANSITION, data2, reply, option);

        MessageParcel data3;
        data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data3.WriteParcelable(windowAnimationTarget.get());
        data3.WriteParcelable(windowAnimationTarget.get());
        data3.WriteRemoteObject(callback->AsObject());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_APP_TRANSITION, data3, reply, option);
        callback = nullptr;
    }

    void RSWindowAnimationStubAppBackTransitionFuzzTest()
    {
        // test
        RSSurfaceNodeConfig config;
        auto animationSurfaceNode = RSSurfaceNode::Create(config, true);
        auto windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
        windowAnimationTarget->bundleName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->abilityName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->windowBounds_ = RRect();
        windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
        windowAnimationTarget->windowId_ = GetData<uint32_t>();
        windowAnimationTarget->displayId_ = GetData<uint64_t>();
        windowAnimationTarget->missionId_ = GetData<int32_t>();
        auto callback = new RSWindowAnimationFinishedCallback(nullptr);

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_APP_BACK_TRANSITION, data, reply, option);
        
        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data1.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_APP_BACK_TRANSITION, data1, reply, option);

        MessageParcel data2;
        data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data2.WriteParcelable(windowAnimationTarget.get());
        data2.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_APP_BACK_TRANSITION, data2, reply, option);

        MessageParcel data3;
        data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data3.WriteParcelable(windowAnimationTarget.get());
        data3.WriteParcelable(windowAnimationTarget.get());
        data3.WriteRemoteObject(callback->AsObject());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_APP_BACK_TRANSITION, data3, reply, option);
        callback = nullptr;
    }

    void RSWindowAnimationStubMinimizeWindowFuzzTest()
    {
        // test
        RSSurfaceNodeConfig config;
        auto animationSurfaceNode = RSSurfaceNode::Create(config, true);
        auto windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
        windowAnimationTarget->bundleName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->abilityName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->windowBounds_ = RRect();
        windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
        windowAnimationTarget->windowId_ = GetData<uint32_t>();
        windowAnimationTarget->displayId_ = GetData<uint64_t>();
        windowAnimationTarget->missionId_ = GetData<int32_t>();
        auto callback = new RSWindowAnimationFinishedCallback(nullptr);

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_MINIMIZE_WINDOW, data, reply, option);
        
        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data1.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_MINIMIZE_WINDOW, data1, reply, option);

        MessageParcel data3;
        data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data3.WriteParcelable(windowAnimationTarget.get());
        data3.WriteRemoteObject(callback->AsObject());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_MINIMIZE_WINDOW, data3, reply, option);
        callback = nullptr;
    }

    void RSWindowAnimationStubMinimizeAllWindowFuzzTest()
    {
        // test
        RSSurfaceNodeConfig config;
        auto animationSurfaceNode = RSSurfaceNode::Create(config, true);
        auto windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
        windowAnimationTarget->bundleName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->abilityName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->windowBounds_ = RRect();
        windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
        windowAnimationTarget->windowId_ = GetData<uint32_t>();
        windowAnimationTarget->displayId_ = GetData<uint64_t>();
        windowAnimationTarget->missionId_ = GetData<int32_t>();
        auto callback = new RSWindowAnimationFinishedCallback(nullptr);

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW, data, reply, option);
        
        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data1.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW, data1, reply, option);

        MessageParcel data2;
        data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data2.WriteUint32(GetData<uint32_t>());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW, data1, reply, option);

        MessageParcel data3;
        data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data3.WriteUint32(1);
        data3.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW, data3, reply, option);

        MessageParcel data4;
        data4.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data4.WriteUint32(1);
        data4.WriteParcelable(windowAnimationTarget.get());
        data4.WriteRemoteObject(callback->AsObject());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW, data4, reply, option);
        callback = nullptr;
    }

    void RSWindowAnimationStubCloseWindowFuzzTest()
    {
        // test
        RSSurfaceNodeConfig config;
        auto animationSurfaceNode = RSSurfaceNode::Create(config, true);
        auto windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
        windowAnimationTarget->bundleName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->abilityName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->windowBounds_ = RRect();
        windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
        windowAnimationTarget->windowId_ = GetData<uint32_t>();
        windowAnimationTarget->displayId_ = GetData<uint64_t>();
        windowAnimationTarget->missionId_ = GetData<int32_t>();
        auto callback = new RSWindowAnimationFinishedCallback(nullptr);

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_CLOSE_WINDOW, data, reply, option);
        
        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data1.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_CLOSE_WINDOW, data1, reply, option);

        MessageParcel data3;
        data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data3.WriteParcelable(windowAnimationTarget.get());
        data3.WriteRemoteObject(callback->AsObject());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_CLOSE_WINDOW, data3, reply, option);
        callback = nullptr;
    }

    void RSWindowAnimationStubScreenUnlockFuzzTest()
    {
        // test
        auto callback = new RSWindowAnimationFinishedCallback(nullptr);

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_SCREEN_UNLOCK, data, reply, option);

        MessageParcel data3;
        data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data3.WriteRemoteObject(callback->AsObject());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_SCREEN_UNLOCK, data3, reply, option);
        callback = nullptr;
    }

    void RSWindowAnimationStubWindowAnimationTargetsUpdateFuzzTest()
    {
        // test
        RSSurfaceNodeConfig config;
        auto animationSurfaceNode = RSSurfaceNode::Create(config, true);
        auto windowAnimationTarget = std::make_shared<RSWindowAnimationTarget>();
        windowAnimationTarget->bundleName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->abilityName_ = GetStringFromData(STR_LEN);
        windowAnimationTarget->windowBounds_ = RRect();
        windowAnimationTarget->surfaceNode_ = animationSurfaceNode;
        windowAnimationTarget->windowId_ = GetData<uint32_t>();
        windowAnimationTarget->displayId_ = GetData<uint64_t>();
        windowAnimationTarget->missionId_ = GetData<int32_t>();

        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE, data, reply, option);
        
        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data1.WriteBool(GetData<bool>());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE, data1, reply, option);

        MessageParcel data2;
        data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data2.WriteBool(GetData<bool>());
        data2.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE, data2, reply, option);

        MessageParcel data3;
        data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data3.WriteBool(GetData<bool>());
        data3.WriteParcelable(windowAnimationTarget.get());
        data3.WriteUint32(GetData<uint32_t>());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE, data3, reply, option);

        MessageParcel data4;
        data4.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data4.WriteBool(GetData<bool>());
        data4.WriteParcelable(windowAnimationTarget.get());
        data4.WriteUint32(1);
        data4.WriteParcelable(windowAnimationTarget.get());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE, data4, reply, option);
    }

    void RSWindowAnimationStubWallpaperUpdateFuzzTest()
    {
        // test
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        auto windowAnimationStub = std::make_shared<RSWindowAnimationStubMock>();
        int res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_WALLPAPER_UPDATE, data, reply, option);
        
        MessageParcel data1;
        data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
        data1.WriteUint32(GetData<uint32_t>());
        res = windowAnimationStub->OnRemoteRequest(
            RSIWindowAnimationController::ON_WALLPAPER_UPDATE, data1, reply, option);
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;
        FinishedCallbackFuzzTest();
        FinishedCallbackProxyFuzzTest();
        FinishedCallbackStubFuzzTest();
        RSWindowAnimationProxy1FuzzTest();
        RSWindowAnimationProxy2FuzzTest();
        RSWindowAnimationStubFuzzTest();
        RSWindowAnimationStubStartAppFuzzTest();
        RSWindowAnimationStubAppTransitionFuzzTest();
        RSWindowAnimationStubAppBackTransitionFuzzTest();
        RSWindowAnimationStubMinimizeWindowFuzzTest();
        RSWindowAnimationStubMinimizeAllWindowFuzzTest();
        RSWindowAnimationStubCloseWindowFuzzTest();
        RSWindowAnimationStubScreenUnlockFuzzTest();
        RSWindowAnimationStubWindowAnimationTargetsUpdateFuzzTest();
        RSWindowAnimationStubWallpaperUpdateFuzzTest();
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

