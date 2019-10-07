#include "FBXLoader.h"
#include "Framework/Utility/Debug.h"

namespace {
static const Framework::Math::Vector2 DEFAULT_UV = Framework::Math::Vector2::ZERO;

Framework::Math::Vector4 toPositionVector4(const FbxVector4& pos) {
    //X�������]���Ă�����ۂ�
    return Framework::Math::Vector4(-pos[0], pos[1], pos[2], 1.0f);
}

Framework::Math::Vector2 toUVVector2(const FbxVector2& uv) {
    Framework::Math::Vector2 res;
    res.x = (float)uv[0];
    res.y = 1.0f - (float)uv[1];
    return res;
}

//�ǂݍ��񂾒��_�𐳂����R���g���[�����邽�߂̍s��̍쐬
FbxAMatrix createPositionOffsetMatrix(FbxMesh* mesh) {
    //�������̂��������͂��������܂��`�悳��Ȃ��Ȃ�
    //FbxVector4 T = mesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
    //FbxVector4 R = mesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
    //FbxVector4 S = mesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
    //FbxAMatrix TRS = FbxAMatrix(T, R, S);

    //�����p�������̂܂܎g�p����ƂȂ������܂�����
    FbxTime time(0);
    FbxAMatrix mat = mesh->GetNode()->EvaluateGlobalTransform(time);
    return mat;
}

std::vector<Framework::Math::Vector4> positions(FbxMesh* mesh) {
    FbxVector4* source = mesh->GetControlPoints();
    const int cpCount = mesh->GetControlPointsCount();
    FbxAMatrix mat = createPositionOffsetMatrix(mesh);
    for (int i = 0; i < cpCount; i++) {
        source[i] = mat.MultT(source[i]);
    }
    int c = mesh->GetPolygonCount();
    //�|���S�����_�����[�v���A�e���_���i�[���Ă���
    const int size = mesh->GetPolygonVertexCount();
    int* polygonVertexIndex = mesh->GetPolygonVertices();
    std::vector<Framework::Math::Vector4> result(size);
    for (int i = 0; i < size; i++) {
        int index = polygonVertexIndex[i];
        result[i] = toPositionVector4(source[index]);
    }
    return result;
}

std::vector<Framework::Math::Vector2> uvs(FbxMesh* mesh) {
    FbxStringList uvNames;
    mesh->GetUVSetNames(uvNames);
    const int size = mesh->GetPolygonVertexCount();
    std::vector<Framework::Math::Vector2> result(size);
    //UV�����݂��Ȃ���΃f�t�H���g�̒l���g�p����
    if (uvNames.GetCount() == 0) {
        for (int i = 0; i < size; i++) {
            result[i] = DEFAULT_UV;
        }
    }
    else {
        FbxArray<FbxVector2> uvs;
        //UV�̓Z�b�g�̐擪�̂��̂̂ݎg�p����
        mesh->GetPolygonVertexUVs(uvNames[0], uvs);
        for (int i = 0; i < size; i++) {
            const FbxVector2& uv = uvs[i];
            result[i] = toUVVector2(uv);
        }
    }
    return result;
}
}

namespace Framework {
namespace Utility {

FBXLoader::FBXLoader(const std::string& filepath, bool triangulate) {
    mManager = FbxManager::Create();
    MY_ERROR_WINDOW(mManager, "FBX�������Ɏ��s���܂���");
    FbxIOSettings* ios = FbxIOSettings::Create(mManager, IOSROOT);
    mManager->SetIOSettings(ios);

    load(filepath, triangulate);
}

FBXLoader::~FBXLoader() {
    mManager->Destroy();
}

void FBXLoader::load(const std::string& filepath, bool triangulate) {
    if (mScene)mScene->Destroy();
    mScene = FbxScene::Create(mManager, "");
    //�V�[���̓ǂݍ���
    FbxImporter* importer = FbxImporter::Create(mManager, "");
    if (!importer->Initialize(filepath.c_str(), -1, mManager->GetIOSettings())) {
        MY_ERROR_WINDOW(false, "���f���f�[�^�̓ǂݍ��݂Ɏ��s���܂����B\n" + filepath);
        return;
    }

    importer->Import(mScene);
    importer->Destroy();

    FbxGeometryConverter converter(mManager);
    if (triangulate) {
        converter.Triangulate(mScene, true);
    }
    //���b�V�����ƂɃ}�e���A���𕪊�����
    converter.SplitMeshesPerMaterial(mScene, true);
}

std::vector<Math::Vector4> FBXLoader::getPosition() const {
    const int meshNum = mScene->GetMemberCount<FbxMesh>();
    std::vector<Math::Vector4> result;
    for (int i = 0; i < meshNum; i++) {
        std::vector<Math::Vector4> pos = positions(mScene->GetMember<FbxMesh>(i));
        result.insert(result.end(), pos.begin(), pos.end());
    }

    return result;
}

bool FBXLoader::hasUV() const {
    return false;
}

std::vector<Math::Vector2> FBXLoader::getUV() const {
    const int meshNum = mScene->GetMemberCount<FbxMesh>();
    std::vector<Math::Vector2> result;

    for (int i = 0; i < meshNum; i++) {
        std::vector<Math::Vector2> uv = uvs(mScene->GetMember<FbxMesh>(i));
        result.insert(result.end(), uv.begin(), uv.end());
    }

    return result;

}

} //Utility 
} //Framework 
