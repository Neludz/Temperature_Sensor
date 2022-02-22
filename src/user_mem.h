#ifndef USER_MEM_H_INCLUDED
#define USER_MEM_H_INCLUDED

#define RET_MEM_OK               0
#define RET_MEM_NOT_FOUND        1


void user_mem_init (void);
uint32_t user_mem_read_reg16 (uint16_t addr_id, uint16_t *reg16);
uint32_t user_mem_update_reg16 (uint16_t addr_id, uint16_t *data);

#endif /* USER_MEM_H_INCLUDED */
