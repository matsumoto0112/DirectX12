#include "FBXLoader.h"
#include "Framework/Utility/Debug.h"

namespace {
Framework::Math::Vector4 toVector4(const FbxVector4& pos) {
    return Framework::Math::Vector4(-pos[0], pos[1], pos[2], 1.0f);
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
        result[i] = toVector4(source[index]);
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

} //Utility 
} //Framework 
