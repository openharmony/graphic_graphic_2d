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

#include "recording/draw_cmd_list.h"

#include <cstddef>
#include <memory>

#include "recording/draw_cmd.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::unordered_map<uint32_t, std::string> typeOpDes = {
    { DrawOpItem::CMD_LIST_OPITEM,          "CMD_LIST_OPITEM" },
    { DrawOpItem::POINT_OPITEM,             "POINT_OPITEM" },
    { DrawOpItem::POINTS_OPITEM,            "POINTS_OPITEM" },
    { DrawOpItem::LINE_OPITEM,              "LINE_OPITEM" },
    { DrawOpItem::RECT_OPITEM,              "RECT_OPITEM" },
    { DrawOpItem::ROUND_RECT_OPITEM,        "ROUND_RECT_OPITEM" },
    { DrawOpItem::NESTED_ROUND_RECT_OPITEM, "NESTED_ROUND_RECT_OPITEM" },
    { DrawOpItem::ARC_OPITEM,               "ARC_OPITEM" },
    { DrawOpItem::PIE_OPITEM,               "PIE_OPITEM" },
    { DrawOpItem::OVAL_OPITEM,              "OVAL_OPITEM" },
    { DrawOpItem::CIRCLE_OPITEM,            "CIRCLE_OPITEM" },
    { DrawOpItem::PATH_OPITEM,              "PATH_OPITEM" },
    { DrawOpItem::BACKGROUND_OPITEM,        "BACKGROUND_OPITEM" },
    { DrawOpItem::SHADOW_OPITEM,            "SHADOW_OPITEM" },
    { DrawOpItem::COLOR_OPITEM,             "COLOR_OPITEM" },
    { DrawOpItem::IMAGE_NINE_OPITEM,        "IMAGE_NINE_OPITEM" },
    { DrawOpItem::IMAGE_ANNOTATION_OPITEM,  "IMAGE_ANNOTATION_OPITEM" },
    { DrawOpItem::IMAGE_LATTICE_OPITEM,     "IMAGE_LATTICE_OPITEM" },
    { DrawOpItem::BITMAP_OPITEM,            "BITMAP_OPITEM" },
    { DrawOpItem::IMAGE_OPITEM,             "IMAGE_OPITEM" },
    { DrawOpItem::IMAGE_RECT_OPITEM,        "IMAGE_RECT_OPITEM" },
    { DrawOpItem::PICTURE_OPITEM,           "PICTURE_OPITEM" },
    { DrawOpItem::TEXT_BLOB_OPITEM,         "TEXT_BLOB_OPITEM" },
    { DrawOpItem::CLIP_RECT_OPITEM,         "CLIP_RECT_OPITEM" },
    { DrawOpItem::CLIP_IRECT_OPITEM,        "CLIP_IRECT_OPITEM" },
    { DrawOpItem::CLIP_ROUND_RECT_OPITEM,   "CLIP_ROUND_RECT_OPITEM" },
    { DrawOpItem::CLIP_PATH_OPITEM,         "CLIP_PATH_OPITEM" },
    { DrawOpItem::CLIP_REGION_OPITEM,       "CLIP_REGION_OPITEM" },
    { DrawOpItem::SET_MATRIX_OPITEM,        "SET_MATRIX_OPITEM" },
    { DrawOpItem::RESET_MATRIX_OPITEM,      "RESET_MATRIX_OPITEM" },
    { DrawOpItem::CONCAT_MATRIX_OPITEM,     "CONCAT_MATRIX_OPITEM" },
    { DrawOpItem::TRANSLATE_OPITEM,         "TRANSLATE_OPITEM" },
    { DrawOpItem::SCALE_OPITEM,             "SCALE_OPITEM" },
    { DrawOpItem::ROTATE_OPITEM,            "ROTATE_OPITEM" },
    { DrawOpItem::SHEAR_OPITEM,             "SHEAR_OPITEM" },
    { DrawOpItem::FLUSH_OPITEM,             "FLUSH_OPITEM" },
    { DrawOpItem::CLEAR_OPITEM,             "CLEAR_OPITEM" },
    { DrawOpItem::SAVE_OPITEM,              "SAVE_OPITEM" },
    { DrawOpItem::SAVE_LAYER_OPITEM,        "SAVE_LAYER_OPITEM" },
    { DrawOpItem::RESTORE_OPITEM,           "RESTORE_OPITEM" },
    { DrawOpItem::DISCARD_OPITEM,           "DISCARD_OPITEM" },
    { DrawOpItem::ATTACH_PEN_OPITEM,        "ATTACH_PEN_OPITEM" },
    { DrawOpItem::ATTACH_BRUSH_OPITEM,      "ATTACH_BRUSH_OPITEM" },
    { DrawOpItem::DETACH_PEN_OPITEM,        "DETACH_PEN_OPITEM" },
    { DrawOpItem::DETACH_BRUSH_OPITEM,      "DETACH_BRUSH_OPITEM" },
    { DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM, "CLIP_ADAPTIVE_ROUND_RECT_OPITEM" },
    { DrawOpItem::ADAPTIVE_IMAGE_OPITEM,    "ADAPTIVE_IMAGE_OPITEM" },
    { DrawOpItem::ADAPTIVE_PIXELMAP_OPITEM, "ADAPTIVE_PIXELMAP_OPITEM" },
    { DrawOpItem::IMAGE_WITH_PARM_OPITEM,   "IMAGE_WITH_PARM_OPITEM" },
    { DrawOpItem::PIXELMAP_WITH_PARM_OPITEM, "PIXELMAP_WITH_PARM_OPITEM" },
    { DrawOpItem::PIXELMAP_RECT_OPITEM,     "PIXELMAP_RECT_OPITEM" },
    { DrawOpItem::REGION_OPITEM,            "REGION_OPITEM" },
    { DrawOpItem::PATCH_OPITEM,             "PATCH_OPITEM" },
    { DrawOpItem::EDGEAAQUAD_OPITEM,        "EDGEAAQUAD_OPITEM" },
    { DrawOpItem::VERTICES_OPITEM,          "VERTICES_OPITEM" },
    { DrawOpItem::IMAGE_SNAPSHOT_OPITEM,    "IMAGE_SNAPSHOT_OPITEM" },
};

