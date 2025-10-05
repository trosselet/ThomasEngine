#include "pch.h"
#include "Header/ScriptSystem.h"
#include "Header/IScript.h"

ScriptSystem::~ScriptSystem()
{
	for (uint16 i = 0; i < m_scriptsByEntity.size(); i++)
	{
		for (uint16 j = 0; j < m_scriptsByEntity[i].size(); j++)
		{
			if (m_scriptsByEntity[i][j])
			{
				delete m_scriptsByEntity[i][j];
				m_scriptsByEntity[i][j] = nullptr;
			}
		}
	}

	m_scriptsByEntity.clear();
}

void ScriptSystem::OnUpdate()
{
	for (auto& [entityID, scripts] : m_scriptsByEntity)
	{
		for (IScript* script : scripts)
		{
			script->OnUpdate();
		}
	}
}

void ScriptSystem::OnFixedUpdate()
{
	for (auto& [entityID, scripts] : m_scriptsByEntity)
	{
		for (IScript* script : scripts)
		{
			script->OnFixedUpdate();
		}
	}
}