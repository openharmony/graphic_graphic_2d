/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "recording/cmd_list.h"

#include <algorithm>
#include <sstream>

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr uint32_t OPITEM_HEAD = 0;

CmdList::CmdList(const CmdListData& cmdListData)
{
    opAllocator_.BuildFromDataWithCopy(cmdListData.first, cmdListData.second);
}

CmdList::~CmdList()
{
#ifdef ROSEN_OHOS
    surfaceBufferEntryVec_.clear();
#endif
}

size_t CmdList::AddCmdListData(const CmdListData& data)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!lastOpItemOffset_.has_value()) {
        void* op = opAllocator_.Allocate<OpItem>(OPITEM_HEAD);
        if (op == nullptr) {
            LOGD("add OpItem head failed!");
            return 0;
        }
        lastOpItemOffset_.emplace(opAllocator_.AddrToOffset(op));
    }
    void* addr = opAllocator_.Add(data.first, data.second);
    if (addr == nullptr) {
        LOGD("CmdList AddCmdListData failed!");
        return 0;
    }
    return opAllocator_.AddrToOffset(addr);
}

const void* CmdList::GetCmdListData(size_t offset, size_t size) const
{
    return opAllocator_.OffsetToAddr(offset, size);
}

CmdListData CmdList::GetData() const
{
    return std::make_pair(opAllocator_.GetData(), opAllocator_.GetSize());
}

bool CmdList::SetUpImageData(const void* data, size_t size)
{
    return imageAllocator_.BuildFromDataWithCopy(data, size);
}

size_t CmdList::AddImageData(const void* data, size_t size)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    void* addr = imageAllocator_.Add(data, size);
    if (addr == nullptr) {
        LOGD("CmdList AddImageData failed!");
        return 0;
    }
    return imageAllocator_.AddrToOffset(addr);
}

const void* CmdList::GetImageData(size_t offset, size_t size) const
{
    return imageAllocator_.OffsetToAddr(offset, size);
}

CmdListData CmdList::GetAllImageData() const
{
    return std::make_pair(imageAllocator_.GetData(), imageAllocator_.GetSize());
}

OpDataHandle CmdList::AddImage(const Image& image)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    OpDataHandle ret = {0, 0};
    uint32_t uniqueId = image.GetUniqueID();

    for (auto iter = imageHandleVec_.begin(); iter != imageHandleVec_.end(); iter++) {
        if (iter->first == uniqueId) {
            return iter->second;
        }
    }

    auto data = image.Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("image is vaild");
        return ret;
    }
    void* addr = imageAllocator_.Add(data->GetData(), data->GetSize());
    if (addr == nullptr) {
        LOGD("CmdList AddImageData failed!");
        return ret;
    }
    size_t offset = imageAllocator_.AddrToOffset(addr);
    imageHandleVec_.push_back(std::pair<size_t, OpDataHandle>(uniqueId, {offset, data->GetSize()}));

    return {offset, data->GetSize()};
}

std::shared_ptr<Image> CmdList::GetImage(const OpDataHandle& imageHandle)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto imageIt = imageMap_.find(imageHandle.offset);
    if (imageIt != imageMap_.end()) {
        return imageMap_[imageHandle.offset];
    }

    if (imageHandle.size == 0) {
        LOGD("image is vaild");
        return nullptr;
    }

    const void* ptr = imageAllocator_.OffsetToAddr(imageHandle.offset, imageHandle.size);
    if (ptr == nullptr) {
        LOGD("get image data failed");
        return nullptr;
    }

    auto imageData = std::make_shared<Data>();
    imageData->BuildWithoutCopy(ptr, imageHandle.size);
    auto image = std::make_shared<Image>();
    if (image->Deserialize(imageData) == false) {
        LOGD("image deserialize failed!");
        return nullptr;
    }
    imageMap_[imageHandle.offset] = image;
    return image;
}

size_t CmdList::AddBitmapData(const void* data, size_t size)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    void* addr = bitmapAllocator_.Add(data, size);
    if (addr == nullptr) {
        LOGD("CmdList AddImageData failed!");
        return 0;
    }
    return bitmapAllocator_.AddrToOffset(addr);
}

