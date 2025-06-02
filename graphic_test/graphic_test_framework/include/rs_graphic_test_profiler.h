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

#ifndef RS_GRAPHIC_TEST_PROFILER_H
#define RS_GRAPHIC_TEST_PROFILER_H

#include <filesystem>
#include "cJSON.h"

#include "common/rs_color.h"
#include "common/rs_rect.h"
#include "rs_graphic_rootnode.h"
#include "rs_graphic_test_ext.h"
#include "rs_graphic_test_utils.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
class RSGraphicRootNode;
class RSGraphicTestProfiler {
public:
    struct PlaybackInfo
    {
        std::string fileName;
        int startTime;
        int endTime;
        int timeInterval;
    };

    int RunNodeTreeTest(const std::string& path);
    int RunPlaybackTest(const std::string& filePath);
    void SetUseBufferDump(bool useBufferDump);
private:
    void NodeTreeTestSetUp();
    void PlaybackTestSetUp();
    void TearDown();

    void CopyDirectoryAndLoadNodeTreeFile(const std::filesystem::path& src, const std::filesystem::path& dest);
    void AnalysePlaybackInfo(
        const std::filesystem::path& rootPath, const std::filesystem::path& imagePath, const cJSON* root);
    std::shared_ptr<RSGraphicRootNode> GetRootNode() const;
    Vector2f GetScreenSize() const;

    void LoadNodeTreeProfilerFile(const std::string& filePath, const std::string& savePath);
    void LoadPlaybackProfilerFile(const std::string& filePath, const std::string& savePath, PlaybackInfo info);

    void TestCaseCapture(bool isScreenshot, const std::string& savePath);
    void TestCaseBufferDump(bool isScreenshot, const std::string& savePath);
    bool CropRawFile(const std::string& srcFileName, const std::string& dstFileName, Size dumpBufferSize);
    void DumpBufferSetUp();
    void DumpBufferTearDown();
    std::string GetImageSavePath();

    std::string rootPath_ = "";
    int runTestCaseNum_ = 0;
    bool useBufferDump_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_GRAPHIC_TEST_PROFILER_H
