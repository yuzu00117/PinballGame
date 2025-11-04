#pragma once  
#include "vector3.h"

class GameObject  
{  
protected:  
    Vector3 m_Position{ 0.0f, 0.0f, 0.0f };  
    Vector3 m_Rotation{ 0.0f, 0.0f, 0.0f };  
    Vector3 m_Scale{ 1.0f, 1.0f, 1.0f };  
public:  
    virtual ~GameObject() {}
    virtual void Init() {}  
    virtual void Uninit() {}  
    virtual void Update() {}  
    virtual void Draw() {}  
};
