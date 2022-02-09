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


#ifndef VSYNC_VSYNC_CONTROLLER_H
#define VSYNC_VSYNC_CONTROLLER_H

#include <refbase.h>
#include <mutex>
#include "vsync_generator.h"

namespace OHOS {
namespace Rosen {
class VSyncController : public VSyncGenerator::Callback {
public:
    class Callback {
    public:
        virtual void OnVSyncEvent(int64_t now) = 0;
    };

    VSyncController(const sptr<VSyncGenerator> &geng, int64_t offset);
    ~VSyncController();

    // nocopyable
    VSyncController(const VSyncController &) = delete;
    VSyncController &operator=(const VSyncController &) = delete;

    void SetEnable(bool enable = false);
    void SetCallback(Callback* cb);
    void SetPhaseOffset(int64_t offset);

private:

    void OnVSyncEvent(int64_t now);
    wptr<VSyncGenerator> generator_;
    std::mutex callbackMutex_;
    Callback* callback_;

    std::mutex offsetMutex_;
    int64_t phaseOffset_;
    bool enabled_;
};
} // namespace Rosen
} // namespace OHOS

#endif
