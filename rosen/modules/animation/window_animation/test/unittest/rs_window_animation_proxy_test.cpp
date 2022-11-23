/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "rs_window_animation_proxy.h"
#include "rs_window_animation_finished_callback.h"
#include "rs_window_animation_target.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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

class RSWindowAnimationProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<RSWindowAnimationProxy> proxy_;
    sptr<RSWindowAnimationProxy> proxyNull_;
    static std::shared_ptr<RSSurfaceNode> animationSurfaceNode;
    sptr<RSWindowAnimationTarget> windowAnimationTarget_;
};
std::shared_ptr<RSSurfaceNode> RSWindowAnimationProxyTest::animationSurfaceNode = nullptr;

void RSWindowAnimationProxyTest::SetUpTestCase()
{
    RSSurfaceNodeConfig config;
    animationSurfaceNode = RSSurfaceNode::Create(config, true);
}

void RSWindowAnimationProxyTest::TearDownTestCase() {}

void RSWindowAnimationProxyTest::SetUp()
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    proxy_ = new RSWindowAnimationProxy(remoteMocker);
    proxyNull_ = new RSWindowAnimationProxy(nullptr);
    windowAnimationTarget_ = new RSWindowAnimationTarget();
    windowAnimationTarget_->bundleName_ = "";
    windowAnimationTarget_->abilityName_ = "";
    windowAnimationTarget_->windowBounds_ = RRect();
    windowAnimationTarget_->surfaceNode_ = animationSurfaceNode;
    windowAnimationTarget_->windowId_ = 0;
    windowAnimationTarget_->displayId_ = 0;
    windowAnimationTarget_->missionId_ = 0;
}

void RSWindowAnimationProxyTest::TearDown() {}

