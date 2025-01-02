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

#include "rsuidirector_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "animation/rs_render_animation.h"
#include "pipeline/rs_render_result.h"
#include "modifier/rs_modifier_manager.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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

bool DoCreate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSUIDirector::Create();
    return true;
}

bool DoGoBackground(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isTextureExport = GetData<bool>();
    director->GoBackground(isTextureExport);
    return true;
}

bool DoGoForeground(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isTextureExport = GetData<bool>();
    director->GoForeground(isTextureExport);
    return true;
}

bool DoInit(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool shouldCreateRenderThread = GetData<bool>();
    director->Init(shouldCreateRenderThread);
    return true;
}

bool DoStartTextureExport(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->StartTextureExport();
    return true;
}

bool DoDestroy(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isTextureExport = GetData<bool>();
    director->Destroy(isTextureExport);
    return true;
}

bool DoSetRSSurfaceNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    director->SetRSSurfaceNode(surfaceNode);
    return true;
}

bool DoSetAbilityBGAlpha(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    uint8_t alpha = GetData<uint8_t>();
    director->SetAbilityBGAlpha(alpha);
    return true;
}

bool DoSetRTRenderForced(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isRenderForced = GetData<bool>();
    director->SetRTRenderForced(isRenderForced);
    return true;
}

bool DoSetContainerWindow(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool hasContainerWindow = GetData<bool>();
    RRect rrect = GetData<RRect>();
    director->SetContainerWindow(hasContainerWindow, rrect);
    return true;
}

bool DoSetFlushEmptyCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    FlushEmptyCallback flushEmptyCallback;
    director->SetFlushEmptyCallback(flushEmptyCallback);
    return true;
}

bool DoSetRoot(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    NodeId root = GetData<NodeId>();
    director->SetRoot(root);
    return true;
}

bool DoSetUITaskRunner(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetUITaskRunner([&](const std::function<void()>& task, uint32_t delay) {});
    return true;
}

bool DoSendMessages(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SendMessages();
    return true;
}

bool DoSetTimeStamp(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    uint64_t timeStamp = GetData<uint64_t>();
    std::string cacheFilePath = "test";
    director->SetTimeStamp(timeStamp, cacheFilePath);
    return true;
}

bool DoSetCacheDir(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    std::string cacheFilePath = "test";
    director->SetCacheDir(cacheFilePath);
    return true;
}

bool DoFlushAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    uint64_t timeStamp = GetData<uint64_t>();
    uint64_t vsyncPeriod = GetData<uint64_t>();
    director->FlushAnimation(timeStamp, vsyncPeriod);
    return true;
}

bool DoFlushModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->FlushModifier();
    return true;
}

bool DoHasUIAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->HasUIRunningAnimation();
    return true;
}

bool DoFlushAnimationStartTime(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    uint64_t timeStamp = GetData<uint64_t>();
    director->FlushAnimationStartTime(timeStamp);
    return true;
}

bool DoSetAppFreeze(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isAppFreeze = GetData<bool>();
    director->SetAppFreeze(isAppFreeze);
    return true;
}

bool DoSetRequestVsyncCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    const std::function<void()>& callback = []() {
        std::cout << "for test" << std::endl;
    };
    director->SetRequestVsyncCallback(callback);
    return true;
}

bool DoPostFrameRateTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    const std::function<void()>& task = []() {
        std::cout << "for test" << std::endl;
    };
    director->PostFrameRateTask(task);
    return true;
}

bool DoGetCurrentRefreshRateMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->GetCurrentRefreshRateMode();
    return true;
}

bool DoGetAnimateExpectedRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->GetAnimateExpectedRate();
    return true;
}

bool DoAttachSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->AttachSurface();
    return true;
}

bool DoRecvMessages001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->RecvMessages();
    return true;
}

bool DoRecvMessages002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    std::shared_ptr<RSTransactionData> cmds = std::make_shared<RSTransactionData>();
    director->RecvMessages(cmds);
    return true;
}

