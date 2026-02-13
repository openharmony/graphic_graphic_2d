/*
 * Copyright (c) 2024-2026 Huawei Device Co., Ltd.
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

#include "rs_profiler_file.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "rs_profiler_cache.h"
#include "rs_profiler_command.h"

namespace OHOS::Rosen {

RSFile::RSFile() = default;

const std::string& RSFile::GetDefaultPath()
{
    static const std::string PATH("RECORD_IN_MEMORY");
    return PATH;
}

bool RSFile::Create(const std::string& fname)
{
    if (file_ != nullptr) {
        Close();
    }
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

#ifdef RENDER_PROFILER_APPLICATION
    file_ = Utils::FileOpen(fname, "wb");
#else
    file_ = Utils::FileOpen(fname, "wbe");
#endif
    if (file_ == nullptr) {
        return false;
    }

    uint32_t headerId = 'ROHR';
    Utils::FileWrite(&headerId, sizeof(headerId), 1, file_);
    Utils::FileWrite(&versionId_, sizeof(versionId_), 1, file_);
    offsetVersionHandler_.SetVersion(versionId_);

    headerOff_ = 0; // TEMP VALUE
    offsetVersionHandler_.WriteU64(file_, headerOff_);

    writeDataOff_ = Utils::FileTell(file_);

    wasChanged_ = true;
    return true;
}

bool RSFile::Open(const std::string& fname, std::string& error)
{
    error.clear();
    if (file_ != nullptr) {
        Close();
    }

#ifdef RENDER_PROFILER_APPLICATION
    file_ = Utils::FileOpen(fname, "rb");
#else
    file_ = Utils::FileOpen(fname, "rbe");
#endif
    if (file_ == nullptr) {
        error = "Invalid file path: " + fname;
        return false;
    }

    uint32_t headerId;
    Utils::FileRead(&headerId, sizeof(headerId), 1, file_);
    if (headerId != 'ROHR') { // Prohibit too old file versions
        Utils::FileClose(file_);
        file_ = nullptr;
        error = "Unsupported version: " + std::to_string(headerId);
        return false;
    }

    Utils::FileRead(&versionId_, sizeof(versionId_), 1, file_);
    offsetVersionHandler_.SetVersion(versionId_);
    offsetVersionHandler_.ReadU64(file_, headerOff_);
    Utils::FileSeek(file_, 0, SEEK_END);
    writeDataOff_ = Utils::FileTell(file_);
    Utils::FileSeek(file_, headerOff_, SEEK_SET);
    error = ReadHeaders();
    if (!error.empty()) {
        Utils::FileClose(file_);
        file_ = nullptr;
        return false;
    }
    wasChanged_ = false;

    return true;
}

bool RSFile::IsOpen() const
{
    return file_ != nullptr;
}

void RSFile::SetWriteTime(double time)
{
    writeStartTime_ = time;
}

double RSFile::GetWriteTime() const
{
    return writeStartTime_;
}

// ***********************************
// *** GLOBAL HEADER

void RSFile::AddHeaderPid(pid_t pid)
{
    if (std::find(std::begin(headerPidList_), std::end(headerPidList_), pid) != std::end(headerPidList_)) {
        return;
    }
    headerPidList_.push_back(pid);

    wasChanged_ = true;
}

const std::vector<pid_t>& RSFile::GetHeaderPids() const
{
    return headerPidList_;
}

void RSFile::SetPreparedHeader(const std::vector<uint8_t>& headerData)
{
    preparedHeader_ = headerData;
}

void RSFile::GetPreparedHeader(std::vector<uint8_t>& headerData)
{
    headerData = preparedHeader_;
}

void RSFile::SetPreparedHeaderMode(bool mode)
{
    preparedHeaderMode_ = mode;
}

void RSFile::WriteHeader()
{
    // WARNING removed redundant mutex
    if (!file_) {
        return;
    }

    headerOff_ = writeDataOff_;
    Utils::FileSeek(file_, writeDataOff_, SEEK_SET);

    if (preparedHeaderMode_) {
        // WRITE RAW
        Utils::FileWrite(preparedHeader_.data(), 1, preparedHeader_.size(), file_);
        preparedHeader_.clear();
    } else {
        // WRITE TIME START
        Utils::FileWrite(&writeStartTime_, 1, sizeof(writeStartTime_), file_);

        // SAVE PID LIST
        const uint32_t recordSize = headerPidList_.size();
        Utils::FileWrite(&recordSize, sizeof(recordSize), 1, file_);
        Utils::FileWrite(headerPidList_.data(), headerPidList_.size(), sizeof(pid_t), file_);

        // SAVE FIRST SCREEN
        uint32_t firstScrSize = headerFirstFrame_.size();
        Utils::FileWrite(&firstScrSize, sizeof(firstScrSize), 1, file_);
        Utils::FileWrite(headerFirstFrame_.data(), headerFirstFrame_.size(), 1, file_);

        WriteAnimationStartTime();

        // ALL TEXTURES
        ImageCache::Serialize(file_);
    }

    // SAVE LAYERS OFFSETS
    const uint32_t recordSize = layerData_.size();
    Utils::FileWrite(&recordSize, sizeof(recordSize), 1, file_);
    for (auto& i : layerData_) {
        // Write separately
        offsetVersionHandler_.WriteU64(file_, i.layerHeader.first);
        offsetVersionHandler_.WriteU64(file_, i.layerHeader.second);
    }

    constexpr int preambleSize = 8;
    Utils::FileSeek(file_, preambleSize, SEEK_SET);
    offsetVersionHandler_.WriteU64(file_, headerOff_);
}

bool RSFile::ReadHeaderPidList()
{
    uint32_t recordSize;
    Utils::FileRead(&recordSize, sizeof(recordSize), 1, file_);
    if (recordSize > chunkSizeMax) {
        return false;
    }
    headerPidList_.resize(recordSize);
    Utils::FileRead(headerPidList_.data(), headerPidList_.size(), sizeof(pid_t), file_);
    return true;
}

void RSFile::WriteAnimationStartTime()
{
    if (versionId_ >= RSFILE_VERSION_RENDER_ANIMESTARTTIMES_ADDED) {
        // ANIME START TIMES
        uint32_t startTimesSize = headerAnimeStartTimes_.size();
        Utils::FileWrite(&startTimesSize, sizeof(startTimesSize), 1, file_);
        Utils::FileWrite(headerAnimeStartTimes_.data(),
            headerAnimeStartTimes_.size() * sizeof(std::pair<uint64_t, int64_t>), 1, file_);
    }
}

bool RSFile::ReadAnimationStartTime()
{
    if (versionId_ >= RSFILE_VERSION_RENDER_ANIMESTARTTIMES_ADDED) {
        uint32_t startTimesSize = 0u;
        if (!Utils::FileRead(&startTimesSize, sizeof(startTimesSize), 1, file_) && startTimesSize > chunkSizeMax) {
            return false;
        }
        headerAnimeStartTimes_.resize(startTimesSize);
        if (!Utils::FileRead(headerAnimeStartTimes_.data(),
            headerAnimeStartTimes_.size() * sizeof(std::pair<uint64_t, int64_t>), 1, file_)) {
            return false;
        }
    }
    return true;
}

bool RSFile::ReadLayersOffset()
{
    uint32_t recordSize = 0u;
    if (!Utils::FileRead(&recordSize, sizeof(recordSize), 1, file_) && recordSize > chunkSizeMax) {
        return false;
    }
    layerData_.resize(recordSize);
    for (auto& i : layerData_) {
        // Read separately because uint32_t, uint32_t or uint64_t, uint64_t
        bool res1 = offsetVersionHandler_.ReadU64(file_, i.layerHeader.first);
        bool res2 = offsetVersionHandler_.ReadU64(file_, i.layerHeader.second);
        if (!res1 || !res2) {
            return false;
        }
        i.readindexRsData = 0;
    }
    return true;
}

bool RSFile::ReadHeaderFirstScreen()
{
    uint32_t size = 0;
    Utils::FileRead(&size, sizeof(size), 1, file_);
    if ((size > headerFirstFrame_.max_size()) || (size + Utils::FileTell(file_) > Utils::FileSize(file_))) {
        return false;
    }
    headerFirstFrame_.resize(size);
    Utils::FileRead(headerFirstFrame_.data(), headerFirstFrame_.size(), 1, file_);
    return true;
}

std::string RSFile::ReadHeader()
{
    if (!file_) {
        return "Cannot read header";
    }

    Utils::FileSeek(file_, headerOff_, SEEK_SET);
    const uint64_t subHeaderStartOff = Utils::FileTell(file_);

    const std::string errCode = "can't read header";

    // READ what was write start time
    if (!Utils::FileRead(&writeStartTime_, 1, sizeof(writeStartTime_), file_)) {
        return errCode;
    }

    // READ PID LIST
    if (!ReadHeaderPidList()) {
        return "Invalid pid list header";
    }

    if (!ReadHeaderFirstScreen()) {
        return "Invalid first frame header";
    }

    // READ ANIME START TIMES
    if (!ReadAnimationStartTime()) {
        return errCode;
    }

    // ALL TEXTURES
    ImageCache::Deserialize(file_);

    if (preparedHeaderMode_) {
        const size_t subHeaderEndOff = Utils::FileTell(file_);
        Utils::FileSeek(file_, subHeaderStartOff, SEEK_SET);
        const size_t subHeaderLen = subHeaderEndOff - subHeaderStartOff;
        if (subHeaderLen > headerSizeMax) {
            return "Invalid subheader";
        }
        preparedHeader_.resize(subHeaderLen);
        if (!Utils::FileRead(preparedHeader_.data(), subHeaderLen, 1, file_)) {
            return errCode;
        }
    }

    // READ LAYERS OFFSETS
    if (!ReadLayersOffset()) {
        return "Invalid layers header";
    }
    return "";
}

// ***********************************
// *** LAYER HEADER

uint32_t RSFile::AddLayer()
{
    const uint32_t newId = layerData_.size();
    const RSFileLayer data;
    layerData_.push_back(data);

    wasChanged_ = true;

    return newId;
}

void RSFile::LayerAddHeaderProperty(uint32_t layer, const std::string& name, const std::string& value)
{
    if (!HasLayer(layer)) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.property.SetProperty(name, value);

    wasChanged_ = true;
}

void RSFile::LayerWriteHeader(uint32_t layer)
{
    if (!file_ || !HasLayer(layer)) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];

    const uint64_t layerHeaderOff = writeDataOff_; // position of layer table
    Utils::FileSeek(file_, writeDataOff_, SEEK_SET);

    std::vector<char> propertyData;
    layerData.property.Serialize(propertyData);

    // SAVE LAYER PROPERTY
    uint32_t recordSize = propertyData.size();
    Utils::FileWrite(&recordSize, sizeof(recordSize), 1, file_);
    Utils::FileWrite(propertyData.data(), propertyData.size(), 1, file_);

    LayerWriteHeaderOfTrack(layerData.rsData);
    LayerWriteHeaderOfTrack(layerData.oglData);
    LayerWriteHeaderOfTrack(layerData.rsMetrics);
    if (versionId_ >= RSFILE_VERSION_RENDER_METRICS_ADDED) {
        LayerWriteHeaderOfTrack(layerData.renderMetrics);
    }
    if (versionId_ >= RSFILE_VERSION_LOG_EVENTS_ADDED) {
        LayerWriteHeaderOfTrack(layerData.logEvents);
    }
    LayerWriteHeaderOfTrack(layerData.trace3DMetrics);
    LayerWriteHeaderOfTrack(layerData.gfxMetrics);
    layerData.layerHeader = { layerHeaderOff, Utils::FileTell(file_) - layerHeaderOff }; // position of layer table

    writeDataOff_ = Utils::FileTell(file_);
}

std::string RSFile::LayerReadHeader(uint32_t layer)
{
    const std::string errCode = "can't read layer header";
    if (!file_ || !HasLayer(layer)) {
        return errCode;
    }

    RSFileLayer& layerData = layerData_[layer];

    Utils::FileSeek(file_, layerData.layerHeader.first, SEEK_SET);

    // READ LAYER PROPERTY
    uint32_t recordSize = 0u;
    Utils::FileRead(&recordSize, sizeof(recordSize), 1, file_);
    if (recordSize > chunkSizeMax) {
        return errCode;
    }
    std::vector<char> propertyData;
    propertyData.resize(recordSize);
    Utils::FileRead(propertyData.data(), recordSize, 1, file_);

    layerData.property.Deserialize(propertyData);
    LayerReadHeaderOfTrack(layerData.rsData);
    LayerReadHeaderOfTrack(layerData.oglData);
    LayerReadHeaderOfTrack(layerData.rsMetrics);
    if (versionId_ >= RSFILE_VERSION_RENDER_METRICS_ADDED) {
        LayerReadHeaderOfTrack(layerData.renderMetrics);
    }
    if (versionId_ >= RSFILE_VERSION_LOG_EVENTS_ADDED) {
        LayerReadHeaderOfTrack(layerData.logEvents);
    }
    LayerReadHeaderOfTrack(layerData.trace3DMetrics);
    LayerReadHeaderOfTrack(layerData.gfxMetrics);

    return "";
}

uint32_t RSFile::GetVersion() const
{
    return versionId_;
}

void RSFile::SetVersion(uint32_t version)
{
    versionId_ = version;
}

// ***********************************
// *** LAYER DATA - UNWRITE

void RSFile::UnwriteRSData()
{
    UnwriteTrackData(&RSFileLayer::rsData, 0);
}

// ***********************************
// *** LAYER DATA - WRITE

void RSFile::WriteRSData(double time, const void* data, size_t size)
{
    WriteTrackData(&RSFileLayer::rsData, 0, time, data, size);
}

void RSFile::WriteOGLData(uint32_t layer, double time, const void* data, size_t size)
{
    WriteTrackData(&RSFileLayer::oglData, layer, time, data, size);
}

void RSFile::WriteRSMetrics(uint32_t layer, double time, const void* data, size_t size)
{
    WriteTrackData(&RSFileLayer::rsMetrics, layer, time, data, size);
}

void RSFile::WriteRenderMetrics(uint32_t layer, double time, const void* data, size_t size)
{
    WriteTrackData(&RSFileLayer::renderMetrics, layer, time, data, size);
}

void RSFile::WriteTrace3DMetrics(uint32_t layer, double time, uint32_t /*frame*/, const void* data, size_t size)
{
    WriteTrackData(&RSFileLayer::trace3DMetrics, layer, time, data, size);
}

