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

#include "feature/memory_info_manager/rs_memory_info_manager.h"

#include "memory/rs_memory_track.h"
#ifdef ROSEN_OHOS
#include "mem_mgr_client.h"
#endif
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
void RSMemoryInfoManager::SetSurfaceMemoryInfo(bool onTree, const std::shared_ptr<RSSurfaceHandler> handler)
{
    if (!RSSystemProperties::GetRSMemoryInfoManagerParam()) {
        return;
    }
    if (!handler || !handler->GetBuffer()) {
        return;
    }
    auto buffer = handler->GetBuffer();
    int fd = buffer->GetFileDescriptor();
    if (!onTree && MemoryTrack::Instance().GetGlobalRootNodeStatusChangeFlag()) {
        Memory::MemMgrClient::GetInstance().SetDmabufInfo(fd,
            Memory::DmabufRsInfo::SURFACE_OFF_TREE_IN_ROOT);
    } else if (!onTree) {
        Memory::MemMgrClient::GetInstance().SetDmabufInfo(fd,
            Memory::DmabufRsInfo::SURFACE_OFF_TREE);
    } else if (onTree && MemoryTrack::Instance().GetGlobalRootNodeStatusChangeFlag()) {
        Memory::MemMgrClient::GetInstance().SetDmabufInfo(fd,
            Memory::DmabufRsInfo::SURFACE_ON_TREE_IN_ROOT);
    } else {
        Memory::MemMgrClient::GetInstance().SetDmabufInfo(fd,
            Memory::DmabufRsInfo::SURFACE_ON_TREE);
    }
}

void RSMemoryInfoManager::SetImageMemoryInfo(const std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (!RSSystemProperties::GetRSMemoryInfoManagerParam()) {
        return;
    }
    if (!pixelMap || pixelMap->GetAllocatorType() != Media::AllocatorType::DMA_ALLOC) {
        return;
    }
    SurfaceBuffer *sbBuffer = reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd());
    if (!sbBuffer) {
        return;
    }
    NODE_ON_TREE_STATUS res = NODE_ON_TREE_STATUS::STATUS_INVALID;
    if (RSSystemProperties::GetClosePixelMapFdEnabled()) {
        res = MemoryTrack::Instance().GetNodeOnTreeStatus(pixelMap->GetPixels());
    } else {
        res = MemoryTrack::Instance().GetNodeOnTreeStatus(pixelMap->GetFd());
    }
    if (res == NODE_ON_TREE_STATUS::STATUS_OFF_TREE) {
        Memory::MemMgrClient::GetInstance().SetDmabufInfo(sbBuffer->GetFileDescriptor(),
            Memory::DmabufRsInfo::IMAGE_OFF_TREE);
    } else if (res == NODE_ON_TREE_STATUS::STATUS_OFF_TREE_IN_ROOT) {
        Memory::MemMgrClient::GetInstance().SetDmabufInfo(sbBuffer->GetFileDescriptor(),
            Memory::DmabufRsInfo::IMAGE_OFF_TREE_IN_ROOT);
    } else if (res == NODE_ON_TREE_STATUS::STATUS_ON_TREE) {
        Memory::MemMgrClient::GetInstance().SetDmabufInfo(sbBuffer->GetFileDescriptor(),
            Memory::DmabufRsInfo::IMAGE_ON_TREE);
    } else if (res == NODE_ON_TREE_STATUS::STATUS_ON_TREE_IN_ROOT) {
        Memory::MemMgrClient::GetInstance().SetDmabufInfo(sbBuffer->GetFileDescriptor(),
            Memory::DmabufRsInfo::IMAGE_ON_TREE_IN_ROOT);
    }
}

void RSMemoryInfoManager::RecordNodeOnTreeStatus(bool flag, NodeId nodeId, NodeId instanceRootNodeId)
{
    if (!RSSystemProperties::GetRSMemoryInfoManagerParam()) {
        return;
    }
    if (nodeId == instanceRootNodeId) {
        MemoryTrack::Instance().SetGlobalRootNodeStatusChangeFlag(true);
    }
    MemoryTrack::Instance().SetNodeOnTreeStatus(nodeId,
        MemoryTrack::Instance().GetGlobalRootNodeStatusChangeFlag(), flag);
}

void RSMemoryInfoManager::ResetRootNodeStatusChangeFlag(NodeId nodeId, NodeId instanceRootNodeId)
{
    if (!RSSystemProperties::GetRSMemoryInfoManagerParam()) {
        return;
    }
    if (nodeId != instanceRootNodeId) {
        return;
    }
    MemoryTrack::Instance().SetGlobalRootNodeStatusChangeFlag(false);
}
}
}