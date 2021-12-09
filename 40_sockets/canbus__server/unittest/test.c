/*
  cunit - tests
 */


#include "can_interface.h"
#include "can_frame.h"
#include "test.h"

#include <linux/can.h>
#include <stdlib.h>
#include <time.h>

#include <mcheck.h> /* mcheck */


void test_pdu_id(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint8_t dest = 0x23;
	set_id_dest(pdu, dest);

	uint8_t prio = 0x01;
	set_id_prio(pdu, prio);

	uint16_t id;
	init_can_id(pdu, &id);
	CU_ASSERT(id == (prio <<8 | dest));

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_pdu_dlc(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint8_t dlc = 8;
	set_pdu_dlc(pdu, dlc);

	uint8_t ret_dlc;
	ret_dlc = get_pdu_dlc(pdu);
	CU_ASSERT(dlc == ret_dlc);

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_pdu_id_dest0(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint8_t dest = 0x00;
	set_id_dest(pdu, dest);

	uint16_t id;
	init_can_id(pdu, &id);
	CU_ASSERT(id == 0x000);

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_pdu_id_dest1(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint8_t dest = 0xff;
	set_id_dest(pdu, dest);

	uint16_t id;
	init_can_id(pdu, &id);
	CU_ASSERT(id == 0x0ff);

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_pdu_id_prio0(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint8_t prio = 0x0;
	set_id_dest(pdu, prio);

	uint16_t id;
	init_can_id(pdu, &id);
	CU_ASSERT(id == 0x000);

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_pdu_id_prio1(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint8_t prio = 0x7;
	set_id_prio(pdu, prio);

	uint16_t id = 0x000;
	init_can_id(pdu, &id);
	CU_ASSERT(id == 0x700);

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_pdu_data_seqn0(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint8_t seqn = 0x00;
	set_data_seqn(pdu, seqn);

	uint8_t data[8];
	memset(data, 0, sizeof(data));
	init_can_data(pdu, data);
	CU_ASSERT(data[0] == 0x00);
	CU_ASSERT(data[1] == 0x00);
	CU_ASSERT(data[2] == 0x00);
	CU_ASSERT(data[3] == 0x00);
	CU_ASSERT(data[4] == 0x00);
	CU_ASSERT(data[5] == 0x00);
	CU_ASSERT(data[6] == 0x00);
	CU_ASSERT(data[7] == 0x00);

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_pdu_data_seqn1(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint64_t payload = 0x00ffffffffffffff;
	set_data_payload(pdu, payload);

	uint8_t data[8];
	memset(data, 0, sizeof(data));
	init_can_data(pdu, data);
	CU_ASSERT(data[0] == 0x00);
	CU_ASSERT(data[1] == 0xff);
	CU_ASSERT(data[2] == 0xff);
	CU_ASSERT(data[3] == 0xff);
	CU_ASSERT(data[4] == 0xff);
	CU_ASSERT(data[5] == 0xff);
	CU_ASSERT(data[6] == 0xff);
	CU_ASSERT(data[7] == 0xff);

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_pdu_data_seqn2(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint64_t payload = 0x00123456789abcde;
	set_data_payload(pdu, payload);

	uint8_t data[8];
	memset(data, 0, sizeof(data));
	init_can_data(pdu, data);
	CU_ASSERT(data[0] == 0x00);
//	fprintf(stderr, "data[0] 0x%02x\n", data[0]);
	CU_ASSERT(data[1] == 0xde);
//	fprintf(stderr, "data[1] 0x%02x\n", data[1]);
	CU_ASSERT(data[2] == 0xbc);
//	fprintf(stderr, "data[2] 0x%02x\n", data[2]);
	CU_ASSERT(data[3] == 0x9a);
//	fprintf(stderr, "data[3] 0x%02x\n", data[3]);
	CU_ASSERT(data[4] == 0x78);
//	fprintf(stderr, "data[4] 0x%02x\n", data[4]);
	CU_ASSERT(data[5] == 0x56);
//	fprintf(stderr, "data[5] 0x%02x\n", data[5]);
	CU_ASSERT(data[6] == 0x34);
//	fprintf(stderr, "data[6] 0x%02x\n", data[6]);
	CU_ASSERT(data[7] == 0x12);
//	fprintf(stderr, "data[7] 0x%02x\n", data[7]);

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_pdu_frame(void)
{
	pdu_p pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	uint8_t dest = 0x23;
	set_id_dest(pdu, dest);

	uint8_t prio = 0x1;
	set_id_prio(pdu, prio);

	uint8_t seqn = 0x7c;
	set_data_seqn(pdu, seqn);

	uint8_t dlc = CAN_MAX_DLEN;
	set_pdu_dlc(pdu, dlc);

	uint64_t payload = 0x123456789abcde;
	set_data_payload(pdu, payload);

	// verification
	uint16_t id;
	init_can_id(pdu, &id);
	CU_ASSERT(id == 0x123);

	uint8_t ret_dlc = 8;
	init_can_dlc(pdu, &ret_dlc);
	CU_ASSERT(CAN_MAX_DLEN == ret_dlc);

	uint8_t data[8];
	memset(data, 0, sizeof(data));
	init_can_data(pdu, data);
	CU_ASSERT(data[0] == 0x7c);
//	fprintf(stderr, "data[0] 0x%02x\n", data[0]);  
	CU_ASSERT(data[1] == 0xde);
//	fprintf(stderr, "data[1] 0x%02x\n", data[1]);  
	CU_ASSERT(data[2] == 0xbc);
//	fprintf(stderr, "data[2] 0x%02x\n", data[2]);  
	CU_ASSERT(data[3] == 0x9a);
//	fprintf(stderr, "data[3] 0x%02x\n", data[3]);  
	CU_ASSERT(data[4] == 0x78);
//	fprintf(stderr, "data[4] 0x%02x\n", data[4]);  
	CU_ASSERT(data[5] == 0x56);
//	fprintf(stderr, "data[5] 0x%02x\n", data[5]);  
	CU_ASSERT(data[6] == 0x34);
//	fprintf(stderr, "data[6] 0x%02x\n", data[6]);  
	CU_ASSERT(data[7] == 0x12);
//	fprintf(stderr, "data[7] 0x%02x\n", data[7]);  

	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
}

void test_queue_empty(void)
{
	int res = FALSE;
	CU_ASSERT(FALSE == res);
	res = tx__empty();
	CU_ASSERT(TRUE == res);
	res = rx__empty();
	CU_ASSERT(TRUE == res);
}

void test_canif_interface(void)
{
	int ret;

	// 1. startup and teardown
	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);

	// 2. startup and teardown
	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);

	// 3. startup and teardown
	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);
}

void test_tx_single(void)
{
	int ret = -1;
	pdu_p pdu;
	uint8_t dest;
	uint8_t prio;
	uint8_t seqn;
	uint64_t payload;
	uint8_t dlc;
	struct can_frame *frame = NULL;

	pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	dest = 0x23;
	set_id_dest(pdu, dest);

	prio = 0x1;
	set_id_prio(pdu, prio);

	seqn = 0x7c;
	set_data_seqn(pdu, seqn);

	payload = 0x123456789abcde;
	set_data_payload(pdu, payload);

	dlc = CAN_MAX_DLEN;
	set_pdu_dlc(pdu, dlc);

	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);
	sleep(1); /* delay: listeners have to be ready first */

	// send
	canif__send(pdu);
	sleep(1); /* delay: pdu has to be processed to test framework */

	// verification
	CU_ASSERT(test__tx_nbytes == CAN_MAX_DLEN);

	frame = test__frame;
	dbg_frame(__func__, frame);

	CU_ASSERT((uint16_t)frame->can_id == 0x123);
	CU_ASSERT(frame->can_dlc == CAN_MAX_DLEN);
	CU_ASSERT(frame->data[0] == 0x7c);
	CU_ASSERT(frame->data[1] == 0xde);
	CU_ASSERT(frame->data[2] == 0xbc);
	CU_ASSERT(frame->data[3] == 0x9a);
	CU_ASSERT(frame->data[4] == 0x78);
	CU_ASSERT(frame->data[5] == 0x56);
	CU_ASSERT(frame->data[6] == 0x34);
	CU_ASSERT(frame->data[7] == 0x12);

	// cleanup
	free(frame); frame = NULL;
	test__frame = NULL;
	destroy_pdu(&pdu);

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);
}

void test_tx_multiple(void)
{
	int ret = -1;
	pdu_p pdu;
	uint8_t dest;
	uint8_t prio;
	uint8_t seqn;
	uint64_t payload;
	uint8_t dlc;
	struct can_frame *frame = NULL;

	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);
	sleep(1); /* delay: listeners have to be ready first */

	{
		// 1. frame
		pdu = create_pdu();
		CU_ASSERT(NULL != pdu);

		dest = 0x23;
		set_id_dest(pdu, dest);

		prio = 0x1;
		set_id_prio(pdu, prio);

		seqn = 0x71;
		set_data_seqn(pdu, seqn);

		payload = 0xaaaaaaaaaaaaaa;
		set_data_payload(pdu, payload);

		dlc = CAN_MAX_DLEN;
		set_pdu_dlc(pdu, dlc);

		canif__send(pdu);
		sleep(1); /* delay: pdu has to be processed to test framework */

		CU_ASSERT(test__tx_nbytes == CAN_MAX_DLEN);

		frame = test__frame;
		dbg_frame(__func__, frame);
		CU_ASSERT((uint16_t)frame->can_id == 0x123);
		CU_ASSERT(frame->can_dlc == CAN_MAX_DLEN);
		CU_ASSERT(frame->data[0] == 0x71);
		CU_ASSERT(frame->data[1] == 0xaa);
		CU_ASSERT(frame->data[2] == 0xaa);
		CU_ASSERT(frame->data[3] == 0xaa);
		CU_ASSERT(frame->data[4] == 0xaa);
		CU_ASSERT(frame->data[5] == 0xaa);
		CU_ASSERT(frame->data[6] == 0xaa);
		CU_ASSERT(frame->data[7] == 0xaa);

		// cleanup
		free(frame); frame = NULL;
		test__frame = NULL;
		destroy_pdu(&pdu);
	}

	{
		// 2. frame
		pdu = create_pdu();
		CU_ASSERT(NULL != pdu);

		dest = 0x23;
		set_id_dest(pdu, dest);

		prio = 0x1;
		set_id_prio(pdu, prio);

		seqn = 0x72;
		set_data_seqn(pdu, seqn);

		payload = 0xbbbbbbbbbbbbbb;
		set_data_payload(pdu, payload);

		dlc = CAN_MAX_DLEN;
		set_pdu_dlc(pdu, dlc);

		canif__send(pdu);
		sleep(1); /* delay: pdu has to be processed to test framework */

		CU_ASSERT(test__tx_nbytes == CAN_MAX_DLEN);

		frame = test__frame;
		dbg_frame(__func__, frame);
		CU_ASSERT((uint16_t)frame->can_id == 0x123);
		CU_ASSERT(frame->can_dlc == CAN_MAX_DLEN);
		CU_ASSERT(frame->data[0] == 0x72);
		CU_ASSERT(frame->data[1] == 0xbb);
		CU_ASSERT(frame->data[2] == 0xbb);
		CU_ASSERT(frame->data[3] == 0xbb);
		CU_ASSERT(frame->data[4] == 0xbb);
		CU_ASSERT(frame->data[5] == 0xbb);
		CU_ASSERT(frame->data[6] == 0xbb);
		CU_ASSERT(frame->data[7] == 0xbb);

		// cleanup
		free(frame); frame = NULL;
		test__frame = NULL;
		destroy_pdu(&pdu);
	}

	{
		// 3. frame
		pdu = create_pdu();
		CU_ASSERT(NULL != pdu);

		dest = 0x23;
		set_id_dest(pdu, dest);

		prio = 0x1;
		set_id_prio(pdu, prio);

		seqn = 0x73;
		set_data_seqn(pdu, seqn);

		payload = 0xcccccccccccccc;
		set_data_payload(pdu, payload);

		dlc = CAN_MAX_DLEN;
		set_pdu_dlc(pdu, dlc);

		canif__send(pdu);
		sleep(1); /* delay: pdu has to be processed to test framework */

		CU_ASSERT(test__tx_nbytes == CAN_MAX_DLEN);

		frame = test__frame;
		dbg_frame(__func__, frame);
		CU_ASSERT((uint16_t)frame->can_id == 0x123);
		CU_ASSERT(frame->can_dlc == CAN_MAX_DLEN);
		CU_ASSERT(frame->data[0] == 0x73);
		CU_ASSERT(frame->data[1] == 0xcc);
		CU_ASSERT(frame->data[2] == 0xcc);
		CU_ASSERT(frame->data[3] == 0xcc);
		CU_ASSERT(frame->data[4] == 0xcc);
		CU_ASSERT(frame->data[5] == 0xcc);
		CU_ASSERT(frame->data[6] == 0xcc);
		CU_ASSERT(frame->data[7] == 0xcc);

		// cleanup
		free(frame); frame = NULL;
		test__frame = NULL;
		destroy_pdu(&pdu);
	}

	{
		// 4. frame
		pdu = create_pdu();
		CU_ASSERT(NULL != pdu);

		dest = 0x23;
		set_id_dest(pdu, dest);

		prio = 0x1;
		set_id_prio(pdu, prio);

		seqn = 0x74;
		set_data_seqn(pdu, seqn);

		payload = 0xdddddddddddddd;
		set_data_payload(pdu, payload);

		dlc = CAN_MAX_DLEN;
		set_pdu_dlc(pdu, dlc);

		canif__send(pdu);
		sleep(1); /* delay: pdu has to be processed to test framework */

		CU_ASSERT(test__tx_nbytes == CAN_MAX_DLEN);

		frame = test__frame;
		dbg_frame(__func__, frame);
		CU_ASSERT((uint16_t)frame->can_id == 0x123);
		CU_ASSERT(frame->can_dlc == CAN_MAX_DLEN);
		CU_ASSERT(frame->data[0] == 0x74);
		CU_ASSERT(frame->data[1] == 0xdd);
		CU_ASSERT(frame->data[2] == 0xdd);
		CU_ASSERT(frame->data[3] == 0xdd);
		CU_ASSERT(frame->data[4] == 0xdd);
		CU_ASSERT(frame->data[5] == 0xdd);
		CU_ASSERT(frame->data[6] == 0xdd);
		CU_ASSERT(frame->data[7] == 0xdd);

		// cleanup
		free(frame); frame = NULL;
		test__frame = NULL;
		destroy_pdu(&pdu);
	}

	{
		// 5. frame
		pdu = create_pdu();
		CU_ASSERT(NULL != pdu);

		dest = 0x23;
		set_id_dest(pdu, dest);

		prio = 0x1;
		set_id_prio(pdu, prio);

		seqn = 0x75;
		set_data_seqn(pdu, seqn);

		payload = 0xeeeeeeeeeeeeee;
		set_data_payload(pdu, payload);

		dlc = CAN_MAX_DLEN;
		set_pdu_dlc(pdu, dlc);

		canif__send(pdu);
		sleep(1); /* delay: pdu has to be processed to test framework */

		CU_ASSERT(test__tx_nbytes == CAN_MAX_DLEN);

		frame = test__frame;
		dbg_frame(__func__, frame);
		CU_ASSERT((uint16_t)frame->can_id == 0x123);
		CU_ASSERT(frame->can_dlc == CAN_MAX_DLEN);
		CU_ASSERT(frame->data[0] == 0x75);
		CU_ASSERT(frame->data[1] == 0xee);
		CU_ASSERT(frame->data[2] == 0xee);
		CU_ASSERT(frame->data[3] == 0xee);
		CU_ASSERT(frame->data[4] == 0xee);
		CU_ASSERT(frame->data[5] == 0xee);
		CU_ASSERT(frame->data[6] == 0xee);
		CU_ASSERT(frame->data[7] == 0xee);

		// cleanup
		free(frame); frame = NULL;
		test__frame = NULL;
		destroy_pdu(&pdu);
	}

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);
}

void test_rx_single(void)
{
	int ret = -1;
	pdu_p pdu;
	uint8_t dest;
	uint8_t prio;
	uint8_t seqn;
	uint64_t payload;
	uint8_t dlc;
	struct can_frame *frame;

	// start
	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);
//	sleep(1); /* delay: listeners have to be ready first */

	// frame
	frame = malloc(sizeof(*frame));
	if (!frame) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	test__frame = frame;

	frame->can_id = (uint16_t) 0x123;
	frame->can_dlc = CAN_MAX_DLEN;
	frame->data[0] = 0x81;
	frame->data[1] = 0xde;
	frame->data[2] = 0xbc;
	frame->data[3] = 0x9a;
	frame->data[4] = 0x78;
	frame->data[5] = 0x56;
	frame->data[6] = 0x34;
	frame->data[7] = 0x12;

	dbg_frame(__func__, test__frame);

	// trigger
	test__rx_ret = sizeof(frame);

	// receive
	for (int idx=0; idx < 3; idx++) {
		ret = canif__recv(&pdu);
		sleep(1);
		if (ret != 0) {
			break;
		}
	}
	CU_ASSERT(0 < ret);

	// verification
	dest = get_id_dest(pdu);
	CU_ASSERT(dest == 0x23);

	prio = get_id_prio(pdu);
	CU_ASSERT(prio == 0x1);

	seqn = get_data_seqn(pdu);
	CU_ASSERT(seqn == 0x81);

	payload = get_data_payload(pdu);
	CU_ASSERT(payload == 0x123456789abcde);

	dlc = get_pdu_dlc(pdu);
	CU_ASSERT(dlc == CAN_MAX_DLEN);

	// cleanup
	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
	ret = canif__shutdown();
	test__frame = NULL;
	CU_ASSERT(ret == 0);
}

void test_rx_multiple(void)
{
	int ret = -1;
	pdu_p pdu;
	uint8_t dest;
	uint8_t prio;
	uint8_t seqn;
	uint64_t payload;
	uint8_t dlc;
	struct can_frame *frame;

	// start
	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);
//	sleep(1); /* delay: listeners have to be ready first */

	{
		// frame
		frame = malloc(sizeof(*frame));
		if (!frame) {
			perror("allocation failed");
			exit(EXIT_FAILURE);
		}

		frame->can_id = (uint16_t) 0x123;
		frame->can_dlc = CAN_MAX_DLEN;
		frame->data[0] = 0x81;
		frame->data[1] = 0xaa;
		frame->data[2] = 0xaa;
		frame->data[3] = 0xaa;
		frame->data[4] = 0xaa;
		frame->data[5] = 0xaa;
		frame->data[6] = 0xaa;
		frame->data[7] = 0xaa;

		dbg_frame(__func__, test__frame);
		test__frame = frame;

		// trigger
		test__rx_ret = sizeof(frame);

		// receive
		for (int idx=0; idx < 3; idx++) {
			ret = canif__recv(&pdu);
			sleep(1);
			if (ret != 0) {
				break;
			}
		}
		CU_ASSERT(0 < ret);

		// verification
		dest = get_id_dest(pdu);
		CU_ASSERT(dest == 0x23);

		prio = get_id_prio(pdu);
		CU_ASSERT(prio == 0x1);

		seqn = get_data_seqn(pdu);
		CU_ASSERT(seqn == 0x81);

		payload = get_data_payload(pdu);
		CU_ASSERT(payload == 0xaaaaaaaaaaaaaa);

		dlc = get_pdu_dlc(pdu);
		CU_ASSERT(dlc == CAN_MAX_DLEN);

		// cleanup
		destroy_pdu(&pdu);
		test__frame = NULL;
	}

	{
		// frame
		frame = malloc(sizeof(*frame));
		if (!frame) {
			perror("allocation failed");
			exit(EXIT_FAILURE);
		}

		frame->can_id = (uint16_t) 0x123;
		frame->can_dlc = CAN_MAX_DLEN;
		frame->data[0] = 0x82;
		frame->data[1] = 0xbb;
		frame->data[2] = 0xbb;
		frame->data[3] = 0xbb;
		frame->data[4] = 0xbb;
		frame->data[5] = 0xbb;
		frame->data[6] = 0xbb;
		frame->data[7] = 0xbb;

		dbg_frame(__func__, test__frame);

		// trigger
		test__frame = frame;
		test__rx_ret = sizeof(frame);

		// receive
		for (int idx=0; idx < 3; idx++) {
			ret = canif__recv(&pdu);
			sleep(1);
			if (ret != 0) {
				break;
			}
		}
		CU_ASSERT(0 < ret);

		// verification
		dest = get_id_dest(pdu);
		CU_ASSERT(dest == 0x23);

		prio = get_id_prio(pdu);
		CU_ASSERT(prio == 0x1);

		seqn = get_data_seqn(pdu);
		CU_ASSERT(seqn == 0x82);

		payload = get_data_payload(pdu);
		CU_ASSERT(payload == 0xbbbbbbbbbbbbbb);

		dlc = get_pdu_dlc(pdu);
		CU_ASSERT(dlc == CAN_MAX_DLEN);

		// cleanup
		destroy_pdu(&pdu);
		test__frame = NULL;
	}

	{
		// frame
		frame = malloc(sizeof(*frame));
		if (!frame) {
			perror("allocation failed");
			exit(EXIT_FAILURE);
		}
		test__frame = frame;

		frame->can_id = (uint16_t) 0x123;
		frame->can_dlc = CAN_MAX_DLEN;
		frame->data[0] = 0x83;
		frame->data[1] = 0xcc;
		frame->data[2] = 0xcc;
		frame->data[3] = 0xcc;
		frame->data[4] = 0xcc;
		frame->data[5] = 0xcc;
		frame->data[6] = 0xcc;
		frame->data[7] = 0xcc;

		dbg_frame(__func__, test__frame);

		// trigger
		test__rx_ret = sizeof(frame);

		// receive
		for (int idx=0; idx < 3; idx++) {
			ret = canif__recv(&pdu);
			sleep(1);
			if (ret != 0) {
				break;
			}
		}
		CU_ASSERT(0 < ret);

		// verification
		dest = get_id_dest(pdu);
		CU_ASSERT(dest == 0x23);

		prio = get_id_prio(pdu);
		CU_ASSERT(prio == 0x1);

		seqn = get_data_seqn(pdu);
		CU_ASSERT(seqn == 0x83);

		payload = get_data_payload(pdu);
		CU_ASSERT(payload == 0xcccccccccccccc);

		dlc = get_pdu_dlc(pdu);
		CU_ASSERT(dlc == CAN_MAX_DLEN);

		// cleanup
		destroy_pdu(&pdu);
		test__frame = NULL;
	}

	{
		// frame
		frame = malloc(sizeof(*frame));
		if (!frame) {
			perror("allocation failed");
			exit(EXIT_FAILURE);
		}
		test__frame = frame;

		frame->can_id = (uint16_t) 0x123;
		frame->can_dlc = CAN_MAX_DLEN;
		frame->data[0] = 0x84;
		frame->data[1] = 0xdd;
		frame->data[2] = 0xdd;
		frame->data[3] = 0xdd;
		frame->data[4] = 0xdd;
		frame->data[5] = 0xdd;
		frame->data[6] = 0xdd;
		frame->data[7] = 0xdd;

		dbg_frame(__func__, test__frame);

		// trigger
		test__rx_ret = sizeof(frame);

		// receive
		for (int idx=0; idx < 3; idx++) {
			ret = canif__recv(&pdu);
			sleep(1);
			if (ret != 0) {
				break;
			}
		}
		CU_ASSERT(0 < ret);

		// verification
		dest = get_id_dest(pdu);
		CU_ASSERT(dest == 0x23);

		prio = get_id_prio(pdu);
		CU_ASSERT(prio == 0x1);

		seqn = get_data_seqn(pdu);
		CU_ASSERT(seqn == 0x84);

		payload = get_data_payload(pdu);
		CU_ASSERT(payload == 0xdddddddddddddd);

		dlc = get_pdu_dlc(pdu);
		CU_ASSERT(dlc == CAN_MAX_DLEN);

		// cleanup
		destroy_pdu(&pdu);
		test__frame = NULL;
	}

	{
		// frame
		frame = malloc(sizeof(*frame));
		if (!frame) {
			perror("allocation failed");
			exit(EXIT_FAILURE);
		}
		test__frame = frame;

		frame->can_id = (uint16_t) 0x123;
		frame->can_dlc = CAN_MAX_DLEN;
		frame->data[0] = 0x85;
		frame->data[1] = 0xee;
		frame->data[2] = 0xee;
		frame->data[3] = 0xee;
		frame->data[4] = 0xee;
		frame->data[5] = 0xee;
		frame->data[6] = 0xee;
		frame->data[7] = 0xee;

		dbg_frame(__func__, test__frame);

		// trigger
		test__rx_ret = sizeof(frame);

		// receive
		for (int idx=0; idx < 3; idx++) {
			ret = canif__recv(&pdu);
			sleep(1);
			if (ret != 0) {
				break;
			}
		}
		CU_ASSERT(0 < ret);

		// verification
		dest = get_id_dest(pdu);
		CU_ASSERT(dest == 0x23);

		prio = get_id_prio(pdu);
		CU_ASSERT(prio == 0x1);

		seqn = get_data_seqn(pdu);
		CU_ASSERT(seqn == 0x85);

		payload = get_data_payload(pdu);
		CU_ASSERT(payload == 0xeeeeeeeeeeeeee);

		dlc = get_pdu_dlc(pdu);
		CU_ASSERT(dlc == CAN_MAX_DLEN);

		// cleanup
		destroy_pdu(&pdu);
		test__frame = NULL;
	}

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);
}


