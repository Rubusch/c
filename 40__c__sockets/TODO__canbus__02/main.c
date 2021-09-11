/*
  socketCAN demo

  based on socketCAN, wikipedia and kernel.org

  +------------------------+ ============
  | Application            |     |
  +------------------------+     |
               A                 |  communication
               |                 }- stack,
               V                 |  e.g. CANopen
  +------------------------+     |
  | Socket Layer           |     |
  +------------------------+ ============
  | PF_CAN | PF_INET | ... |  |
  +--------+---------+-----+  }- SocketCAN
  | Network Device Drivers |  |
  +------------------------+ ===
  | CAN Controller         |
  +------------------------+

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3

  REFERENCE:
  https://en.wikipedia.org/wiki/SocketCAN
  https://www.kernel.org/doc/Documentation/networking/can.txt
 */
// TODO verification via rapsi CAN HAT RS485
// TODO: rework to new layout implementation


#include "can_interface.h"

int
main(void)
{
	// startup
	canif__startup("can0", sizeof("can0"));

	// send (similar "client")
	canif__send();

	// listen (similar "server")
	// loop on a receive stack, continue if something was received
	canif__recv();
	// TODO     

	// shutdown
	canif__shutdown();
	// TODO       

	exit(EXIT_SUCCESS);
}
