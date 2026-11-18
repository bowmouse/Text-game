#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <limits>
using namespace std;

// [기술 종류 정의] 
// 새로운 스킬 타입을 추가하고 싶다면 여기에 이름을 추가하고 아래 useSkill에서 처리 로직을 구현하면 됩니다.
enum SkillType {
    ATTACK,          // 일반 공격
    DEFENSE_DOWN,    // 상대 방어력 감소
    ATTACK_DOWN,     // 상대 공격력 감소
    EXPLOSION,       // 자해 후 고정 피해 (방어력 무시)
    HEKTO_KICK,      // 연속 사용할수록 강해지는 킥
    RIDER_KICK,      // 자주 쓰면 약해지는 킥 (쿨다운 개념)
    ATTACK_UP        // 자신의 공격력 증가
};

// [기술(Skill) 클래스]
// 개별 기술의 이름, 계수(위력 배율), 타입을 관리합니다.
class Skill {
public:
    string skillName;       // 기술 이름 (예: "그냥 때리기")
    double skillCoefficient;// 기술 계수 (데미지 배율)
    int skillType;          // 기술 종류 (SkillType 참고)

    // 기본 생성자
    Skill() {
        skillName = "";
        skillCoefficient = 0.0;
        skillType = ATTACK;
    }

    // 매개변수 생성자
    Skill(string sN, double sC, int sT) {
        skillName = sN;
        skillCoefficient = sC;
        skillType = sT;
    }
};

// [캐릭터(Character) 클래스]
// 캐릭터의 기본 능력치, 버프/디버프 상태, 콤보 기록을 관리합니다.
class Character {
public:
    int maxHP;              // 최대 체력
    int hp;                 // 현재 체력
    int damage;             // 기본 공격력
    int defense;            // 기본 방어력
    int speed;              // 스피드 (선공 결정)
    string name;            // 캐릭터 이름

    Skill skills[4];        // 캐릭터가 보유한 4가지 기술 슬롯

    // 버프와 디버프 수치 (비율 단위: 0.25 = 25%증가/감소)
    double attackBuff;
    double attackDebuff;
    double defenseBuff;
    double defenseDebuff;

    // 특수 기술 사용 기록 (연속 사용 보정용)
    int lastSkillType;      // 직전에 쓴 기술 타입
    int hektoStreak;        // 헥토파스칼 킥 연속 사용 횟수
    int lastRiderTurn;      // 라이더 킥을 마지막으로 쓴 턴
    int riderRepeat;        // 라이더 킥 반복 횟수

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

    // [실제 적용되는 공격력 계산 함수]
    // 기본 공격력에 버프와 디버프를 합산하여 최종 공격력을 산출합니다.
    double getAttack() {
        double result = damage * (1.0 + attackBuff - attackDebuff);

        // 공격력이 1 미만으로 떨어지지 않게 보정
        if (result < 1.0)
            result = 1.0;

        return result;
    }

    // [실제 적용되는 방어력 계산 함수]
    // 기본 방어력에 버프와 디버프를 반영합니다.
    double getDefense() {
        double result = defense * (1.0 + defenseBuff - defenseDebuff);

        // 방어력이 1 미만으로 떨어지지 않게 보정
        if (result < 1.0)
            result = 1.0;

        return result;
    }
};

// [캐릭터별 기술 세팅 함수]
// 캐릭터 이름에 따라 어떤 스킬을 가질지 설정합니다. 
// 친구들이 캐릭터를 추가하고 싶다면 이곳에 코드를 늘려가면 됩니다!
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

// 캐릭터의 현재 상태(체력, 공격력 등)를 화면에 출력합니다.
void showStatus(Character& ch) {
    cout << endl << "[" << ch.name << "]" << endl;
    cout << "HP : " << ch.hp << " / " << ch.maxHP << endl;
    cout << "Attack : " << ch.getAttack() << endl;
    cout << "Defense : " << ch.getDefense() << endl;
    cout << "Speed : " << ch.speed << endl;
}

// 보유한 기술 목록을 출력합니다.
void showSkills(Character& ch) {
    cout << endl << "[" << ch.name << "'s Skills]" << endl;
    for (int i = 0; i < 4; i++) {
        cout << i + 1 << ". " << ch.skills[i].skillName << endl;
    }
}

// [데미지 계산 공식]
// (공격자의 공격력 / 방어자의 방어력) * 기술 계수를 곱해 최종 피해량을 정합니다.
int calculateDamage(Character& attacker, Character& target, double coefficient) {
    double result = attacker.getAttack() / target.getDefense() * coefficient;
    int finalDamage = static_cast<int>(result);

    // 최소 데미지는 항상 1 보장
    if (finalDamage < 1)
        finalDamage = 1;

    return finalDamage;
}

