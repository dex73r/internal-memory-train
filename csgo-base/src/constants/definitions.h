#pragma once
#include "../sdk.h"
typedef unsigned long ulong;

typedef float matrix3x4_t[3][4];

class player_info_t
{
  public:
    char pad_0x0000[0x10];  //0x0000
    char name[64];          //0x0010
    char pad_0x0050[0x40];  //0x0050
    __int32 userID;         //0x0090
    char guid[32];          //0x0094
    char pad_0x00B4[0x180]; //0x00B4
};

enum SolidType_t
{
    SOLID_NONE = 0,     // no solid model
    SOLID_BSP = 1,      // a BSP tree
    SOLID_BBOX = 2,     // an AABB
    SOLID_OBB = 3,      // an OBB (not implemented yet)
    SOLID_OBB_YAW = 4,  // an OBB, constrained so that it can only yaw
    SOLID_CUSTOM = 5,   // Always call into the entity for tests
    SOLID_VPHYSICS = 6, // solid vphysics object, get vcollide from the model and collide with that
    SOLID_LAST,
};

enum SolidFlags_t
{
    FSOLID_CUSTOMRAYTEST = 0x0001, // Ignore solid type + always call into the entity for ray tests
    FSOLID_CUSTOMBOXTEST = 0x0002, // Ignore solid type + always call into the entity for swept box tests
    FSOLID_NOT_SOLID = 0x0004,     // Are we currently not solid?
    FSOLID_TRIGGER = 0x0008,       // This is something may be collideable but fires touch functions
    // even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
    FSOLID_NOT_STANDABLE = 0x0010,        // You can't stand on this
    FSOLID_VOLUME_CONTENTS = 0x0020,      // Contains volumetric contents (like water)
    FSOLID_FORCE_WORLD_ALIGNED = 0x0040,  // Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
    FSOLID_USE_TRIGGER_BOUNDS = 0x0080,   // Uses a special trigger bounds separate from the normal OBB
    FSOLID_ROOT_PARENT_ALIGNED = 0x0100,  // Collisions are defined in root parent's local coordinate space
    FSOLID_TRIGGER_TOUCH_DEBRIS = 0x0200, // This trigger will touch debris objects

    FSOLID_MAX_BITS = 10
};

enum
{
    IN_ATTACK = (1 << 0),
    IN_JUMP = (1 << 1),
    IN_DUCK = (1 << 2),
    IN_FORWARD = (1 << 3),
    IN_BACK = (1 << 4),
    IN_USE = (1 << 5),
    IN_CANCEL = (1 << 6),
    IN_LEFT = (1 << 7),
    IN_RIGHT = (1 << 8),
    IN_MOVELEFT = (1 << 9),
    IN_MOVERIGHT = (1 << 10),
    IN_ATTACK2 = (1 << 11),
    IN_RUN = (1 << 12),
    IN_RELOAD = (1 << 13),
    IN_ALT1 = (1 << 14),
    IN_ALT2 = (1 << 15),
    IN_SCORE = (1 << 16),
    IN_SPEED = (1 << 17),
    IN_WALK = (1 << 18),
    IN_ZOOM = (1 << 19),
    IN_WEAPON1 = (1 << 20),
    IN_WEAPON2 = (1 << 21),
    IN_BULLRUSH = (1 << 22)
};
#define FL_ONGROUND (1 << 0)   // At rest / on the ground
#define FL_DUCKING (1 << 1)    // Player flag -- Player is fully crouched
#define FL_WATERJUMP (1 << 2)  // player jumping out of water
#define FL_ONTRAIN (1 << 3)    // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN (1 << 4)     // Indicates the entity is standing in rain
#define FL_FROZEN (1 << 5)     // Player is frozen for 3rd person camera
#define FL_ATCONTROLS (1 << 6) // Player can't move, but keeps key inputs for controlling another entity
#define FL_CLIENT (1 << 7)     // Is a player
#define FL_FAKECLIENT (1 << 8) // Fake client, simulated server side; don't send network messages to them
// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define FL_INWATER (1 << 9) // In water

