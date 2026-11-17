
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <limits>
using namespace std;

// 기술 종류
enum SkillType {
    ATTACK,
    DEFENSE_DOWN,
    ATTACK_DOWN,
    EXPLOSION,
    HEKTO_KICK,
    RIDER_KICK,
    ATTACK_UP
};

// Skill 클래스
class Skill {
public:
    string skillName;
    double skillCoefficient;
    int skillType;

    Skill() {
        skillName = "";
        skillCoefficient = 0.0;
        skillType = ATTACK;
    }

    Skill(string sN, double sC, int sT) {
        skillName = sN;
        skillCoefficient = sC;
        skillType = sT;
    }
};

// Character 클래스
class Character {
public:
    int maxHP;
    int hp;
    int damage;
    int defense;
    int speed;
    string name;

    Skill skills[4];

    // 기본 능력치 기준 버프와 디버프
    double attackBuff;
    double attackDebuff;
    double defenseBuff;
    double defenseDebuff;

    // 기술 사용 기록
    int lastSkillType;
    int hektoStreak;
    int lastRiderTurn;
    int riderRepeat;

    Character(string n, int mHP, int d, int def, int HP, int spd) {
        name = n;
        maxHP = mHP;
        damage = d;
        defense = def;
        hp = HP;
        speed = spd;

        attackBuff = 0.0;
        attackDebuff = 0.0;
        defenseBuff = 0.0;
        defenseDebuff = 0.0;

        lastSkillType = -1;
        hektoStreak = 0;
        lastRiderTurn = -100;
        riderRepeat = 0;
    }

    // 기본 능력치 기준 공격력 계산
    double getAttack() {
        double result = damage * (1.0 + attackBuff - attackDebuff);

        if (result < 1.0)
            result = 1.0;

        return result;
    }

    // 기본 능력치 기준 방어력 계산
    double getDefense() {
        double result = defense * (1.0 + defenseBuff - defenseDebuff);

        if (result < 1.0)
            result = 1.0;

        return result;
    }
};

// 기술 설정
void setSkills(Character& ch) {

    if (ch.name == "Denia (Ghost)") {

        ch.skills[0] = Skill("애교부리기", 0.0, DEFENSE_DOWN);
        ch.skills[1] = Skill("그냥 때리기", 1.0, ATTACK);
        ch.skills[2] = Skill("귤케이크 남기기", 0.0, ATTACK_DOWN);
        ch.skills[3] = Skill("팡 하고 터지기", 0.0, EXPLOSION);

    }
    else {

        ch.skills[0] = Skill("그냥 때리기", 1.0, ATTACK);
        ch.skills[1] = Skill("헥토파스칼 킥", 1.0, HEKTO_KICK);
        ch.skills[2] = Skill("라이더 킥", 2.2, RIDER_KICK);
        ch.skills[3] = Skill("폭력적으로 변하기", 0.0, ATTACK_UP);

    }
}

// 상태 출력
void showStatus(Character& ch) {

    cout << endl << "[" << ch.name << "]" << endl;
    cout << "HP : " << ch.hp << " / " << ch.maxHP << endl;
    cout << "Attack : " << ch.getAttack() << endl;
    cout << "Defense : " << ch.getDefense() << endl;
    cout << "Speed : " << ch.speed << endl;
}

// 기술 목록 출력
void showSkills(Character& ch) {

    cout << endl << "[" << ch.name << "'s Skills]" << endl;

    for (int i = 0; i < 4; i++) {
        cout << i + 1 << ". " << ch.skills[i].skillName << endl;
    }
}

// 피해량 계산
int calculateDamage(Character& attacker, Character& target, double coefficient) {

    double result = attacker.getAttack() / target.getDefense() * coefficient;

    int finalDamage = static_cast<int>(result);

    if (finalDamage < 1)
        finalDamage = 1;

    return finalDamage;
}

// 헥토파스칼 킥 계수 계산
double getHektoCoefficient(Character& actor) {

    int streak = 0;

    if (actor.lastSkillType == HEKTO_KICK)
        streak = actor.hektoStreak + 1;

    double coefficient = 1.0 + 0.25 * streak;

    if (coefficient > 2.0)
        coefficient = 2.0;

    return coefficient;
}

// 라이더 킥 계수 계산
double getRiderCoefficient(Character& actor, int round) {

    int repeat = 0;

    if (round - actor.lastRiderTurn <= 3)
        repeat = actor.riderRepeat + 1;

    double coefficient = 2.2 - 0.4 * repeat;

    if (coefficient < 1.0)
        coefficient = 1.0;

    return coefficient;
}

