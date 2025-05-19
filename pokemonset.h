//
// Created by ASUS on 25-5-19.
//

#ifndef POKEMONSET_H
#define POKEMONSET_H
#include <vector>
#include <string>
#include <random>
#include <iostream>
#include <windows.h>
#include <easyx.h>
#include <map>
using namespace std;
enum class attribute
{
    Normal,     // һ��
    Fighting,    // ��
    Flying,      // ����
    Grass,      //��
    Ice,
    Fire,
    Poison,
    Water
};
enum class StatusCondition
{
    NONE,
    POISONED,    // �ж���ÿ�غϿ�Ѫ��
    CONFUSED,    // ѣ�Σ����ܹ����Լ���
    // ����״̬...
};
class Skill;
class pokemon;
class ConfusedStatus;
class PoisonedStatus;
class StatusEffect
{
protected:
    int duration;
public:
    virtual ~StatusEffect() = default;
    virtual bool allowsAttack() const = 0;
    StatusEffect(int d) : duration(d) {}
    virtual void applyStartOfTurn(pokemon& target) = 0;
    virtual void applyEndOfTurn(pokemon& target) = 0;
    virtual void onAttack(pokemon& attacker) = 0;
    void decreaseDuration() { if(duration > 0) duration--; }
    bool isActive() const { return duration > 0; }
};

class pokemon
{
private:
    string name;
    int hp;//Ѫ��
    int atk;//����
    int def;//����
    int satk;//�ع�
    int sdef;//�ط�
    int speed;//�ٶ�
protected:
    int level;
    int t_hp;//��ǰѪ��
    int t_atk;
    int t_def;
    int t_satk;
    int t_sdef;
    int t_speed;
    vector <attribute> att;//����

    vector <StatusEffect*> statusEffects; // ��Ϊԭʼָ��
public:
    vector <Skill> skills;
    pokemon(string name,int hp,int atk,int def,int satk,int sdef,int speed,vector <attribute> att,vector <Skill>skills):name(name),hp(hp),atk(atk),def(def),satk(satk),sdef(sdef),speed(speed),att(att),skills(skills)
    {
        level=1;
        t_hp=hp;
        t_atk=atk;
        t_def=def;
        t_satk=satk;
        t_sdef=sdef;
        t_speed=speed;
    }
    // ���ÿ�������͸�ֵ
    pokemon(const pokemon&) = delete;
    pokemon& operator=(const pokemon&) = delete;

    ~pokemon() {
        // �ͷ�����״̬Ч��
        for (auto ptr : statusEffects) {
            delete ptr;
        }
        statusEffects.clear();
    }

    int get_level() { return level; }
    string get_name()
    {
        return name;
    }
    int get_hp()
    {
        return t_hp;
    }
    int get_atk()
    {
        return t_atk;
    }
    int get_satk()
    {
        return t_satk;
    }
    vector<attribute> get_att() const { return att; }
    int get_sdef()
    {
        return t_sdef;
    }
    int get_speed()
    {
        return t_speed;
    }
    int get_def()
    {
        return t_def;
    }
    void change_hp(int _hp)
    {
        t_hp += _hp;
        if(t_hp<0)
        {
            t_hp=0;
        }
    }
    void change_att(int _att)
    {
        t_atk += _att;
        if(t_atk<0)
        {
            t_atk=0;
        }
    }
    void change_def(int _def)
    {
        t_sdef += _def;
        if(t_sdef<0)
        {
            t_sdef=0;
        }
    }
    void change_satk(int _satk)
    {
        t_satk += _satk;
        if(t_satk<0)
        {
            t_satk=0;
        }
    }
    void change_speed(int _speed)
    {
        t_speed += _speed;
        if(t_speed<0)
        {
            t_speed=0;
        }
    }
    void addStatus(StatusEffect* effect) {
        statusEffects.push_back(effect);
    }

    void clearExpiredStatus() {
        auto it = statusEffects.begin();
        while (it != statusEffects.end()) {
            if (!(*it)->isActive()) {
                delete *it; // �ͷ��ڴ�
                it = statusEffects.erase(it);
            } else {
                ++it;
            }
        }
    }

    void processStartOfTurn() {
        for (auto& effect : statusEffects) {
            effect->applyStartOfTurn(*this);
        }
    }

    void processEndOfTurn() {
        for (auto& effect : statusEffects) {
            effect->applyEndOfTurn(*this);
            effect->decreaseDuration();
        }
        clearExpiredStatus();
    }

    bool canAttack() const {
        for (const auto& effect : statusEffects) {
            if (!effect->allowsAttack()) return false;
        }
        return true;
    }
};
class PoisonedStatus : public StatusEffect {
public:
    PoisonedStatus() : StatusEffect(3) {} // Ĭ�ϳ���3�غ�
    bool allowsAttack() const override { return true; }
    void applyStartOfTurn(pokemon& target) override
    {
        // �ж��غϿ�ʼʱ���߼�
        int damage = target.get_hp() / 8;
        target.change_hp(-damage);
        cout << target.get_name() << " is hurt by poison!\n";
    }

    void applyEndOfTurn(pokemon& target) override {}
    void onAttack(pokemon& attacker) override {}
};
class ConfusedStatus : public StatusEffect
{
    std::mt19937 rng; // �����������
public:
    ConfusedStatus() : StatusEffect(2) {
        std::random_device rd;
        rng.seed(rd());
    }
    bool allowsAttack() const override { return true; }
    void onAttack(pokemon& attacker) override {}
    void applyStartOfTurn(pokemon& target) override {
        // 50%���ʹ����Լ�
        std::uniform_int_distribution<int> dist(0,1);
        if(dist(rng) == 0) {
            int selfDamage = target.get_atk() / 3;
            target.change_hp(-selfDamage);
            cout << target.get_name() << " hurt itself in confusion!\n";
        }
    }

    void applyEndOfTurn(pokemon& target) override {}
};
class Skill
{
private:
    string name;
    int pp;          // ʹ�ô���
public:

    attribute type;
    int power;       // ����
    int accuracy;    // ������
    int t_pp;
    struct StatusEffectChance {
        StatusCondition condition;
        int probability;
        int duration;
    };
    vector<StatusEffectChance> additionalEffects;
public:
    Skill( string name, attribute  type, int power,int accuracy, int pp): name(name), type(type), power(power),accuracy(accuracy), pp(pp)
    {
        t_pp = pp;
    }
    attribute getType() {return type;}
    int get_accuracy()
    {
        return accuracy;
    }
    string get_name()
    {
        return name;
    }
    int get_power()
    {
        return power;
    }
    // ���״̬Ч����������
    void addStatusEffect(StatusCondition cond, int prob, int dur) {
        additionalEffects.push_back({cond, prob, dur});
    }

    // �ڹ�������ʱ����
    void tryApplyStatus(pokemon& target) const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 99);

        for (const auto& eff : additionalEffects) {
            if (dis(gen) < eff.probability) {
                switch (eff.condition) {
                    case StatusCondition::POISONED:
                        target.addStatus(new PoisonedStatus());
                        break;
                    case StatusCondition::CONFUSED:
                        target.addStatus(new ConfusedStatus());
                        break;
                    default:
                        break;
                }
            }
        }
    }
};
#endif //POKEMONSET_H
