// Galavant
//  This is a pseudotarget; that means the bin isn't what we care about,
//   it's the libs we linked to, which forced Jam to build them (hack :( )

int main()
{
	return 0;
}

// Latelinked things
namespace gv
{
class CombatFx
{
};

float GetWorldTime()
{
	return 0.f;
}
}