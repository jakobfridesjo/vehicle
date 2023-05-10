# fordon

This is a remote controlled robot car

To car:

    Control motor speed and direction:
        m,<1/2><0-9>,<1/2><0-9>.
            <1/2>: back/forward
            <0-9>: speed level
            
    Set motor speed to half or full:
        g,<0/1>.
            <0/1>: full/half

    Dead man, disables or enables motor control:
        d,3,<0/1>.
            <0/1>: stop/run (green led on/off)
    
    Emergency stop:
        n,<text>.
            <text>: Max 16 characters (red led on)
    
    Emergency stop unlock:
        k.
    
    Write text to display:
        t,<text>
            <text>: Max 16 characters
    
    Add notes to melody buffer:
        p,<a-c/0><3-6><0/s><1-8>.
            <a-c/0>: Note or silent
            <3-6>: Octave
            <0/s>: Full/half notes
            <1-8>: Note length
    
    Returns PONG. from car:
        PING.
    
    Enable buzzer:
        b,3,<0/1>
            <0/1>: off/on
    
    Heartbeat, 3 seconds without disables motors:
        h.

From car:

    PONG. when receiving PING.:
        PONG.
    
    Return sensor values:
        s,<000-255>,<000-255>,<0/1>,<0/1>,<0/1>.
            <000-255>: Padded back distance sensor result in cm
            <000-255>: Padded front distance sensor result in cm
            <0/1>: no motion/motion (PIR sensor)
            <0/1>: off/on left trigger
            <0/1>: off/on right trigger
    
    Car stopped by trigger left:
        f,0.

    Car stopped by trigger right:
        f,1.

    Car stopped by emergency stop:
        f,2.

    Car stopped by triggered dead man:
        f,3.

    Car stopped by no heartbeat received:
        f,4.

    Car stopped by distance <= 15cm:
        f,5.

    Button pressed:
        b,4,1.

    Button released:
        b,4,0.
