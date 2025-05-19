
#include "pokemonset.h"
#include "fun.h"
int main()
{
    Skill flamethrower("flamethrower", attribute::Fire, 90, 100, 15);
    flamethrower.addStatusEffect(StatusCondition::POISONED, 30, 3);
    Skill scratch("scratch", attribute::Normal, 40, 100, 35);

    Skill razorLeaf("razorLeaf", attribute::Grass, 55, 95, 25);
    razorLeaf.addStatusEffect(StatusCondition::CONFUSED, 20, 2);
    Skill tackle("tackle",attribute::Normal, 40, 100, 35);

    // 创建宝可梦
    pokemon Charmander(
        "Charmander", 39, 52, 43, 60, 50, 65,
        {attribute::Fire},
        {flamethrower,scratch}
    );

    pokemon Bulbasaur(
        "Bulbasaur", 45, 49, 49, 65, 65, 45,
        {attribute::Grass},
        {razorLeaf,tackle}
    );

    // 开始战斗
    startBattle(Charmander, Bulbasaur);

    return 0;
}