DrawCmdList::DrawCmdList(int32_t width, int32_t height) : width_(width), height_(height)
{
    opAllocator_.Add(&width_, sizeof(int32_t));
    opAllocator_.Add(&height_, sizeof(int32_t));
}

void DrawCmdList::ClearOp()
{
    opAllocator_.ClearData();
    opAllocator_.Add(&width_, sizeof(int32_t));
    opAllocator_.Add(&height_, sizeof(int32_t));
    imageAllocator_.ClearData();
    bitmapAllocator_.ClearData();
    imageMap_.clear();
    imageHandleVec_.clear();
    unmarshalledOpItems_.clear();
    lastOpGenSize_ = 0;
    lastOpItemOffset_ = std::nullopt;
}

std::shared_ptr<DrawCmdList> DrawCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<DrawCmdList>();
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }

    int32_t* width = static_cast<int32_t*>(cmdList->opAllocator_.OffsetToAddr(0));
    int32_t* height = static_cast<int32_t*>(cmdList->opAllocator_.OffsetToAddr(sizeof(int32_t)));
    if (width && height) {
        cmdList->width_ = *width;
        cmdList->height_ = *height;
    } else {
        cmdList->width_ = 0;
        cmdList->height_ = 0;
    }
    return cmdList;
}

int32_t DrawCmdList::GetWidth() const
{
    return width_;
}

int32_t DrawCmdList::GetHeight() const
{
    return height_;
}

void DrawCmdList::SetWidth(int32_t width)
{
    width_ = width;
}

void DrawCmdList::SetHeight(int32_t height)
{
    height_ = height;
}

bool DrawCmdList::IsEmpty() const
{
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (opAllocator_.GetSize() <= offset && unmarshalledOpItems_.size() == 0) {
        return true;
    }
    return false;
}

size_t DrawCmdList::GetOpItemSize() const
{
    return unmarshalledOpItems_.size();
}

std::string DrawCmdList::GetOpsWithDesc() const
{
    std::string desc;
    for (auto& item : unmarshalledOpItems_) {
        if (item == nullptr) {
            continue;
        }
        desc += typeOpDes[item->GetType()];
        desc += "\n";
    }
    LOGI("DrawCmdList::GetOpsWithDesc %{public}s, opitem sz: %{public}u", desc.c_str(), unmarshalledOpItems_.size());
    return desc;
}

