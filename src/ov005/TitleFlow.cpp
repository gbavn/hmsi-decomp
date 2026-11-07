// Overlay 5, part 7: switches between the title overlay's screens (splash, title, ...).
#include "ov005_title.h"

enum {
    SCENE_SPLASH = 0x40,
    SCENE_TITLE = 0x42,
    SCENE_4D = 0x4D,
    SCENE_4E = 0x4E,
    SCENE_NONE_4F = 0x4F,
};

enum {
    FLOW_KILL,
    FLOW_WAIT_KILL,
    FLOW_CREATE,
    FLOW_STARTED,
    FLOW_IDLE,
};

// sFlow: defined in TitleFlowData.cpp (layout workaround, see there).
#define sFlow sTitleFlow

TitleFlow::TitleFlow(TitleMain *main)
{
    mMain = main;
    sFlow.task = NULL;
    sFlow.state = FLOW_IDLE;
    sFlow.current = SCENE_SPLASH;
}

TitleFlow::~TitleFlow()
{
}

BOOL TitleFlow::Update()
{
    if (!func_020374f8(1) && !func_020374f8(0)) {
    switch (sFlow.state) {
    case FLOW_KILL:
        KillCurrent();
        sFlow.state = FLOW_WAIT_KILL;
        break;
    case FLOW_WAIT_KILL:
        sFlow.state = FLOW_CREATE;
        if (sFlow.next == SCENE_NONE_4F) {
            sFlow.current = sFlow.next;
            sFlow.next = 0xFF;
            sFlow.state = FLOW_IDLE;
        }
        break;
    case FLOW_CREATE:
        CreateScene();
        if (sFlow.fade) {
            func_02037284(2, 0, 1);
            func_02037284(2, 0, 0);
        }
        sFlow.current = sFlow.next;
        sFlow.next = 0xFF;
        sFlow.state = FLOW_STARTED;
        break;
    case FLOW_STARTED:
        sFlow.state = FLOW_IDLE;
        break;
    case FLOW_IDLE:
        if (sFlow.current == SCENE_NONE_4F) {
            sFlow.current = 0xFF;
            sFlow.next = 0xFF;
        }
        break;
    }
    }
    return TRUE;
}

BOOL TitleFlow::ChangeScene(u8 next, u8 fade)
{
    if (sFlow.state != FLOW_IDLE) {
        return FALSE;
    }
    sFlow.next = next;
    sFlow.fade = fade;
    if (fade) {
        func_02037284(7, 0, 1);
        func_02037284(7, 0, 0);
    }
    sFlow.state = FLOW_KILL;
    return TRUE;
}

BOOL TitleFlow::SetNext(u8 next)
{
    if (sFlow.state != FLOW_IDLE) {
        return FALSE;
    }
    sFlow.next = next;
    sFlow.state = FLOW_KILL;
    return TRUE;
}

void TitleFlow::KillCurrent()
{
    if (sFlow.task != NULL) {
        sFlow.task->mFlag = 1;
    }
}

void TitleFlow::CreateScene()
{
    switch (sFlow.next) {
    case 0x3F:
        break;
    case SCENE_SPLASH:
        sFlow.task = new (sTitleManagers[0]) SplashScreen(mMain);
        break;
    case SCENE_4D:
        sFlow.task = new (sTitleManagers[7]) TitleSubScene4D(mMain);
        break;
    case SCENE_4E:
        sFlow.task = new (sTitleManagers[8]) TitleSubScene4E;
        break;
    case 0x41:
        break;
    case SCENE_TITLE:
        sFlow.task = new (sTitleManagers[2]) TitleScene(mMain);
        break;
    }
}