void RSFile::WriteGFXMetrics(uint32_t layer, double time, uint32_t /*frame*/, const void* data, size_t size)
{
    WriteTrackData(&RSFileLayer::gfxMetrics, layer, time, data, size);
}

void RSFile::WriteLogEvent(uint32_t layer, double time, const void* data, size_t size)
{
    WriteTrackData(&RSFileLayer::logEvents, layer, time, data, size);
}

// ***********************************
// *** LAYER DATA - READ

void RSFile::ReadRSDataRestart()
{
    ReadTrackDataRestart(&RSFileLayer::readindexRsData, 0);
}

void RSFile::ReadOGLDataRestart(uint32_t layer)
{
    ReadTrackDataRestart(&RSFileLayer::readindexOglData, layer);
}

void RSFile::ReadRSMetricsRestart(uint32_t layer)
{
    ReadTrackDataRestart(&RSFileLayer::readindexRsMetrics, layer);
}

void RSFile::ReadRenderMetricsRestart(uint32_t layer)
{
    ReadTrackDataRestart(&RSFileLayer::readindexRenderMetrics, layer);
}

void RSFile::ReadTrace3DMetricsRestart(uint32_t layer)
{
    ReadTrackDataRestart(&RSFileLayer::readindexTrace3DMetrics, layer);
}

