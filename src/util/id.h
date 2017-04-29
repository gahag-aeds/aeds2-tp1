#ifndef __ID_H__
#define __ID_H__


typedef unsigned long id;

typedef struct idseed {
  id seed;
} idseed;


extern id create_id(idseed*);


#endif /* __ID_H__ */