const void* CmdList::GetBitmapData(size_t offset, size_t size) const
{
    return bitmapAllocator_.OffsetToAddr(offset, size);
}

bool CmdList::SetUpBitmapData(const void* data, size_t size)
{
    return bitmapAllocator_.BuildFromDataWithCopy(data, size);
}

CmdListData CmdList::GetAllBitmapData() const
{
    return std::make_pair(bitmapAllocator_.GetData(), bitmapAllocator_.GetSize());
}

uint32_t CmdList::AddExtendObject(const std::shared_ptr<ExtendObject>& object)
{
    std::lock_guard<std::mutex> lock(extendObjectMutex_);
    extendObjectVec_.emplace_back(object);
    return static_cast<uint32_t>(extendObjectVec_.size()) - 1;
}

std::shared_ptr<ExtendObject> CmdList::GetExtendObject(uint32_t index)
{
    std::lock_guard<std::mutex> lock(extendObjectMutex_);
    if (index >= extendObjectVec_.size()) {
        return nullptr;
    }
    return extendObjectVec_[index];
}

uint32_t CmdList::GetAllExtendObject(std::vector<std::shared_ptr<ExtendObject>>& objectList)
{
    std::lock_guard<std::mutex> lock(extendObjectMutex_);
    for (const auto &object : extendObjectVec_) {
        objectList.emplace_back(object);
    }
    return objectList.size();
}

uint32_t CmdList::SetupExtendObject(const std::vector<std::shared_ptr<ExtendObject>>& objectList)
{
    std::lock_guard<std::mutex> lock(extendObjectMutex_);
    for (const auto &object : objectList) {
        extendObjectVec_.emplace_back(object);
    }
    return extendObjectVec_.size();
}

uint32_t CmdList::AddDrawingObject(const std::shared_ptr<Object>& object)
{
    std::lock_guard<std::mutex> lock(drawingObjectMutex_);
    drawingObjectVec_.emplace_back(object);
    return static_cast<uint32_t>(drawingObjectVec_.size()) - 1;
}

std::shared_ptr<Object> CmdList::GetDrawingObject(uint32_t index)
{
    std::lock_guard<std::mutex> lock(drawingObjectMutex_);
    if (index >= drawingObjectVec_.size()) {
        return nullptr;
    }
    return drawingObjectVec_[index];
}

uint32_t CmdList::GetAllDrawingObject(std::vector<std::shared_ptr<Object>>& objectList)
{
    std::lock_guard<std::mutex> lock(drawingObjectMutex_);
    for (const auto &object : drawingObjectVec_) {
        objectList.emplace_back(object);
    }
    return objectList.size();
}

uint32_t CmdList::SetupDrawingObject(const std::vector<std::shared_ptr<Object>>& objectList)
{
    std::lock_guard<std::mutex> lock(drawingObjectMutex_);
    for (const auto &object : objectList) {
        drawingObjectVec_.emplace_back(object);
    }
    return drawingObjectVec_.size();
}

uint32_t CmdList::AddRecordCmd(const std::shared_ptr<RecordCmd>& recordCmd)
{
    std::lock_guard<std::mutex> lock(recordCmdMutex_);
    recordCmdVec_.emplace_back(recordCmd);
    return static_cast<uint32_t>(recordCmdVec_.size()) - 1;
}

std::shared_ptr<RecordCmd> CmdList::GetRecordCmd(uint32_t index)
{
    std::lock_guard<std::mutex> lock(recordCmdMutex_);
    if (index >= recordCmdVec_.size()) {
        return nullptr;
    }
    return recordCmdVec_[index];
}

uint32_t CmdList::SetupRecordCmd(std::vector<std::shared_ptr<RecordCmd>>& recordCmdList)
{
    std::lock_guard<std::mutex> lock(recordCmdMutex_);
    for (const auto &recordCmd : recordCmdList) {
        recordCmdVec_.emplace_back(recordCmd);
    }
    return recordCmdVec_.size();
}

