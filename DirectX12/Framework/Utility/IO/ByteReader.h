#pragma once

#include <Windows.h>
#include <vector>
#include <string>

namespace Framework {
namespace Utility {

/**
* @class ByteReader
* @brief ファイルをバイトデータで読み込む
*/
class ByteReader {
public:
    /**
    * @brief コンストラクタ
    * @param filepath ファイルパス
    */
    ByteReader(_In_ const std::string& filepath);
    /**
    * @brief デストラクタ
    */
    ~ByteReader();
    /**
    * @brief データを取得する
    * @return 読み込んだデータを返す
    */
    std::vector<BYTE> get();
private:
    std::vector<BYTE> mData; //!< 読み込んだデータ
};

} //Utility 
} //Framework
