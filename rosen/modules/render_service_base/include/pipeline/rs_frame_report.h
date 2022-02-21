/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_FRAME_REPORT_H
#define ROSEN_RENDER_SERVICE_BASE_RS_FRAME_REPORT_H

#include <string>

#define EXPORT __attribute__((visibility("default")))

namespace OHOS {
namespace Rosen {
using FrameGetEnableFunc = int (*)();
using ProcessCommandsStartFunc = void(*)();
using AnimateStartFunc = void(*)();
using RenderStartFunc = void(*)();
using SendCommandsStartFunc = void(*)();

class EXPORT RsFrameReport final {
public:
    static RsFrameReport& GetInstance();
    void Init();
    int GetEnable();

    void ProcessCommandsStart();
    void AnimateStart();
    void RenderStart();
    void SendCommandsStart();

private:
    RsFrameReport();
    ~RsFrameReport();
    bool LoadLibrary();
    void CloseLibrary();
    void *LoadSymbol(const char *symName);
    void *frameSchedHandle_ = nullptr;
    bool frameSchedSoLoaded_ = false;

    EXPORT FrameGetEnableFunc frameGetEnableFunc_ = nullptr;
    EXPORT ProcessCommandsStartFunc processCommandsStartFun_ = nullptr;
    EXPORT AnimateStartFunc animateStartFunc_ = nullptr;
    EXPORT RenderStartFunc renderStartRunc_ = nullptr;
    EXPORT SendCommandsStartFunc sendCommandsStartFunc_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_FRAME_REPORT_H
