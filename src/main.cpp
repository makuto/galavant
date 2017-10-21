// Galavant
//  This is a pseudotarget; that means the bin isn't what we care about,
//   it's the libs we linked to, which forced Jam to build them (hack :( )

int main()
{
	return 0;
}

// @LatelinkDef Latelinked things
namespace gv
{
class CombatFx
{
};

float GetGameplayTime()
{
	return 0.f;
}

float GetWorldTime()
{
	return 0.f;
}
}