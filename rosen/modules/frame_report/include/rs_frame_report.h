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

#ifndef ROSEN_MODULE_RS_FRAME_REPORT_H
#define ROSEN_MODULE_RS_FRAME_REPORT_H

#include <mutex>
#include <string>
#include <unordered_map>
#if defined (RS_ENABLE_VK) && !defined(ROSEN_ARKUI_X)
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_xeg.h"
#endif

namespace OHOS {
namespace Rosen {
class RsFrameReport final {
public:
    RsFrameReport() = delete;
    ~RsFrameReport() = delete;
    static void InitDeadline();
    static void SendCommandsStart();
    static void SetFrameParam(int requestId, int load, int schedFrameNum, int value);
    static void RenderStart(uint64_t timestamp, int skipFirstFrame = 0);
    static void RenderEnd();
    static void DirectRenderEnd();
    static void UniRenderStart();
    static void UniRenderEnd();
    static void CheckUnblockMainThreadPoint();
    static void CheckPostAndWaitPoint();
    static void CheckBeginFlushPoint();
    static void ReportBufferCount(uint32_t count);
    static void ReportComposerInfo(const int screenId, const int composerTid);
    static void ReportFrameDeadline(int deadline, uint32_t currentRate);
    static void ReportUnmarshalData(int unmarshalTid, size_t dataSize);
    static void ReportDDGRTaskInfo();
    static void ReportScbSceneInfo(const std::string& description, bool eventStatus);
    static void BlurPredict(const std::unordered_map<std::string, std::string>& payload);
    static void ReceiveVSync();
    static void RequestNextVSync();
    static void ReportAddScreenId(const int screenId);
    static void ReportDelScreenId(const int screenId);
    static bool IsInitSchedCompleted();
#if defined (RS_ENABLE_VK) && !defined(ROSEN_ARKUI_X)
    static void ReportWindowInfo(VkDevice device, bool isSingleFullScreenApp, const char* firstFrontBundleName);
#endif
private:
#if defined (RS_ENABLE_VK) && !defined(ROSEN_ARKUI_X)
    struct VkHandleDeleter {
        void operator()(void* ptr) const;
    };
    static bool InitializeVulkanExtensions(VkDevice device);
    static std::atomic<bool> isInit;
    static PFN_vkSetFrontWindowStatusHUAWEI mSetFrontWindowStatusHUAWEI;
    static PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
    static std::unique_ptr<void, VkHandleDeleter> vkhandle;
    static std::function<void*(const char*, int)> dlopenFunc;
    static std::function<void*(void*, const char*)> dlsymFunc;
#endif
    static void InitSched();
    static std::once_flag initFlag_;
    static bool inited;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_MODULE_RS_FRAME_REPORT_H
