#pragma once

#include "../../schema.h"
#include "mathlib/vector.h"
#include "baseentity.h"
#include "ehandle.h"

class CBaseEntity;

class CBaseEntity2 : public CBaseEntity
{
public:
  typedef CBaseEntity2 ThisClass;
  static constexpr const char *ThisClassName = "CBaseEntity";
  static constexpr bool IsStruct = false;

  SCHEMA_FIELD(CBitVec<64>, m_isSteadyState)
  SCHEMA_FIELD(float, m_lastNetworkChange)
  SCHEMA_FIELD_POINTER(void, m_NetworkTransmitComponent)

  CHandle<CBaseEntity> GetHandle() { return m_pEntity->m_EHandle; }
};