/**
 * @tc.name: OnStartApp001
 * @tc.desc: Verify the OnStartApp
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationProxyTest, OnStartApp001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnStartApp001 start";
    ASSERT_TRUE(proxy_ != nullptr);
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);

    proxyNull_->OnStartApp(StartingAppType::FROM_LAUNCHER, nullptr, finishedCallback);
    proxyNull_->OnStartApp(StartingAppType::FROM_LAUNCHER, windowAnimationTarget_, finishedCallback);

    proxy_->OnStartApp(StartingAppType::FROM_LAUNCHER, nullptr, finishedCallback);
    proxy_->OnStartApp(StartingAppType::FROM_LAUNCHER, windowAnimationTarget_, finishedCallback);
    ASSERT_TRUE(proxy_ != nullptr);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnStartApp001 end";
}

/**
 * @tc.name: OnAppTransition001
 * @tc.desc: Verify the OnAppTransition
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationProxyTest, OnAppTransition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnAppTransition001 start";
    ASSERT_TRUE(proxy_ != nullptr);
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);

    proxyNull_->OnAppTransition(nullptr, nullptr, finishedCallback);
    proxyNull_->OnAppTransition(windowAnimationTarget_, nullptr, finishedCallback);
    proxyNull_->OnAppTransition(windowAnimationTarget_, windowAnimationTarget_, finishedCallback);

    proxy_->OnAppTransition(nullptr, nullptr, finishedCallback);
    proxy_->OnAppTransition(windowAnimationTarget_, nullptr, finishedCallback);
    proxy_->OnAppTransition(windowAnimationTarget_, windowAnimationTarget_, finishedCallback);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnAppTransition001 end";
}

/**
 * @tc.name: OnAppBackTransition001
 * @tc.desc: Verify the OnAppBackTransition
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationProxyTest, OnAppBackTransition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnAppBackTransition001 start";
    ASSERT_TRUE(proxy_ != nullptr);
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);

    proxyNull_->OnAppBackTransition(nullptr, nullptr, finishedCallback);
    proxyNull_->OnAppBackTransition(windowAnimationTarget_, nullptr, finishedCallback);
    proxyNull_->OnAppBackTransition(windowAnimationTarget_, windowAnimationTarget_, finishedCallback);

    proxy_->OnAppBackTransition(nullptr, nullptr, finishedCallback);
    proxy_->OnAppBackTransition(windowAnimationTarget_, nullptr, finishedCallback);
    proxy_->OnAppBackTransition(windowAnimationTarget_, windowAnimationTarget_, finishedCallback);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnAppBackTransition001 end";
}

/**
 * @tc.name: OnMinimizeWindow001
 * @tc.desc: Verify the OnMinimizeWindow
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationProxyTest, OnMinimizeWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnMinimizeWindow001 start";
    ASSERT_TRUE(proxy_ != nullptr);
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);

    proxyNull_->OnMinimizeWindow(nullptr, finishedCallback);
    proxyNull_->OnMinimizeWindow(windowAnimationTarget_, finishedCallback);

    proxy_->OnMinimizeWindow(nullptr, finishedCallback);
    proxy_->OnMinimizeWindow(windowAnimationTarget_, finishedCallback);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnMinimizeWindow001 end";
}

/**
 * @tc.name: OnMinimizeAllWindow001
 * @tc.desc: Verify the OnMinimizeAllWindow
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationProxyTest, OnMinimizeAllWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnMinimizeAllWindow001 start";
    ASSERT_TRUE(proxy_ != nullptr);
    std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget1;
    std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget2;
    minimizingWindowsTarget2.push_back(windowAnimationTarget_);
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);

    proxyNull_->OnMinimizeAllWindow(minimizingWindowsTarget1, nullptr);
    proxyNull_->OnMinimizeAllWindow(minimizingWindowsTarget1, finishedCallback);
    proxyNull_->OnMinimizeAllWindow(minimizingWindowsTarget2, finishedCallback);

    proxy_->OnMinimizeAllWindow(minimizingWindowsTarget1, nullptr);
    proxy_->OnMinimizeAllWindow(minimizingWindowsTarget1, finishedCallback);
    proxy_->OnMinimizeAllWindow(minimizingWindowsTarget2, finishedCallback);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnMinimizeAllWindow001 end";
}

/**
 * @tc.name: OnCloseWindow001
 * @tc.desc: Verify the OnCloseWindow
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationProxyTest, OnCloseWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnCloseWindow001 start";
    ASSERT_TRUE(proxy_ != nullptr);
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);

    proxyNull_->OnCloseWindow(nullptr, finishedCallback);
    proxyNull_->OnCloseWindow(windowAnimationTarget_, finishedCallback);

    proxy_->OnCloseWindow(nullptr, finishedCallback);
    proxy_->OnCloseWindow(windowAnimationTarget_, finishedCallback);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnCloseWindow001 end";
}

/**
 * @tc.name: OnScreenUnlock001
 * @tc.desc: Verify the OnScreenUnlock
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationProxyTest, OnScreenUnlock001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnScreenUnlock001 start";
    ASSERT_TRUE(proxy_ != nullptr);
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new RSWindowAnimationFinishedCallback(nullptr);

    proxyNull_->OnScreenUnlock(finishedCallback);

    proxy_->OnScreenUnlock(finishedCallback);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnScreenUnlock001 end";
}


/**
 * @tc.name: OnWindowAnimationTargetsUpdate001
 * @tc.desc: Verify the OnWindowAnimationTargetsUpdate
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationProxyTest, OnWindowAnimationTargetsUpdate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnWindowAnimationTargetsUpdate001 start";
    ASSERT_TRUE(proxy_ != nullptr);
    std::vector<sptr<RSWindowAnimationTarget>> floatingWindowTargets1;
    for (int i = 0; i < 500; i++) {
        floatingWindowTargets1.push_back(windowAnimationTarget_);
    }
    std::vector<sptr<RSWindowAnimationTarget>> floatingWindowTargets2;
    floatingWindowTargets2.push_back(windowAnimationTarget_);

    proxyNull_->OnWindowAnimationTargetsUpdate(windowAnimationTarget_, floatingWindowTargets1);
    proxyNull_->OnWindowAnimationTargetsUpdate(nullptr, floatingWindowTargets2);
    proxyNull_->OnWindowAnimationTargetsUpdate(windowAnimationTarget_, floatingWindowTargets2);

    proxy_->OnWindowAnimationTargetsUpdate(windowAnimationTarget_, floatingWindowTargets1);
    proxy_->OnWindowAnimationTargetsUpdate(nullptr, floatingWindowTargets2);
    proxy_->OnWindowAnimationTargetsUpdate(windowAnimationTarget_, floatingWindowTargets2);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnWindowAnimationTargetsUpdate001 end";
}

/**
 * @tc.name: OnWallpaperUpdate001
 * @tc.desc: Verify the OnWallpaperUpdate
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationProxyTest, OnWallpaperUpdate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnWallpaperUpdate001 start";
    ASSERT_TRUE(proxy_ != nullptr);

    proxyNull_->OnWallpaperUpdate(nullptr);
    proxyNull_->OnWallpaperUpdate(windowAnimationTarget_);

    proxy_->OnWallpaperUpdate(nullptr);
    proxy_->OnWallpaperUpdate(windowAnimationTarget_);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnWallpaperUpdate001 end";
}

} // namespace Rosen
} // namespace OHOS