#!/bin/bash

if [ -z "${WEATHERRRD}" ]
then
	WEATHERRRD=${PWD}/weather.rrd
fi

if [ -z "${WEATHERGRAPHDIR}" ]
then
	WEATHERGRAPHDIR=${PWD}
fi


rrdtool graph ${WEATHERGRAPHDIR}/temperature-day.png \
	--end now \
	--start end-86400s \
	--width 600 \
	-h 100 \
	-a PNG \
	--font DEFAULT:7 \
	--slope-mode \
	--title "Temperature" \
	--watermark "`date`" \
	DEF:temperature=${WEATHERRRD}:temperature:MAX \
	LINE1:temperature#0000FF:"Temperature (C)"

rrdtool graph ${WEATHERGRAPHDIR}/pressure-day.png \
	--end now \
	--start end-86400s \
	--width 600 \
	-h 100 \
	-a PNG \
	--font DEFAULT:7 \
	--slope-mode \
	--title "Atmospheric pressure" \
	--watermark "`date`" \
	DEF:qnh=${WEATHERRRD}:pressure_qnh:MAX \
	DEF:qfe=${WEATHERRRD}:pressure_qfe:MAX \
	LINE1:qnh#0000FF:"QNH (hPa)" \
	GPRINT:qnh:LAST:"Current\:%8.2lf %s"  \
	GPRINT:qnh:AVERAGE:"Average\:%8.2lf %s"  \
	GPRINT:qnh:MAX:"Maximum\:%8.2lf %s\n"  \
	LINE2:qfe#00FF00:"QFE (hPa)" \
	GPRINT:qfe:LAST:"Current\:%8.2lf %s"  \
	GPRINT:qfe:AVERAGE:"Average\:%8.2lf %s"  \
	GPRINT:qfe:MAX:"Maximum\:%8.2lf %s\n" 

rrdtool graph ${WEATHERGRAPHDIR}/humidityrel-day.png \
	--end now \
	--start end-86400s \
	--width 600 \
	-h 100 \
	-a PNG \
	--font DEFAULT:7 \
	--slope-mode \
	--title "Air humidity, relative" \
	--watermark "`date`" \
	DEF:humidityrel=${WEATHERRRD}:humidity_rel:MAX \
	LINE1:humidityrel#0000FF:"Humidity (%)"

rrdtool graph ${WEATHERGRAPHDIR}/humidityabs-day.png \
	--end now \
	--start end-86400s \
	--width 600 \
	-h 100 \
	-a PNG \
	--font DEFAULT:7 \
	--slope-mode \
	--title "Air humidity, absolute" \
	--watermark "`date`" \
	DEF:humidityabs=${WEATHERRRD}:humidity_abs:MAX \
	LINE1:humidityabs#0000FF:"Humidity (g/m^3)"