uint32_t CmdList::GetAllRecordCmd(std::vector<std::shared_ptr<RecordCmd>>& recordCmdList)
{
    std::lock_guard<std::mutex> lock(recordCmdMutex_);
    for (const auto &recordCmd : recordCmdVec_) {
        recordCmdList.emplace_back(recordCmd);
    }
    return recordCmdList.size();
}

uint32_t CmdList::AddImageObject(const std::shared_ptr<ExtendImageObject>& object)
{
    std::lock_guard<std::mutex> lock(imageObjectMutex_);
    imageObjectVec_.emplace_back(object);
    return static_cast<uint32_t>(imageObjectVec_.size()) - 1;
}

std::shared_ptr<ExtendImageObject> CmdList::GetImageObject(uint32_t id)
{
    std::lock_guard<std::mutex> lock(imageObjectMutex_);
    if (id >= imageObjectVec_.size()) {
        return nullptr;
    }
    return imageObjectVec_[id];
}

uint32_t CmdList::GetAllObject(std::vector<std::shared_ptr<ExtendImageObject>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageObjectMutex_);
    for (const auto &object : imageObjectVec_) {
        objectList.emplace_back(object);
    }
    return objectList.size();
}

uint32_t CmdList::SetupObject(const std::vector<std::shared_ptr<ExtendImageObject>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageObjectMutex_);
    for (const auto &object : objectList) {
        imageObjectVec_.emplace_back(object);
    }
    return imageObjectVec_.size();
}

uint32_t CmdList::AddImageBaseObj(const std::shared_ptr<ExtendImageBaseObj>& object)
{
    std::lock_guard<std::mutex> lock(imageBaseObjMutex_);
    imageBaseObjVec_.emplace_back(object);
    return static_cast<uint32_t>(imageBaseObjVec_.size()) - 1;
}

std::shared_ptr<ExtendImageBaseObj> CmdList::GetImageBaseObj(uint32_t id)
{
    std::lock_guard<std::mutex> lock(imageBaseObjMutex_);
    if (id >= imageBaseObjVec_.size()) {
        return nullptr;
    }
    return imageBaseObjVec_[id];
}

uint32_t CmdList::GetAllBaseObj(std::vector<std::shared_ptr<ExtendImageBaseObj>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageBaseObjMutex_);
    for (const auto &object : imageBaseObjVec_) {
        objectList.emplace_back(object);
    }
    return objectList.size();
}

uint32_t CmdList::SetupBaseObj(const std::vector<std::shared_ptr<ExtendImageBaseObj>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageBaseObjMutex_);
    for (const auto &object : objectList) {
        imageBaseObjVec_.emplace_back(object);
    }
    return imageBaseObjVec_.size();
}

uint32_t CmdList::AddImageNineObject(const std::shared_ptr<ExtendImageNineObject>& object)
{
    std::lock_guard<std::mutex> lock(imageNineObjectMutex_);
    imageNineObjectVec_.emplace_back(object);
    return static_cast<uint32_t>(imageNineObjectVec_.size()) - 1;
}

std::shared_ptr<ExtendImageNineObject> CmdList::GetImageNineObject(uint32_t id)
{
    std::lock_guard<std::mutex> lock(imageNineObjectMutex_);
    if (id >= imageNineObjectVec_.size()) {
        return nullptr;
    }
    return imageNineObjectVec_[id];
}

uint32_t CmdList::GetAllImageNineObject(std::vector<std::shared_ptr<ExtendImageNineObject>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageNineObjectMutex_);
    for (const auto &object : imageNineObjectVec_) {
        objectList.emplace_back(object);
    }
    return objectList.size();
}

uint32_t CmdList::SetupImageNineObject(const std::vector<std::shared_ptr<ExtendImageNineObject>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageNineObjectMutex_);
    for (const auto &object : objectList) {
        imageNineObjectVec_.emplace_back(object);
    }
    return imageNineObjectVec_.size();
}