void RSFile::ReadGFXMetricsRestart(uint32_t layer)
{
    ReadTrackDataRestart(&RSFileLayer::readindexGfxMetrics, layer);
}

void RSFile::ReadLogEventRestart(uint32_t layer)
{
    ReadTrackDataRestart(&RSFileLayer::readindexLogEvents, layer);
}

double RSFile::GetEOFTime()
{
    const RSFileLayer& layerData = layerData_[0];
    int lastRecordIndex = layerData.rsData.size();
    lastRecordIndex--;
    if (lastRecordIndex >= 0) {
        double readTime;
        Utils::FileSeek(file_, layerData.rsData[lastRecordIndex].first, SEEK_SET);
        Utils::FileRead(&readTime, sizeof(readTime), 1, file_);
        return readTime;
    }
    return 0.0;
}

bool RSFile::RSDataEOF() const
{
    return TrackEOF({ &RSFileLayer::readindexRsData, &RSFileLayer::rsData }, 0);
}

bool RSFile::OGLDataEOF(uint32_t layer) const
{
    return TrackEOF({ &RSFileLayer::readindexOglData, &RSFileLayer::oglData }, layer);
}

bool RSFile::RSMetricsEOF(uint32_t layer) const
{
    return TrackEOF({ &RSFileLayer::readindexRsMetrics, &RSFileLayer::rsMetrics }, layer);
}

