# !/usr/bin/env python3
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

import struct
import zlib
import os
import argparse
import re
import pip


def lost_module(module_name):
    print("""
need %s module, try install first:

    pip install %s""" % (module_name, module_name))
    exit()


try:
    import cv2
except ImportError:
    pip.main(["install", "opencv-python", "-i", "https://pypi.tuna.tsinghua.edu.cn/simple"])
    try:
        import cv2
    except ImportError:
        cv2 = None
        lost_module("opencv-python")

try:
    from PIL import Image
except ImportError:
    pip.main(["install", "pillow"])
    try:
        from PIL import Image
    except ImportError:
        Image = None
        lost_module("pillow")

try:
    import numpy as np
except ImportError:
    pip.main(["install", "numpy"])
    try:
        import numpy as np
    except ImportError:
        np = None
        lost_module("numpy")


class RawMaker:
    """
    Make a boot video by a MP4 file or some .img files:
    """

    def __init__(self, args):
        self._mp4 = args.mp4
        self._image = args.image
        self._out = args.out
        self._display = [int(i) for i in re.split(r'[xX* ]+', args.display.strip())]
        self._rotate = args.rotate
        self._flip = args.flip
        self._fnp = 0
        self._vdo = None
        self._image_files = []

    def _iter_img(self):
        if self._mp4:
            success, frame = self._vdo.read()
            if success:
                image = Image.fromarray(frame)
                return success, image
            else:
                return False, None
        else:
            if self._fnp >= len(self._image_files):
                return False, None
            image = Image.open(os.path.join(self._image, self._image_files[self._fnp]))
            self._fnp += 1
            return True, image

    def make(self):
        frame_count, width, height = 0, 0, 0
        if self._mp4:
            if not os.path.exists(self._mp4):
                print("mp4 file %s is not exist" % self._mp4)
                exit()
            self._vdo = cv2.VideoCapture(self._mp4)
            fps = int(self._vdo.get(cv2.CAP_PROP_FPS))
            w = int(self._vdo.get(cv2.CAP_PROP_FRAME_WIDTH))
            h = int(self._vdo.get(cv2.CAP_PROP_FRAME_HEIGHT))
            frame_count = int(self._vdo.get(cv2.CAP_PROP_FRAME_COUNT))
            if fps != 30:
                print("video fps :", fps, ", width :", w, ", height :", h, ", frame count :", frame_count)
            if frame_count <= 0:
                exit()
        elif self._image:
            for fn in os.listdir(self._image):
                self._image_files.append(fn)
            frame_count = len(self._image_files)
            if frame_count <= 0:
                exit()
            self._image_files.sort()
        else:
            exit()

        output_bytes = bytearray(b"RAW.diff")
        offset = 8
        screen_old_bytes = None
        num = 0
        while True:
            ret, img = self._iter_img()
            if not ret:
                break
            num += 1
            img = img.convert("RGBA")
            if self._flip:
                img = img.transpose(Image.FLIP_LEFT_RIGHT)
            if self._rotate == 90:
                img = img.transpose(Image.ROTATE_90)
            elif self._rotate == 180:
                img = img.transpose(Image.ROTATE_180)
            elif self._rotate == 270:
                img = img.transpose(Image.ROTATE_270)
            if self._display[0] != 0:
                img = img.resize((self._display[0], self._display[1]))
            img = np.array(img)
            height, width = img.shape[0], img.shape[1]
            img[img < 20] = 0
            img = img.reshape(-1)
            screen_now_bytes = img.tobytes()
            if screen_old_bytes is None:
                screen_old_bytes = screen_now_bytes
                start_pos = 0
                end_pos = width * height * 4
            else:
                start_pos, end_pos = 3, 6
                for i in range(width * height * 4):
                    if screen_now_bytes[i] != screen_old_bytes[i]:
                        start_pos = i
                        break
                for i in range(width * height * 4 - 1, start_pos, -1):
                    if screen_now_bytes[i] != screen_old_bytes[i]:
                        end_pos = i + 1
                        break
                screen_old_bytes = screen_now_bytes
            print("\r|%s%s|" % ("=" * int(num / frame_count * 30), " " * (30 - int(num / frame_count * 30))), 
			        "%.2f%%" % (num / frame_count * 100), end="", flush=True)
            if start_pos == 3 or end_pos == 6:
                output_bytes[offset:offset + 16] = struct.pack("IIII", 0, 0, 0, 0)
                offset += 16
                continue
            compressed_bytes = zlib.compress(screen_old_bytes[start_pos:end_pos])
            raw_len = end_pos - start_pos
            new_len = len(compressed_bytes)
            output_bytes[offset:offset + 16] = struct.pack("IIII", 2, start_pos, raw_len, new_len)
            offset += 16
            output_bytes[offset:offset + new_len] = compressed_bytes
            offset += new_len
            while new_len % 4 != 0:
                new_len += 1
                output_bytes[offset:offset + 1] = b'\0'
                offset += 1

        if not os.path.exists(self._out):
            os.makedirs(self._out)
        with open(os.path.join(self._out, "bootanimation-%dx%d.raw" % (width, height)), "wb") as fp:
            fp.write(output_bytes)
        print("\nGenerate successfully!")


def parse_option():
    parser = argparse.ArgumentParser(description="Make a boot video by a MP4 file or some .img files",
                                     usage="python raw_maker.py (-m <*.mp4> | -i <directory>) [-o <directory>] "
                                           "[-d <size>] [-r <angle>] [-f]\n"
                                           "  eg.: python raw_maker.py -i ./source/png -o ./out -d 640x480\n"
                                           "       python raw_maker.py -m ./animation.mp4 -o ./out -d 640x480")
    exclusive_group = parser.add_mutually_exclusive_group(required=True)
    exclusive_group.add_argument("-m", "--mp4", metavar="<*.mp4>", help="The input <*.mp4> file")
    exclusive_group.add_argument("-i", "--image", metavar="<directory>",
                                 help="The <directory> where image files are stored")
    parser.add_argument("-o", "--out", metavar="<directory>", default=".",
                        help="Place generated .raw files into the <directory>")
    parser.add_argument("-d", "--display", metavar="<size>", default="0x0",
                        help="Set the boot video display <size> and zoom the image, e.g.:640x480")
    parser.add_argument("-r", "--rotate", metavar="<angle>", type=int, help="Rotate video <angle>, e.g.:90 180 270")
    parser.add_argument("-f", "--flip", action="store_true", help="Flip the video", )
    return parser.parse_args()


if __name__ == "__main__":
    raw_maker = RawMaker(parse_option())
    raw_maker.make()
