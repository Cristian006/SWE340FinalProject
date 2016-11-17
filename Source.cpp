#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>

using namespace std;

enum ItemType
{
	none = 0,
	wpn = 1,
	cnsmb = 2,
	qst = 3
};

enum WeaponType
{
	Primary,
	Secondary,
	Throwable,
	None
};

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
		for each (T* i in _objects)
		{
			w += i->getWeight();
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
			return true;
		}
		return false;
	}

	~Container(void)
	{

	}

private:
	vector<T*> _objects;
};

//ItemDatabase is a Singelton Class inheriting from the AbstractDatabase Class
class ItemDatabase : public AbstractDatabase<Item> {
private:
	static ItemDatabase *inst;
public:
	static ItemDatabase *instance();
};
ItemDatabase *ItemDatabase::inst = 0;
ItemDatabase *ItemDatabase::instance() {
	if (!inst) {
		inst = new ItemDatabase();
	}
	return inst;
}

int main() {
	ItemDatabase::instance()->add(new Weapon(0, "sword", wpn, 10, 1, 300));
	ItemDatabase::instance()->add(new Weapon(1, "axe", wpn, 10, 1, 300));
	ItemDatabase::instance()->add(new Consumable(2, "fruit", cnsmb, 10, 1, 300));

	cout << ItemDatabase::instance()->get(0)->getName() << endl;
	cout << ItemDatabase::instance()->get(2)->getName() << endl;

	//create an Inventory with a capacity of 30 items
	//TODO: Change capacity to be based on weight, not on total number of items
	Container<Item, 30> inventory;
	inventory.add(ItemDatabase::instance()->get(0));
	inventory.add(ItemDatabase::instance()->get(1));
	inventory.add(ItemDatabase::instance()->get(2));
	cout << inventory.get(0)->getName() << " " << inventory.getWeight();

	getchar();
}

