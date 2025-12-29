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
#include "layer/rs_surface_layer.h"
#include "irs_composer_to_render_connection.h"
#include "irs_render_to_composer_connection.h"
#include "pipeline/rs_render_composer_client.h"
#include "nativetoken_kit.h"
#include "rs_render_composer_manager.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderComposerClientRemoteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<IRemoteObject> robj_ = nullptr;
    std::shared_ptr<RSRenderComposerClient> composerClient_ = nullptr;
    static inline pid_t pid_ = 0;
    static inline int pipeFd_[2] = {};
    static inline int pipe1Fd_[2] = {};
    static inline int32_t systemAbilityID_ = 345135;
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

void RSRenderComposerClientRemoteTest::SetUpTestCase()
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
        InitNativeTokenInfo();
        uint32_t screenId = 10;
        std::shared_ptr<HdiOutput> output = std::make_shared<HdiOutput>(screenId);
        sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
        property->width_ = 2048;
        property->height_ = 1024;
        std::shared_ptr<RSRenderComposer> renderComposer = std::make_shared<RSRenderComposer>(output, property);
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
        sptr<RSRenderToComposerConnection> renderToComposer =
            sptr<RSRenderToComposerConnection>::MakeSptr("composer_conn_test", screenId, renderComposerAgent);
        ASSERT_NE(renderToComposer, nullptr);

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
        close(pipeFd_[0]);
        close(pipe1Fd_[1]);
        char buf[10];
        read(pipe1Fd_[0], buf, sizeof(buf));
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj_ = sam->GetSystemAbility(systemAbilityID_);
        sptr<RSRenderToComposerConnection> renderToComposer = iface_cast<RSRenderToComposerConnection>(robj_);
        sptr<RSRenderToComposerConnection> composerToRender = sptr<RSComposerToRenderConnection>::MakeSptr();
        composerClient_ = RSRenderComposerClient::Create(renderToComposer, composerToRender, nullptr);
    }
}

void RSRenderComposerClientRemoteTest::TearDownTestCase()
{
    composerClient_ = nullptr;
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
* Function: IsProxyObject
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. check ret for IsProxyObject func
 */
HWTEST_F(RSRenderComposerClientRemoteTest, IsProxy001, TestSize.Level0)
{
    ASSERT_TRUE(robj_->IsProxyObject());
}
}
