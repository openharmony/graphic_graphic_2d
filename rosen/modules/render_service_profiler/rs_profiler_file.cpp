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

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "rs_profiler_file.h"

#ifndef REPLAY_TOOL_CLIENT
#include "platform/common/rs_log.h"
#endif

namespace OHOS::Rosen {

RSFile::RSFile() = default;

const std::string& RSFile::GetDefaultPath()
{
    static const std::string PATH("/data/rsrecord.bin");
    return PATH;
}

void RSFile::Create(const std::string& fname)
{
    if (file_ != NULL) {
        Close();
    }
    const std::lock_guard<std::mutex> lgMutex(writeMutex_);

    file_ = Utils::FileOpen(fname, "wbe");
    if (file_ == NULL) {
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
    if (file_ != NULL) {
        Close();
    }

    file_ = Utils::FileOpen(fname, "rbe");
    if (file_ == NULL) {
        return false;
    }

    uint32_t headerId = 'RPLY';
    Utils::FileRead(&headerId, sizeof(headerId), 1, file_);
    if (headerId != 'RPLY') {
        Utils::FileClose(file_);
        file_ = NULL;
        return false;
    }

    Utils::FileRead(&headerOff_, sizeof(headerOff_), 1, file_);

    Utils::FileSeek(file_, 0, SEEK_END);
    writeDataOff_ = ftell(file_);

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

void RSFile::AddHeaderPID(pid_t pid)
{
    if (std::find(std::begin(headerPidList_), std::end(headerPidList_), pid) != std::end(headerPidList_)) {
        return;
    }
    headerPidList_.push_back(pid);

    wasChanged_ = true;
}

void RSFile::SetImageMapPtr(std::map<uint64_t, ReplayImageCacheRecordFile>* imageMapPtr)
{
    imageMapPtr_ = imageMapPtr;
}

const std::vector<pid_t>& RSFile::GetHeaderPIDList() const
{
    return headerPidList_;
}

void RSFile::WriteHeader()
{
    // WARNING removed redundant mutex
    if (!file_) {
        return;
    }

    headerOff_ = writeDataOff_;
    Utils::FileSeek(file_, writeDataOff_, SEEK_SET);

    // WRITE TIME START
    Utils::FileWrite(&writeStartTime_, 1, sizeof(writeStartTime_), file_);

    // SAVE PID LIST
    uint32_t recordSize = headerPidList_.size();
    Utils::FileWrite(&recordSize, sizeof(recordSize), 1, file_);
    Utils::FileWrite(&headerPidList_[0], headerPidList_.size(), sizeof(pid_t), file_);

    // ALL TEXTURES
    if (imageMapPtr_ == nullptr) {
        recordSize = 0;
        Utils::FileWrite(&recordSize, sizeof(recordSize), 1, file_);
    } else {
        recordSize = imageMapPtr_->size();
        Utils::FileWrite(&recordSize, sizeof(recordSize), 1, file_);
        for (auto item : *imageMapPtr_) {
            Utils::FileWrite(&item.first, sizeof(item.first), 1, file_);                       // unique_id
            Utils::FileWrite(&item.second.skipBytes, sizeof(item.second.skipBytes), 1, file_); // skip bytes
            Utils::FileWrite(&item.second.imageSize, sizeof(item.second.imageSize), 1, file_); // image_size
            Utils::FileWrite(item.second.image.get(), item.second.imageSize, 1, file_);        // image_data
        }
    }

    // SAVE LAYERS OFFSETS
    recordSize = layerData_.size();
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
    Utils::FileRead(&headerPidList_[0], headerPidList_.size(), sizeof(pid_t), file_);

    // ALL TEXTURES
    if (imageMapPtr_ != nullptr) {
        imageMapPtr_->clear();
    }
    Utils::FileRead(&recordSize, sizeof(recordSize), 1, file_);
    for (size_t i = 0; i < recordSize; i++) {
        ReadTextureFromFile();
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
    const int fileOffset = ftell(file_);
    if (fileOffset < 0) {
        ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY WRITE Layer negative file offset"); // NOLINT
    }
    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY WRITE Layer " // NOLINT
               "prop_data_size=%d foff=%d",
        (int)recordSize, fileOffset);
    Utils::FileWrite(propertyData.data(), propertyData.size(), 1, file_);

    LayerWriteHeaderOfTrack(layerData.rsData);
    LayerWriteHeaderOfTrack(layerData.oglData);
    LayerWriteHeaderOfTrack(layerData.rsMetrics);
    LayerWriteHeaderOfTrack(layerData.oglMetrics);
    LayerWriteHeaderOfTrack(layerData.gfxMetrics);

    const int layerLen = ftell(file_) - layerHeaderOff;
    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY WRITE Layer offset=%d len=%d", // NOLINT
        (int)layerHeaderOff, layerLen);
    layerData.layerHeader = { layerHeaderOff, ftell(file_) - layerHeaderOff }; // position of layer table

    writeDataOff_ = ftell(file_);
}

void RSFile::LayerReadHeader(uint32_t layer)
{
    if (!file_ || !HasLayer(layer)) {
        return;
    }

    RSFileLayer& layerData = layerData_[layer];

    Utils::FileSeek(file_, layerData.layerHeader.first, SEEK_SET);
    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY READ Layer offset=%d", (int)layerData.layerHeader.first); // NOLINT

    // READ LAYER PROPERTY
    uint32_t recordSize;
    Utils::FileRead(&recordSize, sizeof(recordSize), 1, file_);
    std::vector<char> propertyData;
    propertyData.resize(recordSize);
    Utils::FileRead(propertyData.data(), recordSize, 1, file_);
    layerData.property.Deserialize(propertyData);
    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY READ Layer prop_data_size=%d", (int)recordSize); // NOLINT

    LayerReadHeaderOfTrack(layerData.rsData);
    LayerReadHeaderOfTrack(layerData.oglData);
    LayerReadHeaderOfTrack(layerData.rsMetrics);
    LayerReadHeaderOfTrack(layerData.oglMetrics);
    LayerReadHeaderOfTrack(layerData.gfxMetrics);
}

void RSFile::ReadTextureFromFile()
{
    uint64_t key = 0;
    uint32_t dataSize = 0;
    uint32_t skipBytes = 0;
    uint8_t* data = nullptr;
    Utils::FileRead(&key, sizeof(key), 1, file_);
    Utils::FileRead(&skipBytes, sizeof(skipBytes), 1, file_);
    Utils::FileRead(&dataSize, sizeof(dataSize), 1, file_);

    constexpr size_t maxPictureSize = 600 * 1024 * 1024; // OHOS::MEDIA::PIXEL_MAP_MAX_RAM_SIZE
    if (dataSize == 0 || dataSize > maxPictureSize) {
        RS_LOGE("Invalid dataSize read: %d", dataSize); // NOLINT
        return;
    }

    data = new uint8_t[dataSize];
    if (data == nullptr) {
        RS_LOGE("Can't allocate %d bytes", dataSize); // NOLINT
        return;
    }

    Utils::FileRead(data, dataSize, 1, file_);
    const std::shared_ptr<void> ptr(data);
    if (imageMapPtr_ != nullptr) {
        ReplayImageCacheRecordFile record;
        record.image = ptr;
        record.imageSize = dataSize;
        record.skipBytes = skipBytes;
        imageMapPtr_->insert({ key, record });
    }
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

    ROSEN_LOGD("RSMainThread::MainLoop Server REPLAY READ RSData offset=%d len=%d read_time=%lf", // NOLINT
        (int)layerData.rsData[layerData.readindexRsData].first, (int)layerData.rsData[layerData.readindexRsData].second,
        readTime);

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
    file_ = NULL;

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
    writeDataOff_ = ftell(file_);
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

} // namespace OHOS::Rosen