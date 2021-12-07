/*
  cunit - tests
 */

#include "can_interface.h"
#include "can_frame.h"

#include "test.h"
#include "stdlib.h"
#include "time.h"


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


void test_queue_empty(void)
{
	int res = FALSE;
	CU_ASSERT(FALSE == res);
	res = tx__empty();
	CU_ASSERT(TRUE == res);
	res = rx__empty();
	CU_ASSERT(TRUE == res);
}

void test_tx(void)
{
	canif__startup("MickeyMouse", strlen("MickeyMouse") + 1);
	// TODO
}

void test_rx(void)
{
	// TODO
}


int main(void)
{
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("queue", init_suite_pdu, clean_suite_pdu);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	TEST_start(pSuite, "pdu id", test_pdu_id)
 		TEST_append(pSuite, "id dest empty", test_pdu_id_dest0)
 		TEST_append(pSuite, "id dest full", test_pdu_id_dest1)
 		TEST_append(pSuite, "id ip empty", test_pdu_id_prio0)
 		TEST_append(pSuite, "id ip full", test_pdu_id_prio1)
 		TEST_append(pSuite, "data seqn empty", test_pdu_data_seqn0)
 		TEST_append(pSuite, "data seqn full", test_pdu_data_seqn1)
 		TEST_append(pSuite, "data seqn series", test_pdu_data_seqn2)
	TEST_end();

	/* add a suite to the registry */
	pSuite = CU_add_suite("queue", init_suite_canif, clean_suite_canif);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* utilities */
	TEST_start(pSuite, "empty queues", test_queue_empty)
/* 		TEST_append(pSuite, "tx", test_tx) // */
/* 		TEST_append(pSuite, "rx", test_rx) // */
	TEST_end();

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	fprintf(stderr, "\n");
	CU_basic_show_failures(CU_get_failure_list());
	fprintf(stderr, "\n\n");

#if defined BASICTEST
	CU_automated_run_tests();
#else
	CU_curses_run_tests();
#endif

	/* clean up registry and return */
	CU_cleanup_registry();
	return CU_get_error();
}
