#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>
using namespace std;

enum StatType
{
	None = 0,
	Health = 1,
	Magic = 2,
	Stamina = 3,
	Armor = 4,
	Mana = 5,
	Speed = 6,
	InventoryCap = 7,
	Intellegence = 8,
	Strength = 9,
	Endurance = 10,
	Agility = 11,
	ArmorProtection = 12
};

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

enum UpgradeQuality {
	common,
	fair,
	great,
	amazing,
	best
};

const unsigned int MAX_INVENTORY_CAP = 99;

#pragma region Database Utils

template <class T>
class AbstractDatabase {
protected:
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
	virtual T* get(unsigned int index) = 0;
	
	//return the object with the given name
	virtual T* get(string name) = 0;

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

	virtual unsigned int getNextAvailableID() {
		return _objects.size();
	}
};

#pragma endregion

#pragma region ItemSystem

#pragma region BaseItems
class Item
{
public:
	//Empty Constructor
	Item() {}

	//Base Constructor
	Item(int id, string name, ItemType iT, int weight, int level, int cost) {
		_id = id;
		_name = name;
		_iType = iT;
		_weight = weight;
		_level = level;
		_cost = cost;
	};

	//Destructor
	~Item() {}

	//pretty much the same as isComposite
	virtual bool isUpgraded() = 0;
	
	//accept a visitor
	virtual void accept(class Visitor &v) = 0;

#pragma region GET AND SET METHODS
	virtual ItemType getType() {
		return none;
	}

	virtual string getName() {
		return _name;
	}

	int getID() {
		return _id;
	}

	void setName(string name) {
		_name = name;
	}

	virtual int getWeight() {
		return _weight;
	}

	void setWeight(unsigned int weight) {
		_weight = weight;
	}
#pragma endregion

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
	Weapon() : Item() {}
	Weapon(int id, string name, ItemType iT, int weight, int level, int cost) : Item(id, name, iT, weight, level, cost) {}
	Weapon(int id, string name, ItemType iT, int weight, int level, int cost, int durability, WeaponType weaponType) : Item(id, name, iT, weight, level, cost){
		_durability = durability;
		_currentDurablity = _durability;
		_weaponType = weaponType;
	}

	virtual bool isUpgraded() {
		return false;
	}

	virtual ItemType getType() {
		return weapon;
	}

	virtual int getDurability() {
		return _durability;
	}

	virtual int getCurrentDurability() {
		return _currentDurablity;
	}

	bool isEquippable() {
		if (getCurrentDurability() > 0) {
			return true;
		}
		return false;
	}

	virtual WeaponType getWeaponType() {
		return _weaponType;
	}

	void accept(Visitor &v);
private:
	int _durability;
	int _currentDurablity;
	WeaponType _weaponType;
};

class Consumable : public Item {
public:
	Consumable() : Item() {}
	Consumable(int id, string name, ItemType iT, int weight, int level, int cost) : Item(id, name, iT, weight, level, cost){}
	Consumable(int id, string name, ItemType iT, int weight, int level, int cost, int effectAmount) : Item(id, name, iT, weight, level, cost) {
		_effectAmount = effectAmount;
	}
	
	virtual bool isUpgraded() {
		return false;
	}

	virtual ItemType getType() {
		return consumable;
	}
	void accept(Visitor &v);
private:
	int _effectAmount;
};

class QuestObject : public Item {
public:
	QuestObject():Item(){}
	QuestObject(int id, string name, ItemType iT, int weight, int level, int cost) : Item(id, name, iT, weight, level, cost) {}
	QuestObject(int id, string name, ItemType iT, int weight, int level, int cost, int rarity) : Item(id, name, iT, weight, level, cost) {
		_rarity = rarity;
	}

	virtual ItemType getType() {
		return quest;
	}

	virtual bool isUpgraded() {
		return false;
	}

	void accept(Visitor &v);

private:
	int _rarity;
};

#pragma endregion

#pragma region ItemUpgrades

