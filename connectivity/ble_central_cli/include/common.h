#ifndef COMMON_H_
#define COMMON_H_

#include "msg_queues.h"
#include "osal.h"

#define AT_CMD_AVAILABLE        (1<<27)
#define AT_ECHO                 (1<<26)
#define AT_NUM_GAP_COMMANDS     (22)
extern msg_queue at_cmd_queue;

typedef struct{
        char header[3];
        char command[16];
        char params[5];
}AT_CMD_DATA;

typedef struct{
        uint8_t hdr;
}at_cmd_hdr;

enum {
        GAPSCAN,
        GAPCONNECT,
        ECHO,
        GAPDISC,
        GATTBROWSE,
        GAPSETCP,
        GAPCANCELCONNECT,
        GATTWRITE,
        GATTREAD,
        SETBAUD,
        GAPSETIOCAP,
        GAPPAIR,
        RESET,
        GATTWRITENORESP,
        IMGBLOCKUPDATE,
        IMGERASEALL,
        PASSKEYENTRY,
        YESNOENTRY,
        IMGCHECK,
        SUOTASTART,
        CLEARBOND,
        GETBONDS
};



OS_TASK command_parser_start_task(const uint8 prio, OS_TASK handle);


#undef CUSTOM_CENTRAL


#endif
