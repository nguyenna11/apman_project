#define tag              CAT(key_t,CAT(_,impl))
#define key_hash         CAT(key_t,_hash)
#define key_compare      CAT(key_t,_compare)
#define key_fprint       CAT(key_t,_fprint)
#define value_t          key_t
#define value_fprint     CAT(value_t,_fprint)