bool RSFile::RenderMetricsEOF(uint32_t layer) const
{
    return TrackEOF({ &RSFileLayer::readindexRenderMetrics, &RSFileLayer::renderMetrics }, layer);
}

bool RSFile::Trace3DMetricsEOF(uint32_t layer) const
{
    return TrackEOF({ &RSFileLayer::readindexTrace3DMetrics, &RSFileLayer::trace3DMetrics }, layer);
}

bool RSFile::GFXMetricsEOF(uint32_t layer) const
{
    return TrackEOF({ &RSFileLayer::readindexGfxMetrics, &RSFileLayer::gfxMetrics }, layer);
}

bool RSFile::LogEventEOF(uint32_t layer) const
{
    return TrackEOF({ &RSFileLayer::readindexLogEvents, &RSFileLayer::logEvents }, layer);
}

bool RSFile::ReadRSData(double untilTime, std::vector<uint8_t>& data, double& readTime)
{
    readTime = 0.0;
    if (!file_ || layerData_.empty()) {
        return false;
    }

    RSFileLayer& layerData = layerData_[0];

    if (layerData.readindexRsData >= layerData.rsData.size()) {
        return false;
    }

    Utils::FileSeek(file_, layerData.rsData[layerData.readindexRsData].first, SEEK_SET);

    Utils::FileRead(&readTime, sizeof(readTime), 1, file_);
    constexpr double epsilon = 1e-9;
    if (readTime >= untilTime + epsilon) {
        return false;
    }

    const uint64_t dataLen = layerData.rsData[layerData.readindexRsData].second - sizeof(readTime);
    if (dataLen > chunkSizeMax) {
        return false;
    }
    data.resize(dataLen);
    Utils::FileRead(data.data(), dataLen, 1, file_);

    layerData.readindexRsData++;
    return true;
}

