/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_RS_RENDER_MODIFIER_UTILS_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_RS_RENDER_MODIFIER_UTILS_H

#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_recording_canvas.h"

namespace OHOS::Rosen {
class ExtendRecordingCanvas;

namespace ModifierNG {
class RSDisplayListModifierUpdater {
public:
    // not copyable and moveable
    RSDisplayListModifierUpdater(const RSDisplayListModifierUpdater&) = delete;
    RSDisplayListModifierUpdater(const RSDisplayListModifierUpdater&&) = delete;
    RSDisplayListModifierUpdater& operator=(const RSDisplayListModifierUpdater&) = delete;
    RSDisplayListModifierUpdater& operator=(const RSDisplayListModifierUpdater&&) = delete;

    explicit RSDisplayListModifierUpdater(RSDisplayListRenderModifier* target);
    virtual ~RSDisplayListModifierUpdater();
    const std::unique_ptr<ExtendRecordingCanvas>& GetRecordingCanvas() const;

protected:
    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas_;
    RSDisplayListRenderModifier* target_;
};
} // namespace ModifierNG
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_RS_MODIFIER_UTILS_H
