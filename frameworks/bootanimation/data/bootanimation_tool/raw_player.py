#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
"""

import zlib
import struct
import time
import numpy as np
import cv2
import sys
import re
import argparse


class RawPlayer:
    """
        Play a boot video file
    """

    def __init__(self, args):
        self._raw = args.raw
        cv2.namedWindow("play", cv2.WINDOW_AUTOSIZE)
        pass

    def play(self):
        screen_size = re.findall("bootanimation-([0-9]+)x([0-9]+).raw", self._raw)
        if len(screen_size) != 1:
            exit()
        width, height = int(screen_size[0][0]), int(screen_size[0][1])
        with open(sys.argv[-1], "rb") as fp:
            data = fp.read()
        off = 8
        img = None
        while off < len(data):
            data_type, offset, length, data_length = struct.unpack("IIII", data[off:off + 16])
            off += 16
            if data_type == 0:
                time.sleep(0.03)
                continue

            out = zlib.decompress(data[off:off + data_length])

            if img is None:
                img = np.copy(np.frombuffer(out, dtype=np.uint8))
            else:
                temp_img = np.frombuffer(out, dtype=np.uint8)
                img[offset:offset + length] = temp_img
            reshape_img = img.reshape((height, width, 4))
            cv2.imshow("play", reshape_img)
            if cv2.waitKey(30) & 0xff == 27 or cv2.getWindowProperty("play", cv2.WND_PROP_VISIBLE) < 1:
                break
            while data_length % 4 != 0:
                data_length += 1
            off += data_length


def parse_option():
    parser = argparse.ArgumentParser(description="Play a boot video file",
                                     usage="python raw_player.py [-h] <*.raw>\n"
                                           "  eg.: python raw_player.py ./bootanimation-640x480.raw")
    parser.add_argument("raw", metavar="<*.raw>", help="file <*.raw> to play")
    return parser.parse_args()


if __name__ == "__main__":
    raw_player = RawPlayer(parse_option())
    raw_player.play()
