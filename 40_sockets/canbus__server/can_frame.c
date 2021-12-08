#include "can_frame.h"

#include <stdint.h>
#include <linux/can.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


pdu_p create_pdu(void)
{
	pdu_p pdu = malloc(sizeof(*pdu));
	if (!pdu) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	memset(pdu, 0, sizeof(*pdu));
	return pdu;
}

void destroy_pdu(pdu_p *pdu)
{
	free(*pdu);
	*pdu = NULL;
}

void init_can_id(const pdu_t* pdu, uint16_t *id)
{
	*id = (uint16_t) pdu->id.id;
}

void init_can_dlc(const pdu_t* pdu, uint8_t *dlc)
{
	*dlc = (uint8_t) pdu->dlc;
}

void init_can_data(const pdu_t* pdu, uint8_t data[CAN_MAX_DLEN])
{
	uint8_t *ptr = data;
	uint8_t *psrc = (uint8_t*) &pdu->data.payload;
	for (int idx = 0; idx < CAN_MAX_DLEN; idx++) {
		ptr[idx] = psrc[idx];
	}
}

void init_pdu_from_can(pdu_t* pdu, const struct can_frame *frame)
{
	pdu->id.id = (uint16_t) frame->can_id & 0x7ff;
	pdu->dlc = (uint8_t) frame->can_dlc;
	pdu->data.payload = *(uint64_t*) &frame->data;
}

uint8_t get_pdu_dlc(pdu_p pdu)
{
	return pdu->dlc;
}

void set_pdu_dlc(pdu_p pdu, uint8_t dlc)
{
	pdu->dlc = dlc;
}

uint8_t get_id_dest(pdu_p pdu)
{
	return (uint8_t) pdu->id.init.dest;
}

void set_id_dest(pdu_p pdu, uint8_t destination)
{
	pdu->id.init.dest = destination & 0xff;
}

uint8_t get_id_prio(pdu_p pdu)
{
	return (uint8_t) pdu->id.init.prio;
}

void set_id_prio(pdu_p pdu, uint8_t prio)
{
	pdu->id.init.prio = prio;
}

uint8_t get_data_seqn(pdu_p pdu)
{
	return (uint8_t) pdu->data.init.seqn;
}

void set_data_seqn(pdu_p pdu, uint8_t seqn)
{
	pdu->data.init.seqn = seqn;
}

uint64_t get_data_payload(pdu_p pdu)
{
	return (uint64_t) pdu->data.init.data;
}

void set_data_payload(pdu_p pdu, uint64_t payload)
{
	pdu->data.init.data = payload;
}
