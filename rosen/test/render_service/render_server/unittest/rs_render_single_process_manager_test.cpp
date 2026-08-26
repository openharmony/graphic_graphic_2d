/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "parameters.h"
#include "common/rs_background_thread.h"
#include "feature/color_picker/rs_color_picker_thread.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "render_process/transaction/rs_service_to_render_connection.h"
#include "render_server/transaction/rs_render_to_service_connection.h"
#include "rs_composer_to_render_connection.h"
#include "rs_render_composer_agent.h"
#include "rs_render_composer_manager.h"
#include "rs_render_single_process_manager.h"
#include "rs_render_to_composer_connection.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/rs_connect_to_render_process.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
class RSSingleRenderProcessManagerMock : public RSRenderProcessManager {
public:
    explicit RSSingleRenderProcessManagerMock(RSRenderService& renderService_)
        : RSRenderProcessManager(renderService_)
    {
        // step2: Create renderPipeline and Following Connections
        auto renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService_);
        auto renderProcessManagerAgent =
            sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService_.renderProcessManager_);
        auto screenManagerAgent = sptr<RSScreenManagerAgent>::MakeSptr(renderService_.screenManager_);
        renderToServiceConnection_ = sptr<RSRenderToServiceConnection>::MakeSptr(renderServiceAgent,
            renderProcessManagerAgent, screenManagerAgent);
        renderService_.renderPipeline_ = RSRenderPipeline::Create(renderService_.handler_,
            nullptr, renderToServiceConnection_, nullptr);

        auto mainThread = RSMainThread::Instance();
        renderService_.renderPipeline_->mainThread_ = mainThread;
        mainThread->RegisterScreenSwitchFinishCallback(renderToServiceConnection_);

        renderService_.renderPipeline_->uniRenderThread_ = &(RSUniRenderThread::Instance());

        auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderService_.renderPipeline_);
        serviceToRenderConnection_ = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
        composerToRenderConnection_ = sptr<RSComposerToRenderConnection>::MakeSptr();

        // step3:
        connectToRenderConnection_ = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);

        // Cancel Thread
        renderService_.renderPipeline_->uniRenderThread_->handler_ = nullptr;
        renderService_.renderPipeline_->uniRenderThread_->runner_ = nullptr;
        renderService_.renderPipeline_->mainThread_->handler_ = nullptr;
    }

    ~RSSingleRenderProcessManagerMock() noexcept override = default;

    sptr<IRemoteObject> OnScreenConnected(ScreenId screenId_,
        const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property) override
    {
        auto composerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(property->GetScreenId());
        renderService_.renderPipeline_->OnScreenConnected(property, composerConn, composerToRenderConnection_, output);
        return connectToRenderConnection_->AsObject();
    }

    void OnScreenDisconnected(ScreenId id) override
    {
        renderService_.renderPipeline_->OnScreenDisconnected(id);
    }

    void OnScreenPropertyChanged(
        ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property) override
    {
        renderService_.renderPipeline_->OnScreenPropertyChanged(id, type, property);
    }

    void OnScreenRefresh(ScreenId id) override
    {
        renderService_.renderPipeline_->OnScreenRefresh(id);
    }

    void OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
        const sptr<RSScreenProperty>& property) override
    {
        renderService_.renderPipeline_->OnScreenConnected(property, nullptr, nullptr, nullptr);
    }

    void OnVirtualScreenDisconnected(ScreenId id) override
    {
        renderService_.renderPipeline_->OnScreenDisconnected(id);
    }

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const override
    {
        return serviceToRenderConnection_;
    }

    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const override
    {
        return { serviceToRenderConnection_ };
    }

    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const override
    {
        return connectToRenderConnection_;
    }
    sptr<IRemoteObject> CreateRenderToServiceConnection(pid_t callingPid) override { return nullptr; }
    int32_t SendTransfer(const std::shared_ptr<RSIpcTransferBase>& transfer) override { return RS_CONNECTION_ERROR; }
    sptr<RSIServiceToRenderConnection> serviceToRenderConnection_ = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection_ = nullptr;
    sptr<RSIRenderToServiceConnection> renderToServiceConnection_ = nullptr;
    sptr<RSIConnectToRenderProcess> connectToRenderConnection_ = nullptr;
};
} // namespace

class RSRenderSingleProcessManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static uint32_t screenId_;
private:
    static inline RSRenderService renderService_;
};

uint32_t RSRenderSingleProcessManagerTest::screenId_ = 0;

