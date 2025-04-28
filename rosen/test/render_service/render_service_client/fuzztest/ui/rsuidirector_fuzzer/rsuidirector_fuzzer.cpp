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
const uint8_t DO_CREATE = 0;
const uint8_t DO_GO_BACKGROUND = 1;
const uint8_t DO_GO_FOREGROUND = 2;
const uint8_t DO_INIT = 3;
const uint8_t DO_START_TEXTURE_EXPORT = 4;
const uint8_t DO_DESTROY = 5;
const uint8_t DO_SET_RSSURFACE_NODE = 6;
const uint8_t DO_SET_ABILITY_BG_ALPHA = 7;
const uint8_t DO_SET_RTRENDER_FORCED = 8;
const uint8_t DO_SET_CONTAINER_WINDOW = 9;
const uint8_t DO_SET_FLUSH_EMPTY_CALLBACK = 10;
const uint8_t DO_SET_ROOT = 11;
const uint8_t DO_SET_UITASK_RUNNER = 12;
const uint8_t DO_SEND_MESSAGES = 13;
const uint8_t DO_SET_TIMESTAMP = 14;
const uint8_t DO_SET_CACHE_DIR = 15;
const uint8_t DO_FLUSH_ANIMATION = 16;
const uint8_t DO_FLUSH_MODIFIER = 17;
const uint8_t DO_HAS_UI_ANIMATION = 18;
const uint8_t DO_FLUSH_ANIMATION_START_TIME = 19;
const uint8_t DO_SET_APP_FREEZE = 20;
const uint8_t DO_SET_REQUEST_VSYNC_CALLBACK = 21;
const uint8_t DO_POST_FRAME_RATE_TASK = 22;
const uint8_t DO_GET_CURRENT_REFRESH_RATE_MODE = 23;
const uint8_t DO_GET_ANIMATE_EXPECTED_RATE = 24;
const uint8_t DO_ATTACH_SURFACE = 25;
const uint8_t DO_RECV_MESSAGES_001 = 26;
const uint8_t DO_RECV_MESSAGES_002 = 27;
const uint8_t DO_PROCESS_MESSAGES = 28;
const uint8_t DO_ANIMATION_CALLBACK_PROCESSOR = 29;
const uint8_t DO_POST_TASK = 30;
const uint8_t DO_HAS_FIRST_FRAME_ANIMATION = 31;
const uint8_t DO_HAS_UIRUNNING_ANIMATION = 32;
const uint8_t DO_GET_INDEX = 33;
const uint8_t DO_DUMP_NODE_TREE_PROCESSOR = 34;
const uint8_t DO_POST_DELAY_TASK = 35;
const uint8_t TARGET_SIZE = 36;

const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

void DoCreate(const uint8_t* data, size_t size)
{
    RSUIDirector::Create();
}

void DoGoBackground(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isTextureExport = GetData<bool>();
    director->GoBackground(isTextureExport);
}

void DoGoForeground(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isTextureExport = GetData<bool>();
    director->GoForeground(isTextureExport);
}

void DoInit(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool shouldCreateRenderThread = GetData<bool>();
    director->Init(shouldCreateRenderThread);
}

void DoStartTextureExport(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->StartTextureExport();
}

void DoDestroy(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isTextureExport = GetData<bool>();
    director->Destroy(isTextureExport);
}

void DoSetRSSurfaceNode(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    director->SetRSSurfaceNode(surfaceNode);
}

void DoSetAbilityBGAlpha(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    uint8_t alpha = GetData<uint8_t>();
    director->SetAbilityBGAlpha(alpha);
}

void DoSetRTRenderForced(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isRenderForced = GetData<bool>();
    director->SetRTRenderForced(isRenderForced);
}

void DoSetContainerWindow(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool hasContainerWindow = GetData<bool>();
    RRect rrect = GetData<RRect>();
    director->SetContainerWindow(hasContainerWindow, rrect);
}

void DoSetFlushEmptyCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    FlushEmptyCallback flushEmptyCallback;
    director->SetFlushEmptyCallback(flushEmptyCallback);
}

void DoSetRoot(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    NodeId root = GetData<NodeId>();
    director->SetRoot(root);
}

void DoSetUITaskRunner(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetUITaskRunner([&](const std::function<void()>& task, uint32_t delay) {});
}

void DoSendMessages(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SendMessages();
}

