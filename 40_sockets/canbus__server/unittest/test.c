/*
  cunit - tests
 */


#include "can_interface.h"
#include "can_frame.h"
#include "test.h"

#include <linux/can.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>


void test_printf(const char* msg, ...)
{
#ifdef DEBUG
	va_list arglist;
	va_start(arglist, msg);
	vprintf(msg, arglist);
	va_end(arglist);
#endif /* DEBUG */
}

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
	test_printf("data[0] 0x%02x\n", data[0]);
	CU_ASSERT(data[1] == 0xde);
	test_printf("data[1] 0x%02x\n", data[1]);
	CU_ASSERT(data[2] == 0xbc);
	test_printf("data[2] 0x%02x\n", data[2]);
	CU_ASSERT(data[3] == 0x9a);
	test_printf("data[3] 0x%02x\n", data[3]);
	CU_ASSERT(data[4] == 0x78);
	test_printf("data[4] 0x%02x\n", data[4]);
	CU_ASSERT(data[5] == 0x56);
	test_printf("data[5] 0x%02x\n", data[5]);
	CU_ASSERT(data[6] == 0x34);
	test_printf("data[6] 0x%02x\n", data[6]);
	CU_ASSERT(data[7] == 0x12);
	test_printf("data[7] 0x%02x\n", data[7]);

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
	test_printf("data[0] 0x%02x\n", data[0]);
	CU_ASSERT(data[1] == 0xde);
	test_printf("data[1] 0x%02x\n", data[1]);
	CU_ASSERT(data[2] == 0xbc);
	test_printf("data[2] 0x%02x\n", data[2]);
	CU_ASSERT(data[3] == 0x9a);
	test_printf("data[3] 0x%02x\n", data[3]);
	CU_ASSERT(data[4] == 0x78);
	test_printf("data[4] 0x%02x\n", data[4]);
	CU_ASSERT(data[5] == 0x56);
	test_printf("data[5] 0x%02x\n", data[5]);
	CU_ASSERT(data[6] == 0x34);
	test_printf("data[6] 0x%02x\n", data[6]);
	CU_ASSERT(data[7] == 0x12);
	test_printf("data[7] 0x%02x\n", data[7]);

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

static void verify_rx_frame(uint8_t dest, uint8_t prio, uint8_t seqn, uint64_t payload, uint8_t dlc)
{
	int ret = -1;
	pdu_p pdu;
	uint8_t exp_dest = dest;
	uint8_t x_dest = 0;
	uint8_t exp_prio = prio;
	uint8_t x_prio = 0;
	uint8_t exp_seqn = seqn;
	uint8_t x_seqn = 0;
	uint64_t exp_payload = payload;
	uint64_t x_payload;
	uint8_t exp_dlc = dlc;
	uint8_t x_dlc = 0;
	struct can_frame *frame;

	// frame
	frame = malloc(sizeof(*frame));
	if (!frame) {
		perror("allocation failed");
		exit(EXIT_FAILURE);
	}
	test__frame = frame;

	frame->can_id = (prio<<8 | dest) & 0x7ff;
	frame->can_dlc = dlc;
	frame->data[0] = seqn;
	frame->data[1] = (uint8_t) ((payload & 0x00000000000000ff));
	frame->data[2] = (uint8_t) ((payload & 0x000000000000ff00) >>8);
	frame->data[3] = (uint8_t) ((payload & 0x0000000000ff0000) >>16);
	frame->data[4] = (uint8_t) ((payload & 0x00000000ff000000) >>24);
	frame->data[5] = (uint8_t) ((payload & 0x000000ff00000000) >>32);
	frame->data[6] = (uint8_t) ((payload & 0x0000ff0000000000) >>40);
	frame->data[7] = (uint8_t) ((payload & 0x00ff000000000000) >>48);

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
	x_dest = get_id_dest(pdu);
	CU_ASSERT(x_dest == exp_dest);

	x_prio = get_id_prio(pdu);
	CU_ASSERT(x_prio == exp_prio);

	x_seqn = get_data_seqn(pdu);
	CU_ASSERT(x_seqn == exp_seqn);

	x_payload = get_data_payload(pdu);
	CU_ASSERT(x_payload == exp_payload);

	x_dlc = get_pdu_dlc(pdu);
	CU_ASSERT(x_dlc == exp_dlc);

	// cleanup
	destroy_pdu(&pdu);
	CU_ASSERT(NULL == pdu);
	test__frame = NULL;
}