uint32_t CmdList::AddImageLatticeObject(const std::shared_ptr<ExtendImageLatticeObject>& object)
{
    std::lock_guard<std::mutex> lock(imageLatticeObjectMutex_);
    imageLatticeObjectVec_.emplace_back(object);
    return static_cast<uint32_t>(imageLatticeObjectVec_.size()) - 1;
}

std::shared_ptr<ExtendImageLatticeObject> CmdList::GetImageLatticeObject(uint32_t id)
{
    std::lock_guard<std::mutex> lock(imageLatticeObjectMutex_);
    if (id >= imageLatticeObjectVec_.size()) {
        return nullptr;
    }
    return imageLatticeObjectVec_[id];
}

uint32_t CmdList::GetAllImageLatticeObject(std::vector<std::shared_ptr<ExtendImageLatticeObject>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageLatticeObjectMutex_);
    for (const auto &object : imageLatticeObjectVec_) {
        objectList.emplace_back(object);
    }
    return objectList.size();
}

uint32_t CmdList::SetupImageLatticeObject(const std::vector<std::shared_ptr<ExtendImageLatticeObject>>& objectList)
{
    std::lock_guard<std::mutex> lock(imageLatticeObjectMutex_);
    for (const auto &object : objectList) {
        imageLatticeObjectVec_.emplace_back(object);
    }
    return imageLatticeObjectVec_.size();
}

void CmdList::CopyObjectTo(CmdList& other) const
{
#ifdef SUPPORT_OHOS_PIXMAP
    other.imageObjectVec_ = imageObjectVec_;
#endif
    other.imageBaseObjVec_ = imageBaseObjVec_;
}

uint32_t CmdList::GetOpCnt() const
{
    return opCnt_;
}

#ifdef ROSEN_OHOS
uint32_t CmdList::AddSurfaceBufferEntry(const std::shared_ptr<SurfaceBufferEntry>& surfaceBufferEntry)
{
    std::lock_guard<std::mutex> lock(surfaceBufferEntryMutex_);
    surfaceBufferEntryVec_.emplace_back(surfaceBufferEntry);
    return static_cast<uint32_t>(surfaceBufferEntryVec_.size()) - 1;
}

std::shared_ptr<SurfaceBufferEntry> CmdList::GetSurfaceBufferEntry(uint32_t id)
{
    std::lock_guard<std::mutex> lock(surfaceBufferEntryMutex_);
    if (id >= surfaceBufferEntryVec_.size()) {
        return nullptr;
    }
    return surfaceBufferEntryVec_[id];
}

uint32_t CmdList::GetAllSurfaceBufferEntry(std::vector<std::shared_ptr<SurfaceBufferEntry>>& objectList)
{
    std::lock_guard<std::mutex> lock(surfaceBufferEntryMutex_);
    for (const auto &object : surfaceBufferEntryVec_) {
        objectList.emplace_back(object);
    }
    return static_cast<uint32_t>(objectList.size());
}

uint32_t CmdList::SetupSurfaceBufferEntry(const std::vector<std::shared_ptr<SurfaceBufferEntry>>& objectList)
{
    std::lock_guard<std::mutex> lock(surfaceBufferEntryMutex_);
    for (const auto &object : objectList) {
        surfaceBufferEntryVec_.emplace_back(object);
    }
    return static_cast<uint32_t>(surfaceBufferEntryVec_.size());
}
#endif

uint32_t CmdList::AddDrawFuncOjb(const std::shared_ptr<ExtendDrawFuncObj> &object)
{
    std::lock_guard<std::mutex> lock(drawFuncObjMutex_);
    drawFuncObjVec_.emplace_back(object);
    return static_cast<uint32_t>(drawFuncObjVec_.size()) - 1;
}

std::shared_ptr<ExtendDrawFuncObj> CmdList::GetDrawFuncObj(uint32_t id)
{
    std::lock_guard<std::mutex> lock(drawFuncObjMutex_);
    if (id >= drawFuncObjVec_.size()) {
        return nullptr;
    }
    return drawFuncObjVec_[id];
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
