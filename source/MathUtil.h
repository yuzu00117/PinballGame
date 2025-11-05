#ifndef _MATHUTIL_H
#define _MATHUTIL_H

#include <algorithm>

/// <summary>
/// 数学ユーティリティクラス
/// </summary>
template<typename T>

/// <summary>
/// 値を指定範囲にクランプする
/// </summary>
/// <typeparam name="T">型</typeparam>
/// <param name="value">値</param>
/// <param name="minVal">最小値</param>
/// <param name="maxVal">最大値</param>
/// <returns>クランプ後の値</returns>
inline T Clamp(T value, T minVal, T maxVal)
{
	return std::min(std::max(value, minVal), maxVal);
}

#endif //_MATHUTIL_H
