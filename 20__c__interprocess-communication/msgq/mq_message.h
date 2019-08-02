// mq_message.h

#ifndef _MQ_MESSAGE_H_
#define _MQ_MESSAGE_H_


#define MQ_MESSAGESIZ 1024
#define MQ_MAXSIZE 524288
#define MQ_NAMESIZE 64

typedef enum mq_datatype_e{
  // tag                 // xml name,                     converted to
  MQDATA_UNDEFINETYPE    // "undef" undefined type 
  
  // command
  , MQDATA_COMMAND       // command

  // primitives
  , MQDATA_STRING=10     // "string"                      char*, onlye one array (string) possible at a time
  , MQDATA_CHARTYPE      // "char",                       char, (ascii number!)
  , MQDATA_UCHARTYPE     // "unsigned char",              unsigned char
  , MQDATA_SHORTTYPE     // "short",                      short
  , MQDATA_USHORTTYPE    // "unsigned short",             unsigned short
  , MQDATA_INTTYPE       // "int",                        int
  , MQDATA_UINTTYPE      // "unsigned int",               unsigned int
  , MQDATA_LLINTTYPE     // "long long int",              long long int
  , MQDATA_ULLINTTYPE    // "unsigned long long int",     unsigned long long int
  , MQDATA_FLOATTYPE     // "float",                      float
  , MQDATA_DOUBLETYPE    // "double",                     double
  , MQDATA_LDOUBLETYPE   // "long double",                long double
} mq_datatype_t;


struct mq_message_s {
  // platform -> exe : 1
  // exe -> platform : 2
  // confirm: platform -> exe : 3
  // confirm: exe -> platform : 4
  long mtype;
  char content_name[MQ_NAMESIZE];
  mq_datatype_t content_type;
  int needs_confirmation;
  int arr_extent; // 1 == variable, 1 < array

  char content[MQ_MESSAGESIZ];
};
typedef struct mq_message_s mq_message_t;
typedef mq_message_t* mq_message_p;


#endif