class Upgrade : public Item {
protected:
	Item* item;
	string upgradeName;
public:
	Upgrade(Item* i) {
		item = i;
	}
	
	virtual ItemType upgradeType() = 0;
	
	virtual string getName() {
		return  + "upgradeName " + item->getName();
	}

	void setUpName(string upName) {
		upgradeName = upName;
	}

	int getWeight() {
		return item->getWeight();
	}

	bool isUpgraded() {
		return true;
	}

	Item* remove() {
		return item;
	}

	//check for whether the Upgrade we're trying to add to the item is accepted.
	bool acceptedItem(ItemType iT) {
		return (upgradeType() == iT);
	}
};

class WeaponUpgrade : public Upgrade {
private:
	UpgradeQuality upQuality;
protected:
	int getWeaponDurability() {
		return ((Weapon*)item)->getDurability();
	}
public:
	WeaponUpgrade(Item* i) : Upgrade(i) {

	}
	
	WeaponUpgrade(UpgradeQuality uQ, Item* i) : Upgrade(i) {
		upQuality = uQ;
		switch (upQuality)
		{
		case common:
			setUpName("COMMONLY UPGRADED");
			break;
		case fair:
			setUpName("FAIRLY UPGRADED");
			break;
		case great:
			setUpName("GREATLY UPGRADED");
			break;
		case amazing:
			setUpName("AMAZINGLY UPGRADED");
			break;
		case best:
			setUpName("BEST UPGRADED");
			break;
		default:
			setUpName("ERROR UPGRADE");
			break;
		}
	}

	int getDurability(){
		int addedDurability = 0;
		switch (upQuality)
		{
		case common:
			addedDurability = 2;
			break;
		case fair:
			addedDurability = 4;
			break;
		case great:
			addedDurability = 6;
			break;
		case amazing:
			addedDurability = 8;
			break;
		case best:
			addedDurability = 10;
			break;
		default:
			addedDurability = 0;
			break;
		}
		return (getWeaponDurability() + addedDurability);
	}

	void accept(Visitor &v);

	ItemType upgradeType() {
		return weapon;
	}
};

class ConsumableUpgrade : public Upgrade {
public:
	ConsumableUpgrade(Item* i) : Upgrade(i) {

	}

	ConsumableUpgrade(UpgradeQuality uQ, Item* i) : Upgrade(i) {
		switch (uQ)
		{
		case common:
			setUpName("COMMONLY UPGRADED");
			break;
		case fair:
			setUpName("FAIRLY UPGRADED");
			break;
		case great:
			setUpName("GREATLY UPGRADED");
			break;
		case amazing:
			setUpName("AMAZINGLY UPGRADED");
			break;
		case best:
			setUpName("BEST UPGRADED");
			break;
		default:
			setUpName("ERROR UPGRADE");
			break;
		}
	}

	void accept(Visitor &v);

	ItemType upgradeType() {
		return consumable;
	}
};

class QuestUpgrade : public Upgrade {
public:
	QuestUpgrade(Item* i) : Upgrade(i) {

	}

	QuestUpgrade(UpgradeQuality uQ, Item* i) : Upgrade(i) {
		switch (uQ)
		{
		case common:
			setUpName("COMMONLY UPGRADED");
			break;
		case fair:
			setUpName("FAIRLY UPGRADED");
			break;
		case great:
			setUpName("GREATLY UPGRADED");
			break;
		case amazing:
			setUpName("AMAZINGLY UPGRADED");
			break;
		case best:
			setUpName("BEST UPGRADED");
			break;
		default:
			setUpName("ERROR UPGRADE");
			break;
		}
	}

	void accept(Visitor &v);

	ItemType upgradeType() {
		return quest;
	}
};

#pragma endregion

class Visitor {
public:
	virtual void visit(Weapon *i) = 0;
	virtual void visit(Consumable *i) = 0;
	virtual void visit(QuestObject *i) = 0;
	virtual void visit(WeaponUpgrade *i) = 0;
	virtual void visit(ConsumableUpgrade *i) = 0;
	virtual void visit(QuestUpgrade *i) = 0;
};


