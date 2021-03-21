# Which Timer and Counter does your Controller have? #
- 8-bit Timer/Counter0 with PWM
- Timer/Counter0 and Timer/Counter1 Prescalers
- 16-bit Timer/Counter1
- 8-bit Timer/Counter2 with PWM and Asynchronous Operation

# Whats the difference between Timer and Counter? #
Ein Timer ist ein Zähler, der per Software initialisiert wird, dann aber programmunabhängig ein bestimmtes Register im μC hochzählt und bei Überlauf bzw. bei Erreichen eines Wertes einen Interrupt auslösen kann. Falls externe Signale gezählt werden, spricht man von einem Counter
A timer is a counter, that gets initialized by software and increments independently a specific register in the µC and  is able to fire an interrupt if a specific value is reached or an overflow happens. 
A counter exclusivly counts external signals. 
