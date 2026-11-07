// Overlay 5, part 10: the villagers and animals standing on the title screen's 3D stage.
#include <string.h>
#include <stdio.h>
#include "ov005_title.h"

// The island's people (object at 0x020f57c8; names are ours).
class Residents {
public:
    BOOL IsResident(int person);   // 0x0203f1f0
    int GetPicData(int person);    // 0x0203f488
};
extern Residents *gResidents;

extern "C" {
BOOL func_0204ed80(int person);
char *func_0206fa64(int person);   // the person's file name (new[])
int func_0206faf4(int person);
extern const char *data_020c0a80[];   // animal file names
extern const int data_020b48b0[];
}

TitleChar::TitleChar(int person, Sprite2D mark)
{
    char *name;

    mPerson = person;
    mAnimal = -1;
    mMark = NULL;
    char path[256] = {0};
    if (gResidents->IsResident(person)) {
        strcpy(path, "rom/pics/char/inhabitant/");
    } else if (!func_0204ed80(person)) {
        strcpy(path, "rom/pics/char/category/");
    } else {
        strcpy(path, "rom/pics/char/inhabitant/");
    }
    name = func_0206fa64(person);
    sprintf(path, "%s%s.xbb", path, name);
    delete[] name;
    {
        Residents *res = gResidents;
        int a = func_0206faf4(person);
        Load(path, a, res->GetPicData(person), 0, 0);
    }
    mMark = new Sprite2D(mark);
    InitMark();
}

TitleChar::TitleChar(u32 animal, Sprite2D mark)
{
    mPerson = 0xFF;
    mAnimal = animal;
    mMark = NULL;
    char path[256] = {0};
    strcpy(path, "rom/pics/char/animal/");
    strcat(path, data_020c0a80[animal]);
    strcat(path, ".xbb");
    Load(path, data_020b48b0[animal], 0, 0, 0);
    if (animal != 20) {
        mMark = new Sprite2D(mark);
    }
    InitMark();
}

TitleChar::~TitleChar()
{
    func_0202ba04(mMark, 0);
    mMark = NULL;
}

BOOL TitleChar::Update()
{
    func_0202b1a4(mSprite, mX, mY, mZ);
    UpdateAnim();
    if (mMark == NULL) {
        return TRUE;
    }
    if (mAnimal != -1) {
        func_0202b1a4(mMark, mX, mY, mZ);
    } else {
        func_0202b1a4(mMark, mX, mY, mZ - FX32_ONE);
    }
    func_0202bc20(data_020ec87c, mMark);
    return TRUE;
}

void TitleChar::Func3()
{
}

void TitleChar::Func4()
{
}

void TitleChar::InitMark()
{
    Sprite2D *mark;

    if (mMark == NULL) {
        return;
    }
    mMark->cellAnim = new NNSG2dCellAnimation;
    mark = mMark;
    NNS_G2dInitCellAnimation(mMark->cellAnim, NNS_G2dGetAnimSequenceByIdx(mark->animBank, 0), mark->cellBank);
    if (mAnimal != -1) {
        func_0202b064(mMark, 0x20);
    } else if (mPerson == 0x22) {
        func_0202b064(mMark, 0x1F);
    } else {
        func_0202b064(mMark, 0x20);
    }
    mMark->alpha = 16;
}
