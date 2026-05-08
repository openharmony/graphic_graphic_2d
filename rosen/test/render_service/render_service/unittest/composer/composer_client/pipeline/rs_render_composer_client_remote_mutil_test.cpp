/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#include <chrono>
#include <thread>
#include <vector>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <gtest/gtest.h>

#include <iservice_registry.h>
#include "accesstoken_kit.h"
#include "layer/rs_surface_rcd_layer.h"
#include "rs_composer_to_render_connection.h"
#include "rs_render_to_composer_connection.h"
#include "pipeline/rs_composer_client.h"
#include "nativetoken_kit.h"
#include "rs_render_composer_manager.h"
#include "surface_buffer_impl.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderComposerClientRemoteMutilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<IRemoteObject> robj_ = nullptr;
    static inline std::shared_ptr<RSComposerClient> composerClient_ = nullptr;
    static inline std::shared_ptr<RSComposerClient> composerClientRemote_ = nullptr;
    static inline pid_t pid_ = 0;
    static inline int pipeFd_[2] = {};
    static inline int pipe1Fd_[2] = {};
    static inline int32_t systemAbilityID_ = 345135;
    static inline sptr<SurfaceBuffer> bufferRemote_ = nullptr;
};

static void InitNativeTokenInfo()
{
    uint64_t tokenId;
    const char *perms[2];
    perms[0] = "ohos.permission.DISTRIBUTED_DATASYNC";
    perms[1] = "ohos.permission.CAMERA";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 2,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "dcamera_client_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    int32_t ret = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    ASSERT_EQ(ret, Security::AccessToken::RET_SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // wait 50ms
}

void RSRenderComposerClientRemoteMutilTest::SetUpTestCase()
{
    if (pipe(pipeFd_) < 0) {
        exit(1);
    }
    if (pipe(pipe1Fd_) < 0) {
        exit(0);
    }
    pid_ = fork();
    if (pid_ < 0) {
        exit(1);
    }
    if (pid_ == 0) {
        // render_service
#ifdef RS_ENABLE_VK
        RsVulkanContext::SetRecyclable(false);
#endif
        InitNativeTokenInfo();
        uint32_t screenId = 10;
        std::shared_ptr<HdiOutput> output = std::make_shared<HdiOutput>(screenId);
        sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
        property->Set<ScreenPropertyType::RENDER_RESOLUTION>(std::make_pair(2048, 1024));
        std::shared_ptr<RSRenderComposer> renderComposer = std::make_shared<RSRenderComposer>(output, property);
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
        sptr<RSRenderToComposerConnection> renderToComposer =
            sptr<RSRenderToComposerConnection>::MakeSptr("composer_conn_test", screenId, renderComposerAgent);
        ASSERT_NE(renderToComposer, nullptr);
        sptr<IRSComposerToRenderConnection> composerToRender = sptr<RSComposerToRenderConnection>::MakeSptr();
        composerClient_ = RSComposerClient::Create(renderToComposer, composerToRender);

        std::shared_ptr<RSLayer> rsLayer2 = RSSurfaceLayer::Create(1, composerClient_->GetComposerContext());
        EXPECT_NE(rsLayer2, nullptr);
        sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
        ASSERT_NE(buffer, nullptr);
        int32_t width = 100;
        int32_t height = 100;
        BufferRequestConfig cfg { width, height, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
            BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
        auto ret = buffer->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);

        auto bufferAddr = static_cast<uint32_t*>(buffer->GetVirAddr());
        if (bufferAddr != nullptr) {
            int32_t stride = buffer->GetStride();
            int32_t strideInPixels = stride / sizeof(uint32_t);
            for (int32_t y = 0; y < height; y++) {
                uint32_t* rowStart = bufferAddr + y * strideInPixels;
                for (int32_t x = 0; x < strideInPixels; x++) {
                    rowStart[x] = x;
                }
            }
        }
        rsLayer2->SetBuffer(buffer);
        ComposerInfo composerInfo;
        composerClient_->CommitLayers(composerInfo);

        bufferRemote_ = SurfaceBuffer::Create();
        ASSERT_NE(bufferRemote_, nullptr);
        ret = bufferRemote_->Alloc(cfg);
        ASSERT_EQ(ret, GSERROR_OK);

        auto bufferRemoteAddr = static_cast<uint32_t*>(bufferRemote_->GetVirAddr());
        if (bufferRemoteAddr != nullptr) {
            int32_t stride = bufferRemote_->GetStride();
            int32_t strideInPixels = stride / sizeof(uint32_t);
            for (int32_t y = 0; y < height; y++) {
                uint32_t* originRowStart = bufferAddr + y * strideInPixels;
                uint32_t* rowStart = bufferRemoteAddr + y * strideInPixels;
                for (int32_t x = 0; x < strideInPixels; x++) {
                    rowStart[x] = originRowStart[x];
                }
            }
        }

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sam->AddSystemAbility(systemAbilityID_, renderToComposer);
        close(pipeFd_[1]);
        close(pipe1Fd_[0]);
        char buf[10] = "start";
        write(pipe1Fd_[1], buf, sizeof(buf));
        sleep(0);
        read(pipeFd_[0], buf, sizeof(buf));
        sam->RemoveSystemAbility(systemAbilityID_);
        close(pipeFd_[0]);
        close(pipe1Fd_[1]);
        exit(0);
    } else {
        // render_process
#ifdef RS_ENABLE_VK
        RsVulkanContext::SetRecyclable(false);
#endif
        close(pipeFd_[0]);
        close(pipe1Fd_[1]);
        char buf[10];
        read(pipe1Fd_[0], buf, sizeof(buf));
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj_ = sam->GetSystemAbility(systemAbilityID_);
        sptr<IRSRenderToComposerConnection> renderToComposer = iface_cast<IRSRenderToComposerConnection>(robj_);
        sptr<IRSComposerToRenderConnection> composerToRender = sptr<RSComposerToRenderConnection>::MakeSptr();
        composerClientRemote_ = RSComposerClient::Create(renderToComposer, composerToRender);
    }
}

void RSRenderComposerClientRemoteMutilTest::TearDownTestCase()
{
    composerClientRemote_ = nullptr;
    robj_ = nullptr;

    char buf[10] = "over";
    write(pipeFd_[1], buf, sizeof(buf));
    close(pipeFd_[1]);
    close(pipe1Fd_[0]);

    int32_t ret = 0;
    do {
        waitpid(pid_, nullptr, 0);
    } while (ret == -1 && errno == EINTR);
}

/*
 * Function: CommitLayers001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. check ret for CommitLayers func
 */
HWTEST_F(RSRenderComposerClientRemoteMutilTest, CommitLayers001, TestSize.Level0)
{
    std::shared_ptr<RSLayer> rsLayer1 = RSSurfaceLayer::Create(1, composerClientRemote_->GetComposerContext());
    EXPECT_NE(rsLayer1, nullptr);

    rsLayer1->SetBuffer(bufferRemote_);
    ComposerInfo composerInfo;
    composerClientRemote_->CommitLayers(composerInfo);
}
}
