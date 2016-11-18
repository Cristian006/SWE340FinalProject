#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>

using namespace std;

enum ItemType
{
	none = 0,
	weapon = 1,
	consumable = 2,
	quest = 3
};

enum WeaponType
{
	primary = 0,
	secondary = 1,
	throwable = 2
};

#pragma region Database Utils

template <class T>
class AbstractDatabase {
private:
	vector<T*> _objects;
public:
	//check if item database is empty
	bool empty() const {
		return _objects.empty();
	}

	//return the total number of items in the database
	unsigned int getSize(void) const
	{
		return _objects.size();
	}

	//return the object at the specified index
	T* get(unsigned int index)
	{
		if (index < _objects.size()) {
			return _objects.at(index);
		}
		else return NULL;
	}

	//add item to the database
	void add(T* i) {
		_objects.push_back(i);
	}

	//remove item from the database
	bool remove(unsigned int index) {
		if (index < _objects.size()) {
			_objects.erase(_objects.begin() + index);
			return true;
		}
		return false;
	}
};

#pragma endregion

#pragma region ItemSystem

class Item
{
public:
	Item(int id, string name, ItemType iT, int weight, int level, int cost) {
		_id = id;
		_name = name;
		_iType = iT;
		_weight = weight;
		_level = level;
		_cost = cost;
	};
	~Item() {}
	string getName() {
		return _name;
	}
	int getWeight() {
		return _weight;
	}
private:
	int _id;
	string _name;
	ItemType _iType;
	int _weight;
	int _level;
	int _cost;
};

class Weapon : public Item {
public:
	Weapon(int id, string name, ItemType iT, int weight, int level, int cost) : Item(id, name, iT, weight, level, cost) {}
	Weapon(int id, string name, ItemType iT, int weight, int level, int cost, int durability, bool equipable, WeaponType weaponType) : Item(id, name, iT, weight, level, cost){
		_durability = durability;
		_currentDurablity = _durability;
		_equipable = equipable;
		_weaponType = weaponType;
	}
private:
	int _durability;
	int _currentDurablity;
	bool _equipable;
	WeaponType _weaponType;
};

class Consumable : public Item {
public:
	Consumable(int id, string name, ItemType iT, int weight, int level, int cost) : Item(id, name, iT, weight, level, cost) {

	}
private:
};

class QuestObject : public Item {
public:
	QuestObject(int id, string name, ItemType iT, int weight, int level, int cost) : Item(id, name, iT, weight, level, cost) {

	}
private:
};

//ItemDatabase is a Singelton Class inheriting from the AbstractDatabase Class
class ItemDatabase : public AbstractDatabase<Item> {
private:
	static ItemDatabase *inst;
	static void populate() {
		//Weapons
		inst->add(new Weapon(0, "sword", weapon, 2, 1, 300));
		inst->add(new Weapon(1, "axe", weapon, 3, 1, 450));

		//Consumables
		inst->add(new Consumable(2, "apple", consumable, 1, 1, 15));
		inst->add(new Consumable(3, "pair", consumable, 1, 1, 15));
		inst->add(new Consumable(4, "carrot", consumable, 1, 1, 10));

		//QuestObjects
		inst->add(new QuestObject(5, "scarf", quest, 1, 1, 200));
	}
public:
	static ItemDatabase *instance();
};
ItemDatabase *ItemDatabase::inst = 0;
ItemDatabase *ItemDatabase::instance() {
	if (!inst) {
		inst = new ItemDatabase();
		populate();
	}
	return inst;
}

#pragma endregion

#pragma region InventorySystem

//The container class for an inventory
template<typename T, unsigned int capacity>
class Container{
public:
	Container(void) {}

	// Returns true if the item was added successfully, false if the container was full
	bool add(T* item)
	{
		//int w = item->getWeight() + getWeight();
		if (capacity > _objects.size())
		{
			_objects.push_back(item);
			onObjectAdded();
			return true;
		}
		return false;
	}

	unsigned int getCapacity(void) const
	{
		return capacity;
	}

	unsigned int getSize(void) const
	{
		return _objects.size();
	}

	unsigned int getWeight(void) const 
	{
		int w = 0;
		if (_objects.size() > 0) {
			for each (T* i in _objects)
			{
				w += i->getWeight();
			}
		}
		return w;
	}

	T* get(unsigned int index)
	{
		if (index < _objects.size()) {
			return _objects.at(index);
		}
		else return NULL;
	}

	// Returns true if the index was removed, false if the index was out of bounds
	bool remove(unsigned int index)
	{
		if (index < _objects.size())
		{
			_objects.erase(_objects.begin() + index);
			onObjectRemoved();
			return true;
		}
		return false;
	}

	~Container(void)
	{

	}

private:
	vector<T*> _objects;
	virtual void onObjectAdded() = 0;
	virtual void onObjectRemoved() = 0;
};

//encapsulating the Container Class
template<unsigned int cap>
class Inventory : public Container<Item, cap> {
private:
	void onObjectAdded() {
		cout << "\nAdded Object" << endl;
	}
	void onObjectRemoved() {
		cout << "\nRemoved Object" << endl;
	}
};

#pragma endregion

#pragma region CharacterSystem

//Private Class Data for all of the Character's variables
class CharacterData {

private:
	string name;
	string description;

public:
	CharacterData(string n, string d) {
		name = n;
		description = d;
	}

	string getName(void) {
		return name;
	}

	string getDescription(void){
		return description;
	}
};

class Character {
private:
	CharacterData* data;
	
public:
	Character(string n, string d) {
		data = new CharacterData(n, d);
	}
	string getName() {
		return data->getName();
	}
	string getDescription() {
		return data->getDescription();
	}
	Inventory<30>* inventory = new Inventory<30>();
};

#pragma endregion


int main() {
	cout << "New Game" << endl;
	
	Character* c = new Character("johny", "apple seed collector");
	
	cout << "New Character: " << c->getName() << endl;
	cout << "Starting Journey..." << endl;

	cout << "Walking across the desert..." << endl;

	c->inventory->add(ItemDatabase::instance()->get(0));
	
	cout << "Item Collected: " << c->inventory->get(c->inventory->getSize() - 1)->getName() << endl;
	cout << "New Weight: " << c->inventory->getWeight() << "/" << c->inventory->getCapacity() << endl;
	cout << "Onward!" << endl;
	
	c->inventory->add(ItemDatabase::instance()->get(1));

	cout << "Finally made it out of the desert, made it to the forest" << endl;

	cout << "Item Collected: " << c->inventory->get(c->inventory->getSize() - 1)->getName() << endl;
	cout << "New Weight: " << c->inventory->getWeight() << "/" << c->inventory->getCapacity() << endl;
	cout << "Onward!" << endl;

	c->inventory->add(ItemDatabase::instance()->get(2));

	cout << "Item Collected: " << c->inventory->get(c->inventory->getSize() - 1)->getName() << endl;
	cout << "New Weight: " << c->inventory->getWeight() << "/" << c->inventory->getCapacity() << endl;
	cout << "Onward!" << endl;

	cout << "killed... Game Over" << endl;
}