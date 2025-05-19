//
// Created by ASUS on 25-5-19.
//
#include "pokemonset.h"
float getMultiplier(attribute attack, attribute defense) {
    static map<pair<attribute, attribute>, float> chart = {
        {{attribute::Fire, attribute::Grass}, 2.0f},
        {{attribute::Fire, attribute::Fire}, 0.5f},
        {{attribute::Fire, attribute::Ice}, 2.0f},
        {{attribute::Grass, attribute::Fire}, 0.5f},
        {{attribute::Grass, attribute::Water}, 2.0f},
        {{attribute::Water, attribute::Fire}, 2.0f},
    };

    auto it = chart.find({attack, defense});
    return it != chart.end() ? it->second : 1.0f;
}

// �˺�����
int calculateDamage(pokemon& attacker, pokemon& defender, Skill& skill) {
    // �жϹ������ͣ��򵥼��裺FireΪ�ع�������Ϊ�﹥��
    bool isSpecial = skill.getType() != attribute::Normal;
    int attack = isSpecial ? attacker.get_satk() : attacker.get_atk();
    int defense = isSpecial ? defender.get_sdef() : defender.get_def();

    // ��������
    int base = (2 * attacker.get_level() * attack * skill.get_power()) / (defense * 50) + 2;

    // ���Լӳ�
    float multiplier = 1.0f;
    for(auto att : defender.get_att()) {
        multiplier *= getMultiplier(skill.getType(), att);
    }

    // ������ӣ�0.85-1.0��
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dis(0.85, 1.0);

    return static_cast<int>(base * multiplier * dis(gen));
}

// ִ�й���
void executeAttack(pokemon& attacker, pokemon& defender, Skill& skill) {
    if(skill.t_pp <= 0) {
        cout << attacker.get_name() << " ' " << skill.get_name() << " PP doesn't enough!\n";
        return;
    }
    skill.t_pp--;

    // �����ж�
    static mt19937 gen(random_device{}());
    uniform_int_distribution<> dis(0, 99);
    if(dis(gen) >= skill.get_accuracy()) {
        cout << attacker.get_name() << " 's " << skill.get_name() << " miss!\n";
        return;
    }

    // �����˺�
    int damage = calculateDamage(attacker, defender, skill);
    defender.change_hp(-damage);
    cout << attacker.get_name() << " 's " << skill.get_name() << " makes "
         << damage << " hurt!\n";

    // ʩ��״̬Ч��
    skill.tryApplyStatus(defender);
}
Skill& choose(pokemon& attacker)
{
    int i;
    cin>>i;
    switch(i)
    {
        case 0:
            return attacker.skills[0];
        break;

        case 1:
            return attacker.skills[1];
        break;
        case 2:
            return attacker.skills[2];
        break;
        default:
            return attacker.skills[3];

    }
}
// ս������
void startBattle(pokemon& p1, pokemon& p2) {
    cout << "===== battle begin =====" << endl;
    cout << p1.get_name() << " vs " << p2.get_name() << "\n\n";

    int turn = 1;






    while(p1.get_hp() > 0 && p2.get_hp() > 0) {
        cout << "\n=== turn " << turn++ << " ===\n";

        // ����غϿ�ʼ״̬
        p1.processStartOfTurn();
        p2.processStartOfTurn();
        if(p1.get_hp() <= 0 || p2.get_hp() <= 0) break;

        // ѡ���ܣ���ʹ�õ�һ�����ܣ�
        Skill& skill1 =choose(p1);
        Skill& skill2 =choose(p2);

        // �ٶ��ж�
        bool p1First = p1.get_speed() >= p2.get_speed();

        // ����˳��
        if(p1First) {
            executeAttack(p1, p2, skill1);
            if(p2.get_hp() <= 0) break;
            executeAttack(p2, p1, skill2);
        } else {
            executeAttack(p2, p1, skill2);
            if(p1.get_hp() <= 0) break;
            executeAttack(p1, p2, skill1);
        }

        // ����غϽ���״̬
        p1.processEndOfTurn();
        p2.processEndOfTurn();
    }

    cout << "\n===== battle end =====" << endl;
    cout << (p1.get_hp() > 0 ? p1.get_name() : p2.get_name()) << " win!\n";
}