#pragma once

#include "EnemyBase.h"

/// <summary>
/// EnemyStraightクラス
/// ホールに向かってそのまま直進する敵
/// </summary>
class EnemyStraight : public EnemyBase
{
public:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;

    // その他関数、変数などは親クラスのまま
};
