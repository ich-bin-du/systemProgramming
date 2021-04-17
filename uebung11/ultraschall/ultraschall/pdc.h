/*
 * pdc.h
 *
 * Created: 17.04.2021 17:23:08
 *  Author: Dominik Amthor
 */ 


#ifndef PDC_H_
#define PDC_H_

#define C8			4186
#define D5			587
#define SILENCE		0


void speaker_init( void );

void speaker_init_timer1( void );

void speaker_set_signal( uint32_t frequency );

void my_delay( uint32_t delay );

void speaker_play_peep( uint32_t delay );

void speaker_distance_sound( uint32_t distance );


#endif /* PDC_H_ */