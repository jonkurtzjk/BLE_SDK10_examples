#ifndef GATT_COMMANDS_H_
#define GATT_COMMANDS_H_


typedef struct{
        uint16_t conn_idx;
        att_uuid_t *at_uuid;
}gatt_browse_cmd;

typedef struct{
        uint16_t conn_idx;
        uint16_t handle;
        uint16_t offset;
        uint16_t length;
        uint8_t  value[20];

}gatt_write_cmd;

typedef struct{

        uint16_t conn_idx;
        uint16_t handle;
        uint16_t offset;

}gatt_read_cmd;

void handle_gatt_read_cmd(gatt_read_cmd *evt);
void handle_gatt_write_cmd(gatt_write_cmd *evt);
void handle_gatt_write_no_rsp_cmd(gatt_write_cmd *evt);
void handle_gatt_browse_cmd(gatt_browse_cmd *evt);

#endif
