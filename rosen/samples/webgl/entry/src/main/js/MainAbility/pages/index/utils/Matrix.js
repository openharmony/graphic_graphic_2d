/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

export class Matrix4 {
    elements;

    constructor() {
        this.elements = new Float32Array(16);
        this.setIdentity();
    }

    setIdentity() {
        this.elements[0] = 1;
        this.elements[5] = 1;
        this.elements[10] = 1;
        this.elements[15] = 1;
        return this;
    }

    set(m) {
        let elements = m.getElements();
        for (let i = 0; i < 16; i++) {
            this.elements[i] = elements[i];
        }
        return this;
    }

    setTranslate(x, y, z) {
        this.elements[12] = x;
        this.elements[13] = y;
        this.elements[14] = z;
        return this;
    }

    setScale(x, y, z) {
        this.elements[0] = x;
        this.elements[5] = y;
        this.elements[10] = z;
        return this;
    }

    rotateX(e, s, c) {
        e[0] = 1;
        e[4] = 0;
        e[8] = 0;
        e[12] = 0;
        e[1] = 0;
        e[5] = c;
        e[9] = -s;
        e[13] = 0;
        e[2] = 0;
        e[6] = s;
        e[10] = c;
        e[14] = 0;
        e[3] = 0;
        e[7] = 0;
        e[11] = 0;
        e[15] = 1;
    }

    rotateY(e, s, c) {
        e[0] = c;
        e[4] = 0;
        e[8] = s;
        e[12] = 0;
        e[1] = 0;
        e[5] = 1;
        e[9] = 0;
        e[13] = 0;
        e[2] = -s;
        e[6] = 0;
        e[10] = c;
        e[14] = 0;
        e[3] = 0;
        e[7] = 0;
        e[11] = 0;
        e[15] = 1;
    }

    rotateZ(e, s, c) {
        e[0] = c;
        e[4] = -s;
        e[8] = 0;
        e[12] = 0;
        e[1] = s;
        e[5] = c;
        e[9] = 0;
        e[13] = 0;
        e[2] = 0;
        e[6] = 0;
        e[10] = 1;
        e[14] = 0;
        e[3] = 0;
        e[7] = 0;
        e[11] = 0;
        e[15] = 1;
    }

    setRotate(angle, x, y, z) {
        let e, s, c, len, rlen, nc, xy, yz, zx, xs, ys, zs;
        angle = Math.PI * angle / 180;
        e = this.elements;
        s = Math.sin(angle);
        c = Math.cos(angle);
        if (0 !== x && 0 === y && 0 === z) {
            if (x < 0) {
                s = -s;
            }
            this.rotateX(e, s, c)
        } else if (0 === x && 0 !== y && 0 === z) {
            if (y < 0) {
                s = -s;
            }
            this.rotateY(e, s, c)
        } else if (0 === x && 0 === y && 0 !== z) {
            if (z < 0) {
                s = -s;
            }
            this.rotateZ(e, s, c)
        } else {
            len = Math.sqrt(x * x + y * y + z * z);
            if (len !== 1) {
                rlen = 1 / len;
                x *= rlen;
                y *= rlen;
                z *= rlen;
            }
            nc = 1 - c;
            xy = x * y;
            yz = y * z;
            zx = z * x;
            xs = x * s;
            ys = y * s;
            zs = z * s;
            e[0] = x * x * nc + c;
            e[1] = xy * nc + zs;
            e[2] = zx * nc - ys;
            e[3] = e[7] = 0;
            e[4] = xy * nc - zs;
            e[5] = y * y * nc + c;
            e[6] = yz * nc + xs;
            e[8] = zx * nc + ys;
            e[9] = yz * nc - xs;
            e[10] = z * z * nc + c;
            e[11] = e[12] = e[13] = e[14] = 0;
            e[15] = 1;
        }
        return this;
    }

