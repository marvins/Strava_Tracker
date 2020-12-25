#!/bin/bash
rm ga_routes.gif
convert -verbose -delay 80 -loop 0 -dispose previous frames/*.png ga_routes.gif
