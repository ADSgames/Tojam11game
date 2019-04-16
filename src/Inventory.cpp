#include "Inventory.h"

// Constructor
Inventory::Inventory () {

}

Inventory::~Inventory() {}

// Push item to contents (if it fits)
bool Inventory::addItem(Item *item, int quantity) {
  // Null item
  if (item == nullptr)
    return false;

  // Stack
  ItemStack *stk = findStack(item);
  if (stk != nullptr) {
    stk -> Add(quantity);
    return true;
  }

  // Find a new spot
  int emptyStk = findFirstEmpty();
  if (emptyStk != -1) {
    contents[emptyStk] -> SetItem(item, quantity);
    return true;
  }

  return false;
}

// Remove item at index
bool Inventory::removeItem (int index) {
  if (getStack(index) != nullptr) {
    contents[index] -> Clear();
    return true;
  }
  return false;
}

// Add a space
void Inventory::addSpace() {
  contents.push_back (new ItemStack());
}

// Gets item at index if exists
ItemStack* Inventory::getStack (int index) {
  if (index < getSize() && index >= 0)
    return contents[index];
  return nullptr;
}

// Just returns first item
ItemStack* Inventory::getFirstItem() {
  return getStack(0);
}

// FInd stack
ItemStack* Inventory::findStack(Item *item) {
  for (unsigned int i = 0; i < contents.size(); i++) {
    if (contents.at(i) && contents.at(i) -> GetItem()) {
      if (contents.at(i) -> GetItem() -> getID() == item -> getID()) {
        return contents.at(i);
      }
    }
  }
  return nullptr;
}

// Get first empty
int Inventory::findFirstEmpty() {
  for (unsigned int i = 0; i < contents.size(); i++) {
    if (contents.at(i) && !(contents.at(i) -> GetItem())) {
      return i;
    }
  }
  return -1;
}

// Current item count
int Inventory::getSize() {
  return contents.size();
}

// Clear all contents
void Inventory::empty() {
  contents.clear();

  for (int i = 0; i < getSize(); i++)
    contents.push_back (new ItemStack());
}