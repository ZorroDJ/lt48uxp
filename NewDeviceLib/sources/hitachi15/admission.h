#define LT48_HITACHI15_STAGE 2
static int h15_admitted(unsigned d){return d==13?LT48_HITACHI15_STAGE>=1:(d==14||d==15)&&LT48_HITACHI15_STAGE>=2;}
