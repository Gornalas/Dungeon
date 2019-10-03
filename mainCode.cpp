#include <iostream>
#include <string>
#include <ctime>

//Рандомайзер
int getRandomNumber(int min, int max)
{
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

//Классы
class Creature {
protected:
	std::string cr_name;	//Имя создания
	char cr_symbol;			//Символ
	int cr_hp;				//Очки здоровья
	int cr_damage;			//Очки урона по врагам
	int cr_gold;			//Золото

public:
	Creature(std::string name, char symbol, int hp, int damage, int gold)
		: cr_name(name), cr_symbol(symbol), cr_hp(hp), cr_damage(damage), cr_gold(gold) {}

	//Пополнение кошелька
	void addGold(int gold) {	
		cr_gold += gold;
	}

	//Потеря здоровья
	void reduceHealth(int hp) {	
		cr_hp -= hp;
	}

	//Условия проигрыша
	bool isDead() { return cr_hp <= 0; } 

	//Имя
	const std::string& getName() { return cr_name; }
	//Знак
	char getSymbol() { return cr_symbol; }
	//Здоровье
	int getHealth() { return cr_hp; }
	//Наносимый урон
	int getDamage() { return cr_damage; }
	//Золото в кошеле
	int getGold() { return cr_gold; }
};
class Player : public Creature {
private:
	int p_level;

public:
	Player(std::string name): Creature (name, '@', 10, 1, 0){
		p_level = 1;	//Уровень существует только у игрока.
	}

	//При победе увеличивается уровень и урон игрока.
	void levelUp() {	
		++p_level;
		++cr_damage;
	};
	//Геттер уровня игрока
	int getLevel() { return p_level; }

	//Условия победы
	bool hasWon() { return p_level >= 20; }
};
class Monster: public Creature{
public:
	//Виды монстров в игре
	enum Type {				
		DRAGON,
		ORC,
		SLIME,
		MAX_TYPES
	};

	//Параметры монстров
	struct MonsterData {	
		std::string name;
		char symbol;
		int health;
		int damage;
		int gold;
	};

	//Создание примитивной БД из массива.
	static MonsterData monsterData[];	

	Monster(Type type) : Creature(monsterData[type].name, monsterData[type].symbol, monsterData[type].health, monsterData[type].damage, monsterData[type].gold){}

	//Рандомайзер для монстров.
	static Type getRandomMonster() {	
		int temp = getRandomNumber(0, MAX_TYPES-1);
		return static_cast<Type>(temp);
	}
};

//Прототипы функций.
void fightMonster(Player&);
char decision();
void playerDecision(Monster&, Player&);
void attackMonster(Monster&, Player&);
void attackPlayer(Monster&, Player&);
void playerWin(Player&);
void interface(Monster&, Player&);

//База данных монстров
Monster::MonsterData Monster::monsterData[Monster::MAX_TYPES]{
	{ "Dragon", 'D', 20, 4, 100 },
	{ "Orc", 'o', 4, 2, 25 },
	{ "Slime", 's', 1, 1, 10 }
};

//Новая комната
void fightMonster(Player& p){
	//генерация монстра.
	Monster m(Monster::getRandomMonster());
	interface(m, p);
	std::cout << "You have encountered a " << m.getName() << " (" << m.getSymbol() << ")." << std::endl;

	//Переход к бою после генерации монстра
	playerDecision(m, p);
}

//Выбор игрока
void playerDecision(Monster& m, Player& p) {
	char dec = decision();						
	if (dec == 'R' || dec == 'r') {				//Выбран побег
		int chance = getRandomNumber(0, 1);		//Вероятность побега 50%
		if (chance == 1) {					
			interface(m, p);
			std::cout << "You successfully fled.\n" << std::endl;
			system("pause");
			fightMonster(p);					//К следующему монстру
		}
		else {
			interface(m, p);
			std::cout << "You can't fled.\n" << std::endl;
			system("pause");
			attackPlayer(m, p);					//Побег не удался и монстр атаковал первым.
		}
	}
	else
		attackMonster(m, p);					//Выбран бой.
}

//Обработка пользовательского ввода во избежание ошибок и краша.
char decision() {
	while (true) // цикл продолжается до тех пор, пока пользователь не введёт корректное значение
	{
		std::cout << "(R)un of (F)ight: ";
		char decision;
		std::cin >> decision;

		std::cin.ignore(32767, '\n'); // Удаляем лишнее, так как введено по ошибке

		// Выполняем проверку пользовательского ввода
		if (decision != 'R' && decision != 'F' && decision != 'r' && decision != 'f')
		{
			std::cout << "Oops, that input is invalid. Please try again.\n";
		}
		else
		{
			return decision;
		}
	}
}

//Удар по монстру
void attackMonster(Monster& m, Player& p) {	
	m.reduceHealth(p.getDamage());			
	interface(m, p);
	if (m.getHealth() <= 0) {
		p.levelUp();
		p.addGold(m.getGold());
		interface(m, p);
		std::cout << "You are winner!" << std::endl;
		system("pause");
		playerWin(p);
	}
	else {
		attackPlayer(m, p);
	}
}

//Урон по игроку
void attackPlayer(Monster& m, Player& p) {
	p.reduceHealth(m.getDamage());
	interface(m, p);
	std::cout << m.getName() << " attack your. Your health is " << p.getHealth() << " points." << std::endl;
	if (p.isDead()) {
		system("cls");
		std::cout << "You are loser!" << std::endl;
		std::cout << "You take " << p.getLevel() << " level and " << p.getGold() << " gold." << std::endl;
	}
	else {
		playerDecision(m, p);
	}
}

//Провека победы
void playerWin(Player& p) {
	if (p.hasWon()) {
		system("cls");
		std::cout << "Dungeon accomplete." << std::endl;
		std::cout << "You can steel " << p.getGold() << " gold." << std::endl;
	}
	else {
		fightMonster(p);
	}
}
//Обработка консоли для удобной игры.
void interface(Monster& m, Player& p) {
	system("cls");	
	//Параметры игрока
	std::cout << "Name: " << p.getName() << "    Level: " << p.getLevel() << "    Health: " << p.getHealth() << "    Gold: " << p.getGold() << "    Damage: " << p.getDamage() << std::endl;
	//Параметры монстра
	std::cout << "\nMonster:" << m.getName() << "    Health: " << m.getHealth() << "    Damage: " << m.getDamage() << "\n\n\n" << std::endl;
}

int main() {
	srand(static_cast<unsigned int>(time(0))); // устанавливаем значение системных часов в качестве стартового числа
	rand(); // сбрасываем первый результат

	//Создание персонажа.
	std::string playerName;
	std::cout << "Enter your name: ";
	std::cin >> playerName;
	Player p(playerName);
	std::cout << "Welcome, " << p.getName() << std::endl;
	std::cout << "You have " << p.getHealth() << " health and are carrying " << p.getGold() << " gold.\n" << std::endl;
	system("pause");

	//Начало игры
	fightMonster(p);

	return 0;
}
