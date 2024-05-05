/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_INNOVATION_H
#define RENDER_SERVICE_BASE_COMMON_RS_INNOVATION_H
namespace OHOS {
namespace Rosen {
class RSInnovation {
public:
    ~RSInnovation() = default;
    static void OpenInnovationSo();
    static void CloseInnovationSo();

    static inline void* innovationHandle = nullptr;

    // parallel composition
    static bool GetParallelCompositionEnabled(bool isUniRender);

    static inline bool _s_parallelCompositionLoaded = false;
    static inline void* _s_createParallelSyncSignal = nullptr;
    static inline void* _s_signalCountDown = nullptr;
    static inline void* _s_signalAwait = nullptr;
    static inline void* _s_assignTask = nullptr;
    static inline void* _s_removeStoppedThreads = nullptr;
    static inline void* _s_checkForSerialForced = nullptr;

private:
    RSInnovation() = default;

    static void GetParallelCompositionFunc();
    static void ResetParallelCompositionFunc();
};
}
}

#endif // RENDER_SERVICE_BASE_COMMON_RS_INNOVATION_H
