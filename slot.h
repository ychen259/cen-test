#ifndef SLOT_H_
#define SLOT_H_

struct slot {
	unsigned int x;
	unsigned int y;
};

struct slot make_slot(unsigned int x, unsigned int y);
int compare_slots(struct slot a, struct slot b);

#endif