void Weapon::accept(Visitor &v) {
	v.visit(this);
}

void Consumable::accept(Visitor &v) {
	v.visit(this);
}

void QuestObject::accept(Visitor &v) {
	v.visit(this);
}

void WeaponUpgrade::accept(Visitor &v) {
	v.visit(this);
}

void ConsumableUpgrade::accept(Visitor &v) {
	v.visit(this);
}

void QuestUpgrade::accept(Visitor &v) {
	v.visit(this);
}

class Weight : public Visitor{

	void visit(Weapon *i) {
		totalWeight += i->getWeight();
	}

	void visit(Consumable *i) {
		totalWeight += i->getWeight();
	}

	void visit(QuestObject *i) {
		totalWeight += i->getWeight();
	}

	void visit(WeaponUpgrade *i) {
		totalWeight += i->getWeight();
	}

	void visit(ConsumableUpgrade *i) {
		totalWeight += i->getWeight();
	}

	void visit(QuestUpgrade *i) {
		totalWeight += i->getWeight();
	}

	void reset() {
		totalWeight = 0;
	}
public:
	int getWeight() {
		return totalWeight;
	}

protected:
	int totalWeight = 0;

};

//ItemDatabase is a Singelton Class inheriting from the AbstractDatabase Class that will hold all of the available items
class ItemDatabase : public AbstractDatabase<Item> {
public:
	Item* get(unsigned int index) {
		if (index < inst->_objects.size()) {
			return _objects.at(index);
		}
		return NULL;
	}

	Item* get(string name) {
		for each(Item* i in _objects) {
			if (i->getName() == name) {
				return i;
			}
		}
		return NULL;
	}

private:
	static ItemDatabase *inst;
	static void populate() {
		//Weapons
		inst->add(new Weapon(inst->getNextAvailableID(), "sword", weapon, 2, 1, 300, 20, primary));
		inst->add(new Weapon(inst->getNextAvailableID(), "axe", weapon, 3, 1, 450, 30, primary));
		inst->add(new Weapon(inst->getNextAvailableID(), "spear", weapon, 1, 1, 250, 20, throwable));

		//Consumables
		inst->add(new Consumable(inst->getNextAvailableID(), "potion", consumable, 1, 1, 15, 10));
		inst->add(new Consumable(inst->getNextAvailableID(), "apple", consumable, 1, 1, 15, 8));
		inst->add(new Consumable(inst->getNextAvailableID(), "carrot", consumable, 1, 1, 10, 7));

		//QuestObjects
		inst->add(new QuestObject(inst->getNextAvailableID(), "scarf", quest, 1, 1, 200, 6));
		inst->add(new QuestObject(inst->getNextAvailableID(), "key", quest, 1, 1, 200, 6));
		inst->add(new QuestObject(inst->getNextAvailableID(), "orb", quest, 1, 1, 200, 6));
	}
public:
	static ItemDatabase *instance();
};

//Setting up the Singelton Design Pattern for the Item Database
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
protected:
	vector<T*> _objects;
public:
	Container(void) {}

	//check if inventory is empty
	bool empty() const {
		return _objects.empty();
	}

	// Returns true if the item was added successfully, false if the container was full
	bool add(T* item)
	{
		//int w = item->getWeight() + getWeight();
		if (capacity > _objects.size())
		{
			_objects.push_back(item);
			onObjectAdded(item);
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
		Weight weight;
		//Pass in the visitor to each Object to add up the total weight of the inventory
		if (_objects.size() > 0) {
			for each (T* i in _objects)
			{
				i->accept(weight);
			}
		}
		return weight.getWeight();
	}

	virtual T* get(unsigned int index) = 0;

	virtual T* get(string name) = 0;

	void set(unsigned int index, T* newObject) {
		if (index < _objects.size()) {
			_objects[index] = newObject;
		}
		else return;
	}

	// Returns true if the index was removed, false if the index was out of bounds
	bool remove(unsigned int index)
	{
		if (index < _objects.size())
		{
			onObjectRemoved(get(index));
			_objects.erase(_objects.begin() + index);
			return true;
		}
		return false;
	}

	void print() {
		cout << "\nINVENTORY---------------" << endl;
		for each (T* i in _objects)
		{
			cout << i->getName() << endl;
		}
		cout << "------------------------\n" << endl;
	}

	~Container(void)
	{

	}

private:
	virtual void onObjectAdded(T* i) = 0;
	virtual void onObjectRemoved(T* i) = 0;
};

