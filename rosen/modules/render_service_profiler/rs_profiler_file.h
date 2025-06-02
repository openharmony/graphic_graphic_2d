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

#ifdef RENDER_PROFILER_APPLICATION
#include <memory>

#include "rs_adapt.h"
#endif

namespace OHOS::Rosen {

#define RSFILE_VERSION_RENDER_METRICS_ADDED 0x240701
#define RSFILE_VERSION_RENDER_ANIMESTARTTIMES_ADDED 0x240723
#define RSFILE_VERSION_RENDER_TYPEFACE_FIX 0x240801
#define RSFILE_VERSION_ISREPAINT_BOUNDARY 0x250516
#define RSFILE_VERSION_LATEST RSFILE_VERSION_ISREPAINT_BOUNDARY

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
    TrackMarkup renderMetrics;

    uint32_t readindexRsData = 0;
    uint32_t readindexOglData = 0;
    uint32_t readindexRsMetrics = 0;
    uint32_t readindexOglMetrics = 0;
    uint32_t readindexGfxMetrics = 0;
    uint32_t readindexRenderMetrics = 0;
};

class RSFile final {
public:
    RSFile();

    void Create(const std::string& fname);
    bool Open(const std::string& fname);

    bool IsOpen() const;

    void SetWriteTime(double time);
    double GetWriteTime() const;

    const std::string& GetHeaderFirstFrame() const;
    void AddHeaderFirstFrame(const std::string& dataFirstFrame);

    const std::vector<std::pair<uint64_t, int64_t>>& GetAnimeStartTimes() const;
    void AddAnimeStartTimes(const std::vector<std::pair<uint64_t, int64_t>>& startTimes);

    void AddHeaderPid(pid_t pid);
    const std::vector<pid_t>& GetHeaderPids() const;

    uint32_t AddLayer();
    void LayerAddHeaderProperty(uint32_t layer, const std::string& name, const std::string& value);

    void UnwriteRSData();

    void WriteRSData(double time, const void* data, size_t size);
    void WriteOGLData(uint32_t layer, double time, const void* data, size_t size);
    void WriteRSMetrics(uint32_t layer, double time, const void* data, size_t size);
    void WriteRenderMetrics(uint32_t layer, double time, const void* data, size_t size);
    void WriteOGLMetrics(uint32_t layer, double time, uint32_t frame, const void* data, size_t size);
    void WriteGFXMetrics(uint32_t layer, double time, uint32_t frame, const void* data, size_t size);

    void ReadRSDataRestart();
    void ReadOGLDataRestart(uint32_t layer);
    void ReadRSMetricsRestart(uint32_t layer);
    void ReadRenderMetricsRestart(uint32_t layer);
    void ReadOGLMetricsRestart(uint32_t layer);
    void ReadGFXMetricsRestart(uint32_t layer);

    bool RSDataEOF() const;
    bool OGLDataEOF(uint32_t layer) const;
    bool RSMetricsEOF(uint32_t layer) const;
    bool RenderMetricsEOF(uint32_t layer) const;
    bool OGLMetricsEOF(uint32_t layer) const;
    bool GFXMetricsEOF(uint32_t layer) const;

    bool ReadRSData(double untilTime, std::vector<uint8_t>& data, double& readTime);
    bool ReadOGLData(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadRSMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadRenderMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadOGLMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool ReadGFXMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    bool GetDataCopy(std::vector<uint8_t>& data); // copy the content of RSFile so far

    bool HasLayer(uint32_t layer) const;

    void SetPreparedHeader(const std::vector<uint8_t>& headerData);
    void GetPreparedHeader(std::vector<uint8_t>& headerData);
    void SetPreparedHeaderMode(bool mode);
    void Close();

    uint32_t GetVersion() const;
    void SetVersion(uint32_t version);

    static const std::string& GetDefaultPath();

    void CacheVsyncId2Time(uint32_t layer);
    double ConvertVsyncId2Time(int64_t vsyncId);
    int64_t ConvertTime2VsyncId(double time) const;

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

    std::string ReadHeaders();
    std::string ReadHeader();
    bool ReadHeaderPidList();
    bool ReadHeaderFirstScreen();
    std::string LayerReadHeader(uint32_t layer);

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

    using LayerTrackIndexPtr = uint32_t RSFileLayer::*;
    using LayerTrackMarkupPtr = RSFileLayer::TrackMarkup RSFileLayer::*;
    struct LayerTrackPtr {
        LayerTrackIndexPtr index;
        LayerTrackMarkupPtr markup;
    };

    void UnwriteTrackData(LayerTrackMarkupPtr trackMarkup, uint32_t layer);
    void WriteTrackData(LayerTrackMarkupPtr trackMarkup, uint32_t layer, double time, const void* data, size_t size);
    bool ReadTrackData(
        LayerTrackPtr track, double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime);
    void ReadTrackDataRestart(LayerTrackIndexPtr trackIndex, uint32_t layer);
    bool TrackEOF(LayerTrackPtr track, uint32_t layer) const;

private:
    FILE* file_ = nullptr;
    uint32_t versionId_ = 0;
    double writeStartTime_ = 0.0;
    uint32_t headerOff_ = 0u;
    std::vector<pid_t> headerPidList_;
    std::vector<RSFileLayer> layerData_;
    uint32_t writeDataOff_ = 0u; // last byte of file where we can continue writing
    std::string headerFirstFrame_;
    std::vector<std::pair<uint64_t, int64_t>> headerAnimeStartTimes_;
    std::mutex writeMutex_;
    bool wasChanged_ = false;
    std::vector<uint8_t> preparedHeader_;
    bool preparedHeaderMode_ = false;
    std::map<int64_t, double> mapVsyncId2Time_;

    static constexpr size_t chunkSizeMax = 100'000'000;
    static constexpr size_t headerSizeMax = 512u * 1024u * 1024u;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_FILE_H