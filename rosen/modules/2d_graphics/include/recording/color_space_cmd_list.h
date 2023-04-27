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

#ifndef COLOR_SPACE_CMD_LILST_H
#define COLOR_SPACE_CMD_LILST_H

#include "effect/color_space.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorSpaceCmdList : public CmdList {
public:
    ColorSpaceCmdList() = default;
    ~ColorSpaceCmdList() override = default;

    /*
     * @brief       Creates a ColorSpaceCmdList with contiguous buffers.
     * @param data  A contiguous buffers.
     */
    static std::shared_ptr<ColorSpaceCmdList> CreateFromData(const CmdListData& data);

    /*
     * @brief                  Creates a ColorSpaceCmdList with contiguous buffers and large memory object.
     * @param data             A contiguous buffers.
     * @param largeObjectData  A large memory object buffers.
     */
    static std::shared_ptr<ColorSpaceCmdList> CreateFromData(
        const CmdListData& data, const LargeObjectData& largeObjectData);

    /*
     * @brief       Add large object data to the buffers of ColorSpaceCmdList.
     * @param data  A large object data.
     * @return      Returns the offset of the contiguous buffers and ColorSpaceCmdList head point.
     * @note        using for recording, should to remove after using shared memory
     */
    int AddLargeObject(const LargeObjectData& data);

    /*
     * @brief  Gets the large Object buffers of the ColorSpaceCmdList.
     */
    LargeObjectData GetLargeObjectData() const;

    /*
     * @brief  Creates a ColorSpace by the ColorSpaceCmdList playback operation.
     */
    std::shared_ptr<ColorSpace> Playback() const;

private:
    MemAllocator largeObjectAllocator_;
};

/* OpItem */
class ColorSpaceOpItem : public OpItem {
public:
    ColorSpaceOpItem(uint32_t type) : OpItem(type) {}
    ~ColorSpaceOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD = 0,    // OPITEM_HEAD must be 0
        CREATE_SRGB,
        CREATE_SRGB_LINEAR,
        CREATE_REF_IMAGE,
        CREATE_RGB,
    };
};

class CreateSRGBOpItem : public ColorSpaceOpItem {
public:
    CreateSRGBOpItem();
    ~CreateSRGBOpItem() = default;

    /*
     * @brief   Plays back the OpItem to create ColorSpace.
     */
    std::shared_ptr<ColorSpace> Playback() const;
};

class CreateSRGBLinearOpItem : public ColorSpaceOpItem {
public:
    CreateSRGBLinearOpItem();
    ~CreateSRGBLinearOpItem() = default;

    /*
     * @brief   Plays back the OpItem to create ColorSpace.
     */
    std::shared_ptr<ColorSpace> Playback() const;
};

class CreateRefImageOpItem : public ColorSpaceOpItem {
public:
    CreateRefImageOpItem(const LargeObjectInfo& image);
    ~CreateRefImageOpItem() = default;

    /*
     * @brief            Restores arguments from contiguous memory and plays back the OpItem to create ColorSpace.
     * @param allocator  A contiguous memory for storing large objects.
     */
    std::shared_ptr<ColorSpace> Playback(const MemAllocator& largeObjectAllocator) const;

private:
    LargeObjectInfo image_;
};

class CreateRGBOpItem : public ColorSpaceOpItem {
public:
    CreateRGBOpItem(const CMSTransferFuncType& func, const CMSMatrixType& matrix);
    ~CreateRGBOpItem() = default;

    /*
     * @brief   Plays back the OpItem to create ColorSpace.
     */
    std::shared_ptr<ColorSpace> Playback() const;

private:
    CMSTransferFuncType func_;
    CMSMatrixType matrix_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