// NOTE if you move things up, make sure to change this value
#define PLAYER_FLAG_BITS 10

#define FL_FLY (1 << 10)  // Changes the SV_Movestep() behavior to not need to be on ground
#define FL_SWIM (1 << 11) // Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
#define FL_CONVEYOR (1 << 12)
#define FL_NPC (1 << 13)
#define FL_GODMODE (1 << 14)
#define FL_NOTARGET (1 << 15)
#define FL_AIMTARGET (1 << 16)     // set if the crosshair needs to aim onto the entity
#define FL_PARTIALGROUND (1 << 17) // not all corners are valid
#define FL_STATICPROP (1 << 18)    // Eetsa static prop!
#define FL_GRAPHED (1 << 19)       // worldgraph has this ent listed as something that blocks a connection
#define FL_GRENADE (1 << 20)
#define FL_STEPMOVEMENT (1 << 21)          // Changes the SV_Movestep() behavior to not do any processing
#define FL_DONTTOUCH (1 << 22)             // Doesn't generate touch functions, generates Untouch() for anything it was touching when this flag was set
#define FL_BASEVELOCITY (1 << 23)          // Base velocity has been applied this frame (used to convert base velocity into momentum)
#define FL_WORLDBRUSH (1 << 24)            // Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
#define FL_OBJECT (1 << 25)                // Terrible name. This is an object that NPCs should see. Missiles, for example.
#define FL_KILLME (1 << 26)                // This entity is marked for death -- will be freed by game DLL
#define FL_ONFIRE (1 << 27)                // You know...
#define FL_DISSOLVING (1 << 28)            // We're dissolving!
#define FL_TRANSRAGDOLL (1 << 29)          // In the process of turning into a client side ragdoll.
#define FL_UNBLOCKABLE_BY_PLAYER (1 << 30) // pusher that can't be blocked by the player

enum ClientFrameStage_t
{
    FRAME_UNDEFINED = -1, // (haven't run any frames yet)
    FRAME_START,

    // A network packet is being recieved
    FRAME_NET_UPDATE_START,
    // Data has been received and we're going to start calling PostDataUpdate
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    // Data has been received and we've called PostDataUpdate on all data recipients
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    // We've received all packets, we can now do interpolation, prediction, etc..
    FRAME_NET_UPDATE_END,

    // We're about to start rendering the scene
    FRAME_RENDER_START,
    // We've finished rendering the scene.
    FRAME_RENDER_END
};

// ConVar flags
#define FCVAR_NONE 0

#define FCVAR_UNREGISTERED (1 << 0)
#define FCVAR_DEVELOPMENTONLY (1 << 1)
#define FCVAR_GAMEDLL (1 << 2)
#define FCVAR_CLIENTDLL (1 << 3)
#define FCVAR_HIDDEN (1 << 4)

#define FCVAR_PROTECTED (1 << 5)
#define FCVAR_SPONLY (1 << 6)
#define FCVAR_ARCHIVE (1 << 7)
#define FCVAR_NOTIFY (1 << 8)
#define FCVAR_USERINFO (1 << 9)
#define FCVAR_CHEAT (1 << 14)

#define FCVAR_PRINTABLEONLY (1 << 10)
#define FCVAR_UNLOGGED (1 << 11)
#define FCVAR_NEVER_AS_STRING (1 << 12)
#define FCVAR_RELEASE (1 << 19)

#define FCVAR_REPLICATED (1 << 13)
#define FCVAR_DEMO (1 << 16)
#define FCVAR_DONTRECORD (1 << 17)

#define FCVAR_NOT_CONNECTED (1 << 22)
#define FCVAR_ARCHIVE_XBOX (1 << 24)
#define FCVAR_SERVER_CAN_EXECUTE (1 << 28)
#define FCVAR_SERVER_CANNOT_QUERY (1 << 29)
#define FCVAR_CLIENTCMD_CAN_EXECUTE (1 << 30)