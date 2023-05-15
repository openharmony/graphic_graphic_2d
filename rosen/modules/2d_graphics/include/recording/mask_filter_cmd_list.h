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

#ifndef MASK_FILTER_CMD_LILST_H
#define MASK_FILTER_CMD_LILST_H

#include "effect/mask_filter.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class MaskFilterCmdList : public CmdList {
public:
    MaskFilterCmdList() = default;
    ~MaskFilterCmdList() override = default;

    uint32_t GetType() const override
    {
        return Type::MASK_FILTER_CMD_LIST;
    }

    /*
     * @brief       Creates a MaskFilterCmdList with contiguous buffers.
     * @param data  A contiguous buffers.
     */
    static std::shared_ptr<MaskFilterCmdList> CreateFromData(const CmdListData& data);

    /*
     * @brief  Creates a MaskFilter by the MaskFilterCmdList playback operation.
     */
    std::shared_ptr<MaskFilter> Playback() const;
};

/* OpItem */
class MaskFilterOpItem : public OpItem {
public:
    explicit MaskFilterOpItem(uint32_t type) : OpItem(type) {}

    enum Type : uint32_t {
        OPITEM_HEAD = 0,    // OPITEM_HEAD must be 0
        CREATE_BLUR,
    };
};

class CreateBlurMaskFilterOpItem : public MaskFilterOpItem {
public:
    CreateBlurMaskFilterOpItem(BlurType blurType, scalar sigma);
    ~CreateBlurMaskFilterOpItem() = default;

    std::shared_ptr<MaskFilter> Playback() const;

private:
    BlurType blurType_;
    scalar sigma_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
