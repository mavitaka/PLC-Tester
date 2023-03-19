#ifndef CONFIG_H
#define CONFIG_H
#include <QTime>

/******************config type_Band ******************/
#define CENELEC '1'
#define FCC     '2'
#define ARIB    '3'
/******************config modulation_type ************/
#define BPSK  '0'
#define QPSK  '1'
#define EPSK  '2'
#define ROB   '4'
/***************** carrier_config *******************/
#define NUM_CARR   "55"
#define FIR_CARR   "33"
#define LAS_CARR   "60"
/********************** command_config *************/
#define TEST '1'
#define CONF '2'
#define DATA '3'
#define MOD_SCHEME_DIFFERENTIAL '0'
#define MOD_SCHEME_COHERENT     '1'
class time
{
public:
QTime timer;
void start_timer(void)
{
    timer.restart();
}
int get_time(void)
{
   return(timer.elapsed()) ;
}



};
















#endif // CONFIG_H