void DoSetTimeStamp(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    uint64_t timeStamp = GetData<uint64_t>();
    std::string cacheFilePath = "test";
    director->SetTimeStamp(timeStamp, cacheFilePath);
}

void DoSetCacheDir(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    std::string cacheFilePath = "test";
    director->SetCacheDir(cacheFilePath);
}

void DoFlushAnimation(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    uint64_t timeStamp = GetData<uint64_t>();
    uint64_t vsyncPeriod = GetData<uint64_t>();
    director->FlushAnimation(timeStamp, vsyncPeriod);
}

void DoFlushModifier(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->FlushModifier();
}

void DoHasUIAnimation(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->HasUIRunningAnimation();
}

void DoFlushAnimationStartTime(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    uint64_t timeStamp = GetData<uint64_t>();
    director->FlushAnimationStartTime(timeStamp);
}

void DoSetAppFreeze(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    bool isAppFreeze = GetData<bool>();
    director->SetAppFreeze(isAppFreeze);
}

void DoSetRequestVsyncCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    const std::function<void()>& callback = []() {
        std::cout << "for test" << std::endl;
    };
    director->SetRequestVsyncCallback(callback);
}

void DoPostFrameRateTask(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    const std::function<void()>& task = []() {
        std::cout << "for test" << std::endl;
    };
    director->PostFrameRateTask(task);
}

void DoGetCurrentRefreshRateMode(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->GetCurrentRefreshRateMode();
}

void DoGetAnimateExpectedRate(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->GetAnimateExpectedRate();
}

void DoAttachSurface(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->AttachSurface();
}

void DoRecvMessages001(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->RecvMessages();
}

void DoRecvMessages002(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    std::shared_ptr<RSTransactionData> cmds = std::make_shared<RSTransactionData>();
    director->RecvMessages(cmds);
}

void DoProcessMessages(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    std::shared_ptr<RSTransactionData> cmds = std::make_shared<RSTransactionData>();
    director->ProcessMessages(cmds);
}

void DoAnimationCallbackProcessor(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    NodeId nodeId = GetData<NodeId>();
    AnimationId animId = GetData<AnimationId>();
    uint64_t token = GetData<uint64_t>();
    AnimationCallbackEvent event = GetData<AnimationCallbackEvent>();
    director->AnimationCallbackProcessor(nodeId, animId, token, event);
}

void DoPostTask(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    const std::function<void()>& task = []() {
        std::cout << "for test" << std::endl;
    };
    director->PostTask(task);
}

void DoHasFirstFrameAnimation(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->HasFirstFrameAnimation();
}

void DoHasUIRunningAnimation(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->HasUIRunningAnimation();
}

void DoGetIndex(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->GetIndex();
}

void DoDumpNodeTreeProcessor(const uint8_t* data, size_t size)
{
    NodeId nodeId = GetData<NodeId>();
    pid_t pid = GetData<pid_t>();
    uint32_t taskId = GetData<uint32_t>();
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->DumpNodeTreeProcessor(nodeId, pid, taskId);
}

