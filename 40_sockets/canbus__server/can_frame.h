#ifndef CAN_FRAME_H
#define CAN_FRAME_H


#include <stdint.h>
#include <linux/can.h>

typedef union {
	struct {
		uint16_t dest:8;
		uint16_t prio:3;
		uint16_t forbidden:5;
	} init;
	uint16_t id;
} __attribute__((__packed__)) pdu_id_t;

typedef union {
	struct {
		uint8_t seqn;
		uint64_t data:56;
	} init;
	uint64_t payload;
} __attribute__((__packed__)) pdu_payload_t;


typedef struct pdu_s {
	pdu_id_t id;
	uint8_t dlc;
	pdu_payload_t data;
} pdu_t;
typedef struct pdu_s* pdu_p;

pdu_p create_pdu(void);
void destroy_pdu(pdu_p *pdu);

void init_can_id(const pdu_t* pdu, uint16_t *id);
void init_can_dlc(const pdu_t* pdu, uint8_t *dlc);
void init_can_data(const pdu_t* pdu, uint8_t data[CAN_MAX_DLEN]);

uint8_t get_pdu_dlc(pdu_p pdu);
void set_pdu_dlc(pdu_p pdu, uint8_t dlc);

uint8_t get_id_dest(pdu_p pdu);
void set_id_dest(pdu_p pdu, uint8_t destination);

uint8_t get_id_prio(pdu_p pdu);
void set_id_prio(pdu_p pdu, uint8_t prio);

uint8_t get_data_seqn(pdu_p pdu);
void set_data_seqn(pdu_p pdu, uint8_t seqn);

uint64_t get_data_payload(pdu_p pdu);
void set_data_payload(pdu_p pdu, uint64_t payload);


#endif /* CAN_FRAME_H */
