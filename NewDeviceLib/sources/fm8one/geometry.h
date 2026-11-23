#ifndef LT48_FM8ONE_GEOMETRY_H
#define LT48_FM8ONE_GEOMETRY_H
/* Dataman13.10 GDACA D9/M7/P8 says12bits. The original Read/Program
 * workers mask the high byte with1F, and Feeling FM8P54/56 Rev1.21 p4
 * specifies1024x13. Storage remains2048bytes; source resources unchanged. */
static unsigned fm8one_element_bits(unsigned cls,unsigned d,unsigned f,unsigned p,unsigned original,size_t capacity){
 return cls==2035&&d==9&&f==7&&p==8&&original==12&&capacity==2048?13:original;
}
#endif