void DoPostDelayTask(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    const std::function<void()>& task = []() {
        std::cout << "for test" << std::endl;
    };
    uint32_t delay = GetData<uint32_t>();
    int32_t instanceId = GetData<int32_t>();
    director->PostDelayTask(task);
    director->PostDelayTask(task, delay, instanceId);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE:
            OHOS::Rosen::DoCreate(data, size);
            break;
        case OHOS::Rosen::DO_GO_BACKGROUND:
            OHOS::Rosen::DoGoBackground(data, size);
            break;
        case OHOS::Rosen::DO_GO_FOREGROUND:
            OHOS::Rosen::DoGoForeground(data, size);
            break;
        case OHOS::Rosen::DO_INIT:
            OHOS::Rosen::DoInit(data, size);
            break;
        case OHOS::Rosen::DO_START_TEXTURE_EXPORT:
            OHOS::Rosen::DoStartTextureExport(data, size);
            break;
        case OHOS::Rosen::DO_DESTROY:
            OHOS::Rosen::DoDestroy(data, size);
            break;
        case OHOS::Rosen::DO_SET_RSSURFACE_NODE:
            OHOS::Rosen::DoSetRSSurfaceNode(data, size);
            break;
        case OHOS::Rosen::DO_SET_ABILITY_BG_ALPHA:
            OHOS::Rosen::DoSetAbilityBGAlpha(data, size);
            break;
        case OHOS::Rosen::DO_SET_RTRENDER_FORCED:
            OHOS::Rosen::DoSetRTRenderForced(data, size);
            break;
        case OHOS::Rosen::DO_SET_CONTAINER_WINDOW:
            OHOS::Rosen::DoSetContainerWindow(data, size);
            break;
        case OHOS::Rosen::DO_SET_FLUSH_EMPTY_CALLBACK:
            OHOS::Rosen::DoSetFlushEmptyCallback(data, size);
            break;
        case OHOS::Rosen::DO_SET_ROOT:
            OHOS::Rosen::DoSetRoot(data, size);
            break;
        case OHOS::Rosen::DO_SET_UITASK_RUNNER:
            OHOS::Rosen::DoSetUITaskRunner(data, size);
            break;
        case OHOS::Rosen::DO_SEND_MESSAGES:
            OHOS::Rosen::DoSendMessages(data, size);
            break;
        case OHOS::Rosen::DO_SET_TIMESTAMP:
            OHOS::Rosen::DoSetTimeStamp(data, size);
            break;
        case OHOS::Rosen::DO_SET_CACHE_DIR:
            OHOS::Rosen::DoSetCacheDir(data, size);
            break;
        case OHOS::Rosen::DO_FLUSH_ANIMATION:
            OHOS::Rosen::DoFlushAnimation(data, size);
            break;
        case OHOS::Rosen::DO_FLUSH_MODIFIER:
            OHOS::Rosen::DoFlushModifier(data, size);
            break;
        case OHOS::Rosen::DO_HAS_UI_ANIMATION:
            OHOS::Rosen::DoHasUIAnimation(data, size);
            break;
        case OHOS::Rosen::DO_FLUSH_ANIMATION_START_TIME:
            OHOS::Rosen::DoFlushAnimationStartTime(data, size);
            break;
        case OHOS::Rosen::DO_SET_APP_FREEZE:
            OHOS::Rosen::DoSetAppFreeze(data, size);
            break;
        case OHOS::Rosen::DO_SET_REQUEST_VSYNC_CALLBACK:
            OHOS::Rosen::DoSetRequestVsyncCallback(data, size);
            break;
        case OHOS::Rosen::DO_POST_FRAME_RATE_TASK:
            OHOS::Rosen::DoPostFrameRateTask(data, size);
            break;
        case OHOS::Rosen::DO_GET_CURRENT_REFRESH_RATE_MODE:
            OHOS::Rosen::DoGetCurrentRefreshRateMode(data, size);
            break;
        case OHOS::Rosen::DO_GET_ANIMATE_EXPECTED_RATE:
            OHOS::Rosen::DoGetAnimateExpectedRate(data, size);
            break;
        case OHOS::Rosen::DO_ATTACH_SURFACE:
            OHOS::Rosen::DoAttachSurface(data, size);
            break;
        case OHOS::Rosen::DO_RECV_MESSAGES_001:
            OHOS::Rosen::DoRecvMessages001(data, size);
            break;
        case OHOS::Rosen::DO_RECV_MESSAGES_002:
            OHOS::Rosen::DoRecvMessages002(data, size);
            break;
        case OHOS::Rosen::DO_PROCESS_MESSAGES:
            OHOS::Rosen::DoProcessMessages(data, size);
            break;
        case OHOS::Rosen::DO_ANIMATION_CALLBACK_PROCESSOR:
            OHOS::Rosen::DoAnimationCallbackProcessor(data, size);
            break;
        case OHOS::Rosen::DO_POST_TASK:
            OHOS::Rosen::DoPostTask(data, size);
            break;
        case OHOS::Rosen::DO_HAS_FIRST_FRAME_ANIMATION:
            OHOS::Rosen::DoHasFirstFrameAnimation(data, size);
            break;
        case OHOS::Rosen::DO_HAS_UIRUNNING_ANIMATION:
            OHOS::Rosen::DoHasUIRunningAnimation(data, size);
            break;
        case OHOS::Rosen::DO_GET_INDEX:
            OHOS::Rosen::DoGetIndex(data, size);
            break;
        case OHOS::Rosen::DO_DUMP_NODE_TREE_PROCESSOR:
            OHOS::Rosen::DoDumpNodeTreeProcessor(data, size);
            break;
        case OHOS::Rosen::DO_POST_DELAY_TASK:
            OHOS::Rosen::DoPostDelayTask(data, size);
            break;
        default:
            return -1;
    }
    return 0;
}

