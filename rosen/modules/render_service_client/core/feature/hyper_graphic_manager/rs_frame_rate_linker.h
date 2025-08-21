/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_FRAME_RATE_LINKER_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_FRAME_RATE_LINKER_H

#include <atomic>
#include <memory>
#include <unistd.h>

#include "common/rs_common_def.h"
#include "animation/rs_frame_rate_range.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSFrameRateLinker {
public:
    RSFrameRateLinker();
    ~RSFrameRateLinker();

    FrameRateLinkerId GetId() const;
    bool IsUniRenderEnabled() const;
    void UpdateFrameRateRange(const FrameRateRange& range, int32_t animatorExpectedFrameRate = -1,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    void UpdateFrameRateRangeImme(const FrameRateRange& range, int32_t animatorExpectedFrameRate = -1);
    void SetEnable(bool enabled);
    bool IsEnable();
    static std::shared_ptr<RSFrameRateLinker> Create();
private:
    void InitUniRenderEnabled();
    static FrameRateLinkerId GenerateId();
    void AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand);
    const FrameRateLinkerId id_;
    FrameRateRange currentRange_;
    int32_t currAnimatorExpectedFrameRate_ = -1;
    std::atomic<bool> isEnabled_ = true;
    std::weak_ptr<RSUIContext> rsUIContext_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_FRAME_RATE_LINKER_H
