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

#include "recording/mask_cmd_list.h"

#include "recording/cmd_list_helper.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
Pen defaultPen;
}

std::shared_ptr<MaskCmdList> MaskCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<MaskCmdList>();
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }
    return cmdList;
}

bool MaskCmdList::Playback(std::shared_ptr<Path>& path, Brush& brush) const
{
    uint32_t offset = 0;
    MaskPlayer player(path, brush, *this);
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        OpItem* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr != nullptr) {
            if (!player.Playback(curOpItemPtr->GetType(), itemPtr)) {
                LOGD("MaskCmdList::Playback failed!");
                break;
            }

            offset = curOpItemPtr->GetNextOpItemOffset();
        } else {
            LOGE("MaskCmdList::Playback failed, opItem is nullptr");
            break;
        }
    } while (offset != 0);

    return true;
}

bool MaskCmdList::Playback(std::shared_ptr<Path>& path, Pen& pen, Brush& brush) const
{
    uint32_t offset = 0;
    MaskPlayer player(path, brush, pen, *this);
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        OpItem* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr != nullptr) {
            if (!player.Playback(curOpItemPtr->GetType(), itemPtr)) {
                LOGE("MaskCmdList::Playback failed!");
                break;
            }

            offset = curOpItemPtr->GetNextOpItemOffset();
        } else {
            LOGE("MaskCmdList::Playback failed, opItem is nullptr");
            break;
        }
    } while (offset != 0);

    return true;
}

/* OpItem */
std::unordered_map<uint32_t, MaskPlayer::MaskPlaybackFunc> MaskPlayer::opPlaybackFuncLUT_ = {
    { MaskOpItem::MASK_BRUSH_OPITEM,          MaskBrushOpItem::Playback },
    { MaskOpItem::MASK_PATH_OPITEM,           MaskPathOpItem::Playback },
    { MaskOpItem::MASK_PEN_OPITEM,           MaskPenOpItem::Playback },
};

MaskPlayer::MaskPlayer(std::shared_ptr<Path>& path, Brush& brush, const CmdList& cmdList)
    : path_(path), brush_(brush), pen_(defaultPen), cmdList_(cmdList) {}

MaskPlayer::MaskPlayer(std::shared_ptr<Path>& path, Brush& brush, Pen& pen, const CmdList& cmdList)
    : path_(path), brush_(brush), pen_(pen), cmdList_(cmdList) {}

bool MaskPlayer::Playback(uint32_t type, const void* opItem)
{
    if (type == MaskOpItem::OPITEM_HEAD) {
        return true;
    }

    auto it = opPlaybackFuncLUT_.find(type);
    if (it == opPlaybackFuncLUT_.end() || it->second == nullptr) {
        return false;
    }

    auto func = it->second;
    (*func)(*this, opItem);

    return true;
}

MaskBrushOpItem::MaskBrushOpItem(const BrushHandle& brushHandle)
    : MaskOpItem(MASK_BRUSH_OPITEM), brushHandle_(brushHandle) {}

void MaskBrushOpItem::Playback(MaskPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const MaskBrushOpItem*>(opItem);
        op->Playback(player.brush_, player.cmdList_);
    }
}

void MaskBrushOpItem::Playback(Brush& brush, const CmdList& cmdList) const
{
    auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(
        cmdList, brushHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(
        cmdList, brushHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(
        cmdList, brushHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(
        cmdList, brushHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(
        cmdList, brushHandle_.maskFilterHandle);

    Filter filter;
    filter.SetColorFilter(colorFilter);
    filter.SetImageFilter(imageFilter);
    filter.SetMaskFilter(maskFilter);
    filter.SetFilterQuality(brushHandle_.filterQuality);

    const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
        brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };

    brush.SetColor(color4f, colorSpace);
    brush.SetShaderEffect(shaderEffect);
    brush.SetBlendMode(brushHandle_.mode);
    brush.SetAntiAlias(brushHandle_.isAntiAlias);
    brush.SetFilter(filter);
}

MaskPathOpItem::MaskPathOpItem(const OpDataHandle& pathHandle)
    : MaskOpItem(MASK_PATH_OPITEM), pathHandle_(pathHandle) {}

void MaskPathOpItem::Playback(MaskPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const MaskPathOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void MaskPathOpItem::Playback(std::shared_ptr<Path>& path, const CmdList& cmdList) const
{
    auto readPath = CmdListHelper::GetPathFromCmdList(cmdList, pathHandle_);
    path = readPath;
}

MaskPenOpItem::MaskPenOpItem(const PenHandle& penHandle)
    : MaskOpItem(MASK_PEN_OPITEM), penHandle_(penHandle) {}

void MaskPenOpItem::Playback(MaskPlayer &player, const void *opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const MaskPenOpItem*>(opItem);
        op->Playback(player.pen_, player.cmdList_);
    }
}

void MaskPenOpItem::Playback(Pen& pen, const CmdList& cmdList) const
{
    pen.SetWidth(penHandle_.width);
    pen.SetMiterLimit(penHandle_.miterLimit);
    pen.SetJoinStyle(penHandle_.joinStyle);
    pen.SetCapStyle(penHandle_.capStyle);

    auto pathEffect = CmdListHelper::GetPathEffectFromCmdList(cmdList, penHandle_.pathEffectHandle);
    pen.SetPathEffect(pathEffect);
    pen.SetColor(penHandle_.color);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
