#include <Engine/pch.h>
#include <Engine/Header/Component.h>

#include <Engine/Header/GameObject.h>

void Component::Destroy()
{
    if (m_toBeDeleted)
    {
        return;
    }

    m_toBeDeleted = true;
}

bool Component::IsActive() const
{
    return m_active&& m_pOwner->IsActive();
}

void Component::Deactivate()
{
    m_active = false;
}

void Component::Activate()
{
    m_active = true;
}

void Component::SetActive(bool activate)
{
    m_active = activate;
}

GameObject const& Component::GetOwner() const
{
    return *m_pOwner;
}

GameObject& Component::GetOwner()
{
    return *m_pOwner;
}