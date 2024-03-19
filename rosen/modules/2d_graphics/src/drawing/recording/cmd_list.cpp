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

#include "recording/cmd_list.h"

#include <algorithm>

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
    surfaceBufferVec_.clear();
#endif
}

uint32_t CmdList::AddCmdListData(const CmdListData& data)
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

const void* CmdList::GetCmdListData(uint32_t offset) const
{
    return opAllocator_.OffsetToAddr(offset);
}

CmdListData CmdList::GetData() const
{
    return std::make_pair(opAllocator_.GetData(), opAllocator_.GetSize());
}

bool CmdList::SetUpImageData(const void* data, size_t size)
{
    return imageAllocator_.BuildFromDataWithCopy(data, size);
}

uint32_t CmdList::AddImageData(const void* data, size_t size)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    void* addr = imageAllocator_.Add(data, size);
    if (addr == nullptr) {
        LOGD("CmdList AddImageData failed!");
        return 0;
    }
    return imageAllocator_.AddrToOffset(addr);
}

const void* CmdList::GetImageData(uint32_t offset) const
{
    return imageAllocator_.OffsetToAddr(offset);
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
    uint32_t offset = imageAllocator_.AddrToOffset(addr);
    imageHandleVec_.push_back(std::pair<uint32_t, OpDataHandle>(uniqueId, {offset, data->GetSize()}));

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

    const void* ptr = imageAllocator_.OffsetToAddr(imageHandle.offset);
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

uint32_t CmdList::AddBitmapData(const void* data, size_t size)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    void* addr = bitmapAllocator_.Add(data, size);
    if (addr == nullptr) {
        LOGD("CmdList AddImageData failed!");
        return 0;
    }
    return bitmapAllocator_.AddrToOffset(addr);
}

const void* CmdList::GetBitmapData(uint32_t offset) const
{
    return bitmapAllocator_.OffsetToAddr(offset);
}

bool CmdList::SetUpBitmapData(const void* data, size_t size)
{
    return bitmapAllocator_.BuildFromDataWithCopy(data, size);
}

CmdListData CmdList::GetAllBitmapData() const
{
    return std::make_pair(bitmapAllocator_.GetData(), bitmapAllocator_.GetSize());
}

OpDataHandle CmdList::AddTypeface(const std::shared_ptr<Typeface>& typeface)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    OpDataHandle ret = {0, 0};
    uint32_t uniqueId = typeface->GetUniqueID();

    for (auto iter = typefaceHandleVec_.begin(); iter != typefaceHandleVec_.end(); iter++) {
        if (iter->first == uniqueId) {
            return iter->second;
        }
    }

    auto data = typeface->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("typeface is vaild");
        return ret;
    }
    void* addr = imageAllocator_.Add(data->GetData(), data->GetSize());
    if (addr == nullptr) {
        LOGD("CmdList AddTypefaceData failed!");
        return ret;
    }
    uint32_t offset = imageAllocator_.AddrToOffset(addr);
    typefaceHandleVec_.push_back(std::pair<uint32_t, OpDataHandle>(uniqueId, {offset, data->GetSize()}));

    return {offset, data->GetSize()};
}

std::shared_ptr<Typeface> CmdList::GetTypeface(const OpDataHandle& typefaceHandle)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto typefaceIt = typefaceMap_.find(typefaceHandle.offset);
    if (typefaceIt != typefaceMap_.end()) {
        return typefaceMap_[typefaceHandle.offset];
    }

    if (typefaceHandle.size == 0) {
        LOGD("typeface is vaild");
        return nullptr;
    }

    const void* ptr = imageAllocator_.OffsetToAddr(typefaceHandle.offset);
    if (ptr == nullptr) {
        LOGD("get typeface data failed");
        return nullptr;
    }

    auto typefaceData = std::make_shared<Data>();
    typefaceData->BuildWithoutCopy(ptr, typefaceHandle.size);
    auto typeface = Typeface::Deserialize(typefaceData->GetData(), typefaceData->GetSize());
    typefaceMap_[typefaceHandle.offset] = typeface;
    return typeface;
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
uint32_t CmdList::AddSurfaceBuffer(const sptr<SurfaceBuffer>& surfaceBuffer)
{
    std::lock_guard<std::mutex> lock(surfaceBufferMutex_);
    surfaceBufferVec_.emplace_back(surfaceBuffer);
    return static_cast<uint32_t>(surfaceBufferVec_.size()) - 1;
}

sptr<SurfaceBuffer> CmdList::GetSurfaceBuffer(uint32_t id)
{
    std::lock_guard<std::mutex> lock(surfaceBufferMutex_);
    if (id >= surfaceBufferVec_.size()) {
        return nullptr;
    }
    return surfaceBufferVec_[id];
}

uint32_t CmdList::GetAllSurfaceBuffer(std::vector<sptr<SurfaceBuffer>>& objectList)
{
    std::lock_guard<std::mutex> lock(surfaceBufferMutex_);
    for (const auto &object : surfaceBufferVec_) {
        objectList.emplace_back(object);
    }
    return static_cast<uint32_t>(objectList.size());
}

uint32_t CmdList::SetupSurfaceBuffer(const std::vector<sptr<SurfaceBuffer>>& objectList)
{
    std::lock_guard<std::mutex> lock(surfaceBufferMutex_);
    for (const auto &object : objectList) {
        surfaceBufferVec_.emplace_back(object);
    }
    return static_cast<uint32_t>(surfaceBufferVec_.size());
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