bool RSFile::ReadOGLData(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    return ReadTrackData({ &RSFileLayer::readindexOglData, &RSFileLayer::oglData }, untilTime, layer, data, readTime);
}

bool RSFile::ReadRSMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    return ReadTrackData(
        { &RSFileLayer::readindexRsMetrics, &RSFileLayer::rsMetrics }, untilTime, layer, data, readTime);
}

bool RSFile::ReadRenderMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    return ReadTrackData(
        { &RSFileLayer::readindexRenderMetrics, &RSFileLayer::renderMetrics }, untilTime, layer, data, readTime);
}

bool RSFile::ReadTrace3DMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    return ReadTrackData(
        { &RSFileLayer::readindexTrace3DMetrics, &RSFileLayer::trace3DMetrics }, untilTime, layer, data, readTime);
}

bool RSFile::ReadGFXMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    return ReadTrackData(
        { &RSFileLayer::readindexGfxMetrics, &RSFileLayer::gfxMetrics }, untilTime, layer, data, readTime);
}

bool RSFile::ReadLogEvent(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    return ReadTrackData(
        { &RSFileLayer::readindexLogEvents, &RSFileLayer::logEvents }, untilTime, layer, data, readTime);
}

bool RSFile::GetDataCopy(std::vector<uint8_t>& data)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    WriteHeaders(); // Make sure the header is written

    size_t fileSize = Utils::FileSize(file_);
    if (fileSize == 0) {
        return false;
    }

    // File size threshold is set to ensure that the file is valid
    const size_t maxFileSize = 300000000;
    if (fileSize > maxFileSize) {
        return false;
    }

    data.clear();
    data.resize(fileSize);

    const int64_t position = static_cast<int64_t>(Utils::FileTell(file_));
    Utils::FileSeek(file_, 0, SEEK_SET);
    Utils::FileRead(file_, data.data(), fileSize);
    Utils::FileSeek(file_, position, SEEK_SET); // set ptr back

    return true;
}

