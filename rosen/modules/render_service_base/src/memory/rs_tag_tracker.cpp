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

#include "memory/rs_tag_tracker.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace {
static bool g_releaseResourceEnabled_ = true;
}
RSTagTracker::RSTagTracker(Drawing::GPUContext* gpuContext, RSTagTracker::TAGTYPE tagType) : gpuContext_(gpuContext)
{
    if (!gpuContext_) {
        RS_LOGE("RSTagTracker tag fail, gpuContext_ is nullptr");
        return;
    }
    if (!g_releaseResourceEnabled_) {
        return;
    }
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    Drawing::GPUResourceTag tag(0, 0, 0, tagType);
    gpuContext_->SetCurrentGpuResourceTag(tag);
#endif
}

void RSTagTracker::UpdateReleaseResourceEnabled(bool releaseResEnabled)
{
    g_releaseResourceEnabled_ = releaseResEnabled;
}

std::string RSTagTracker::TagType2String(TAGTYPE type)
{
    std::string tagType;
    switch (type) {
        case TAG_SAVELAYER_DRAW_NODE :
            tagType = "savelayer_draw_node";
            break;
        case TAG_RESTORELAYER_DRAW_NODE :
            tagType = "restorelayer_draw_node";
            break;
        case TAG_SAVELAYER_COLOR_FILTER :
            tagType = "savelayer_color_filter";
            break;
        case TAG_FILTER :
            tagType = "filter";
            break;
        case TAG_CAPTURE :
            tagType = "capture";
            break;
        case TAG_SUB_THREAD :
            tagType = "sub_thread";
            break;
        case TAG_ACQUIRE_SURFACE :
            tagType = "acquire_surface";
            break;
        case TAG_RENDER_FRAME :
            tagType = "render_frame";
            break;
        case TAG_DRAW_SURFACENODE :
            tagType = "draw_surface_node";
            break;
        case TAG_UNTAGGED :
            tagType = "untagged";
            break;
        default :
            tagType = "";
            break;
    }
    return tagType;
}

RSTagTracker::RSTagTracker(Drawing::GPUContext* gpuContext, NodeId nodeId, RSTagTracker::TAGTYPE tagType)
    : gpuContext_(gpuContext)
{
    if (!gpuContext_) {
        RS_LOGE("RSTagTracker tag fail, gpuContext_ is nullptr");
        return;
    }
    if (!g_releaseResourceEnabled_) {
        return;
    }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::GPUResourceTag tag(ExtractPid(nodeId), 0, nodeId, tagType);
    gpuContext_->SetCurrentGpuResourceTag(tag);
#endif
}

RSTagTracker::RSTagTracker(Drawing::GPUContext* gpuContext, Drawing::GPUResourceTag& tag) : gpuContext_(gpuContext)
{
    if (!gpuContext_) {
        RS_LOGE("RSTagTracker tag fail, gpuContext_ is nullptr");
        return;
    }
    if (!g_releaseResourceEnabled_) {
        return;
    }
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    gpuContext_->SetCurrentGpuResourceTag(tag);
#endif
}

void RSTagTracker::SetTagEnd()
{
    if (!gpuContext_) {
        RS_LOGE("RSTagTracker tag fail, gpuContext_ is nullptr");
        return;
    }
    if (!g_releaseResourceEnabled_) {
        return;
    }
    isSetTagEnd_ = true;
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    Drawing::GPUResourceTag tagEnd(0, 0, 0, 0);
    gpuContext_->SetCurrentGpuResourceTag(tagEnd);
#endif
}

RSTagTracker::~RSTagTracker()
{
    if (!g_releaseResourceEnabled_) {
        return;
    }
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    // Set empty tag to notify skia that the tag is complete
    if (!isSetTagEnd_ && gpuContext_) {
        Drawing::GPUResourceTag tagEnd(0, 0, 0, 0);
        gpuContext_->SetCurrentGpuResourceTag(tagEnd);
    }
#endif
}
} // namespace OHOS::Rosen