int main(void)
{
//	mtrace(); // mem check

	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("pdu", init_suite_pdu, clean_suite_pdu);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	TEST_start(pSuite, "pdu id", test_pdu_id)
 		TEST_append(pSuite, "pdu dlc", test_pdu_dlc)
 		TEST_append(pSuite, "id dest empty", test_pdu_id_dest0)
 		TEST_append(pSuite, "id dest full", test_pdu_id_dest1)
 		TEST_append(pSuite, "id ip empty", test_pdu_id_prio0)
 		TEST_append(pSuite, "id ip full", test_pdu_id_prio1)
 		TEST_append(pSuite, "data seqn empty", test_pdu_data_seqn0)
 		TEST_append(pSuite, "data seqn full", test_pdu_data_seqn1)
 		TEST_append(pSuite, "data seqn series", test_pdu_data_seqn2)
 		TEST_append(pSuite, "pdu frame", test_pdu_frame)
	TEST_end();

	/* add a suite to the registry */
	pSuite = CU_add_suite("canif", init_suite_canif, clean_suite_canif);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "empty queues", test_queue_empty)
 		TEST_append(pSuite, "interface setup", test_canif_interface) // */
 		TEST_append(pSuite, "tx single", test_tx_single) // */
 		TEST_append(pSuite, "tx multiple", test_tx_multiple) // */
 		TEST_append(pSuite, "rx single", test_rx_single) // */
 		TEST_append(pSuite, "rx multiple", test_rx_multiple) // */
	TEST_end();

	CU_basic_set_mode(CU_BRM_VERBOSE);
#if defined BASICTEST
	CU_basic_run_tests();
#else
	CU_curses_run_tests();
#endif
	fprintf(stderr, "\n");
	CU_basic_show_failures(CU_get_failure_list());
	fprintf(stderr, "\n\n");

	/* clean up registry and return */
	CU_cleanup_registry();

//	muntrace(); // mem check

	return CU_get_error();
}
