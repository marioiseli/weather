#!/bin/bash

if [ -z "${WEATHERFILE}" ]
then
	WEATHERFILE=${PWD}/weather.json
fi

if [ -z "${WEATHERRRD}" ]
then
	WEATHERRRD=${PWD}/weather.rrd
fi

if [ ! -f "${WEATHERRRD}" ];
then
	echo "Creating RRD"
	rrdtool create ${WEATHERRRD} \
		--start `date +%s` \
		--step 60 \
		DS:temperature:GAUGE:120:-10:50 \
		DS:pressure_qnh:GAUGE:120:900:1100 \
		DS:pressure_qfe:GAUGE:120:900:1100 \
		DS:humidity_abs:GAUGE:120:0:100 \
		DS:humidity_rel:GAUGE:120:0:100 \
		RRA:MAX:0.5:1:1440 \
		RRA:AVERAGE:0.5:12:24 \
		RRA:AVERAGE:0.5:288:31
fi

rrdtool update ${WEATHERRRD} \
	-t temperature:pressure_qnh:pressure_qfe:humidity_abs:humidity_rel \
	$(jq .timestamp ${WEATHERFILE}):$(jq .weather.temperature ${WEATHERFILE}):$(jq .weather.qnh ${WEATHERFILE}):$(jq .weather.qfe ${WEATHERFILE}):$(jq .weather.humidity_absolute  ${WEATHERFILE}):$(jq .weather.humidity_relative ${WEATHERFILE})