void DrawCmdList::UnmarshallingOps()
{
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (opAllocator_.GetSize() <= offset) {
        return;
    }

    UnmarshallingPlayer player = { *this };
    unmarshalledOpItems_.clear();
    lastOpGenSize_ = 0;

    uint32_t opReplaceIndex = 0;
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("DrawCmdList::UnmarshallingOps failed, opItem is nullptr");
            break;
        }
        uint32_t type = curOpItemPtr->GetType();
        if (type == DrawOpItem::CMD_LIST_OPITEM) {
            unmarshalledOpItems_.clear();
            return;
        }
        auto op = player.Unmarshalling(type, itemPtr);
        if (!op) {
            offset = curOpItemPtr->GetNextOpItemOffset();
            continue;
        }
        if (opReplaceIndex < replacedOpList_.size() &&
            replacedOpList_[opReplaceIndex].first == offset) {
            auto* replacePtr = opAllocator_.OffsetToAddr(replacedOpList_[opReplaceIndex].second);
            if (replacePtr == nullptr) {
                LOGE("DrawCmdList::Unmarshalling replace Ops failed, replace op is nullptr");
                break;
            }
            auto* replaceOpItemPtr = static_cast<OpItem*>(replacePtr);
            unmarshalledOpItems_.emplace_back(player.Unmarshalling(replaceOpItemPtr->GetType(), replacePtr));
            opReplacedByDrivenRender_.emplace_back((unmarshalledOpItems_.size() - 1), op);
            opReplaceIndex++;
        } else {
            unmarshalledOpItems_.emplace_back(op);
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
        if (!replacedOpList_.empty() && offset >= replacedOpList_[0].second) {
            LOGD("DrawCmdList::UnmarshallingOps seek end by cache textOps");
            break;
        }
    } while (offset != 0);
    lastOpGenSize_ = opAllocator_.GetSize();

    if ((int)imageAllocator_.GetSize() > 0) {
        imageAllocator_.ClearData();
    }
}

std::vector<std::shared_ptr<DrawOpItem>> DrawCmdList::UnmarshallingCmdList()
{
    std::vector<std::shared_ptr<DrawOpItem>> opItems;
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (width_ <= 0 || height_ <= 0 || opAllocator_.GetSize() <= offset) {
        return opItems;
    }

    UnmarshallingPlayer player = { *this };
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("DrawCmdList::UnmarshallingCmdList failed, opItem is nullptr");
            break;
        }
        auto op = player.Unmarshalling(curOpItemPtr->GetType(), itemPtr);
        if (op) {
            unmarshalledOpItems_.push_back(op);
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0);
    return opItems;
}

void DrawCmdList::Playback(Canvas& canvas, const Rect* rect)
{
    if (canvas.GetDrawingType() == DrawingType::RECORDING) {
        AddOpToCmdList(static_cast<RecordingCanvas&>(canvas).GetDrawCmdList());
        return;
    }
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (width_ <= 0 || height_ <= 0 || (opAllocator_.GetSize() <= offset && unmarshalledOpItems_.size() == 0)) {
        return;
    }

#ifdef ROSEN_OHOS
    // invalidate cache if high contrast flag changed
    if (isCached_ && canvas.isHighContrastEnabled() != cachedHighContrast_) {
        ClearCache();
    }
    // Generate or clear cache if cache state changed
    if (canvas.GetCacheType() == Drawing::CacheType::ENABLED && !isCached_) {
        GenerateCacheInRenderService(&canvas, rect);
    } else if (canvas.GetCacheType() == Drawing::CacheType::DISABLED && isCached_) {
        ClearCache();
    }
#endif

    Rect tmpRect;
    if (rect != nullptr) {
        tmpRect = *rect;
    }

    if (lastOpGenSize_ != opAllocator_.GetSize() || unmarshalledOpItems_.size() == 0) {
        UnmarshallingPlayer player = { *this };
        unmarshalledOpItems_.clear();
        do {
            void* itemPtr = opAllocator_.OffsetToAddr(offset);
            auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
            if (curOpItemPtr != nullptr) {
                uint32_t type = curOpItemPtr->GetType();
                auto op = player.Unmarshalling(type, itemPtr);
                if (op) {
                    unmarshalledOpItems_.emplace_back(op);
                    op->Playback(&canvas, &tmpRect);
                }
                offset = curOpItemPtr->GetNextOpItemOffset();
            }
        } while (offset != 0);
        lastOpGenSize_ = opAllocator_.GetSize();
    } else {
        for (auto op : unmarshalledOpItems_) {
            op->Playback(&canvas, &tmpRect);
        }
    }
}

