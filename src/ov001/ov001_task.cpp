// Overlay 1: a single task that immediately requests scene 1. Names are ours.
#include "Task.h"

class Ov001Task : public Task {
public:
    Ov001Task();
    virtual ~Ov001Task();
    virtual BOOL Update();
};

Ov001Task::Ov001Task()
{
}

Ov001Task::~Ov001Task()
{
}

BOOL Ov001Task::Update()
{
    gSceneControl->Request(1);
    return FALSE;
}