// 기술의 예상 피해량 계산
int estimateSkillDamage(Character& actor, Character& target, int index, int round) {

    Skill skill = actor.skills[index];

    switch (skill.skillType) {

    case ATTACK:
        return calculateDamage(actor, target, skill.skillCoefficient);

    case HEKTO_KICK:
        return calculateDamage(actor, target, getHektoCoefficient(actor));

    case RIDER_KICK:
        return calculateDamage(actor, target, getRiderCoefficient(actor, round));

    case EXPLOSION: {

        int cost = (actor.maxHP + 1) / 2;

        if (actor.hp - cost < 1)
            return 0;

        int damage = target.hp / 3;

        if (damage < 1)
            damage = 1;

        return damage;
    }

    default:
        return 0;
    }
}

// 특정 종류의 기술 검색
int findSkill(Character& ch, int type) {

    for (int i = 0; i < 4; i++) {

        if (ch.skills[i].skillType == type)
            return i;

    }

    return -1;
}

// 가장 강력한 공격 기술 검색
int findStrongestAttack(Character& actor, Character& target, int round, bool allowExplosion) {

    int bestIndex = -1;
    int bestDamage = -1;

    for (int i = 0; i < 4; i++) {

        int type = actor.skills[i].skillType;

        bool isDamageSkill =
            (type == ATTACK ||
                type == HEKTO_KICK ||
                type == RIDER_KICK ||
                type == EXPLOSION);

        if (!isDamageSkill)
            continue;

        if (type == EXPLOSION && !allowExplosion)
            continue;

        int estimated = estimateSkillDamage(actor, target, i, round);

        if (estimated > bestDamage) {
            bestDamage = estimated;
            bestIndex = i;
        }
    }

    if (bestIndex == -1)
        bestIndex = findSkill(actor, ATTACK);

    return bestIndex;
}

// 기술 실행
bool useSkill(Character& actor, Character& target, int index, int round) {

    if (index < 0 || index >= 4)
        return false;

    Skill skill = actor.skills[index];

    // 팡 하고 터지기 사용 가능 여부 확인
    if (skill.skillType == EXPLOSION) {

        int cost = (actor.maxHP + 1) / 2;

        if (actor.hp - cost < 1) {
            cout << "Not enough HP to use this skill. Choose another skill." << endl;
            return false;
        }
    }

    cout << endl << actor.name << " used " << skill.skillName << "!" << endl;

    int finalDamage = 0;
    double coefficient = skill.skillCoefficient;

    switch (skill.skillType) {

        // 일반 공격
    case ATTACK:

        finalDamage = calculateDamage(actor, target, coefficient);
        target.hp -= finalDamage;

        cout << target.name << " took " << finalDamage << " damage!" << endl;
        break;

        // 상대 방어력 감소
    case DEFENSE_DOWN:

        target.defenseDebuff += 0.20;

        cout << target.name << "'s Defense decreased by 20% of base Defense!" << endl;
        break;

        // 상대 공격력 감소
    case ATTACK_DOWN:

        target.attackDebuff += 0.20;

        cout << target.name << "'s Attack decreased by 20% of base Attack!" << endl;
        break;

        // HP 절반 소모 후 상대 현재 HP 1/3 감소
    case EXPLOSION: {

        int cost = (actor.maxHP + 1) / 2;

        actor.hp -= cost;

        finalDamage = target.hp / 3;

        if (finalDamage < 1)
            finalDamage = 1;

        target.hp -= finalDamage;

        cout << actor.name << " lost " << cost << " HP!" << endl;
        cout << target.name << " lost " << finalDamage << " HP!" << endl;
        cout << "Defense ignored!" << endl;

        break;
    }

                  // 헥토파스칼 킥
    case HEKTO_KICK:

        if (actor.lastSkillType == HEKTO_KICK)
            actor.hektoStreak++;
        else
            actor.hektoStreak = 0;

        coefficient = 1.0 + 0.25 * actor.hektoStreak;

        if (coefficient > 2.0)
            coefficient = 2.0;

        finalDamage = calculateDamage(actor, target, coefficient);
        target.hp -= finalDamage;

        cout << "Hekto power: " << coefficient << endl;
        cout << target.name << " took " << finalDamage << " damage!" << endl;

        break;

        // 라이더 킥
    case RIDER_KICK:

        if (round - actor.lastRiderTurn <= 3)
            actor.riderRepeat++;
        else
            actor.riderRepeat = 0;

        actor.lastRiderTurn = round;

        coefficient = 2.2 - 0.4 * actor.riderRepeat;

        if (coefficient < 1.0)
            coefficient = 1.0;

        finalDamage = calculateDamage(actor, target, coefficient);
        target.hp -= finalDamage;

        cout << "Rider Kick power: " << coefficient << endl;
        cout << target.name << " took " << finalDamage << " damage!" << endl;

        break;

        // 자신의 공격력 증가
    case ATTACK_UP:

        actor.attackBuff += 0.25;

        cout << actor.name << "'s Attack increased by 25% of base Attack!" << endl;
        break;
    }

    if (target.hp < 0)
        target.hp = 0;

    if (actor.hp < 0)
        actor.hp = 0;

    actor.lastSkillType = skill.skillType;

    return true;
}