// [헥토파스칼 킥 특수 계수 계산]
// 연속으로 쓸수록 계수가 0.25씩 증가하며 최대 2.0까지 뻥튀기됩니다.
double getHektoCoefficient(Character& actor) {
    int streak = 0;
    if (actor.lastSkillType == HEKTO_KICK)
        streak = actor.hektoStreak + 1;

    double coefficient = 1.0 + 0.25 * streak;
    if (coefficient > 2.0)
        coefficient = 2.0;

    return coefficient;
}

// [라이더 킥 특수 계수 계산]
// 3턴 이내에 난사하면 데미지가 점점 줄어드는(0.4씩 감소) 패널티가 있습니다.
double getRiderCoefficient(Character& actor, int round) {
    int repeat = 0;
    if (round - actor.lastRiderTurn <= 3)
        repeat = actor.riderRepeat + 1;

    double coefficient = 2.2 - 0.4 * repeat;
    if (coefficient < 1.0)
        coefficient = 1.0;

    return coefficient;
}

// AI가 공격할 때 예상되는 피해량을 미리 계산해 보는 함수입니다.
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
        if (actor.hp - cost < 1) return 0; // 체력이 부족하면 사용 불가
        int damage = target.hp / 3;
        return (damage < 1) ? 1 : damage;
    }
    default:
        return 0;
    }
}

// 특정 타입의 스킬이 캐릭터에게 몇 번째 슬롯에 있는지 찾습니다.
int findSkill(Character& ch, int type) {
    for (int i = 0; i < 4; i++) {
        if (ch.skills[i].skillType == type)
            return i;
    }
    return -1;
}

