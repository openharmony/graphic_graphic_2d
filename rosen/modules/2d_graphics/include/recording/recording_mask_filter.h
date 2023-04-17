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

#ifndef RECORDING_MASK_FILTER_H
#define RECORDING_MASK_FILTER_H

#include "effect/mask_filter.h"
#include "recording/mask_filter_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingMaskFilter : public MaskFilter {
public:
    static std::shared_ptr<RecordingMaskFilter> CreateBlurMaskFilter(BlurType blurType, scalar sigma);

    RecordingMaskFilter() noexcept;
    ~RecordingMaskFilter() = default;

    std::shared_ptr<MaskFilterCmdList> GetCmdList() const
    {
        return cmdList_;
    }

private:
    std::shared_ptr<MaskFilterCmdList> cmdList_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