bool RSFile::HasLayer(uint32_t layer) const
{
    // if this condition is true, then layerData_ is surely not empty
    return layer < layerData_.size();
}

// ***********************************
// *** READ/SAVE HEADERS

void RSFile::WriteHeaders()
{
    if (!(file_ && wasChanged_)) {
        return;
    }
    for (size_t i = 0; i < layerData_.size(); i++) {
        LayerWriteHeader(i);
    }
    WriteHeader();
}

std::string RSFile::ReadHeaders()
{
    std::string errReason = ReadHeader();
    if (errReason.size()) {
        return errReason;
    }
    for (size_t i = 0; i < layerData_.size(); i++) {
        errReason = LayerReadHeader(i);
        if (errReason.size()) {
            return errReason;
        }
    }
    return "";
}

void RSFile::Close()
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (!file_) {
        return;
    }

    WriteHeaders();

    Utils::FileClose(file_);
    file_ = nullptr;

    headerOff_ = 0;
    headerPidList_.clear();
    layerData_.clear();

    writeDataOff_ = 0;
    wasChanged_ = false;

    preparedHeaderMode_ = false;
    headerFirstFrame_.clear();
    headerAnimeStartTimes_.clear();
    preparedHeader_.clear();
    mapVsyncId2Time_.clear();
}

void RSFile::UnwriteTrackData(LayerTrackMarkupPtr trackMarkup, uint32_t layer)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (!file_ || !HasLayer(layer)) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    if ((layerData.*trackMarkup).size()) {
        (layerData.*trackMarkup).pop_back();
    }
}

void RSFile::WriteTrackData(LayerTrackMarkupPtr trackMarkup, uint32_t layer, double time, const void* data, size_t size)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (!file_ || !HasLayer(layer)) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    (layerData.*trackMarkup).emplace_back(writeDataOff_, size + sizeof(time));

    Utils::FileSeek(file_, writeDataOff_, SEEK_SET);
    Utils::FileWrite(&time, sizeof(time), 1, file_);
    Utils::FileWrite(data, size, 1, file_);
    writeDataOff_ = Utils::FileTell(file_);
}

bool RSFile::ReadTrackData(
    LayerTrackPtr track, double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    if (!file_ || !HasLayer(layer)) {
        return false;
    }

    RSFileLayer& layerData = layerData_[layer];
    auto& trackIndex = layerData.*track.index;
    auto& trackData = layerData.*track.markup;

    if (trackIndex >= trackData.size()) {
        return false;
    }

    Utils::FileSeek(file_, trackData[trackIndex].first, SEEK_SET);
    Utils::FileRead(&readTime, sizeof(readTime), 1, file_);
    if (readTime > untilTime) {
        return false;
    }

    const uint64_t dataLen = trackData[trackIndex].second - sizeof(readTime);
    if (dataLen > chunkSizeMax) {
        return false;
    }
    data.resize(dataLen);
    Utils::FileRead(data.data(), dataLen, 1, file_);

    trackIndex++;
    return true;
}

void RSFile::ReadTrackDataRestart(LayerTrackIndexPtr trackIndex, uint32_t layer)
{
    if (!HasLayer(layer)) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];
    layerData.*trackIndex = 0;
}

bool RSFile::TrackEOF(LayerTrackPtr track, uint32_t layer) const
{
    if (!file_ || !HasLayer(layer)) {
        return true;
    }

    const RSFileLayer& layerData = layerData_[layer];
    return layerData.*track.index >= (layerData.*track.markup).size();
}

const std::string& RSFile::GetHeaderFirstFrame() const
{
    return headerFirstFrame_;
}

void RSFile::AddHeaderFirstFrame(const std::string& dataFirstFrame)
{
    headerFirstFrame_ = dataFirstFrame;
    wasChanged_ = true;
}

