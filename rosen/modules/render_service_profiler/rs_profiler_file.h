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
#include "rs_profiler_utils.h"

#ifdef REPLAY_TOOL_CLIENT // adapt to windows on client side
#include <memory>

#include "rs_adapt.h"
#endif

namespace OHOS::Rosen {

struct FileImageCacheRecord {
    std::shared_ptr<void> image;
    uint32_t imageSize = 0u;
    uint32_t skipBytes = 0u;
};

using FileImageCache = std::map<uint64_t, FileImageCacheRecord>;

struct RSFileLayer final {
    std::pair<uint32_t, uint32_t> layerHeader; // to put in GLOBAL HEADER

    RSCaptureData property;

    using TrackMarkup = std::vector<std::pair<uint32_t, uint32_t>>;
    static constexpr size_t MARKUP_SIZE = sizeof(TrackMarkup::value_type);

    TrackMarkup rsData;
    TrackMarkup oglData;
    TrackMarkup rsMetrics;
    TrackMarkup oglMetrics;
    TrackMarkup gfxMetrics;

    uint32_t readindexRsData = 0;
    uint32_t readindexOglData = 0;
    uint32_t readindexRsMetrics = 0;
    uint32_t readindexOglMetrics = 0;
    uint32_t readindexGfxMetrics = 0;
};

class RSFile final {
public:
    RSFile();

    void Create(const std::string& fname);
    bool Open(const std::string& fname);

    bool IsOpen() const;

    void SetWriteTime(double time);
    double GetWriteTime() const;

    std::string& GetHeaderFirstFrame();
    void AddHeaderFirstFrame(const std::string& dataFirstFrame);

    void AddHeaderPid(pid_t pid);
    const std::vector<pid_t>& GetHeaderPids() const;
    void SetImageCache(FileImageCache* cache);

    uint32_t AddLayer();
    void LayerAddHeaderProperty(uint32_t layer, const std::string& name, const std::string& value);

    void WriteRSData(double time, const void* data, size_t size);
    void WriteOGLData(uint32_t layer, double time, const void* data, size_t size);
    void WriteRSMetrics(uint32_t layer, double time, const void* data, size_t size);
    void WriteOGLMetrics(uint32_t layer, double time, uint32_t frame, const void* data, size_t size);
    void WriteGFXMetrics(uint32_t layer, double time, uint32_t frame, const void* data, size_t size);

    void ReadRSDataRestart();
    void ReadOGLDataRestart(uint32_t layer);
    void ReadRSMetricsRestart(uint32_t layer);
    void ReadOGLMetricsRestart(uint32_t layer);
    void ReadGFXMetricsRestart(uint32_t layer);

    bool RSDataEOF() const;
    bool OGLDataEOF(uint32_t layer) const;
    bool RSMetricsEOF(uint32_t layer) const;
    bool OGLMetricsEOF(uint32_t layer) const;
    bool GFXMetricsEOF(uint32_t layer) const;

    bool ReadRSData(double untilTime, std::vector<uint8_t>& data, double& readTime);
    bool ReadOGLData(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadRSMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadOGLMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadGFXMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);

    bool HasLayer(uint32_t layer) const;

    void Close();

    static const std::string& GetDefaultPath();

private:
    void WriteHeaders();
    void WriteHeader();
    void LayerWriteHeader(uint32_t layer);

    template<typename Track>
    void LayerWriteHeaderOfTrack(const Track& track)
    {
        uint32_t recordSize = track.size();
        Utils::FileWrite(&recordSize, sizeof(recordSize), 1, file_);
        for (auto item : track) {
            Utils::FileWrite(&item.first, sizeof(item.first), 1, file_);
            Utils::FileWrite(&item.second, sizeof(item.second), 1, file_);
        }
    }

    void ReadHeaders();
    void ReadHeader();
    void LayerReadHeader(uint32_t layer);

    template<typename Track>
    void LayerReadHeaderOfTrack(Track& track)
    {
        uint32_t recordSize;
        Utils::FileRead(&recordSize, sizeof(recordSize), 1, file_);
        track.resize(recordSize);
        for (size_t i = 0; i < recordSize; i++) {
            Utils::FileRead(&track[i].first, sizeof(track[i].first), 1, file_);
            Utils::FileRead(&track[i].second, sizeof(track[i].second), 1, file_);
        }
    }

    void ReadTextureFromFile();

    using LayerTrackIndexPtr = uint32_t RSFileLayer::*;
    using LayerTrackMarkupPtr = RSFileLayer::TrackMarkup RSFileLayer::*;
    struct LayerTrackPtr {
        LayerTrackIndexPtr index;
        LayerTrackMarkupPtr markup;
    };

    void WriteTrackData(LayerTrackMarkupPtr trackMarkup, uint32_t layer, double time, const void* data, size_t size);
    bool ReadTrackData(
        LayerTrackPtr track, double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    void ReadTrackDataRestart(LayerTrackIndexPtr trackIndex, uint32_t layer);
    bool TrackEOF(LayerTrackPtr track, uint32_t layer) const;

private:
    FILE* file_ = nullptr;
    double writeStartTime_ = 0.0;
    uint32_t headerOff_ = 0u;
    std::vector<pid_t> headerPidList_;
    std::vector<RSFileLayer> layerData_;
    uint32_t writeDataOff_ = 0u; // last byte of file where we can continue writing
    FileImageCache* imageCache_ = nullptr;
    std::string headerFirstFrame_;
    std::mutex writeMutex_;
    bool wasChanged_ = false;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_FILE_H