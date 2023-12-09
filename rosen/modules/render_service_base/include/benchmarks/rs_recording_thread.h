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

#ifndef ROSEN_RENDER_SERVICE_BASE_BENCHMARKS_RS_RECORDING_THREAD_H
#define ROSEN_RENDER_SERVICE_BASE_BENCHMARKS_RS_RECORDING_THREAD_H


#include <cstdint>
#include <sys/stat.h>
#include "benchmarks/file_utils.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "event_handler.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "include/core/SkSurface.h"
#if defined(NEW_SKIA)
#include "include/gpu/GrDirectContext.h"
#else
#include "include/gpu/GrContext.h"
#endif
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_draw_cmd_list.h"
#else
#include "recording/draw_cmd_list.h"
#endif
#include "render_context/render_context.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSRecordingThread {
public:
    enum class RecordingMode {
        STOP_RECORDING,
        LOW_SPEED_RECORDING,
        HIGH_SPPED_RECORDING,
    };

    RSRecordingThread(RenderContext* context) : renderContext_(context){};
    ~RSRecordingThread();
    
    static RSRecordingThread& Instance(RenderContext* context);
    void Start();
    bool IsIdle();
    void PostTask(const std::function<void()> & task);
    bool CheckAndRecording();
    void FinishRecordingOneFrame();
#ifndef USE_ROSEN_DRAWING
    void RecordingToFile(const std::shared_ptr<DrawCmdList> & drawCmdList);
#else
    void RecordingToFile(const std::shared_ptr<Drawing::DrawCmdList> & drawCmdList);
#endif
    [[nodiscard]] int GetCurDumpFrame() const
    {
        return curDumpFrame_;
    }
    bool GetRecordingEnabled() const
    {
        return mode_ != RecordingMode::STOP_RECORDING;
    }
 
private:
    void CreateShareEglContext();
    void DestroyShareEglContext();
    void FinishRecordingOneFrameTask(RecordingMode mode);
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> CreateShareGrContext();
#else
    sk_sp<GrContext> CreateShareGrContext();
#endif
#else
    std::shared_ptr<Drawing::GPUContext> CreateShareGrContext();
#endif

    const static size_t RECORDING_PARCEL_CAPCITY = 234 * 1000 * 1024;
    RenderContext *renderContext_ = nullptr;
#ifdef RS_ENABLE_GL
    EGLContext eglShareContext_ = EGL_NO_CONTEXT;
#endif
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext_ = nullptr;
#else
    sk_sp<GrContext> grContext_ = nullptr;
#endif
#else
    std::shared_ptr<Drawing::GPUContext> grContext_ = nullptr;
#endif
    RecordingMode mode_ = RecordingMode::STOP_RECORDING;
#ifndef USE_ROSEN_DRAWING
    std::vector<std::shared_ptr<DrawCmdList>> drawCmdListVec_;
#else
    std::vector<std::shared_ptr<Drawing::DrawCmdList>> drawCmdListVec_;
#endif
    std::vector<std::shared_ptr<MessageParcel>> messageParcelVec_;
    std::vector<std::string> opsDescriptionVec_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::string fileDir_ = "/data/";
    int dumpFrameNum_ = 0;
    int curDumpFrame_ = 0;
};
} // namespace OHOS::Rosen

#endif // ROSEN_RENDER_SERVICE_BASE_BENCHMARKS_RS_RECORDING_THREAD_H