    multiply(other) {
        let i, e, a, b, ai0, ai1, ai2, ai3;
        e = this.elements;
        a = this.elements;
        b = other.elements;
        if (e === b) {
            b = new Float32Array(16);
            for (i = 0; i < 16; ++i) {
                b[i] = e[i];
            }
        }
        for (i = 0; i < 4; i++) {
            ai0 = a[i];
            ai1 = a[i + 4];
            ai2 = a[i + 8];
            ai3 = a[i + 12];
            e[i] = ai0 * b[0] + ai1 * b[1] + ai2 * b[2] + ai3 * b[3];
            e[i + 4] = ai0 * b[4] + ai1 * b[5] + ai2 * b[6] + ai3 * b[7];
            e[i + 8] = ai0 * b[8] + ai1 * b[9] + ai2 * b[10] + ai3 * b[11];
            e[i + 12] = ai0 * b[12] + ai1 * b[13] + ai2 * b[14] + ai3 * b[15];
        }
        return this;
    }

    getElements() {
        return this.elements;
    }

    getInverse() {
        let result = new Matrix4();
        let a = this.elements;
        let b = result.elements;
        let A0 = a[0] * a[5] - a[1] * a[4];
        let A1 = a[0] * a[6] - a[2] * a[4];
        let A2 = a[0] * a[7] - a[3] * a[4];
        let A3 = a[1] * a[6] - a[2] * a[5];
        let A4 = a[1] * a[7] - a[3] * a[5];
        let A5 = a[2] * a[7] - a[3] * a[6];
        let B0 = a[8] * a[13] - a[9] * a[12];
        let B1 = a[8] * a[14] - a[10] * a[12];
        let B2 = a[8] * a[15] - a[11] * a[12];
        let B3 = a[9] * a[14] - a[10] * a[13];
        let B4 = a[9] * a[15] - a[11] * a[13];
        let B5 = a[10] * a[15] - a[11] * a[14];
        let det = A0 * B5 - A1 * B4 + A2 * B3 + A3 * B2 - A4 * B1 + A5 * B0;
        if (det == 0) {
            return null;
        }
        let invDet = 1 / det;
        b[0] = (a[5] * B5 - a[6] * B4 + a[7] * B3) * invDet;
        b[1] = (-a[1] * B5 + a[2] * B4 - a[3] * B3) * invDet;
        b[2] = (a[13] * A5 - a[14] * A4 + a[15] * A3) * invDet;
        b[3] = (-a[9] * A5 + a[10] * A4 - a[11] * A3) * invDet;
        b[4] = (-a[4] * B5 + a[6] * B2 - a[7] * B1) * invDet;
        b[5] = (a[0] * B5 - a[2] * B2 + a[3] * B1) * invDet;
        b[6] = (-a[12] * A5 + a[14] * A2 - a[15] * A1) * invDet;
        b[7] = (a[8] * A5 - a[10] * A2 + a[11] * A1) * invDet;
        b[8] = (a[4] * B4 - a[5] * B2 + a[7] * B0) * invDet;
        b[9] = (-a[0] * B4 + a[1] * B2 - a[3] * B0) * invDet;
        b[10] = (a[12] * A4 - a[13] * A2 + a[15] * A0) * invDet;
        b[11] = (-a[8] * A4 + a[9] * A2 - a[11] * A0) * invDet;
        b[12] = (-a[4] * B3 + a[5] * B1 - a[6] * B0) * invDet;
        b[13] = (a[0] * B3 - a[1] * B1 + a[2] * B0) * invDet;
        b[14] = (-a[12] * A3 + a[13] * A1 - a[14] * A0) * invDet;
        b[15] = (a[8] * A3 - a[9] * A1 + a[10] * A0) * invDet;
        return result;
    }

    setPerspective(fovy, aspect, near, far) {
        let e, rd, s, ct;
        if (near === far || aspect === 0) {
            throw 'null frustum';
        }
        if (near <= 0) {
            throw 'near <= 0';
        }
        if (far <= 0) {
            throw 'far <= 0';
        }
        fovy = Math.PI * fovy / 180 / 2;
        s = Math.sin(fovy);
        if (s === 0) {
            throw 'null frustum';
        }
        rd = 1 / (far - near);
        ct = Math.cos(fovy) / s;
        e = this.elements;
        e[0] = ct / aspect;
        e[1] = 0;
        e[2] = 0;
        e[3] = 0;
        e[4] = 0;
        e[5] = ct;
        e[6] = 0;
        e[7] = 0;
        e[8] = 0;
        e[9] = 0;
        e[10] = -(far + near) * rd;
        e[11] = -1;
        e[12] = 0;
        e[13] = 0;
        e[14] = -2 * near * far * rd;
        e[15] = 0;
        return this;
    }