void RSFile::InsertHeaderData(size_t offset, const std::string& data)
{
    if (offset > headerFirstFrame_.size()) {
        return;
    }
    headerFirstFrame_.insert(offset, data);
    wasChanged_ = true;
}

void RSFile::SetHeaderFailedNodeCount(size_t offset, uint32_t failedNodeCount)
{
    if (!failedNodeCount) {
        return;
    }
    if (offset + sizeof(uint32_t) >= headerFirstFrame_.size()) {
        return;
    }
    uint32_t *ptr = reinterpret_cast<uint32_t *>(headerFirstFrame_.data() + offset);
    if (*ptr >= failedNodeCount) {
        *ptr -= failedNodeCount;
    }
}

const std::vector<std::pair<uint64_t, int64_t>>& RSFile::GetAnimeStartTimes() const
{
    return headerAnimeStartTimes_;
}

void RSFile::AddAnimeStartTimes(const std::vector<std::pair<uint64_t, int64_t>>& startTimes)
{
    headerAnimeStartTimes_ = startTimes;
    wasChanged_ = true;
}

int64_t RSFile::GetClosestVsyncId(int64_t vsyncId)
{
    if (mapVsyncId2Time_.count(vsyncId)) {
        return vsyncId;
    }
    if (!mapVsyncId2Time_.size()) {
        return 0;
    }

    int64_t minVSync = mapVsyncId2Time_.begin()->first;
    int64_t maxVSync = mapVsyncId2Time_.rbegin()->first;

    auto it = mapVsyncId2Time_.lower_bound(vsyncId);
    if (it != mapVsyncId2Time_.end()) {
        return it->first;
    }
    return minVSync;
}

double RSFile::ConvertVsyncId2Time(int64_t vsyncId)
{
    if (mapVsyncId2Time_.count(vsyncId)) {
        return mapVsyncId2Time_[vsyncId];
    }
    return 0.0;
}

int64_t RSFile::ConvertTime2VsyncId(double time) const
{
    constexpr double numericError = 1e-5;
    for (const auto& item : mapVsyncId2Time_) {
        if (time <= item.second + numericError) {
            return item.first;
        }
    }
    return 0;
}

void RSFile::GetVsyncList(std::set<int64_t>& vsyncList) const
{
    vsyncList.clear();
    for (auto& item : mapVsyncId2Time_) {
        vsyncList.insert(item.first);
    }
}

void RSFile::GetStartAndEndTime(std::pair<double, double>& startAndEndTime) const
{
    startAndEndTime.first = mapVsyncId2Time_.begin()->second;
    startAndEndTime.second = mapVsyncId2Time_.rbegin()->second;
}

void RSFile::CacheVsyncId2Time(uint32_t layer)
{
    mapVsyncId2Time_.clear();

    if (!file_ || !HasLayer(layer)) {
        return;
    }

    LayerTrackPtr track = { &RSFileLayer::readindexRsMetrics, &RSFileLayer::rsMetrics };

    RSFileLayer& layerData = layerData_[layer];
    const auto& trackData = layerData.*track.markup;

    double readTime;

    for (const auto& trackItem : trackData) {
        Utils::FileSeek(file_, trackItem.first, SEEK_SET);
        Utils::FileRead(&readTime, sizeof(readTime), 1, file_);

        constexpr char packetTypeRsMetrics = 2;
        char packetType;

        Utils::FileRead(&packetType, sizeof(packetType), 1, file_);
        if (packetType != packetTypeRsMetrics) {
            continue;
        }

        const int64_t dataLen = trackItem.second - sizeof(readTime) - 1;
        constexpr int64_t dataLenMax = 100'000;
        if (dataLen < 0 || dataLen > dataLenMax) {
            continue;
        }
        std::vector<char> data;
        data.resize(dataLen);
        Utils::FileRead(data.data(), dataLen, 1, file_);

        RSCaptureData captureData;
        captureData.Deserialize(data);
        int64_t readVsyncId = captureData.GetPropertyInt64(RSCaptureData::KEY_RS_VSYNC_ID);
        if (readVsyncId > 0 && !mapVsyncId2Time_.count(readVsyncId)) {
            mapVsyncId2Time_.insert({readVsyncId, readTime});
        }
    }
}

} // namespace OHOS::Rosen