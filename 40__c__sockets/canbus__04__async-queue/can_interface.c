/*
  can_interface: implementation

  @author: Lothar Rubusch <L.Rubusch@gmail.com>
  @license: GPLv3
 */

#include "can_interface.h"


/*
  "private"
*/
static int sockfd;
static pid_t pid_listener;
static struct sockaddr_can addr;
static struct ifreq ifr;

// receveiver
typedef struct rcv_queue_s {
	struct rcv_queue_s *next;
	struct can_frame* content;
} rcv_queue_t;
typedef struct rcv_queue_s* rcv_queue_p;

typedef struct receiver_s {
	int rcv_queue_cnt;
	rcv_queue_p first;
	rcv_queue_p last;
	// TODO lock
} receiver_t;

static receiver_t receiver = {
	.rcv_queue_cnt = 0,
	.first = NULL,
	.last = NULL
};


static int canif__rcv__pushback(struct can_frame *pf)
{
	rcv_queue_p pelem = NULL;

	if (RCV_QUEUE_MAX == 1 + receiver.rcv_queue_cnt) {
		fprintf(stderr, "%s(): frame dropped!\n", __func__);
		return 0;
	}

	if (NULL == (pelem = malloc(sizeof(*pelem)))) {
		perror("Error at allocation of queue element");
		return -1;
	}
	pelem->next = NULL;
	pelem->content = pf;

// TODO receiver.lock    
	if (NULL == receiver.first) {
		receiver.last = pelem;
		receiver.first = receiver.last;
	} else {
		receiver.last->next = pelem;
	}
	receiver.rcv_queue_cnt++;
// TODO receiver.unlock    

	return 0;
}

static struct can_frame* canif__rcv__popfirst()
{
	struct can_frame* pf;
	rcv_queue_p iter = receiver.first;

// TODO receiver.lock    
	if (!receiver.last) {
		receiver.rcv_queue_cnt = 0;
		//receiver.rcv_queue_cnt--;
		//assert(0 == receiver.rcv_queue_cnt); // TEST
		pf = NULL;
	} else {
		pf = receiver.last->content;
		if (!iter->next) {
			free(receiver.last);
			receiver.last = NULL;

		} else {
			while (&receiver.last != &iter->next) { // TODO verify  
				iter = iter->next;
			}
			iter->next = NULL;
			free(receiver.last);
			receiver.last = iter;
		}
		receiver.rcv_queue_cnt--;
	}
// TODO receiver.unlock    

	return pf;
}

void canif__listener()
{
	struct can_frame* pf;
	int ret = -1;
	do {
		pf = malloc(sizeof(*pf));
		if (NULL == pf) {
			perror("Error in allocation");
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		// TODO check number of connected clients?   
		ret = read(sockfd, pf, sizeof(*pf)); // TODO check blocking?      
		if (-1 == ret) {
			perror("Error in read()");
			free(pf);
			return;
		}
		ret = canif__rcv__pushback(pf);
		if (0 > ret) {
			free(pf);
			return;
		}
	} while (1);
}


/*
  "public" interface
*/

int canif__startup(const char *ifname, size_t ifname_size)
{
	if (-1 == (sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW))) {
		perror("Error while opening socket");
		exit(EXIT_FAILURE);
	}

	strncpy(ifr.ifr_name, ifname, ifname_size);
	ifr.ifr_name[ifname_size-1] = '\0';
	if (0 > ioctl(sockfd, SIOCGIFINDEX, &ifr)) {
		perror("Error in ioctl");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	fprintf(stdout, "%s at index %d\n", ifname, ifr.ifr_ifindex);
	if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("Error at binding socket to address");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	pid_listener = fork();
	if (0 > pid_listener) {
		perror("Error at fork()");
	} else if (0 == pid_listener) {
		canif__listener();
		canif__shutdown();
		return -1;
	}

	return 0;
}

//int canif__shutdown(const char *ifname, size_t ifname_size)
int canif__shutdown()
{
	// free queue
	uint32_t dummy_can_id;
	uint8_t dummy_can_dlc;
	uint8_t dummy_data[CAN_MAX_DLEN];

	while (0 == canif__recv(&dummy_can_id, &dummy_can_dlc, dummy_data))
	       ;

	return close(sockfd);
}

//int canif__send(uint32_t *can_id, uint8_t *can_dlc, uint8_t *data[CAN_MAX_DLEN])
int canif__send(const uint32_t *can_id, const uint8_t *can_dlc, uint8_t data[])
{
	int nbytes = -1;
	struct can_frame frame;
/*
  // TODO test, rm
	frame.can_id  = 0x123;
	frame.can_dlc = 2;
	frame.data[0] = 0x11;
	frame.data[1] = 0x22;
*/
	frame.can_id = *can_id;
	frame.can_dlc = *can_dlc;
	memcpy(frame.data, data, *can_dlc);

	nbytes = write(sockfd, &frame, sizeof(struct can_frame));
	// TODO check write error conditions

	return nbytes;
}

//int canif__recv(uint32_t *can_id, uint8_t* can_dlc, uint8_t *data[CAN_MAX_DLEN])
//int canif__recv(uint32_t *can_id, uint8_t* can_dlc, uint8_t *data[])
//int canif__recv(uint32_t *can_id, uint8_t* can_dlc, uint8_t **data)
int canif__recv(uint32_t *can_id, uint8_t* can_dlc, uint8_t data[])
{
	struct can_frame* pf;

	pf = canif__rcv__popfirst();
	if (!pf)
		return -1;

	*can_id = pf->can_id; 
	*can_dlc = pf->can_dlc; 
	memcpy(pf->data, data, pf->can_dlc); 
	free(pf);
	return 0;
}

