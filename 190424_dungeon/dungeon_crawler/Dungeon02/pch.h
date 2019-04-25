// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

#include <vector>
#include <array>
#include <map>

#include <string>
#include <ctime>

// manager
#include "ObjectPool.h"
#include "TableGetter.h"
// 
#include "Command.h"

// base
#include "Pos.h"
#include "Composite.h"
#include "CreatedObj.h"
#include "InfoBase.h"
#include "CreatedObjCmd.h"


// Calculater
#include "Radius.h"

// Obj
#include "Skill.h"
#include "Stats.h"


#include "Buff.h"

// Character
#include "Character.h"
#include "CharacterCmd.h"
#include "Player.h"
#include "PlayerCmd.h"

#include "Item.h"


// DM
#include "DungeonMaster.h"

// table




// TODO: add headers that you want to pre-compile here



#endif //PCH_H
