#ifndef _MATHUTIL_H
#define _MATHUTIL_H

#include <algorithm>

/// <summary>
/// ���w���[�e�B���e�B�N���X
/// </summary>
template<typename T>

/// <summary>
/// �l��w��͈͂ɃN�����v����
/// </summary>
/// <typeparam name="T">�^</typeparam>
/// <param name="value">�l</param>
/// <param name="minVal">�ŏ��l</param>
/// <param name="maxVal">�ő�l</param>
/// <returns>�N�����v��̒l</returns>
inline T Clamp(T value, T minVal, T maxVal)
{
	if (value < minVal) return minVal;
	if (value > maxVal) return maxVal;
	return value;
}

#endif //_MATHUTIL_H
