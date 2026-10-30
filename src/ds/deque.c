#include <ds/deque.h>
#include <core/tagging.h>
#include <assert.h>


/**
 * A deque is a dynamically allocated doubly linked structure.
 * It keeps pointers to its first and last nodes.
 * Each node stores pointers to its previous and next neighbors (NULL when absent)
 * and a payload of size deque->stride.
 *
 * A deque is created with deque_of(T), where T is the stored type.
 */

deque_t deque__(size_t stride){
    deque_t deque = calloc(1,sizeof(*deque));
    deque->length = 0;
    deque->end=NULL;
    deque->start=NULL;
    deque->stride = stride;
    return TAG(deque, T_QUEUE);
}


/**
 * Ajoute une élément à droite de la deque.
 *
 * Cette fonction vérifie d'abord que le pointeur fourni correspond bien à
 * un objet de type T_QUEUE, non NULL après UNTAG.
 * Puis copie deque->stride bytes depuis objecp vers la nouvelle node de fin.
 *  
 * Ex d'utilisation:
 *  int i = 25;
 *  deque_appendright(deque, &i);
 * 
 * @param tagged Pointeur taggué vers la deque à modifier.
 * @return la deque tagguée, NULL non taggé si pas une deque valide.
 */
deque_t deque_appendright__(deque_t tagged, void * objectp){
    assert(typeof(tagged)==T_QUEUE && "Tagged pointer must be of type queue.");

    deque_t deque = UNTAG(tagged);
    if (deque==NULL){
        return NULL;
    }

    deque_node_t new_node = calloc(1, sizeof(*new_node)+deque->stride);
    memcpy(object_of(new_node), objectp, deque->stride);

    deque->length++;
    if (deque->length==1){
        deque->start = new_node;
        deque->end = new_node;
        return deque;
    }
    new_node->prev = deque->end;
    deque->end->next = new_node;
    deque->end = new_node;
    return TAG(deque, T_QUEUE);
}

/**
 * Ajoute une élément à gauche de la deque.
 *
 * Cette fonction vérifie d'abord que le pointeur fourni correspond bien à
 * un objet de type T_QUEUE, non NULL après UNTAG.
 * Puis copie deque->stride bytes depuis objecp vers la nouvelle node de début.
 *
 * Ex d'utilisation:
 *  int i = 25;
 * deque_appendleft(
 * deque_appendleft(
 * deque_appendleft(deque, &i), &i) &i);
 *
 * @param tagged Pointeur taggué vers la deque à modifier.
 * @return la deque tagguée, NULL non taggé si pas une deque valide.
 */
deque_t deque_appendleft__(deque_t tagged, void * objectp){
    assert(typeof(tagged)==T_QUEUE && "Tagged pointer must be of type queue.");

    deque_t deque = UNTAG(tagged);
    if (deque==NULL){
        return NULL;
    }
    
    deque_node_t new_node = calloc(1, sizeof(*new_node)+deque->stride);
    memcpy(object_of(new_node), objectp, deque->stride);
    
    deque->length++;
    if (deque->length==1){ // we jsut inserted the first node
        deque->start = new_node;
        deque->end = new_node;
        return deque;
    }
    new_node->next = deque->start;
    deque->start->prev = new_node;
    deque->start = new_node;
    return TAG(deque, T_QUEUE);
}


/**
 * Pop l'élément de gauche d'une deque.
 *
 * Cette fonction vérifie d'abord que le pointeur fourni correspond bien à
 * un objet de type T_QUEUE, non NULL après UNTAG.
 * Puis selon si on a donné une addresse de retour,
 * copie l'objet stocké dans la deque à l'emplacement de retour.
 * Dans tous les cas elle supprime l'élément de gauche.
 * 
 * Ex d'utilisation:
 *  int i;
 *  unsigned int ret = deque_popleft(deque, &i);
 *  if (ret==-1) *handle type error* ;
 *  
 *  int i;
 *  while (deque_popleft(deque, &i)!>0) printf("%d\n",i)
 * 
 *  //le &i est optionnel:
 *  deque_popright(deque);
 * 
 * @param tagged Pointeur taggué vers la deque à modifier.
 * @return la longueur après suppression, -1 si pop sur liste vide, -2 si pas du bon type.
 */
