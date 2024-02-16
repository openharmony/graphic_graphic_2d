/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_PROFILER_FILE_H
#define RS_PROFILER_FILE_H

#include <mutex>

#include "rs_profiler_capturedata.h"

#ifdef REPLAY_TOOL_CLIENT
#include <memory>
using pid_t = int;
#endif

namespace OHOS::Rosen {

struct ReplayImageCacheRecordFile {
    std::shared_ptr<void> image;
    uint32_t imageSize = 0u;
    uint32_t skipBytes = 0u;
};

class RSFileLayer final {
public:
    std::pair<uint32_t, uint32_t> layerHeader; // to put in GLOBAL HEADER

    RSCaptureData property;

    std::vector<std::pair<uint32_t, uint32_t>> rsData;
    std::vector<std::pair<uint32_t, uint32_t>> oglData;
    std::vector<std::pair<uint32_t, uint32_t>> rsMetrics;
    std::vector<std::pair<uint32_t, uint32_t>> oglMetrics;
    std::vector<std::pair<uint32_t, uint32_t>> gfxMetrics;

    uint32_t readindexRsData = 0;
    uint32_t readindexOglData = 0;
    uint32_t readindexRsMetrics = 0;
    uint32_t readindexOglMetrics = 0;
    uint32_t readindexGfxMetrics = 0;
};

class RSFile final {
private:
    FILE* f_ = nullptr;

    double writeStartTime_ = 0.0;

    uint32_t headerOff_ = 0u;
    std::vector<pid_t> headerPidList_;

    std::vector<RSFileLayer> layerData_;

    uint32_t writeDataOff_ = 0u; // last byte of file where we can continue writing

    std::map<uint64_t, ReplayImageCacheRecordFile>* imageMapPtr_ = nullptr;

    std::mutex writeMutex_;

    bool wasChanged_ = false;

public:
    RSFile();

    void Create(const std::string& fname);
    bool Open(const std::string& fname);

    bool IsOpen();

    void SetWriteTime(double time);
    double GetWriteTime();

    void AddHeaderPID(pid_t pid);
    std::vector<pid_t>& GetHeaderPIDList();
    void SetImageMapPtr(std::map<uint64_t, ReplayImageCacheRecordFile>* imageMapPtr);

    uint32_t AddLayer();
    void LayerAddHeaderProperty(uint32_t layer, const std::string& name, const std::string& value);

    void WriteRSData(double time, void* data, int dataLen);
    void WriteOGLData(uint32_t layer, double time, void* data, int dataLen);
    void WriteRSMetrics(uint32_t layer, double time, void* data, int dataLen);
    void WriteOGLMetrics(uint32_t layer, double time, uint32_t frame, void* data, int dataLen);
    void WriteGFXMetrics(uint32_t layer, double time, uint32_t frame, void* data, int dataLen);

    void ReadRSDataRestart();
    void ReadOGLDataRestart(uint32_t layer);
    void ReadRSMetricsRestart(uint32_t layer);
    void ReadOGLMetricsRestart(uint32_t layer);
    void ReadGFXMetricsRestart(uint32_t layer);

    bool RSDataEOF();
    bool OGLDataEOF(uint32_t layer);
    bool RSMetricsEOF(uint32_t layer);
    bool OGLMetricsEOF(uint32_t layer);
    bool GFXMetricsEOF(uint32_t layer);

    bool ReadRSData(double untilTime, std::vector<uint8_t>& data, double& readTime);
    bool ReadOGLData(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadRSMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadOGLMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadGFXMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);

    void Close();

    static const std::string& GetDefaultPath();

private:
    void WriteHeaders();
    void WriteHeader();
    void LayerWriteHeader(uint32_t layer);

    void ReadHeaders();
    void ReadHeader();
    void LayerReadHeader(uint32_t layer);
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_FILE_H