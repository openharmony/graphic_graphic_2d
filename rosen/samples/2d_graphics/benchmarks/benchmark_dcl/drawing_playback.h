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

#ifndef DRAWING_PLAYBACK_H
#define DRAWING_PLAYBACK_H

#include <chrono>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <message_parcel.h>
#include "pipeline/rs_draw_cmd_list.h"

#include "benchmark.h"
#include "drawing_command.h"
#include "skia_recording.h"
#include "include/core/SkBBHFactory.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkData.h"
#include "include/core/SkPicture.h"
#include "src/utils/SkMultiPictureDocument.h"
#include "tools/flags/CommonFlagsConfig.h"
#include "tools/gpu/MemoryCache.h"
#include "src/utils/SkOSPath.h"

namespace OHOS {
namespace Rosen {
class MSKPSrc {
public:
    explicit MSKPSrc(const std::string& path);

    bool Draw(SkCanvas* c) const;
    bool NextFrame();
    SkString Name() const { return SkOSPath::Basename(fPath_.c_str()); }
    int GetCurFrameNum() const { return curFrameNum_; }
private:
    uint16_t curFrameNum_ = 0;
    std::string fPath_;
    mutable SkTArray<SkDocumentPage> fPages_;
};

class DrawingDCL : public BenchMark {
public:
    DrawingDCL() { std::cout << "DrawingDCL" << std::endl; }
    DrawingDCL(int32_t argc, char* argv[]);
    ~DrawingDCL();
    bool GetDirectionAndStep(std::string command, bool &isMoreOps);
    bool IterateFrame(int &curLoop, int &frame);
    bool ReplayMSKP(SkCanvas *skiaCanvas);
    void ReplaySKP(SkCanvas *skiaCanvas);
    bool PlayBackByFrame(Drawing::Canvas* canvas, bool isDumpPictures = false);
    bool PlayBackByOpItem(SkCanvas *skiaCanvas, bool isMoreOps = true);
    void UpdateParameters(bool notNeeded);
    void UpdateParametersFromDCLCommand(const DCLCommand& dclCommand);
    void PrintDurationTime(const std::string &description, std::chrono::time_point<std::chrono::system_clock> start);
    void Start() override {};
    void Stop() override {};
    void Test(Drawing::Canvas* canvas, int width, int height) override;
    void Output() override;
    int LoadDrawCmdList(const std::string& dclFile);
    std::string GetRealPathStr(const std::string& filePath);
    bool IsValidFile(const std::string& realPathStr);

private:
    friend class DCLCommand;
    const static size_t recordingParcelMaxCapcity_ = 234 * 1000 * 1024;
    inline const static std::string dclFileDir_ = "/data/";
    std::shared_ptr<Drawing::DrawCmdList> dcl_ = nullptr;
    IterateType iterateType_ = IterateType::ITERATE_FRAME;
    int beginFrame_ = 0;
    int endFrame_ = 100;
    int curFrameNo_ = beginFrame_;
    int loop_ = 1;
    double opItemStep_ = 4;
    std::string inputFilePath_ = "/data/lkx/";
    std::string outputFilePath_ = "/data/lkx/";
    SkiaRecording skiaRecording;
    SkCanvas* orgSkiaCanvas_ = nullptr;
    SkCanvas* skiaCanvas_ = nullptr;
    std::unique_ptr<MSKPSrc> mskpPtr;
    sk_sp<SkPicture> skpPtr;
};
}
}

#endif