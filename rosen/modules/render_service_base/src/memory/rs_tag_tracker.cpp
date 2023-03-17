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
bool RSTagTracker::releaseGpuResourceEnable_ = false;
RSTagTracker::RSTagTracker(GrContext* grContext, RSTagTracker::TAGTYPE tagType)
    : grContext_(grContext)
{
    if (!grContext_) {
        RS_LOGE("RSTagTracker tag fail, grContext is nullptr");
        return;
    }
    if (!releaseGpuResourceEnable_) {
        return;
    }
#ifdef RS_ENABLE_GL
    GrGpuResourceTag tag(0, 0, 0, tagType);
    grContext_->setCurrentGrResourceTag(tag);
#endif
}

void RSTagTracker::UpdateReleseGpuReousrceEnable(bool releaseResEnable)
{
    releaseGpuResourceEnable_ = releaseResEnable;
}

RSTagTracker::RSTagTracker(GrContext* grContext, NodeId nodeId, RSTagTracker::TAGTYPE tagType)
    : grContext_(grContext)
{
    if (!grContext_) {
        RS_LOGE("RSTagTracker tag fail, grContext is nullptr");
        return;
    }
    if (!releaseGpuResourceEnable_) {
        return;
    }
#ifdef RS_ENABLE_GL
    GrGpuResourceTag tag(ExtractPid(nodeId), 0, nodeId, tagType);
    grContext_->setCurrentGrResourceTag(tag);
#endif
}

RSTagTracker::RSTagTracker(GrContext* grContext, GrGpuResourceTag& tag)
    : grContext_(grContext)
{
    if (!grContext_) {
        RS_LOGE("RSTagTracker tag fail, grContext is nullptr");
        return;
    }
    if (!releaseGpuResourceEnable_) {
        return;
    }
#ifdef RS_ENABLE_GL
    grContext_->setCurrentGrResourceTag(tag);
#endif
}

void RSTagTracker::SetTagEnd()
{
    if (!grContext_) {
        RS_LOGE("RSTagTracker tag fail, grContext is nullptr");
        return;
    }
    if (!releaseGpuResourceEnable_) {
        return;
    }
    isSetTagEnd_ = true;
#ifdef RS_ENABLE_GL
    GrGpuResourceTag tagEnd;
    grContext_->setCurrentGrResourceTag(tagEnd);
#endif
}

RSTagTracker::~RSTagTracker()
{
    if (!releaseGpuResourceEnable_) {
        return;
    }
#ifdef RS_ENABLE_GL
    // Set empty tag to notify skia that the tag is complete
    if (!isSetTagEnd_ && grContext_) {
        GrGpuResourceTag tagEnd;
        grContext_->setCurrentGrResourceTag(tagEnd);
    }
#endif
}
} // namespace OHOS::Rosen