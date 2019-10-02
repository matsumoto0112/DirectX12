#include "ByteReader.h"
#include <cstdio>
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Utility {

ByteReader::ByteReader(_In_ const std::string& filepath) {
    FILE* fp = nullptr;
    //�o�C�i���f�[�^�Ńt�@�C�����J��
    const int ret = fopen_s(&fp, filepath.c_str(), "rb");
    MY_ERROR_WINDOW(ret == 0, filepath + "���J���܂���ł����B");
    fseek(fp, 0, SEEK_END);
    //�z��̃T�C�Y���f�[�^�̑傫���ɂ���
    mData = std::vector<BYTE>(ftell(fp));
    fseek(fp, 0, SEEK_SET);
    fread(mData.data(), mData.size(), 1, fp);
    fclose(fp);
}

ByteReader::~ByteReader() { }

std::vector<BYTE> ByteReader::get() {
    return mData;
}

} //Utility 
} //Framework
