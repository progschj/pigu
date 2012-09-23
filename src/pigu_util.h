#ifndef PIGU_UTIL_H
#define PIGU_UTIL_H


inline int PIGU_get_bit(uint32_t *bits, uint32_t bit)
{
   return (bits[bit/32] >> (bit%32)) & 1;
}
inline void PIGU_enable_bit(uint32_t *bits, uint32_t bit)
{
   bits[bit/32] |= 1u << (bit%32);
}
inline void PIGU_disable_bit(uint32_t *bits, uint32_t bit)
{
   bits[bit/32] &= ~(1u << (bit%32));
}
inline void PIGU_set_bit(uint32_t *bits, uint32_t bit, int value)
{
   if(value)
      PIGU_enable_bit(bits, bit);
   else
      PIGU_disable_bit(bits, bit);
}

#endif