static void verify_tx_frame(uint8_t dest, uint8_t prio, uint8_t seqn, uint64_t payload, uint8_t dlc)
{
	pdu_p pdu;
	struct can_frame *frame = NULL;

	// expected
	uint16_t exp_id = (prio<<8 | dest) & 0x7ff;
	uint8_t exp_dlc = dlc;
	uint8_t exp_data[CAN_MAX_DLEN];
	exp_data[0] = seqn;
	exp_data[1] = (uint8_t) ((payload & 0x00000000000000ff));
	exp_data[2] = (uint8_t) ((payload & 0x000000000000ff00) >>8);
	exp_data[3] = (uint8_t) ((payload & 0x0000000000ff0000) >>16);
	exp_data[4] = (uint8_t) ((payload & 0x00000000ff000000) >>24);
	exp_data[5] = (uint8_t) ((payload & 0x000000ff00000000) >>32);
	exp_data[6] = (uint8_t) ((payload & 0x0000ff0000000000) >>40);
	exp_data[7] = (uint8_t) ((payload & 0x00ff000000000000) >>48);

	// init pdu
	pdu = create_pdu();
	CU_ASSERT(NULL != pdu);

	set_id_dest(pdu, dest);
	set_id_prio(pdu, prio);
	set_data_seqn(pdu, seqn);
	set_data_payload(pdu, payload);
	set_pdu_dlc(pdu, dlc);

	canif__send(pdu);
	sleep(1); /* delay: pdu has to be processed to test framework */

	// verification
	CU_ASSERT(test__tx_nbytes == exp_dlc);

	frame = test__frame;
	dbg_frame(__func__, frame);
	CU_ASSERT((uint16_t)frame->can_id == exp_id);
	test_printf("frame->can_id %03x [%03x]\n", frame->can_id, exp_id);
	CU_ASSERT(frame->can_dlc == exp_dlc);
	test_printf("frame->can_dlc %d [%d]\n", frame->can_dlc, exp_dlc);
	CU_ASSERT(frame->data[0] == exp_data[0]);
	test_printf("frame->data[0] 0x%02x [0x%02x]\n", frame->data[0], exp_data[0]);
	CU_ASSERT(frame->data[1] == exp_data[1]);
	test_printf("frame->data[1] 0x%02x [0x%02x]\n", frame->data[1], exp_data[1]);
	CU_ASSERT(frame->data[2] == exp_data[2]);
	test_printf("frame->data[2] 0x%02x [0x%02x]\n", frame->data[2], exp_data[2]);
	CU_ASSERT(frame->data[3] == exp_data[3]);
	test_printf("frame->data[3] 0x%02x [0x%02x]\n", frame->data[3], exp_data[3]);
	CU_ASSERT(frame->data[4] == exp_data[4]);
	test_printf("frame->data[4] 0x%02x [0x%02x]\n", frame->data[4], exp_data[4]);
	CU_ASSERT(frame->data[5] == exp_data[5]);
	test_printf("frame->data[5] 0x%02x [0x%02x]\n", frame->data[5], exp_data[5]);
	CU_ASSERT(frame->data[6] == exp_data[6]);
	test_printf("frame->data[6] 0x%02x [0x%02x]\n", frame->data[6], exp_data[6]);
	CU_ASSERT(frame->data[7] == exp_data[7]);
	test_printf("frame->data[7] 0x%02x [0x%02x]\n", frame->data[7], exp_data[7]);

	// cleanup
	free(frame); frame = NULL;
	test__frame = NULL;
	destroy_pdu(&pdu);
}

void test_tx_single(void)
{
	int ret = -1;

	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);
	sleep(1); // safety

	verify_tx_frame(0x23, 0x1, 0x7c, 0x123456789abcde, CAN_MAX_DLEN);

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);
}

void test_tx_multiple(void)
{
	int ret = -1;

	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);
	sleep(1); // safety

	verify_tx_frame(0x23, 0x1, 0x71, 0xaaaaaaaaaaaaaa, CAN_MAX_DLEN);
	verify_tx_frame(0x23, 0x1, 0x72, 0xbbbbbbbbbbbbbb, CAN_MAX_DLEN);
	verify_tx_frame(0x23, 0x1, 0x73, 0xcccccccccccccc, CAN_MAX_DLEN);
	verify_tx_frame(0x23, 0x1, 0x74, 0xdddddddddddddd, CAN_MAX_DLEN);
	verify_tx_frame(0x23, 0x1, 0x75, 0xeeeeeeeeeeeeee, CAN_MAX_DLEN);

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);
}

void test_rx_single(void)
{
	int ret = -1;
	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);
	sleep(1); // safety

	verify_rx_frame(0x23, 0x1, 0x81, 0x123456789abcde, CAN_MAX_DLEN);

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);
}

void test_rx_multiple(void)
{
	int ret = -1;

	ret = canif__startup("can0", strlen("can0") + 1);
	CU_ASSERT(ret == 0);
	sleep(1); // safety

	verify_rx_frame(0x23, 0x1, 0x31, 0xaaaaaaaaaaaaaa, CAN_MAX_DLEN);
	verify_rx_frame(0x23, 0x1, 0x32, 0xbbbbbbbbbbbbbb, CAN_MAX_DLEN);
	verify_rx_frame(0x23, 0x1, 0x33, 0xcccccccccccccc, CAN_MAX_DLEN);
	verify_rx_frame(0x23, 0x1, 0x34, 0xdddddddddddddd, CAN_MAX_DLEN);
	verify_rx_frame(0x23, 0x1, 0x35, 0xeeeeeeeeeeeeee, CAN_MAX_DLEN);
	verify_rx_frame(0x23, 0x1, 0x35, 0xffffffffffffff, CAN_MAX_DLEN);

	ret = canif__shutdown();
	CU_ASSERT(ret == 0);
}


int main(void)
{
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

	return CU_get_error();
}
