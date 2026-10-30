#include <ds/deque.h>

#include <unitest/unitest.h>
#include <stdlib.h>

static int int_fprint(FILE *fp, const void *intp) {
  return fprintf(fp, "%d", *(int*)intp);
}

void core() {
    test_suite("Allocation");
    deque_t deque = deque_of(int);
    printf("deque      = %p\n", (void *)deque);
    printf("UNTAG      = %p\n", (void *)UNTAG(deque));
    printf("length     = %u\n", deque_length(deque));

    test_assert(deque_length(deque)==0, "A new deque has length zero");
    test_assert(deque_start(deque)==NULL, "A new deque has no start");
    test_assert(deque_end(deque)==NULL, "A new deque has no end");
    test_suite("Insertion 1");
    int i = 25;
    deque_appendright(deque, &i);
    printf("Deque length: %d\n", deque_length(deque));
    test_assert(deque_length(deque)==1, "Length is 1 after insertion of 1 element");
    test_assert(deque_start(deque)!=NULL, "This deque has a start");
    test_assert(deque_end(deque)==deque_start(deque), "Start and end are the same");
    test_assert(deque_end(deque)->next==NULL, "The node has no right neighbor");
    test_assert(deque_end(deque)->prev==NULL, "The node has no left neighbor");
    
    test_suite("Pop1.1 left");
    int j;
    deque_popleft(deque, &j);
    test_assert(j==25, "Can pop left with correct return value.");
    test_assert(deque_length(deque)==0, "Deque has length zero");
    test_assert(deque_start(deque)==NULL, "Deque has no start");
    test_assert(deque_end(deque)==NULL, "Deque has no end");
    test_suite("Pop1.1 right");
    i=26;
    deque_appendright(deque, &i);
    deque_popleft(deque, &j);
    test_assert(j==26, "Can pop left with correct return value.");
    test_assert(deque_length(deque)==0, "Deque has length zero");
    test_assert(deque_start(deque)==NULL, "Deque has no start");
    test_assert(deque_end(deque)==NULL, "Deque has no end");
    
    test_suite("Insertion 2");
    i=27;
    deque_appendleft(deque, &i);
    i=28;
    deque_appendright(deque, &i);

    deque_fprint(stdout, deque, int_fprint);
    fflush(stdout);

    deque_popleft(deque, &j);
    test_assert(deque_length(deque)==1, "Length is 1 after insertion of 2 then deletion of 1 element");
    test_assert(deque_start(deque)!=NULL, "This deque has a start");
    test_assert(deque_end(deque)==deque_start(deque), "Start and end are the same");
    test_assert(deque_end(deque)->next==NULL, "The node has no right neighbor");
    test_assert(deque_end(deque)->prev==NULL, "The node has no left neighbor");
    
    test_suite("Deletion");
    test_assert(typeof(deque)==T_QUEUE, "Deque is still tagged after all that.");
    int retval = deque_delete(&deque);
    test_assert(deque==NULL, "Deletion sets deque to NULL");
    test_assert(retval==0, "Deletion returns 0, not -1.");
}

int main(int argc, char *argv[]) {
  unit_test(argc, argv);

  core();

  exit(EXIT_SUCCESS);
}