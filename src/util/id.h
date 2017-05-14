#ifndef __ID_H__
#define __ID_H__


typedef unsigned long id; // identifier.

// A seed to generate new identifiers.
typedef struct idseed {
  id seed;
} idseed;


// Create an identifier from a seed.
// Successive calls create different identifiers.
// Complexity: O(1)
extern id create_id(idseed*);


#endif /* __ID_H__ */
