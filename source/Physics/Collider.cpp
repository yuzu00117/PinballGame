#include "Collider.h"
#include "GameObject.h"

// ----------------------------------------------------------------------
// GameObjectに転送するイベント 
// ----------------------------------------------------------------------
void Collider::InvokeOnCollisionEnter(const CollisionInfo& info)
{
    if (m_Owner) m_Owner->OnCollisionEnter(info);
}

void Collider::InvokeOnCollisionStay(const CollisionInfo& info)
{
    if (m_Owner) m_Owner->OnCollisionStay(info);
}

void Collider::InvokeOnCollisionExit(const CollisionInfo& info)
{
    if (m_Owner) m_Owner->OnCollisionExit(info);
}

// / トリガー用イベント転送
void Collider::InvokeOnTriggerEnter(const CollisionInfo& info)
{
    if (m_Owner) m_Owner->OnTriggerEnter(info);
}

void Collider::InvokeOnTriggerStay(const CollisionInfo& info)
{
    if (m_Owner) m_Owner->OnTriggerStay(info);
}

void Collider::InvokeOnTriggerExit(const CollisionInfo& info)
{
    if (m_Owner) m_Owner->OnTriggerExit(info);
}