    setOrtho(left, right, bottom, top, near, far) {
        let e = this.elements;
        let rw = 1 / (right - left);
        let rh = 1 / (top - bottom);
        let rd = 1 / (far - near);
        e[0] = 2 * rw;
        e[1] = 0;
        e[2] = 0;
        e[3] = 0;
        e[4] = 0;
        e[5] = 2 * rh;
        e[6] = 0;
        e[7] = 0;
        e[8] = 0;
        e[9] = 0;
        e[10] = -2 * rd;
        e[11] = 0;
        e[12] = -(right + left) * rw;
        e[13] = -(top + bottom) * rh;
        e[14] = -(far + near) * rd;
        e[15] = 1;
    }

    lookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ) {
        let lookAt = new Matrix4().setLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
        return this.multiply(lookAt);
    }

    setLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ) {
        let e, fx, fy, fz, rlf, sx, sy, sz, rls, ux, uy, uz;
        fx = centerX - eyeX;
        fy = centerY - eyeY;
        fz = centerZ - eyeZ;
        rlf = 1 / Math.sqrt(fx * fx + fy * fy + fz * fz);
        fx *= rlf;
        fy *= rlf;
        fz *= rlf;
        sx = fy * upZ - fz * upY;
        sy = fz * upX - fx * upZ;
        sz = fx * upY - fy * upX;
        rls = 1 / Math.sqrt(sx * sx + sy * sy + sz * sz);
        sx *= rls;
        sy *= rls;
        sz *= rls;
        ux = sy * fz - sz * fy;
        uy = sz * fx - sx * fz;
        uz = sx * fy - sy * fx;
        e = this.elements;
        e[0] = sx;
        e[1] = ux;
        e[2] = -fx;
        e[3] = 0;
        e[4] = sy;
        e[5] = uy;
        e[6] = -fy;
        e[7] = 0;
        e[8] = sz;
        e[9] = uz;
        e[10] = -fz;
        e[11] = 0;
        e[12] = 0;
        e[13] = 0;
        e[14] = 0;
        e[15] = 1;
        return this.translate(-eyeX, -eyeY, -eyeZ);
    }

    translate(x, y, z) {
        let e = this.elements;
        e[12] += e[0] * x + e[4] * y + e[8] * z;
        e[13] += e[1] * x + e[5] * y + e[9] * z;
        e[14] += e[2] * x + e[6] * y + e[10] * z;
        e[15] += e[3] * x + e[7] * y + e[11] * z;
        return this;
    }

    scale(x, y, z) {
        let e = this.elements;
        e[0] *= x;
        e[1] *= x;
        e[2] *= x;
        e[3] *= x;
        e[4] *= y;
        e[5] *= y;
        e[6] *= y;
        e[7] *= y;
        e[8] *= z;
        e[9] *= z;
        e[10] *= z;
        e[11] *= z;
    }

    rotate(angle, x, y, z) {
        let e = this.elements;
        let radian = angle * Math.PI / 180;
        let cosB = Math.cos(radian);
        let sinB = Math.sin(radian);
        let len = Math.sqrt(x * x + y * y + z * z);
        if (len != 1) {
            let rlen = 1 / len;
            x *= rlen;
            y *= rlen;
            z *= rlen;
        }
        let nc = 1 - cosB;
        let xy = x * y;
        let yz = y * z;
        let zx = z * x;
        let xs = x * sinB;
        let ys = y * sinB;
        let zs = z * sinB;
        let f00 = x * x * nc + cosB;
        let f01 = xy * nc + zs;
        let f02 = zx * nc - ys;
        let f10 = xy * nc - zs;
        let f11 = y * y * nc + cosB;
        let f12 = yz * nc + xs;
        let f20 = zx * nc + ys;
        let f21 = yz * nc - xs;
        let f22 = z * z * nc + cosB;
        let t00 = e[0] * f00 + e[4] * f01 + e[8] * f02;
        let t01 = e[1] * f00 + e[5] * f01 + e[9] * f02;
        let t02 = e[2] * f00 + e[6] * f01 + e[10] * f02;
        let t03 = e[3] * f00 + e[7] * f01 + e[11] * f02;
        let t10 = e[0] * f10 + e[4] * f11 + e[8] * f12;
        let t11 = e[1] * f10 + e[5] * f11 + e[9] * f12;
        let t12 = e[2] * f10 + e[6] * f11 + e[10] * f12;
        let t13 = e[3] * f10 + e[7] * f11 + e[11] * f12;
        e[8] = e[0] * f20 + e[4] * f21 + e[8] * f22;
        e[9] = e[1] * f20 + e[5] * f21 + e[9] * f22;
        e[10] = e[2] * f20 + e[6] * f21 + e[10] * f22;
        e[11] = e[3] * f20 + e[7] * f21 + e[11] * f22;
        e[0] = t00;
        e[1] = t01;
        e[2] = t02;
        e[3] = t03;
        e[4] = t10;
        e[5] = t11;
        e[6] = t12;
        e[7] = t13;
    }

    setInverseOf(other) {
        let i, s, d, arr, det;
        s = other.elements;
        d = this.elements;
        arr = new Float32Array(16);
        arr[0] = s[5] * s[10] * s[15] - s[5] * s[11] * s[14] - s[9] * s[6] * s[15] + s[9] * s[7] * s[14] + s[13] * s[6] * s[11] - s[13] * s[7] * s[10];
        arr[4] = -s[4] * s[10] * s[15] + s[4] * s[11] * s[14] + s[8] * s[6] * s[15] - s[8] * s[7] * s[14] - s[12] * s[6] * s[11] + s[12] * s[7] * s[10];
        arr[8] = s[4] * s[9] * s[15] - s[4] * s[11] * s[13] - s[8] * s[5] * s[15] + s[8] * s[7] * s[13] + s[12] * s[5] * s[11] - s[12] * s[7] * s[9];
        arr[12] = -s[4] * s[9] * s[14] + s[4] * s[10] * s[13] + s[8] * s[5] * s[14] - s[8] * s[6] * s[13] - s[12] * s[5] * s[10] + s[12] * s[6] * s[9];
        arr[1] = -s[1] * s[10] * s[15] + s[1] * s[11] * s[14] + s[9] * s[2] * s[15] - s[9] * s[3] * s[14] - s[13] * s[2] * s[11] + s[13] * s[3] * s[10];
        arr[5] = s[0] * s[10] * s[15] - s[0] * s[11] * s[14] - s[8] * s[2] * s[15] + s[8] * s[3] * s[14] + s[12] * s[2] * s[11] - s[12] * s[3] * s[10];
        arr[9] = -s[0] * s[9] * s[15] + s[0] * s[11] * s[13] + s[8] * s[1] * s[15] - s[8] * s[3] * s[13] - s[12] * s[1] * s[11] + s[12] * s[3] * s[9];
        arr[13] = s[0] * s[9] * s[14] - s[0] * s[10] * s[13] - s[8] * s[1] * s[14] + s[8] * s[2] * s[13] + s[12] * s[1] * s[10] - s[12] * s[2] * s[9];
        arr[2] = s[1] * s[6] * s[15] - s[1] * s[7] * s[14] - s[5] * s[2] * s[15] + s[5] * s[3] * s[14] + s[13] * s[2] * s[7] - s[13] * s[3] * s[6];
        arr[6] = -s[0] * s[6] * s[15] + s[0] * s[7] * s[14] + s[4] * s[2] * s[15] - s[4] * s[3] * s[14] - s[12] * s[2] * s[7] + s[12] * s[3] * s[6];
        arr[10] = s[0] * s[5] * s[15] - s[0] * s[7] * s[13] - s[4] * s[1] * s[15] + s[4] * s[3] * s[13] + s[12] * s[1] * s[7] - s[12] * s[3] * s[5];
        arr[14] = -s[0] * s[5] * s[14] + s[0] * s[6] * s[13] + s[4] * s[1] * s[14] - s[4] * s[2] * s[13] - s[12] * s[1] * s[6] + s[12] * s[2] * s[5];
        arr[3] = -s[1] * s[6] * s[11] + s[1] * s[7] * s[10] + s[5] * s[2] * s[11] - s[5] * s[3] * s[10] - s[9] * s[2] * s[7] + s[9] * s[3] * s[6];
        arr[7] = s[0] * s[6] * s[11] - s[0] * s[7] * s[10] - s[4] * s[2] * s[11] + s[4] * s[3] * s[10] + s[8] * s[2] * s[7] - s[8] * s[3] * s[6];
        arr[11] = -s[0] * s[5] * s[11] + s[0] * s[7] * s[9] + s[4] * s[1] * s[11] - s[4] * s[3] * s[9] - s[8] * s[1] * s[7] + s[8] * s[3] * s[5];
        arr[15] = s[0] * s[5] * s[10] - s[0] * s[6] * s[9] - s[4] * s[1] * s[10] + s[4] * s[2] * s[9] + s[8] * s[1] * s[6] - s[8] * s[2] * s[5];
        det = s[0] * arr[0] + s[1] * arr[4] + s[2] * arr[8] + s[3] * arr[12];
        if (det == 0) {
            return false;
        }
        det = 1 / det;
        for (i = 0; i < 16; i++) {
            d[i] = arr[i] * det;
        }
        return true;
    }

    invert() {
        this.setInverseOf(this);
    }

    setTranslateRotateScale(tx, ty, tz, rx, ry, rz, sx, sy, sz) {
        let radian = rx * Math.PI / 180;
        let cosB = Math.cos(radian);
        let sinB = Math.sin(radian);
        let cosA = 1, sinA = 0;
        if (ry != 0) {
            cosA = this.elements[5];
            sinA = this.elements[6];
        }
        if (rz != 0) {
            cosA = this.elements[0];
            sinA = this.elements[1];
        }
        this.elements[0] = cosA * cosB;
        this.elements[1] = sinA * cosB;
        this.elements[2] = -sinB;
        this.elements[4] = -sinA;
        this.elements[5] = cosA;
        this.elements[6] = 0;
        this.elements[8] = cosA * sinB;
        this.elements[9] = sinA * sinB;
        this.elements[10] = cosB;
        this.elements[12] = tx;
        this.elements[13] = ty;
        this.elements[14] = tz;
        this.elements[3] = 0;
        this.elements[7] = 0;
        this.elements[11] = 0;
        this.elements[15] = 1;
        this.scale(sx, sy, sz);
    }

    setTRS(tx, ty, tz, rx, ry, rz, sx, sy, sz) {
        let radian = rx * Math.PI / 180;
        let cosB = Math.cos(radian);
        let sinB = Math.sin(radian);
        let cosA = 1, sinA = 0;
        if (ry != 0) {
            cosA = this.elements[5];
            sinA = this.elements[6];
        }
        if (rz != 0) {
            cosA = this.elements[0];
            sinA = this.elements[1];
        }
        this.elements[0] = cosA * cosB;
        this.elements[1] = sinA * cosB;
        this.elements[2] = -sinB;
        this.elements[4] = -sinA;
        this.elements[5] = cosA;
        this.elements[6] = 0;
        this.elements[8] = cosA * sinB;
        this.elements[9] = sinA * sinB;
        this.elements[10] = cosB;
        this.elements[12] = tx;
        this.elements[13] = ty;
        this.elements[14] = tz;
        this.elements[3] = 0;
        this.elements[7] = 0;
        this.elements[11] = 0;
        this.elements[15] = 1;
        this.scale(sx, sy, sz);
    }

    transpose() {
        let e, t;
        e = this.elements;
        t = e[1];
        e[1] = e[4];
        e[4] = t;
        t = e[2];
        e[2] = e[8];
        e[8] = t;
        t = e[3];
        e[3] = e[12];
        e[12] = t;
        t = e[6];
        e[6] = e[9];
        e[9] = t;
        t = e[7];
        e[7] = e[13];
        e[13] = t;
        t = e[11];
        e[11] = e[14];
        e[14] = t;
        return this;
    }
}
