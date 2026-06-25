/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "pipeline/rs_simple_draw_cmd_list.h"

#include "pipeline/rs_recording_canvas.h"
#include "platform/common/rs_log.h"
#include "utils/performanceCaculate.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<RSSimpleDrawCmdList> RSSimpleDrawCmdList::CreateFromDrawCmdList(Drawing::DrawCmdListPtr drawCmdList)
{
    if (drawCmdList == nullptr) {
        return nullptr;
    }
    if (!drawCmdList->UnmarshallingDrawOpsSimple()) {
        RS_LOGE("RSSimpleDrawCmdList::CreateFromDrawCmdList: UnmarshallingDrawOpsSimple fail.");
        return nullptr;
    }
    auto simpleCmdList = std::make_shared<RSSimpleDrawCmdList>(
        drawCmdList->GetWidth(), drawCmdList->GetHeight(), drawCmdList->GetDrawOpItems());
    simpleCmdList->noNeedUICaptured_ = drawCmdList->GetNoNeedUICaptured();
    return simpleCmdList;
}
    
RSSimpleDrawCmdList::RSSimpleDrawCmdList() = default;

Drawing::DrawCmdListPtr RSSimpleDrawCmdList::ConvertToDrawCmdList() const
{
    auto recordingCanvas_ = ExtendRecordingCanvas::Obtain(10, 10, true);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!drawOpItems_.empty()) {
        Drawing::Rect tmpRect;
        for (auto op : drawOpItems_) {
            if (op) {
                op->Playback(recordingCanvas_.get(), &tmpRect);
            }
        }
    }

    auto drawCmdList = recordingCanvas_->GetDrawCmdList();
    drawCmdList->SetNoNeedUICaptured(noNeedUICaptured_);
    return drawCmdList;
}

RSSimpleDrawCmdList::RSSimpleDrawCmdList(
    int32_t width, int32_t height, std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems)
    : width_(width), height_(height), drawOpItems_(std::move(opItems))
{}

std::vector<std::shared_ptr<Drawing::DrawOpItem>> RSSimpleDrawCmdList::GetDrawOpItems() const
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> drawOpItems(drawOpItems_);
    return drawOpItems;
}

Drawing::RectF RSSimpleDrawCmdList::GetCmdlistDrawRegion()
{
    Drawing::Rect cmdlistDrawRegion;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        for (const auto& op : drawOpItems_) {
            if (!op) {
                continue;
            }
            const auto& type = op->GetType();
            switch (type) {
                // dst opItem
                case Drawing::DrawOpItem::PATH_OPITEM:
                case Drawing::DrawOpItem::TEXT_BLOB_OPITEM:
                case Drawing::DrawOpItem::RECT_OPITEM:
                    cmdlistDrawRegion.Join(op->GetOpItemCmdlistDrawRegion());
                    break;
                // not dst opItem, but will appear in dst scene
                case Drawing::DrawOpItem::CLIP_RECT_OPITEM:
                case Drawing::DrawOpItem::CLIP_ROUND_RECT_OPITEM:
                case Drawing::DrawOpItem::CONCAT_MATRIX_OPITEM:
                case Drawing::DrawOpItem::SCALE_OPITEM:
                case Drawing::DrawOpItem::SAVE_OPITEM:
                case Drawing::DrawOpItem::RESTORE_OPITEM:
                case Drawing::DrawOpItem::PIXELMAP_RECT_OPITEM:
                    break;
                default:
                    return Drawing::Rect(0, 0, 0, 0);
            }
        }
    }
    return cmdlistDrawRegion;
}

bool RSSimpleDrawCmdList::IsEmpty() const
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return drawOpItems_.empty();
}

void RSSimpleDrawCmdList::Dump(std::string& out)
{
    bool found = false;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto& item : drawOpItems_) {
        if (item == nullptr) {
            continue;
        }
        found = true;
        item->Dump(out);
        out += ' ';
    }
    if (found) {
        out.pop_back();
    }
}

size_t RSSimpleDrawCmdList::GetSize() const
{
    size_t totalSize = sizeof(*this);
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        for (const auto& op : drawOpItems_) {
            if (op) {
                totalSize += op->GetOpSize();
            }
        }
    }
    return totalSize;
}

size_t RSSimpleDrawCmdList::GetOpItemSize() const
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return drawOpItems_.size();
}

void RSSimpleDrawCmdList::ClearOp()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    drawOpItems_.clear();
}

void RSSimpleDrawCmdList::AddDrawOp(std::shared_ptr<Drawing::DrawOpItem>&& drawOpItem)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    drawOpItems_.emplace_back(drawOpItem);
}

void RSSimpleDrawCmdList::PlaybackByVector(Drawing::Canvas& canvas, const Drawing::Rect* rect)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (drawOpItems_.empty()) {
        return;
    }
    for (auto op : drawOpItems_) {
        if (op) {
            op->Playback(&canvas, rect);
        }
    }
    canvas.DetachPaint();
}

void RSSimpleDrawCmdList::UpdateNodeIdToPicture(Drawing::NodeId nodeId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (const auto& opItem : drawOpItems_) {
        if (opItem) {
            opItem->SetNodeId(nodeId);
        }
    }
}

void RSSimpleDrawCmdList::Purge()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto op : drawOpItems_) {
        if (!op) {
            continue;
        }
        auto type = op->GetType();
        if (type == Drawing::DrawOpItem::PIXELMAP_RECT_OPITEM ||
            type == Drawing::DrawOpItem::PIXELMAP_WITH_PARM_OPITEM ||
            type == Drawing::DrawOpItem::PIXELMAP_NINE_OPITEM ||
            type == Drawing::DrawOpItem::PIXELMAP_LATTICE_OPITEM) {
            op->Purge();
        }
    }
}

void RSSimpleDrawCmdList::Playback(Drawing::Canvas& canvas, const Drawing::Rect* rect)
{
    if (canvas.GetUICapture() && noNeedUICaptured_) {
        return;
    }
    
    if (width_ <= 0 || height_ <= 0) {
        return;
    }

    Drawing::Rect tmpRect;
    if (rect != nullptr) {
        tmpRect = *rect;
    }
    PlaybackByVector(canvas, &tmpRect);
}
} // namespace Rosen
} // namespace OHOS