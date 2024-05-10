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

#include "rs_profiler_file.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "rs_profiler_cache.h"

namespace OHOS::Rosen {

RSFile::RSFile() = default;

const std::string& RSFile::GetDefaultPath()
{
    static const std::string PATH("RECORD_IN_MEMORY");
    return PATH;
}

void RSFile::Create(const std::string& fname)
{
    if (file_ != nullptr) {
        Close();
    }
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

#ifdef REPLAY_TOOL_CLIENT
    file_ = Utils::FileOpen(fname, "wb");
#else
    file_ = Utils::FileOpen(fname, "wbe");
#endif
    if (file_ == nullptr) {
        return;
    }

    uint32_t headerId = 'RPLY';
    Utils::FileWrite(&headerId, sizeof(headerId), 1, file_);

    headerOff_ = 0; // TEMP VALUE
    Utils::FileWrite(&headerOff_, sizeof(headerOff_), 1, file_);

    writeDataOff_ = RSFileLayer::MARKUP_SIZE;

    wasChanged_ = true;
}

bool RSFile::Open(const std::string& fname)
{
    if (file_ != nullptr) {
        Close();
    }

#ifdef REPLAY_TOOL_CLIENT
    file_ = Utils::FileOpen(fname, "rb");
#else
    file_ = Utils::FileOpen(fname, "rbe");
#endif
    if (file_ == nullptr) {
        return false;
    }

    uint32_t headerId = 'RPLY';
    Utils::FileRead(&headerId, sizeof(headerId), 1, file_);
    if (headerId != 'RPLY') {
        Utils::FileClose(file_);
        file_ = nullptr;
        return false;
    }

    Utils::FileRead(&headerOff_, sizeof(headerOff_), 1, file_);

    Utils::FileSeek(file_, 0, SEEK_END);
    writeDataOff_ = Utils::FileTell(file_);

    Utils::FileSeek(file_, headerOff_, SEEK_SET);
    ReadHeaders();

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

        // ALL TEXTURES
        ImageCache::Serialize(file_);
    }

    // SAVE LAYERS OFFSETS
    const uint32_t recordSize = layerData_.size();
    Utils::FileWrite(&recordSize, sizeof(recordSize), 1, file_);
    for (auto& i : layerData_) {
        Utils::FileWrite(&i.layerHeader, sizeof(i.layerHeader), 1, file_);
    }

    constexpr int offset4 = 4;
    Utils::FileSeek(file_, offset4, SEEK_SET);
    Utils::FileWrite(&headerOff_, sizeof(headerOff_), 1, file_);
}

void RSFile::ReadHeader()
{
    if (!file_) {
        return;
    }

    Utils::FileSeek(file_, headerOff_, SEEK_SET);

    // READ what was write start time
    Utils::FileRead(&writeStartTime_, 1, sizeof(writeStartTime_), file_);

    // READ PID LIST
    uint32_t recordSize;
    Utils::FileRead(&recordSize, sizeof(recordSize), 1, file_);
    headerPidList_.resize(recordSize);
    Utils::FileRead(headerPidList_.data(), headerPidList_.size(), sizeof(pid_t), file_);

    // READ FIRST SCREEN
    uint32_t firstScrSize;
    Utils::FileRead(&firstScrSize, sizeof(firstScrSize), 1, file_);
    headerFirstFrame_.resize(firstScrSize);
    Utils::FileRead(headerFirstFrame_.data(), headerFirstFrame_.size(), 1, file_);

    // ALL TEXTURES
    ImageCache::Deserialize(file_);

    if (preparedHeaderMode_) {
        const size_t subHeaderEndOff = Utils::FileTell(file_);
        Utils::FileSeek(file_, headerOff_, SEEK_SET);
        const size_t subHeaderLen = subHeaderEndOff - headerOff_;
        preparedHeader_.resize(subHeaderLen);
        Utils::FileRead(preparedHeader_.data(), subHeaderLen, 1, file_);
    }

    // READ LAYERS OFFSETS
    Utils::FileRead(&recordSize, sizeof(recordSize), 1, file_);
    layerData_.resize(recordSize);
    for (auto& i : layerData_) {
        Utils::FileRead(&i.layerHeader, sizeof(i.layerHeader), 1, file_);
        i.readindexRsData = 0;
    }
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
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (!file_ || !HasLayer(layer)) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];

    const uint32_t layerHeaderOff = writeDataOff_; // position of layer table
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
    LayerWriteHeaderOfTrack(layerData.oglMetrics);
    LayerWriteHeaderOfTrack(layerData.gfxMetrics);
    layerData.layerHeader = { layerHeaderOff, Utils::FileTell(file_) - layerHeaderOff }; // position of layer table

    writeDataOff_ = Utils::FileTell(file_);
}