int deque_popleft__(deque_t tagged, void * retp){
    assert(typeof(tagged)==T_QUEUE && "Tagged pointer must be of type queue.");

    deque_t deque = UNTAG(tagged);
    
    if (deque->length==0){
        return -1;
    }
    //copy the obejct to retp
    deque_node_t topop = deque->start;
    if (retp!=NULL){
        void * object = object_of(topop);
        memcpy(retp, object, deque->stride);
    }

    if (deque->length==1){
        deque->start = NULL;
        deque->end = NULL;
    } else{
        deque->start=topop->next;
        deque->start->prev=NULL;
    }
    
    //then free the node;
    free(topop);

    deque->length--;
    return deque->length;
}


/**
 * Pop l'élément de droite d'une deque.
 *
 * Cette fonction vérifie d'abord que le pointeur fourni correspond bien à
 * un objet de type T_QUEUE, non NULL après UNTAG.
 * Puis selon si on a donné une addresse de retour,
 * copie l'objet stocké dans la deque à l'emplacement de retour.
 * Dans tous les cas elle supprime l'élément de gauche.
 * 
 * Ex d'utilisation:
 *  int i;
 *  unsigned int ret = deque_popright(deque, &i);
 *  if (ret==-1) *handle type error* ;
 *  
 *  int i;
 *  while (deque_popright(deque, &i)!>0) printf("%d\n",i)
 * 
 *  //le &i est optionnel:
 *  deque_popright(deque);
 * 
 * @param tagged Pointeur taggué vers la deque à modifier.
 * @return la longueur après suppression, -1 si pop sur liste vide.
 */
int deque_popright__(deque_t tagged, void * retp){
    assert(typeof(tagged)==T_QUEUE && "Tagged pointer must be of type queue.");
    deque_t deque = UNTAG(tagged);
    
    if (deque->length==0){
        return -1;
    }
    //copy the obejct to retp
    deque_node_t topop = deque->end;
    if (retp!=NULL){
        void * object = object_of(topop);
        memcpy(retp, object, deque->stride);
    }
    
    if (deque->length==1){
        deque->start = NULL;
        deque->end = NULL;
    } else{
        deque->end=topop->prev;
        deque->end->next=NULL;
    }
    
    //then free the node;
    free(topop);
    
    deque->length--;
    return deque->length;
}

/**
 * Libère entièrement une file doublement chaînée allouée dynamiquement.
 *
 * Cette fonction vérifie d'abord que le pointeur fourni correspond bien à
 * un objet de type T_QUEUE, puis libère chaque noeud de la deque ainsi que la
 * structure principale du conteneur.
 *
 * @param tagged Pointeur taggué vers la deque à supprimer.
 * @return 0 si la deque a bien été supprimée, -1 si le type ne correspond pas.
 */
int deque_delete(deque_t * taggedp){
    assert(taggedp && "Missing tagged addr in delete.");
    deque_t tagged = *taggedp;
    assert(typeof(tagged)==T_QUEUE && "Tagged pointer must be of type queue.");

    deque_t deque = UNTAG(tagged);

    if (deque==NULL){ //ignore if deque is null
        return 0;
    }
    for (deque_node_t node = deque_start(deque); node!=NULL;){
        deque_node_t newnode = node->next; //bc else we access freed memory
        free(node); //the object is allocated along w the node
        node=newnode;
    }
    free(deque);
    *taggedp = NULL;
    return 0;
}

int deque_fprint(FILE *fp, void *tagged, int (*elem_fprint)(FILE *fp, const void *elemp)) {
  int nchars = 0;
  
  assert(fp && "Missing output FILE*");
  deque_t untagged = UNTAG(tagged);
  assert(untagged!=NULL && "Please pass non-null to deque_fprint.");


  nchars += fprintf(fp, "[ ");

  for (deque_node_t node = untagged->start; node!=NULL; node=node->next) {
    void *elemp = object_of(node);
    nchars += elem_fprint ? elem_fprint(fp, elemp) + fprintf(fp, (node->next!=NULL) ? "<->" : " ") : fprintf(fp, "## ");
  }

  nchars += fprintf(fp, "]\n");

  return nchars;
}