void RSRenderSingleProcessManagerTest::SetUpTestCase()
{
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    renderService_.handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    renderService_.renderProcessManager_ = sptr<RSSingleRenderProcessManagerMock>::MakeSptr(renderService_);
    renderService_.rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(
        renderService_.handler_);
}
void RSRenderSingleProcessManagerTest::TearDownTestCase()
{
    // Clean up RSColorPickerThread GPU references before releasing engine
    auto& colorPickerThread = RSColorPickerThread::Instance();
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    colorPickerThread.gpuContext_ = nullptr;
    colorPickerThread.renderContext_ = nullptr;
#endif

    // Clean up RSBackgroundThread GPU references
    auto& backgroundThread = RSBackgroundThread::Instance();
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    backgroundThread.gpuContext_ = nullptr;
    backgroundThread.renderContext_ = nullptr;
#endif

    // Full cleanup of RSMainThread renderEngine
    auto mainThread = RSMainThread::Instance();
    if (mainThread->renderEngine_) {
        if (mainThread->renderEngine_->renderContext_) {
            mainThread->renderEngine_->renderContext_->drGPUContext_ = nullptr;
            mainThread->renderEngine_->renderContext_ = nullptr;
        }
        if (mainThread->renderEngine_->protectedRenderContext_) {
            mainThread->renderEngine_->protectedRenderContext_->drGPUContext_ = nullptr;
            mainThread->renderEngine_->protectedRenderContext_ = nullptr;
        }
        mainThread->renderEngine_->skContext_ = nullptr;
        mainThread->renderEngine_->imageManager_ = nullptr;
        mainThread->renderEngine_->gpuCacheManager_ = nullptr;
#ifdef USE_VIDEO_PROCESSING_ENGINE
        mainThread->renderEngine_->colorSpaceConverterDisplay_ = nullptr;
#endif
        mainThread->renderEngine_ = nullptr;
    }

    // Full cleanup of RSUniRenderThread uniRenderEngine
    auto& uniRenderThread = RSUniRenderThread::Instance();
    if (uniRenderThread.uniRenderEngine_) {
        if (uniRenderThread.uniRenderEngine_->renderContext_) {
            uniRenderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
            uniRenderThread.uniRenderEngine_->renderContext_ = nullptr;
        }
        if (uniRenderThread.uniRenderEngine_->protectedRenderContext_) {
            uniRenderThread.uniRenderEngine_->protectedRenderContext_->drGPUContext_ = nullptr;
            uniRenderThread.uniRenderEngine_->protectedRenderContext_ = nullptr;
        }
        uniRenderThread.uniRenderEngine_->skContext_ = nullptr;
        uniRenderThread.uniRenderEngine_->imageManager_ = nullptr;
        uniRenderThread.uniRenderEngine_->gpuCacheManager_ = nullptr;
#ifdef USE_VIDEO_PROCESSING_ENGINE
        uniRenderThread.uniRenderEngine_->colorSpaceConverterDisplay_ = nullptr;
#endif
        uniRenderThread.uniRenderEngine_ = nullptr;
    }
}
void RSRenderSingleProcessManagerTest::SetUp() {}
void RSRenderSingleProcessManagerTest::TearDown() {}

/**
 * @tc.name: OnScreenConnectedTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderSingleProcessManagerTest, OnScreenConnectedTest, TestSize.Level1)
{
    renderService_.renderProcessManager_->OnScreenDisconnected(screenId_);
    renderService_.renderProcessManager_->OnScreenRefresh(screenId_);
    auto output = std::make_shared<HdiOutput>(screenId_);
    output->Init();
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    renderService_.renderProcessManager_->OnScreenConnected(screenId_, output, property);
    sptr<ScreenPropertyBase> propertyBase = sptr<ScreenProperty<bool>>::MakeSptr();
    renderService_.renderProcessManager_->OnScreenPropertyChanged(
        screenId_, ScreenPropertyType::IS_VIRTUAL, propertyBase);
    ASSERT_TRUE(renderService_.renderProcessManager_);
}

/**
 * @tc.name: OnVirtualScreenConnectedTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderSingleProcessManagerTest, OnVirtualScreenConnectedTest, TestSize.Level1)
{
    renderService_.renderProcessManager_->OnVirtualScreenDisconnected(screenId_);
    renderService_.renderProcessManager_->GetServiceToRenderConn(screenId_);
    renderService_.renderProcessManager_->GetServiceToRenderConns();
    renderService_.renderProcessManager_->GetConnectToRenderConnection(screenId_);
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    property->Set<ScreenPropertyType::IS_VIRTUAL>(true);
    renderService_.renderProcessManager_->OnVirtualScreenConnected(screenId_, INVALID_SCREEN_ID, property);
    ASSERT_TRUE(renderService_.renderProcessManager_);
}
} // namespace OHOS::Rosen
