#include "Needs.hpp"

#include <cmath>

namespace gv
{
ResourceDictionary<NeedDef> g_NeedDefDictionary;

bool NeedLevelTrigger::ConditionsMet(Need& need) const
{
	return (
	    (Condition == NeedLevelTrigger::ConditionType::Zero && fabs(need.Level) < 0.01f) ||
	    (Condition == NeedLevelTrigger::ConditionType::GreaterThanLevel && need.Level > Level) ||
	    (Condition == NeedLevelTrigger::ConditionType::LessThanLevel && need.Level < Level));
}

Need::Need(ResourceKey needDefKey)
{
	Def = g_NeedDefDictionary.GetResource(needDefKey);
	if (Def)
	{
		Level = Def->InitialLevel;
	}
}
}