// 적 AI
int enemyAI(Character& enemy, Character& player, int round) {

    // 승부를 봐야 하는 상황인지 판단
    bool urgent =
        (enemy.hp <= enemy.maxHP * 0.30 ||
            player.hp <= player.maxHP * 0.30);

    if (urgent) {
        return findStrongestAttack(enemy, player, round, true);
    }

    // 공격력 감소를 당했다면 버프로 회복
    // 체력이 여유로우면 공격력 버프를 최대 2회 사용
    int buffIndex = findSkill(enemy, ATTACK_UP);

    if (buffIndex != -1) {

        bool needsRecovery = enemy.attackDebuff > enemy.attackBuff;

        bool hasTimeToBuff =
            enemy.hp > enemy.maxHP * 0.60 &&
            enemy.attackBuff < 0.50;

        if (needsRecovery || hasTimeToBuff)
            return buffIndex;
    }

    // 상대 방어력 감소
    int defenseDown = findSkill(enemy, DEFENSE_DOWN);

    if (defenseDown != -1 && player.defenseDebuff < 0.001)
        return defenseDown;

    // 상대 공격력 감소
    int attackDown = findSkill(enemy, ATTACK_DOWN);

    if (attackDown != -1 && player.attackDebuff < 0.001)
        return attackDown;

    // 일반 상황에서는 가장 강한 공격 선택
    return findStrongestAttack(enemy, player, round, false);
}

// 플레이어 턴
void playerTurn(Character& player, Character& enemy, int round) {

    while (true) {

        showSkills(player);

        int choice;

        cout << endl << "Select skill (1~4): ";

        if (!(cin >> choice)) {

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Please enter a number." << endl;
            continue;
        }

        if (choice < 1 || choice > 4) {

            cout << "Invalid selection! Choose 1 to 4." << endl;
            continue;
        }

        if (useSkill(player, enemy, choice - 1, round))
            break;
    }
}

// 적 턴
void enemyTurn(Character& enemy, Character& player, int round) {

    int choice = enemyAI(enemy, player, round);

    if (choice < 0 || choice >= 4) {
        cout << "Enemy has no usable skill." << endl;
        return;
    }

    cout << endl << "Enemy AI selected: " << enemy.skills[choice].skillName << endl;

    useSkill(enemy, player, choice, round);
}

int main() {

    srand(static_cast<unsigned int>(time(0)));

    Character selected("Unknown", 0, 0, 0, 0, 0);
    Character enemy("Unknown", 0, 0, 0, 0, 0);

    cout << "0. Denia (Ghost)" << endl;
    cout << "1. Sigrika (Kamen Rider)" << endl;

    int select;

    cout << endl << "Select your character: ";

    if (!(cin >> select) || (select != 0 && select != 1)) {

        cout << "Are you trying to turn me into Skynet, human?" << endl;
        return 0;
    }

    // 플레이어 캐릭터 선택
    if (select == 0) {
        selected = Character("Denia (Ghost)", 100, 10, 5, 100, 10);
    }
    else {
        selected = Character("Sigrika (Kamen Rider)", 100, 10, 5, 100, 12);
    }

    // 적 캐릭터 랜덤 선택
    int enemySelect = rand() % 2;

    if (enemySelect == 0) {
        enemy = Character("Denia (Ghost)", 100, 10, 5, 100, 10);
    }
    else {
        enemy = Character("Sigrika (Kamen Rider)", 100, 10, 5, 100, 12);
    }

    cout << endl << "Player selected: " << selected.name << endl;
    cout << "Enemy selected: " << enemy.name << endl;

    setSkills(selected);
    setSkills(enemy);

    showStatus(selected);
    showStatus(enemy);

    cout << endl << "========== BATTLE START ==========" << endl;

    int round = 1;

    while (selected.hp > 0 && enemy.hp > 0) {

        cout << endl << "========== ROUND " << round << " ==========" << endl;

        // 스피드가 높은 캐릭터가 먼저 행동
        // 스피드가 같으면 플레이어가 먼저 행동

        if (selected.speed >= enemy.speed) {

            playerTurn(selected, enemy, round);

            if (enemy.hp > 0)
                enemyTurn(enemy, selected, round);

        }
        else {

            enemyTurn(enemy, selected, round);

            if (selected.hp > 0)
                playerTurn(selected, enemy, round);

        }

        showStatus(selected);
        showStatus(enemy);

        round++;
    }

    cout << endl << "========== BATTLE END ==========" << endl;

    if (selected.hp > 0)
        cout << "YOU WIN!" << endl;
    else
        cout << "YOU LOSE!" << endl;

    return 0;
}
