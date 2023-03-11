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

#include "render_frame_trace_impl.h"
#include <unistd.h>
#include "frame_trace.h"

namespace FRAME_TRACE {

#ifdef __aarch64__
const char* g_frameTraceSoPath = "/system/lib64/libframe_trace_intf.z.so";
#else
const char* g_frameTraceSoPath = "/system/lib/libframe_trace_intf.z.so";
#endif

RenderFrameTraceImpl* RenderFrameTraceImpl::instance_ = new RenderFrameTraceImpl();

RenderFrameTraceImpl::RenderFrameTraceImpl()
{
    RenderFrameTrace::RegisterImpl(this);
}

bool RenderFrameTraceImpl::AccessFrameTrace()
{
    if (!judgeFrameTrace_) {
        judgeFrameTrace_ = true;
        accessFrameTrace_ = access(g_frameTraceSoPath, F_OK) ? false : true;
    }
    return accessFrameTrace_;
}

void RenderFrameTraceImpl::RenderStartFrameTrace(const std::string& traceTag)
{
    if (AccessFrameTrace()) {
        QuickStartFrameTrace(traceTag);
    }
}

void RenderFrameTraceImpl::RenderEndFrameTrace(const std::string& traceTag)
{
    if (AccessFrameTrace()) {
        QuickEndFrameTrace(traceTag);
    }
}

bool RenderFrameTraceImpl::RenderFrameTraceOpen()
{
    if (AccessFrameTrace()) {
        return FrameAwareTraceOpen();
    }
    return false;
}

bool RenderFrameTraceImpl::RenderFrameTraceIsOpen()
{
    if (AccessFrameTrace()) {
        return FrameAwareTraceIsOpen();
    }
    return false;
}

bool RenderFrameTraceImpl::RenderFrameTraceClose()
{
    if (AccessFrameTrace()) {
        return FrameAwareTraceClose();
    }
    return false;
}

} // namespace FRAME_TRACE
