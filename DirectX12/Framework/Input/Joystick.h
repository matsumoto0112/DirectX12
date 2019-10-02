#pragma once

#include "Framework/Input/ID.h"
#include "Framework/Input/ButtonID.h"
#pragma comment(lib,"WinMM.lib")

namespace Framework {
namespace Input {

class Joystick
{
public:
    /**
    * @brief コンストラクタ
    * @param id ジョイスティックID
    */
    Joystick(ID id = ID::Joystick1);
    /**
    * @brief デストラクタ
    */
    ~Joystick();
    /**
    * @brief コントローラ情報更新
    */
    void update();
    /**
    * @brief 利用可能か?
    */
    bool isEnabled() const;
    /**
    * @brief ボタンを押しているか
    * @param button 判定するボタン
    * @return 今updateで押していたらtrue
    押されていない、コントローラが存在しない場合はfalse
    */
    bool getButton(ButtonID button) const;
    /**
    * @brief ボタンを押したか（トリガー）
    * @param button 判定するボタン
    * @return 前updateで押されていないかつ今updateで押されていたらtrue
    それ以外の場合、コントローラが存在しない場合はfalse
    */
    bool getButtonDown(ButtonID button) const;
    /**
    * @brief ボタンを離したか（トリガー）
    * @param button 判定するボタン
    * @return 前updateで押されていてかつ今updateで離されたらtrue
    それ以外の場合、コントローラが存在しない場合はfalse
    */
    bool getButtonUp(ButtonID button) const;
    /**
    * @brief 軸の入力値を取得
    * @axis 判定する軸
    * @details 軸の入力具合を（－１～１）までで返す。
    トリガーの場合　左トリガー：０～１、右トリガー０～－１の値を返す
    */
    float getAxis(AxisID axis) const;
private:
    const UINT mID; //!< ジョイスティックID
    bool mCanGetJoyInfo; //!< ジョイスティック情報を取得で来ていたか
    JOYINFOEX mJoyInfo; //!< ジョイスティック情報を格納する構造体
    JOYINFOEX mPrevJoyInfo; //!< 前updateでのボタンの押下情報
};

} //Input 
} //Framework
