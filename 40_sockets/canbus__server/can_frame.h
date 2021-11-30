#ifndef CAN_FRAME_H
#define CAN_FRAME_H


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
typedef struct pdu_s *pdu_p;


/* // TODO provide access to the particular fileds
void get_id_dest();
void set_id_dest();

void get_data_seqn();
void set_data_seqn();
...
// */

#endif /* CAN_FRAME_H */
