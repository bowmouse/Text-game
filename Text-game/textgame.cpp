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
	
	showStatus(Character("Denia (Ghost)", 100, 10, 5, 100));
	cout << endl<<endl;
	showStatus(Character("Sigrika (Kamen Rider)", 100, 10, 5, 100));
	int select;
	cout << "Select your character (0 for Denia (Ghost), 1 for Sigrika(Kamen Rider)) : ";
	Character selected("Unknown", 0, 0, 0, 0);
	cin >> select;
	switch (select)
	{
	case 0:
		cout << "Denia (Ghost) is selected" << endl<<endl;
		selected = Character("Denia (Ghost)", 100, 10, 5, 100);
		break;
	case 1:
		cout << "Sigrika (Kamen Rider) is selected" << endl<<endl;
		selected = Character("Sigrika (Kamen Rider)", 100, 10, 5, 100);
		break;
	default:
		cout << "Are you trying to turn me into Skynet, human?" << endl;
		break;
	}
	
	return 0;
}