void RSFile::LayerReadHeader(uint32_t layer)
{
    if (!file_ || !HasLayer(layer)) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];

    Utils::FileSeek(file_, layerData.layerHeader.first, SEEK_SET);

    // READ LAYER PROPERTY
    uint32_t recordSize = 0u;
    Utils::FileRead(&recordSize, sizeof(recordSize), 1, file_);
    std::vector<char> propertyData;
    propertyData.resize(recordSize);
    Utils::FileRead(propertyData.data(), recordSize, 1, file_);

    layerData.property.Deserialize(propertyData);
    LayerReadHeaderOfTrack(layerData.rsData);
    LayerReadHeaderOfTrack(layerData.oglData);
    LayerReadHeaderOfTrack(layerData.rsMetrics);
    LayerReadHeaderOfTrack(layerData.oglMetrics);
    LayerReadHeaderOfTrack(layerData.gfxMetrics);
}

// ***********************************
// *** LAYER DATA - WRITE

// XXX: static std::map<uint64_t, std::pair<std::shared_ptr<void>, int>> ImageMap;

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

void RSFile::WriteOGLMetrics(uint32_t layer, double time, uint32_t /*frame*/, const void* data, size_t size)
{
    WriteTrackData(&RSFileLayer::oglMetrics, layer, time, data, size);
}

void RSFile::WriteGFXMetrics(uint32_t layer, double time, uint32_t /*frame*/, const void* data, size_t size)
{
    WriteTrackData(&RSFileLayer::gfxMetrics, layer, time, data, size);
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

void RSFile::ReadOGLMetricsRestart(uint32_t layer)
{
    ReadTrackDataRestart(&RSFileLayer::readindexOglMetrics, layer);
}

void RSFile::ReadGFXMetricsRestart(uint32_t layer)
{
    ReadTrackDataRestart(&RSFileLayer::readindexGfxMetrics, layer);
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

bool RSFile::OGLMetricsEOF(uint32_t layer) const
{
    return TrackEOF({ &RSFileLayer::readindexOglMetrics, &RSFileLayer::oglMetrics }, layer);
}

bool RSFile::GFXMetricsEOF(uint32_t layer) const
{
    return TrackEOF({ &RSFileLayer::readindexGfxMetrics, &RSFileLayer::gfxMetrics }, layer);
}

bool RSFile::ReadRSData(double untilTime, std::vector<uint8_t>& data, double& readTime)
{
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

    const uint32_t dataLen = layerData.rsData[layerData.readindexRsData].second - sizeof(readTime);
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

bool RSFile::ReadOGLMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    return ReadTrackData(
        { &RSFileLayer::readindexOglMetrics, &RSFileLayer::oglMetrics }, untilTime, layer, data, readTime);
}

bool RSFile::ReadGFXMetrics(double untilTime, uint32_t layer, std::vector<uint8_t>& data, double& readTime)
{
    return ReadTrackData(
        { &RSFileLayer::readindexGfxMetrics, &RSFileLayer::gfxMetrics }, untilTime, layer, data, readTime);
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
    for (size_t i = 0; i < layerData_.size(); i++) {
        LayerWriteHeader(i);
    }
    WriteHeader();
}

void RSFile::ReadHeaders()
{
    ReadHeader();
    for (size_t i = 0; i < layerData_.size(); i++) {
        LayerReadHeader(i);
    }
}

void RSFile::Close()
{
    if (!file_) {
        return;
    }

    if (wasChanged_) {
        WriteHeaders();
        wasChanged_ = false;
    }

    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    Utils::FileClose(file_);
    file_ = nullptr;

    headerOff_ = 0;
    headerPidList_.clear();
    layerData_.clear();

    writeDataOff_ = 0;
    wasChanged_ = false;
}

void RSFile::WriteTrackData(LayerTrackMarkupPtr trackMarkup, uint32_t layer, double time, const void* data, size_t size)
{
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    if (!file_ || layerData_.empty()) {
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

    const uint32_t dataLen = trackData[trackIndex].second - RSFileLayer::MARKUP_SIZE;
    data.resize(dataLen);
    Utils::FileRead(data.data(), dataLen, 1, file_);

    trackIndex++;
    return true;
}

void RSFile::ReadTrackDataRestart(LayerTrackIndexPtr trackIndex, uint32_t layer)
{
    if (layerData_.empty()) {
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

} // namespace OHOS::Rosen