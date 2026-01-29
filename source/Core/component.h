#pragma once

#include <memory>

class GameObject;

/// <summary>
/// </summary>
class Component
{
public:
	Component() = default;
	virtual ~Component() = default;
	
	/// <summary>
	/// </summary>
	virtual void Init() {};
	virtual void Uninit() {};
	virtual void Update(float deltaTime) {};
	virtual void Draw() {};

	GameObject* m_Owner = nullptr;
};
