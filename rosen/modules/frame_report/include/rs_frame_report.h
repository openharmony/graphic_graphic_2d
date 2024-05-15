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

#include <string>

namespace OHOS {
namespace Rosen {
using FrameGetEnableFunc = int (*)();
using InitFunc = void (*)();
using ProcessCommandsStartFunc = void(*)();
using AnimateStartFunc = void(*)();
using RenderStartFunc = void(*)(uint64_t);
using ParallelRenderStartFunc = void(*)();
using RenderEndFunc = void(*)();
using SendCommandsStartFunc = void(*)();
using ParallelRenderEndFunc = void(*)();
using SetFrameParamFunc = void(*)(int, int, int, int);
class RsFrameReport final {
public:
    static RsFrameReport& GetInstance();
    void Init();
    int GetEnable();

    void ProcessCommandsStart();
    void AnimateStart();
    void RenderStart(uint64_t timestamp);
    void RSRenderStart();
    void RenderEnd();
    void RSRenderEnd();
    void SendCommandsStart();
    void SetFrameParam(int requestId, int load, int schedFrameNum, int value);

private:
    RsFrameReport();
    ~RsFrameReport();
    bool LoadLibrary();
    void CloseLibrary();
    void *LoadSymbol(const char *symName);
    void *frameSchedHandle_ = nullptr;
    bool frameSchedSoLoaded_ = false;

    FrameGetEnableFunc frameGetEnableFunc_ = nullptr;
    InitFunc initFunc_ = nullptr;
    ProcessCommandsStartFunc processCommandsStartFun_ = nullptr;
    AnimateStartFunc animateStartFunc_ = nullptr;
    RenderStartFunc renderStartFunc_ = nullptr;
    ParallelRenderStartFunc parallelRenderStartFunc_ = nullptr;
    RenderEndFunc renderEndFunc_ = nullptr;
    ParallelRenderEndFunc parallelRenderEndFunc_ = nullptr;
    SendCommandsStartFunc sendCommandsStartFunc_ = nullptr;
    SetFrameParamFunc setFrameParamFunc_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_MODULE_RS_FRAME_REPORT_H
