#ifndef NETCODE_COMMANDS
#define NETCODE_COMMANDS

#define NC_PACKET_SIZE 256

#define NC_CREATE_ROOM  "cr"
#define NC_LIST_ROOMS   "lr"
#define NC_GET_ROOM     "getr"
#define NC_ENTER_ROOM   "enterr"
#define NC_EXIT_ROOM    "exitr"

#define NC_SET_NAME     "setName"
#define NC_SET_SEED     "sseed"
#define NC_GET_SEED     "gseed"

#define NC_SEPARATOR        ';'
#define NC_COMMAND_END      '\t'
#define NC_OBJECT_SEPARATOR "|"
#define NC_VALID_KEY        "valid_key"
#define NC_INVALID_KEY      "invalid_key"

#define NC_SUCCESS          "success"
#define NC_ERROR            "error"

#endif
