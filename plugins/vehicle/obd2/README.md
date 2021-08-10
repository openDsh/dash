# OBD2 integration

To test without OBD2, send packets via CANBUS:

```bash
cansend can0 7e8#03010D1e11100000 # speed 30 km/h
cansend can0 7e8#03010C1e11100000 # rpm to 1.9
cansend can0 7e8#0301056e00000000 # temp to 70C
cansend can0 7e8#0301046e11100000 # engine load 43%
# mpg  - 10l/100km (23mpg)
cansend can0 7e8#030110004f111110 && cansend can0 7e8#03010D1e11100000
*/
```

Current functionality:

* Speed
* RPM
* Engine temp
* Engine load
* MPG
