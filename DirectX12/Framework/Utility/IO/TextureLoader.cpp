#include "TextureLoader.h"
#include <vector>
#include <atlstr.h>
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Utility {

TextureLoader::TextureLoader() {
    //イメージファクトリ作成
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
        nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&mFactory));
    MY_ASSERTION(SUCCEEDED(hr), "イメージファクトリ作成失敗");

    //フォーマット変換器作成
    hr = mFactory->CreateFormatConverter(&mConverter);
    MY_ASSERTION(SUCCEEDED(hr), "FormatConverter作成失敗");
}

TextureLoader::~TextureLoader() { }

std::vector<BYTE> TextureLoader::load(const std::string& filepath, UINT* width, UINT* height) {
    //マルチバイト文字に変換
    CStringW wPath(filepath.c_str());
    //パスからデコーダーを作成
    HRESULT hr = mFactory->CreateDecoderFromFilename(
        wPath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &mDecoder);
    MY_ASSERTION(SUCCEEDED(hr), "デコーダー作成失敗\nファイル名は" + filepath);

    //フレームの取得
    hr = mDecoder->GetFrame(0, &mFrame);
    MY_ASSERTION(SUCCEEDED(hr), "GetFrame失敗");

    UINT w, h;
    mFrame->GetSize(&w, &h);
    //ピクセル形式を取得
    WICPixelFormatGUID pixelFormat;
    hr = mFrame->GetPixelFormat(&pixelFormat);
    MY_ASSERTION(SUCCEEDED(hr), "GetPixelFrame失敗");

    int stride = w * 4;
    int bufferSize = stride * h;
    std::vector<BYTE> buffer(bufferSize);
    //ピクセル形式が32bitRGBAでなかったら変換する
    if (pixelFormat != GUID_WICPixelFormat32bppRGBA) {
        hr = mFactory->CreateFormatConverter(&mConverter);
        MY_ASSERTION(SUCCEEDED(hr), "FormatConverter作成失敗");

        hr = mConverter->Initialize(mFrame, GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeErrorDiffusion, nullptr, 0, WICBitmapPaletteTypeCustom);
        MY_ASSERTION(SUCCEEDED(hr), "Converterの初期化失敗");

        hr = mConverter->CopyPixels(0, stride, bufferSize, buffer.data());
        MY_ASSERTION(SUCCEEDED(hr), "ピクセルデータのコピー失敗");
    }
    else {
        hr = mFrame->CopyPixels(nullptr, stride, bufferSize, buffer.data());
        MY_ASSERTION(SUCCEEDED(hr), "ピクセルデータのコピー失敗");
    }

    *width = w;
    *height = h;
    return buffer;
}
} //Utility 
} //Framework 