// AI가 현재 상황에서 가장 아픈 공격 기술을 골라내는 함수입니다.
int findStrongestAttack(Character& actor, Character& target, int round, bool allowExplosion) {
    int bestIndex = -1;
    int bestDamage = -1;

    for (int i = 0; i < 4; i++) {
        int type = actor.skills[i].skillType;
        bool isDamageSkill = (type == ATTACK || type == HEKTO_KICK || type == RIDER_KICK || type == EXPLOSION);

        if (!isDamageSkill) continue;
        if (type == EXPLOSION && !allowExplosion) continue;

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

// [실제 기술 실행 함수]
// 선택한 번호의 스킬 효과를 적용하고 HP와 버프 상태를 갱신합니다.
bool useSkill(Character& actor, Character& target, int index, int round) {
    if (index < 0 || index >= 4) return false;

    Skill skill = actor.skills[index];

    // '팡 하고 터지기' 시전 시 체력 소모 조건 체크
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
    case ATTACK: // 일반 공격
        finalDamage = calculateDamage(actor, target, coefficient);
        target.hp -= finalDamage;
        cout << target.name << " took " << finalDamage << " damage!" << endl;
        break;

    case DEFENSE_DOWN: // 방어력 깎기
        target.defenseDebuff += 0.20;
        cout << target.name << "'s Defense decreased by 20% of base Defense!" << endl;
        break;

    case ATTACK_DOWN: // 공격력 깎기
        target.attackDebuff += 0.20;
        cout << target.name << "'s Attack decreased by 20% of base Attack!" << endl;
        break;

    case EXPLOSION: { // 자해 후 방어 무시 광역 피해
        int cost = (actor.maxHP + 1) / 2;
        actor.hp -= cost;
        finalDamage = target.hp / 3;
        if (finalDamage < 1) finalDamage = 1;
        target.hp -= finalDamage;
        cout << actor.name << " lost " << cost << " HP!" << endl;
        cout << target.name << " lost " << finalDamage << " HP!" << endl;
        cout << "Defense ignored!" << endl;
        break;
    }

    case HEKTO_KICK: // 헥토파스칼 킥 (연속 사용 시 강화)
        if (actor.lastSkillType == HEKTO_KICK)
            actor.hektoStreak++;
        else
            actor.hektoStreak = 0;

        coefficient = 1.0 + 0.25 * actor.hektoStreak;
        if (coefficient > 2.0) coefficient = 2.0;

        finalDamage = calculateDamage(actor, target, coefficient);
        target.hp -= finalDamage;
        cout << "Hekto power: " << coefficient << endl;
        cout << target.name << " took " << finalDamage << " damage!" << endl;
        break;

    case RIDER_KICK: // 라이더 킥 (난사 시 약화)
        if (round - actor.lastRiderTurn <= 3)
            actor.riderRepeat++;
        else
            actor.riderRepeat = 0;

        actor.lastRiderTurn = round;
        coefficient = 2.2 - 0.4 * actor.riderRepeat;
        if (coefficient < 1.0) coefficient = 1.0;

        finalDamage = calculateDamage(actor, target, coefficient);
        target.hp -= finalDamage;
        cout << "Rider Kick power: " << coefficient << endl;
        cout << target.name << " took " << finalDamage << " damage!" << endl;
        break;

    case ATTACK_UP: // 내 공격력 강화
        actor.attackBuff += 0.25;
        cout << actor.name << "'s Attack increased by 25% of base Attack!" << endl;
        break;
    }

    // 체력이 0 미만으로 내려가지 않게 방지
    if (target.hp < 0) target.hp = 0;
    if (actor.hp < 0) actor.hp = 0;

    // 이번에 쓴 기술 타입 기록 (다음 턴 콤보 계산용)
    actor.lastSkillType = skill.skillType;

    return true;
}

// [적 AI 의사결정 함수]
// 체력 상황이나 디버프 여부에 따라 어떤 행동을 할지 스마트하게 판단합니다.
int enemyAI(Character& enemy, Character& player, int round) {
    // 양측 중 체력이 30% 이하로 떨어지면 필사적으로 가장 센 공격을 퍼붓습니다.
    bool urgent = (enemy.hp <= enemy.maxHP * 0.30 || player.hp <= player.maxHP * 0.30);
    if (urgent) {
        return findStrongestAttack(enemy, player, round, true);
    }

    // 공격력 디버프를 맞았거나 여유가 있다면 버프를 시전합니다.
    int buffIndex = findSkill(enemy, ATTACK_UP);
    if (buffIndex != -1) {
        bool needsRecovery = enemy.attackDebuff > enemy.attackBuff;
        bool hasTimeToBuff = (enemy.hp > enemy.maxHP * 0.60 && enemy.attackBuff < 0.50);
        if (needsRecovery || hasTimeToBuff)
            return buffIndex;
    }

    // 상대에게 방어력 디버프가 없다면 디버프를 우선 겁니다.
    int defenseDown = findSkill(enemy, DEFENSE_DOWN);
    if (defenseDown != -1 && player.defenseDebuff < 0.001)
        return defenseDown;

    // 상대에게 공격력 디버프가 없다면 디버프를 겁니다.
    int attackDown = findSkill(enemy, ATTACK_DOWN);
    if (attackDown != -1 && player.attackDebuff < 0.001)
        return attackDown;

    // 평상시에는 가장 강력한 공격 선택
    return findStrongestAttack(enemy, player, round, false);
}

// 플레이어의 입력을 받아 스킬을 선택하고 실행하는 턴 함수
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

// 적 AI의 턴을 처리하는 함수
void enemyTurn(Character& enemy, Character& player, int round) {
    int choice = enemyAI(enemy, player, round);
    if (choice < 0 || choice >= 4) {
        cout << "Enemy has no usable skill." << endl;
        return;
    }

    cout << endl << "Enemy AI selected: " << enemy.skills[choice].skillName << endl;
    useSkill(enemy, player, choice, round);
}

// [메인 함수] 게임의 전체 흐름(시작 -> 라운드 반복 -> 승패 판정)을 제어합니다.
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

    // 플레이어 캐릭터 초기화 (이름, 최대체력, 공격력, 방어력, 현재체력, 스피드)
    if (select == 0) {
        selected = Character("Denia (Ghost)", 100, 10, 5, 100, 10);
    }
    else {
        selected = Character("Sigrika (Kamen Rider)", 100, 10, 5, 100, 12);
    }

    // 적 캐릭터 랜덤 선택 (0 또는 1)
    int enemySelect = rand() % 2;
    if (enemySelect == 0) {
        enemy = Character("Denia (Ghost)", 100, 10, 5, 100, 10);
    }
    else {
        enemy = Character("Sigrika (Kamen Rider)", 100, 10, 5, 100, 12);
    }

    cout << endl << "Player selected: " << selected.name << endl;
    cout << "Enemy selected: " << enemy.name << endl;

    // 양쪽 캐릭터에게 스킬 장착
    setSkills(selected);
    setSkills(enemy);

    showStatus(selected);
    showStatus(enemy);

    cout << endl << "========== BATTLE START ==========" << endl;

    int round = 1;

    // 어느 한쪽의 체력이 0이 될 때까지 턴을 무한 반복합니다.
    while (selected.hp > 0 && enemy.hp > 0) {
        cout << endl << "========== ROUND " << round << " ==========" << endl;

        // 스피드가 높은 캐릭터가 선공, 같으면 플레이어 우선 행동
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

        // 라운드 종료 후 상태 표시
        showStatus(selected);
        showStatus(enemy);

        round++;
    }

    cout << endl << "========== BATTLE END ==========" << endl;

    // 승패 판정 출력
    if (selected.hp > 0)
        cout << "YOU WIN!" << endl;
    else
        cout << "YOU LOSE!" << endl;

    return 0;
}