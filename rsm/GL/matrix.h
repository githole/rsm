#ifndef _MATRIX_H_
#define _MATRIX_H_


class Matrix {
private:
public:
	float m[16];
	
	inline void Set(const float m00, const float m01, const float m02, const float m03,
					const float m10, const float m11, const float m12, const float m13,
					const float m20, const float m21, const float m22, const float m23,
					const float m30, const float m31, const float m32, const float m33) {
		m[0] = m00; m[1] = m01; m[2] = m02; m[3] = m03;
		m[4] = m10; m[5] = m11; m[6] = m12; m[7] = m13;
		m[8] = m20; m[9] = m21; m[10] = m22; m[11] = m23;
		m[12] = m30; m[13] = m31; m[14] = m32; m[15] = m33;
	}

	Matrix() {
		Set(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	static Matrix TranslateMatrix(const float x, const float y, const float z) {
		Matrix m;
		m.Set(1.0f, 0.0f, 0.0f, 0,
			  0.0f, 1.0f, 0.0f, 0,
			  0.0f, 0.0f, 1.0f, 0,
			 x, y, z, 1.0f);
		return m;
	}

	static Matrix PerspectiveMatrix(const float fovy, const float aspect, const float znear, const float zfar) {
		const float radian = 2.0f * (3.1415926536f) * fovy/360.0f;
		const float t = (float)(1.0 / tan(radian/2));
		Matrix m;
		m.Set(t/aspect, 0, 0, 0,
			         0, t, 0, 0,
			         0, 0, (zfar + znear) / (znear - zfar), -1,
			         0, 0, (2 * zfar * znear) / (znear - zfar), 0);
		return m;
	}

	static Matrix OrthoMatrix(const float left, const float right, const float bottom, const float top, const float znear, const float zfar) {
		const float tx = -(right + left) / (right - left);
		const float ty = -(top + bottom) / (top - bottom);
		const float tz = -(zfar + znear) / (zfar - znear);
		Matrix m;
		m.Set(2.0f / (right - left), 0, 0, 0,
			  0, 2.0f / (top - bottom), 0, 0,
			  0, 0, -2.0f / (zfar - znear), 0,
			  tx, ty, tz, 1.0f);
		return m;
	}

private:
	static void cross(const float a[3], const float b[3], float out[3]) {
		out[0] = a[1] * b[2] - a[2] * b[1];
		out[1] = a[2] * b[0] - a[0] * b[2];
		out[2] = a[0] * b[1] - a[1] * b[0];
	}
	static void normalize(float* v){
		float m = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
		m = 1.0f / m;
		v[0] *= m;
		v[1] *= m;
		v[2] *= m;
	}

	static void multiplication(const GLfloat src1[16], const GLfloat src2[16], GLfloat dst[16])
	{
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				dst[y*4 + x] = src2[y*4] * src1[x] + 
							   src2[y*4 + 1] * src1[x + 4] + 
							   src2[y*4 + 2] * src1[x + 8] + 
							   src2[y*4 + 3] * src1[x + 12];
			}
		}
	}

public:
	static void transpose(const GLfloat m[16], GLfloat t[16]) {
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				t[y*4 + x] = m[x * 4 + y];
			}
		}
	}

	// http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
	static bool inverse(const GLfloat m[16], GLfloat invOut[16])
	{
		GLfloat inv[16], det;
		int i;

		inv[0] = m[5]  * m[10] * m[15] - 
					m[5]  * m[11] * m[14] - 
					m[9]  * m[6]  * m[15] + 
					m[9]  * m[7]  * m[14] +
					m[13] * m[6]  * m[11] - 
					m[13] * m[7]  * m[10];

		inv[4] = -m[4]  * m[10] * m[15] + 
					m[4]  * m[11] * m[14] + 
					m[8]  * m[6]  * m[15] - 
					m[8]  * m[7]  * m[14] - 
					m[12] * m[6]  * m[11] + 
					m[12] * m[7]  * m[10];

		inv[8] = m[4]  * m[9] * m[15] - 
					m[4]  * m[11] * m[13] - 
					m[8]  * m[5] * m[15] + 
					m[8]  * m[7] * m[13] + 
					m[12] * m[5] * m[11] - 
					m[12] * m[7] * m[9];

		inv[12] = -m[4]  * m[9] * m[14] + 
					m[4]  * m[10] * m[13] +
					m[8]  * m[5] * m[14] - 
					m[8]  * m[6] * m[13] - 
					m[12] * m[5] * m[10] + 
					m[12] * m[6] * m[9];

		inv[1] = -m[1]  * m[10] * m[15] + 
					m[1]  * m[11] * m[14] + 
					m[9]  * m[2] * m[15] - 
					m[9]  * m[3] * m[14] - 
					m[13] * m[2] * m[11] + 
					m[13] * m[3] * m[10];

		inv[5] = m[0]  * m[10] * m[15] - 
					m[0]  * m[11] * m[14] - 
					m[8]  * m[2] * m[15] + 
					m[8]  * m[3] * m[14] + 
					m[12] * m[2] * m[11] - 
					m[12] * m[3] * m[10];

		inv[9] = -m[0]  * m[9] * m[15] + 
					m[0]  * m[11] * m[13] + 
					m[8]  * m[1] * m[15] - 
					m[8]  * m[3] * m[13] - 
					m[12] * m[1] * m[11] + 
					m[12] * m[3] * m[9];

		inv[13] = m[0]  * m[9] * m[14] - 
					m[0]  * m[10] * m[13] - 
					m[8]  * m[1] * m[14] + 
					m[8]  * m[2] * m[13] + 
					m[12] * m[1] * m[10] - 
					m[12] * m[2] * m[9];

		inv[2] = m[1]  * m[6] * m[15] - 
					m[1]  * m[7] * m[14] - 
					m[5]  * m[2] * m[15] + 
					m[5]  * m[3] * m[14] + 
					m[13] * m[2] * m[7] - 
					m[13] * m[3] * m[6];

		inv[6] = -m[0]  * m[6] * m[15] + 
					m[0]  * m[7] * m[14] + 
					m[4]  * m[2] * m[15] - 
					m[4]  * m[3] * m[14] - 
					m[12] * m[2] * m[7] + 
					m[12] * m[3] * m[6];

		inv[10] = m[0]  * m[5] * m[15] - 
					m[0]  * m[7] * m[13] - 
					m[4]  * m[1] * m[15] + 
					m[4]  * m[3] * m[13] + 
					m[12] * m[1] * m[7] - 
					m[12] * m[3] * m[5];

		inv[14] = -m[0]  * m[5] * m[14] + 
					m[0]  * m[6] * m[13] + 
					m[4]  * m[1] * m[14] - 
					m[4]  * m[2] * m[13] - 
					m[12] * m[1] * m[6] + 
					m[12] * m[2] * m[5];

		inv[3] = -m[1] * m[6] * m[11] + 
					m[1] * m[7] * m[10] + 
					m[5] * m[2] * m[11] - 
					m[5] * m[3] * m[10] - 
					m[9] * m[2] * m[7] + 
					m[9] * m[3] * m[6];

		inv[7] = m[0] * m[6] * m[11] - 
					m[0] * m[7] * m[10] - 
					m[4] * m[2] * m[11] + 
					m[4] * m[3] * m[10] + 
					m[8] * m[2] * m[7] - 
					m[8] * m[3] * m[6];

		inv[11] = -m[0] * m[5] * m[11] + 
					m[0] * m[7] * m[9] + 
					m[4] * m[1] * m[11] - 
					m[4] * m[3] * m[9] - 
					m[8] * m[1] * m[7] + 
					m[8] * m[3] * m[5];

		inv[15] = m[0] * m[5] * m[10] - 
					m[0] * m[6] * m[9] - 
					m[4] * m[1] * m[10] + 
					m[4] * m[2] * m[9] + 
					m[8] * m[1] * m[6] - 
					m[8] * m[2] * m[5];

		det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

		if (det == 0)
			return false;

		det = 1.0 / det;

		for (i = 0; i < 16; i++)
			invOut[i] = inv[i] * det;

		return true;
	}
	static Matrix LookAt(const float eyex, const float eyey, const float eyez, const float centerx, const float centery, const float centerz, const float upx, const float upy, const float upz) {
		float f[3] = {centerx - eyex, centery - eyey, centerz - eyez};
		normalize(f);
		float UP[3] = {upx, upy, upz};
		normalize(UP);
		float s[3];
		cross(f, UP, s);
		float u[3];
		normalize(s);
		cross(s, f, u);

		Matrix m1;
		m1.Set(s[0], u[0], -f[0], 0,
			  s[1], u[1], -f[1], 0,
			  s[2], u[2], -f[2], 0,
			  0, 0, 0, 1);

		Matrix m2;
		m2.Set( 1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				-eyex,-eyey,-eyez,1);

		Matrix m3;
		multiplication(m1.m, m2.m, m3.m);
		return m3;
	}


};

#endif