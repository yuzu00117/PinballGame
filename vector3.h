#ifndef _VECTOR3_H
#define _VECTOR3_H

#include<math.h>

class Vector3
{
public:
//パブリックな表現
	float x, y, z;

//コンストラクタ
	Vector3(){}

//コピコン
	Vector3(const Vector3& a) :x(a.x), y(a.y), z(a.z) {}

//3つの値で作成
	Vector3(float nx,float ny,float nz):x(nx), y(ny), z(nz){}

//標準的なオブジェクトの保守
	//代入
	Vector3& operator = (const Vector3& a)
	{
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	}

	//等しさのチェック
	bool operator == (const Vector3& a)const
	{
		return x == a.x && y == a.y && z == a.z;
	}

//ベクトル操作
	//ベクトルを0に設定する
	void zero()
	{
		x = y = z = 0;
	}

	//単項式のマイナスは反転したベクトルを返す
	Vector3 operator -() const
	{
		return Vector3(-x, -y, -z);
	}

	//二公式の+と-はベクトルを加算し、減算する

	Vector3 operator +(const Vector3& a)const
	{
		return Vector3(x + a.x, y + a.y, z + a.z);
	}

	Vector3 operator -(const Vector3& a)const
	{
		return Vector3(x - a.x, y - a.y, z - a.z);
	}

	//スカラーによる乗算と除算

	Vector3 operator *(float a)const
	{
		return Vector3(x * a, y * a, z * a);
	}

	Vector3 operator /(float a)const
	{
		float oneOverA = 1.0f / a;
		if (a <= 0)
		{
			a = 1.0f;
		}

		return Vector3(x * oneOverA, y * oneOverA, z * oneOverA);
	}

	//Cの表記法に準拠するための組み合わせ代入演算
	Vector3& operator +=(const Vector3& a)
	{
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}

	Vector3& operator -=(const Vector3& a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}

	Vector3& operator *=(float a)
	{
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}

	Vector3& operator /=(float a)
	{
		float OneOverA = 1.0f / a;
		if (a <= 0)
		{
			a = 1.0f;
		}

		x *= a;
		y *= a;
		z *= a;
		return *this;
	}

//ベクトルを正規化する
	void nomalize()
	{
		float magSq = x * x + y * y + z * z;
		if (magSq > 0.0f)
		{
			float OneOverMag = 1.0f / sqrtf(magSq);
			x *= OneOverMag;
			y *= OneOverMag;
			z *= OneOverMag;
		}
	}

	// ベクトルの長さを返す
	float Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	/*
//ベクトルの内積
	//標準乗算記号をこれにオーバーロードする

	float operator *(const Vector3& a)const
	{
		return x * a.x + y * a.y + z * a.z;
	}
	*/

};

/*
////////////////////////////////////////////////////////////
//
//	非メンバ関数
// 
////////////////////////////////////////////////////////////

//ベクトルの大きさを計算する
inline float VectorMag(const Vector3& a)
{
	return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

//2つのベクトルの大きさを計算する
inline Vector3 CrossProduct(const Vector3& a, const Vector3& b)
{
	return Vector3
	(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

//対称性のため、左からスカラーを乗算する
inline Vector3 operator *(float k, const Vector3& v)
{
	return Vector3(k * v.x, k * v.y, k * v.z);
}

//2つの点距離を計算する
inline float Distance(const Vector3& a, const Vector3& b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;

	return sqrtf(dx * dx + dy * dy + dz * dz);
}

/////////////////////////////////////////////////////////////
// 
//	グローバル変数
// 
/////////////////////////////////////////////////////////////

//グローバルなゼロベクトル定数を提供する

extern const Vector3 kZeroVector;

*/

#endif //_VECTOR3_H