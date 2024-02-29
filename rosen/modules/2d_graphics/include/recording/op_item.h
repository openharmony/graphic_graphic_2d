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

#ifndef OP_ITEM_H
#define OP_ITEM_H

#include <utility>

namespace OHOS {
namespace Rosen {
namespace Drawing {
class OpItem {
public:
    explicit OpItem(uint32_t type) : type_(type) {}
    virtual ~OpItem() = default;

    /**
     * @brief Gets the offset of next OpItem.
     */
    uint32_t GetNextOpItemOffset() const
    {
        return nextOpItem_;
    }

    /**
     * @brief Sets the offset of next OpItem.
     * @param offset  The offset of next OpItem.
     */
    void SetNextOpItemOffset(uint32_t offset)
    {
        nextOpItem_ = offset;
    }

    /**
     * @brief Gets the type of OpItem.
     */
    uint32_t GetType() const
    {
        return type_;
    }

    OpItem(OpItem&&) = delete;
    OpItem(const OpItem&) = default;
    OpItem& operator=(OpItem&&) = delete;
    OpItem& operator=(const OpItem&) = default;
private:
    uint32_t nextOpItem_ = 0;
    uint32_t type_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // OP_ITEM_H
