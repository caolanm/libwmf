/*
this titchy piece of code might as well be considered GPLed

Caolan.McNamara@ul.ie
http://www.csn.ul.ie/~caolan
*/

struct taglink
{
struct taglink *last;
void *info;
struct taglink *next;
};

typedef struct taglink link_c;

struct tagstack
{
link_c *fpointer;
link_c *bpointer;
int length;
};

typedef struct tagstack c_stack;

void *pop_c_stk(c_stack *stack);
int push_c_stk(void *info,c_stack *stack);
void init_c_stk(c_stack *stack);
int len_c_stk(c_stack *stack);
