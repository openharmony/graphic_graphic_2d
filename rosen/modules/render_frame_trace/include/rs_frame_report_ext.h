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

#ifndef ROSEN_RS_FRAME_REPORT_EXT_H
#define ROSEN_RS_FRAME_REPORT_EXT_H

#include <string>

namespace OHOS {
namespace Rosen {
using FrameGetEnableFunc = int (*)();
using InitFunc = void (*)();
using HandleSwapBufferFunc = void(*)();
using RequestNextVSyncFunc = void(*)();
using ReceiveVSyncFunc = void(*)();

class RsFrameReportExt {
public:
    static RsFrameReportExt& GetInstance();
    void Init();
    int GetEnable();
    void HandleSwapBuffer();
    void RequestNextVSync();
    void ReceiveVSync();

private:
    RsFrameReportExt();
    ~RsFrameReportExt();
    bool LoadLibrary();
    void CloseLibrary();
    void *LoadSymbol(const char *symName);
    void *frameSchedHandle_ = nullptr;
    bool frameSchedSoLoaded_ = false;

    FrameGetEnableFunc frameGetEnableFunc_ = nullptr;
    InitFunc initFunc_ = nullptr;
    HandleSwapBufferFunc handleSwapBufferFunc_ = nullptr;
    RequestNextVSyncFunc requestNextVSyncFunc_ = nullptr;
    ReceiveVSyncFunc receiveVSyncFunc_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RS_FRAME_REPORT_EXT_H
