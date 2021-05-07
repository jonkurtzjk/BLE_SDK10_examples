
#ifndef GAP_COMMANDS_H_
#define GAP_COMMANDS_H_

#include "ble_gap.h"

typedef struct{
       uint8_t hdr;
       bool            scan_start;
       gap_scan_type_t scan_type;
       gap_scan_mode_t scan_mode;
       uint16_t        scan_interval;
       uint16_t        scan_window;
       bool            scan_filt_wlist;
       bool            scan_filt_dupl;
}gap_scan_cmd;

typedef struct{
     bd_address_t addr;
     gap_conn_params_t cp;
}gap_connect_cmd;

typedef struct{
     uint16_t  conn_idx;
     ble_hci_error_t reason;
}gap_disc_cmd;


typedef struct{
        uint16_t conn_idx;
        gap_conn_params_t gap_cp;
}gap_set_cp_cmd;

typedef struct{
        gap_io_cap_t io_cap;
}gap_set_io_cap_cmd;

typedef struct{
        uint16_t conn_idx;
        bool     bond;
}gap_pair_cmd;

typedef struct
{
      uint16_t conn_idx;
      bool     accept;
}gap_yes_no_reply_cmd_t;

typedef struct
{
        uint16_t        conn_idx;
        bool            accept;
        uint32_t        passkey;
}gap_pk_reply_cmd_t;

typedef struct
{
        bd_address_t address;
}clear_bond_cmd_t;

const char *format_bd_address(const bd_address_t *addr);
void handle_gap_pair(gap_pair_cmd *cmd);
void handle_gap_pk_reply(gap_pk_reply_cmd_t *cmd);
void handle_gap_yes_no_reply(gap_yes_no_reply_cmd_t *cmd);
void handle_gap_set_io_cmd(gap_set_io_cap_cmd *cmd);
void handle_gap_cancel_conn_cmd(void);
void handle_gap_set_cp(gap_set_cp_cmd *cmd);
void handle_gap_disc(gap_disc_cmd *cmd);
void gap_get_cmd(msg *message);
void handle_gap_connect(gap_connect_cmd *cmd);
void handle_gap_scan_request( gap_scan_cmd *cmd );
void handle_clear_bond(clear_bond_cmd_t *cmd);
void handle_get_bonds(void);



#endif

