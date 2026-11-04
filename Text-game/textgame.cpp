#include<iostream>
using namespace std;

class Character {
public:
	int maxHP;
	int hp;
	int damage;
	int defense;
	string name;
	Character(string n, int mHP, int d, int def, int HP)
	{
		name = n;
		maxHP = mHP;
		damage = d;
		defense = def;
		hp = HP;
	}
};

void showStatus(Character ch)
{
	cout << "name : " << ch.name << endl;
	cout << "maxHP : " << ch.maxHP << endl;
	cout << "hp : " << ch.hp << endl;
	cout << "damage : " << ch.damage << endl;
	cout << "defense : " << ch.defense << endl;
	
}

int main(void) {
	
	showStatus(Character("Denia", 100, 10, 5, 100));
	return 0;
}