//Inventory is using composite design pattern because it holds upgraded items that branch out further
//encapsulating the base Container Class
template<unsigned int cap>
class Inventory : public Container<Item, cap> {
public:
	Item* get(unsigned int index) {
		if (index < _objects.size()) {
			return _objects.at(index);
		}
		return NULL;
	}

	Item* get(string name) {
		for each(Item* i in _objects) {
			if (i->getName() == name) {
				return i;
			}
		}
		return NULL;
	}

	//Adding an upgrade ("decorator") to the item
	void upgradeItem(int index, UpgradeQuality upgradeQuality) {
		Item* newItem;
		
		//if the item has been previously upgraded
		//remove the previous upgrade and continue with the new overriding upgrade.
		if (get(index)->isUpgraded()) {
			downgradeItem(index);
		}
		
		switch (get(index)->getType()) {
		case weapon:
			cout << "UPGRADING WEAPON: " << get(index)->getName() << endl;
			newItem = new WeaponUpgrade(upgradeQuality, get(index));
			set(index, newItem);
			break;
		case consumable:
			cout << "UPGRADING CONSUMABLE: " << get(index)->getName() << endl;
			newItem = new ConsumableUpgrade(upgradeQuality, get(index));
			set(index, newItem);
			break;
		case quest:
			cout << "UPGRADING QUEST OBJECT: " << get(index)->getName() << endl;
			newItem = new QuestUpgrade(upgradeQuality, get(index));
			set(index, newItem);
			break;
		default:
			cout << "ERROR!!!!!!!!!!!";
			break;
		}
	}

	//Removing an upgrade ("decoration") to the item
	void downgradeItem(int index) 
	{
		//if item hasn't been upgraded - return
		if (!get(index)->isUpgraded()) { return; }
		
		set(index, ((Upgrade*)get(index))->remove());
	}

private:
	//Special Callback functions for when objects are added or removed from the Inventory
	void onObjectAdded(Item* i) 
	{
		cout << "Added Object" << endl;
	}

	void onObjectRemoved(Item* i) 
	{
		cout << "Removed Object" << endl;
	}
};

#pragma endregion

#pragma region CharacterSystem
//Class to hold all of the base Character's variables
//Private Class Data Design Pattern
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

	Inventory<MAX_INVENTORY_CAP>* inventory = new Inventory<MAX_INVENTORY_CAP>();
};