void DrawCmdList::GenerateCache(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    uint32_t offset = 2 * sizeof(int32_t);
    if (opAllocator_.GetSize() <= offset) {
        return;
    }

    if (isCached_) {
        LOGD("DrawCmdList::GenerateCache Invoke multiple times");
        return;
    }
    GenerateCachedOpItemPlayer player = {*this, canvas, rect};

    uint32_t maxOffset = opAllocator_.GetSize();
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("DrawCmdList::GenerateCache failed, opItem is nullptr");
            break;
        }
        bool replaceSuccess = player.GenerateCachedOpItem(curOpItemPtr->GetType(), itemPtr);
        if (replaceSuccess) {
            replacedOpList_.push_back({offset, lastOpItemOffset_.value()});
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0 && offset < maxOffset);

    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::GenerateCacheInRenderService(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    if (unmarshalledOpItems_.size() == 0) {
        return;
    }

    DrawOpItem* brushOp = nullptr;
    DrawOpItem* penOp = nullptr;

    for (int i = 0; i < unmarshalledOpItems_.size(); ++i) {
        auto opItem = unmarshalledOpItems_[i];
        uint32_t type = opItem->GetType();
        switch (type) {
            case DrawOpItem::ATTACH_PEN_OPITEM:
                penOp = opItem.get();
                break;
            case DrawOpItem::ATTACH_BRUSH_OPITEM:
                brushOp = opItem.get();
                break;
            case DrawOpItem::DETACH_PEN_OPITEM:
                penOp = nullptr;
                break;
            case DrawOpItem::DETACH_BRUSH_OPITEM:
                brushOp = nullptr;
                break;
            case DrawOpItem::TEXT_BLOB_OPITEM: {
                DrawTextBlobOpItem* textBlobOp = static_cast<DrawTextBlobOpItem*>(opItem.get());
                auto replaceCache = textBlobOp->GenerateCachedOpItem(
                    canvas, static_cast<AttachPenOpItem*>(penOp), static_cast<AttachBrushOpItem*>(brushOp));
                if (replaceCache) {
                    opReplacedByDrivenRender_.emplace_back(i, unmarshalledOpItems_[i]);
                    unmarshalledOpItems_[i] = replaceCache;
                }
                break;
            }
            default:
                break;
        }
    }

    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::ClearCache()
{
    // restore the original op
    for (auto& [index, op] : opReplacedByDrivenRender_) {
        unmarshalledOpItems_[index] = op;
    }
    opReplacedByDrivenRender_.clear();
    replacedOpList_.clear();
    isCached_ = false;
}

bool DrawCmdList::GetIsCache()
{
    return isCached_;
}

void DrawCmdList::SetIsCache(bool isCached)
{
    isCached_ = isCached;
}

bool DrawCmdList::GetCachedHighContrast()
{
    return cachedHighContrast_;
}

void DrawCmdList::SetCachedHighContrast(bool cachedHighContrast)
{
    cachedHighContrast_ = cachedHighContrast;
}

std::vector<std::pair<uint32_t, uint32_t>> DrawCmdList::GetReplacedOpList()
{
    return replacedOpList_;
}

void DrawCmdList::SetReplacedOpList(std::vector<std::pair<uint32_t, uint32_t>> replacedOpList)
{
    replacedOpList_ = replacedOpList;
}

void DrawCmdList::AddOpToCmdList(std::shared_ptr<DrawCmdList> cmdList)
{
    uint32_t offset = 2 * sizeof(int32_t);
    void* addr = opAllocator_.OffsetToAddr(offset);
    if (addr == nullptr) {
        return;
    }

#ifdef SUPPORT_OHOS_PIXMAP
    imageObjectVec_.swap(cmdList->imageObjectVec_);
#endif
    imageBaseOjVec_.swap(cmdList->imageBaseOjVec_);
    size_t size = opAllocator_.GetSize() - offset;
    auto imageData = GetAllImageData();
    auto bitmapData = GetAllBitmapData();
    std::lock_guard<std::mutex> lock(cmdList->mutex_);
    cmdList->opAllocator_.Add(addr, size);
    if (imageData.first != nullptr && imageData.second != 0) {
        cmdList->imageAllocator_.Add(imageData.first, imageData.second);
    }

    if (bitmapData.first != nullptr && bitmapData.second != 0) {
        cmdList->bitmapAllocator_.Add(bitmapData.first, bitmapData.second);
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
