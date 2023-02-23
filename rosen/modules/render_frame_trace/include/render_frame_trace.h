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

#ifndef OHOS_RENDER_FRAME_TRACE_H
#define OHOS_RENDER_FRAME_TRACE_H

#include <string>

namespace FRAME_TRACE {

class RenderFrameTrace {
public:
    virtual ~RenderFrameTrace() = default;
    RenderFrameTrace(const RenderFrameTrace& fm) = delete;
    RenderFrameTrace& operator=(const RenderFrameTrace& fm) = delete;
    RenderFrameTrace(RenderFrameTrace&& fm) = delete;
    RenderFrameTrace& operator=(RenderFrameTrace&& fm) = delete; 
    static inline RenderFrameTrace& GetInstance()
    {
        if (implInstance_ != nullptr) {
            return *implInstance_;
        }
        implInstance_ = new RenderFrameTrace();
        return *implInstance_;
    }
    virtual void RenderStartFrameTrace(const std::string& traceTag) {}
    virtual void RenderEndFrameTrace(const std::string& traceTag) {}
    virtual bool RenderFrameTraceIsOpen();
    virtual bool RenderFrameTraceOpen();
    virtual bool RenderFrameTraceClose();
protected:
    static void RegisterImpl(RenderFrameTrace* implInstance)
    {
        implInstance_ = implInstance;
    }
    RenderFrameTrace() = default;
private:
    static RenderFrameTrace* implInstance_;
};

} // namespace FRAME_TRACE
#endif // OHOS_RENDER_FRAME_TRACE_H
