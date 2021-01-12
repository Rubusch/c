#include "lib_socket_demo.h"

#include <stdlib.h>
#include <stdio.h>
//#define NDEBUG /* uncomment this, turn off assert testing */
#include <assert.h>

#include "test.h"


// test definitions

TEST__BEGIN(connect_nonb) {
// TODO
} TEST__END

TEST__BEGIN(lothars__connect_timeo) {
// TODO
} TEST__END

TEST__BEGIN(daemon_init) {
// TODO
} TEST__END

TEST__BEGIN(daemon_inetd) {
// TODO
} TEST__END

TEST__BEGIN(dg_cli) {
// TODO
} TEST__END

TEST__BEGIN(dg_echo) {
// TODO
} TEST__END

TEST__BEGIN(lothars__family_to_level) {
// TODO
} TEST__END

TEST__BEGIN(lothars__socket_to_family) {
// TODO
} TEST__END

TEST__BEGIN(gf_time) {
// TODO
} TEST__END

TEST__BEGIN(lothars__host_serv) {
// TODO
} TEST__END

TEST__BEGIN(lothars__my_addrs) {
// TODO
} TEST__END

TEST__BEGIN(worker__echo_serv) {
// TODO
} TEST__END

TEST__BEGIN(worker__echo_cli) {
// TODO
} TEST__END

TEST__BEGIN(worker__echo_serv_str) {
// TODO
} TEST__END

TEST__BEGIN(worker__echo_serv_bin) {
// TODO
} TEST__END

TEST__BEGIN(worker__echo_cli_bin) {
// TODO
} TEST__END

TEST__BEGIN(lothars__tcp_connect) {
// TODO
} TEST__END

TEST__BEGIN(lothars__tcp_listen) {
// TODO
} TEST__END

TEST__BEGIN(tv_sub) {
// TODO
} TEST__END

TEST__BEGIN(lothars__udp_client) {
// TODO
} TEST__END

TEST__BEGIN(lothars__udp_connect) {
// TODO
} TEST__END

TEST__BEGIN(lothars__udp_server) {
// TODO
} TEST__END

TEST__BEGIN(lothars__writable_timeo) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sock_bind_wild) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sock_ntop) {
// TODO
} TEST__END

TEST__BEGIN(lothars__sock_ntop_host) {
// TODO
} TEST__END

TEST__BEGIN(lothars__readable_timeo) {
// TODO
} TEST__END

TEST__BEGIN(sock_cmp_addr) {
// TODO
} TEST__END

TEST__BEGIN(sock_cmp_port) {
// TODO
} TEST__END

TEST__BEGIN(sock_get_port) {
// TODO
} TEST__END

TEST__BEGIN(sock_set_addr) {
// TODO
} TEST__END

TEST__BEGIN(sock_set_port) {
// TODO
} TEST__END

TEST__BEGIN(sock_set_wild) {
// TODO
} TEST__END


/*
*/
int main(int argc, char* argv[])
{
	test__connect_nonb();
	test__lothars__connect_timeo();
	test__daemon_init();
	test__daemon_inetd();
	test__dg_cli();
	test__dg_echo();
	test__lothars__family_to_level();
	test__lothars__socket_to_family();
	test__gf_time();
	test__lothars__host_serv();
	test__lothars__my_addrs();
	test__worker__echo_serv();
	test__worker__echo_cli();
	test__worker__echo_serv_str();
	test__worker__echo_serv_bin();
	test__worker__echo_cli_bin();
	test__lothars__tcp_connect();
	test__lothars__tcp_listen();
	test__tv_sub();
	test__lothars__udp_client();
	test__lothars__udp_connect();
	test__lothars__udp_server();
	test__lothars__writable_timeo();
	test__lothars__sock_bind_wild();
	test__lothars__sock_ntop();
	test__lothars__sock_ntop_host();
	test__lothars__readable_timeo();
	test__sock_cmp_addr();
	test__sock_cmp_port();
	test__sock_get_port();
	test__sock_set_addr();
	test__sock_set_port();
	test__sock_set_wild();

	fprintf(stdout, "READY.\n");
	exit(EXIT_FAILURE);
}
