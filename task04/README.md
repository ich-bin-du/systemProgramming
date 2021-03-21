# Which Timer and Counter does your Controller have? #
- 8-bit Timer/Counter0 with PWM
- Timer/Counter0 and Timer/Counter1 Prescalers
- 16-bit Timer/Counter1
- 8-bit Timer/Counter2 with PWM and Asynchronous Operation

# Whats the difference between Timer and Counter? #
A timer is a counter, that gets initialized by software and increments independently a specific register in the µC and  is able to fire an interrupt if a specific value is reached or an overflow happens. A counter exclusivly counts external signals. 

# Are there timers or counters which have impacts on the I/O-Pins? #
Yes every counter needs an external signal on an Input. 

# How does a timer support the output of a pulse width modulation (PWM)? Whats in phase, in phase inverted and fast PWM? #
The Timer/Counter Overflow Flag (TOV0) is set each time the counter reaches MAX. If the interrupt is enabled, the interrupt handler routine can be used for updating the compare value.


## in phase ##
## in phase inverted ##
## fast PWM ##
