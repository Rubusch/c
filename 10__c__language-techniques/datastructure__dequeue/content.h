/*
  some dummy content header
*/

#ifndef CONTENT_H_
#define CONTENT_H_


#define MESSAGE_LENGTH 32

struct content_s {
	/* this can be literally anything */
	char msg[MESSAGE_LENGTH];
	int len;
};
typedef struct content_s content_t;
typedef content_t* content_p;


#endif /* CONTENT_H_ */
