// Overlay 8, part 2: the sheep model (runs on the spot, hides joints when touched). Names are ours.
#include "ov008_sheep.h"
#include "input.h"

static int sCallbackCount;

static const NNSG3dResName sNodeNames[] = {
    {"Head"}, {"Head_End"}, {"head_null"}, {"WOOL_4"}, {"WOOL_5"}, {"WOOL_6"}, {"WOOL_7"},
    {"WOOL_8"}, {"WOOL_9"}, {"WOOL_10"}, {"WOOL_11"}, {"WOOL_12"}, {"WOOL_13"}, {"WOOL_14"},
    {"WOOL_15"}, {"WOOL_16"}, {"WOOL_17"}, {"WOOL_18"}, {"WOOL_19"}, {"WOOL_20"}, {"WOOL_21"},
    {"WOOL_22"}, {"WOOL_23"}, {"WOOL_24"}, {"WOOL_25"},
};

void SheepModel::NodeCallback(NNSG3dRS *rs)
{
    int i;

    for (i = 1; i < 3; i++) {
        int idx = NNS_G3dGetNodeIdxByName(NNS_G3dGetNodeInfo(rs->pRenderObj->resMdl), &sNodeNames[i]);

        if (idx >= 0 && idx == NNS_G3dRSGetCurrentNodeDescID(rs)) {
            MtxFx43 mtx;

            (void)NNS_G3dGetCurrentMtx(&mtx, NULL);
            rs->pVisAnmResult->isVisible = FALSE;
        }
    }
}

SheepModel::SheepModel()
{
    mModel = Model3D_LoadAnimated("rom/mdls/SHEEP_wool_model.nsbmd", "rom/mdls/SHEEP_wool_run.nsbca");
    MTX_RotY33(&mModel->rot, -FX32_ONE, 0);
    sCallbackCount = 0;
}

SheepModel::~SheepModel()
{
}

BOOL SheepModel::Update()
{
    u32 i;

    for (i = 0; i < mModel->numAnims; i++) {
        if (mModel->anims[i] != NULL) {
            NNSG3dAnmObj *anm;

            mModel->anims[i]->anmObj->frame += FX32_CONST(4);
            anm = mModel->anims[i]->anmObj;
            if (anm->frame >= NNS_G3dAnmObjGetNumFrame(anm)) {
                anm->frame = 0;
            }
        }
    }
    if ((gInput.touchFlags & INPUT_TOUCH_PRESSED) && sCallbackCount < 25) {
        NNS_G3dRenderObjSetCallBack(&mModel->renderObj, NodeCallback, NULL, NNS_G3D_SBC_NODEDESC,
                                    NNS_G3D_SBC_CALLBACK_TIMING_B);
        sCallbackCount++;
    }
    return TRUE;
}

void SheepModel::Draw()
{
    gRenderer3D->Draw(mModel);
}