bool DoProcessMessages(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    std::shared_ptr<RSTransactionData> cmds = std::make_shared<RSTransactionData>();
    director->ProcessMessages(cmds);
    return true;
}

bool DoAnimationCallbackProcessor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    NodeId nodeId = GetData<NodeId>();
    AnimationId animId = GetData<AnimationId>();
    AnimationCallbackEvent event = GetData<AnimationCallbackEvent>();
    director->AnimationCallbackProcessor(nodeId, animId, event);
    return true;
}

bool DoPostTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    const std::function<void()>& task = []() {
        std::cout << "for test" << std::endl;
    };
    director->PostTask(task);
    return true;
}

bool DoHasFirstFrameAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->HasFirstFrameAnimation();
    return true;
}

bool DoHasUIRunningAnimation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->HasUIRunningAnimation();
    return true;
}

bool DoGetIndex(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->GetIndex();
    return true;
}

bool DoDumpNodeTreeProcessor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId nodeId = GetData<NodeId>();
    pid_t pid = GetData<pid_t>();
    uint32_t taskId = GetData<uint32_t>();
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->DumpNodeTreeProcessor(nodeId, pid, taskId);
    return true;
}

bool DoPostDelayTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    const std::function<void()>& task = []() {
        std::cout << "for test" << std::endl;
    };
    uint32_t delay = GetData<uint32_t>();
    int32_t instanceId = GetData<int32_t>();
    director->PostDelayTask(task);
    director->PostDelayTask(task, delay, instanceId);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreate(data, size);
    OHOS::Rosen::DoGoBackground(data, size);
    OHOS::Rosen::DoGoForeground(data, size);
    OHOS::Rosen::DoInit(data, size);
    OHOS::Rosen::DoStartTextureExport(data, size);
    OHOS::Rosen::DoDestroy(data, size);
    OHOS::Rosen::DoSetRSSurfaceNode(data, size);
    OHOS::Rosen::DoSetAbilityBGAlpha(data, size);
    OHOS::Rosen::DoSetRTRenderForced(data, size);
    OHOS::Rosen::DoSetContainerWindow(data, size);
    OHOS::Rosen::DoSetFlushEmptyCallback(data, size);
    OHOS::Rosen::DoSetRoot(data, size);
    OHOS::Rosen::DoSetUITaskRunner(data, size);
    OHOS::Rosen::DoSendMessages(data, size);
    OHOS::Rosen::DoSetTimeStamp(data, size);
    OHOS::Rosen::DoSetCacheDir(data, size);
    OHOS::Rosen::DoFlushAnimation(data, size);
    OHOS::Rosen::DoFlushModifier(data, size);
    OHOS::Rosen::DoHasUIAnimation(data, size);
    OHOS::Rosen::DoFlushAnimationStartTime(data, size);
    OHOS::Rosen::DoSetAppFreeze(data, size);
    OHOS::Rosen::DoSetRequestVsyncCallback(data, size);
    OHOS::Rosen::DoPostFrameRateTask(data, size);
    OHOS::Rosen::DoGetCurrentRefreshRateMode(data, size);
    OHOS::Rosen::DoGetAnimateExpectedRate(data, size);
    OHOS::Rosen::DoAttachSurface(data, size);
    OHOS::Rosen::DoRecvMessages001(data, size);
    OHOS::Rosen::DoRecvMessages002(data, size);
    OHOS::Rosen::DoProcessMessages(data, size);
    OHOS::Rosen::DoAnimationCallbackProcessor(data, size);
    OHOS::Rosen::DoPostTask(data, size);
    OHOS::Rosen::DoHasFirstFrameAnimation(data, size);
    OHOS::Rosen::DoHasUIRunningAnimation(data, size);
    OHOS::Rosen::DoGetIndex(data, size);
    OHOS::Rosen::DoDumpNodeTreeProcessor(data, size);
    OHOS::Rosen::DoPostDelayTask(data, size);
    return 0;
}

