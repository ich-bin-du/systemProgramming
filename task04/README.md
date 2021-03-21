# Which Timer and Counter does your Controller have? #
- 8-bit Timer/Counter0 with PWM
- Timer/Counter0 and Timer/Counter1 Prescalers
- 16-bit Timer/Counter1
- 8-bit Timer/Counter2 with PWM and Asynchronous Operation

# Whats the difference between Timer and Counter? #
A timer is a counter, that gets initialized by software and increments independently a specific register in the µC and  is able to fire an interrupt if a specific value is reached or an overflow happens. A counter exclusivly counts external signals. 

# Are there timers or counters which have impacts on the I/O-Pins? #
Yes every counter needs an external signal on an Input. 

# How does a timer support the output of a pulse width modulation (PWM)? Whats Phase Correct, Phase Correct Inverted and fast PWM? #
The Timer/Counter Overflow Flag (TOV0) is set each time the counter reaches MAX. If the interrupt is enabled, the interrupt handler routine can be used for updating the compare value.


## Phase Correct PWM Mode ##
 In non inverting Compare Output mode, the Output Compare (OC0) is cleared on the compare match
between TCNT0 and OCR0 while upcounting, and set on the compare match while downcounting. The counter counts repeatedly from **BOTTOM** to **MAX** and then from **MAX** to **BOTTOM**.
## Phase Correct Inverted PWM Mode ##
In inverting Output Compare mode, the operation is inverted. The dual-slope operation has lower maximum operation frequency than single slope operation. However, due to the symmetric feature of the dual-slope PWM modes, these modes are preferred for motor control applications
## fast PWM ##
the fast PWM mode uses the overflow and resets after counting from **BOTTOM** to **MAX on BOTTOM**. 


for further information read pages 79 and 80 in ATmega32A