#pragma endregion
/*
#pragma region STAT SYSTEM

class Stat {
private:
	string _statName;
	int _statBaseValue;

public:
	Stat() {
		setStatName("");
		setStatBaseValue(0);
	}

	Stat(string name, int val) {
		setStatName(name);
		setStatBaseValue(val);
	}
	
	~Stat() {}

	string getStatName() {
		return _statName;
	}
	void setStatName(string name) {
		_statName = name;
	}

	virtual int getStatValue(){
		return getStatBaseValue();
	}

	virtual int getStatBaseValue() {
		return _statBaseValue;
	}

	virtual int setStatBaseValue(int value) {
		_statBaseValue = value;
	}

	virtual string print() {
		return "Stat: " + getStatName() + " Value: " + to_string(getStatValue());
	};
};

class StatCollection {
private:
	map<StatType, Stat>* _statDictionary; //= new map<StatType, Stat*>();
protected:
	virtual void ConfigureStats() = 0;

public:
	StatCollection() {
		_statDictionary = new map<StatType, Stat>();
	}
	map<StatType, Stat>* getStatDict() {
		if (_statDictionary->empty()) {
			_statDictionary = new map<StatType, Stat>();
			ConfigureStats();
		}
		return _statDictionary;
	}

	bool ContainsStat(StatType statType) {
		return getStatDict()->count(statType);
	}

	template <typename T>
	T getStat<T>(StatType type){
		return getStat(type)
	}
};

#pragma endregion
*/
//Examples of basic system mechanics that would be in place if there was a UI
int main() {
	cout << "New Game\n" << endl;
	
	//creating a new character 
	Character* c = new Character("johny", "apple seed collector");
	
	cout << "New Character: " << c->getName() << endl;
	cout << "Starting Journey...\n" << endl;

	cout << "Walking across the desert...\n" << endl;
	
	//adding item from the available database into the character's inventory
	c->inventory->add(ItemDatabase::instance()->get("sword"));
	
	cout << "Item Collected: " << c->inventory->get(c->inventory->getSize() - 1)->getName() << endl;
	cout << "New Weight: " << c->inventory->getWeight() << "/" << c->inventory->getCapacity() << endl;
	cout << "Onward!\n" << endl;
	
	cout << "Finally made it out of the desert!\n\nEntering the forest\n" << endl;

	//adding item from the available database into the character's inventory
	c->inventory->add(ItemDatabase::instance()->get("axe"));

	cout << "Item Collected: " << c->inventory->get(c->inventory->getSize() - 1)->getName() << endl;
	cout << "New Weight: " << c->inventory->getWeight() << "/" << c->inventory->getCapacity() << endl;
	cout << "Onward!\n" << endl;

	//adding item from the available database into the character's inventory
	c->inventory->add(ItemDatabase::instance()->get("potion"));

	cout << "Item Collected: " << c->inventory->get(c->inventory->getSize() - 1)->getName() << endl;
	cout << "New Weight: " << c->inventory->getWeight() << "/" << c->inventory->getCapacity() << endl;
	cout << "Onward!" << endl;

	c->inventory->print();

	//Upgrading an Item using the Decorator Design Pattern
	c->inventory->upgradeItem(0, common);

	cout << "UPGRADED SUCCESSFULLY- ISUPGRADED:" << boolalpha << c->inventory->get(0)->isUpgraded() << endl;

	c->inventory->print();

	//Downgrading an Item by removing the Decoration
	c->inventory->downgradeItem(0);

	cout << "DOWNGRADED SUCCESSFULLY- ISUPGRADED:" << boolalpha << c->inventory->get(0)->isUpgraded() << endl;

	c->inventory->print();

	cout << "OVERRIDING AN UPGRADE WITH A BETTER QUALITY ONE----------------" << endl;

	//upgraded a weapon
	c->inventory->upgradeItem(0, common);

	cout << "UPGRADED SUCCESSFULLY- ISUPGRADED:" << boolalpha << c->inventory->get(0)->isUpgraded() << endl;


	if (c->inventory->get(0)->isUpgraded()) {
		//The weapon that was upgraded added more points to it's durability
		cout << "UPGRADED DURABILITY: " << ((WeaponUpgrade*)c->inventory->get(0))->getDurability();
	}

	c->inventory->print();

	//re-upgrading the same weapon with a better upgrade quality which overrides the last upgrade by deleting the previous decorator and adding a new one
	c->inventory->upgradeItem(0, amazing);

	cout << "UPGRADED SUCCESSFULLY- ISUPGRADED:" << boolalpha << c->inventory->get(0)->isUpgraded() << endl;

	if (c->inventory->get(0)->isUpgraded()) {
		//The overrided upgraded weapon now has a better durability
		cout << "UPGRADED DURABILITY: " << ((WeaponUpgrade*)c->inventory->get(0))->getDurability();
	}

	c->inventory->print();

	//end of example use cases
	cout << "killed... Game Over" << endl;
	getchar();
}