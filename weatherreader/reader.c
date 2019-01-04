/* 
 * Copyright (c) 2019 Mario Iseli
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 * Compile with: gcc -lm -g -o reader reader.c 
 */

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define TTY_DEFAULT "/dev/ttyACM0"
#define JSON_DEFAULT "weather.json"
#define ALTITUDE_DEFAULT 440 // Altitude of Biel, Switerland
#define BSIZE 1024
#define MSG_PREFIX_TEMP "Temperature"
#define MSG_PREFIX_PRESSURE "Pressure"
#define MSG_PREFIX_HUMIDITY "Relative humidity"
#define MSG_SUFFIX_TEMP 'C'
#define MSG_SUFFIX_PRESSURE 'P'
#define MSG_SUFFIX_HUMIDITY '%'

struct weather {
	float temperature; // degrees Celsius
	int humidity_relative; // Relative humidity in percent
	float humidity_absolute; // Absolute humidity in g/m^3
	float pressure_qfe; // measured pressure in hectopascal (hPa) at station level
	float pressure_qnh; // calculated pressure on sealevel (ICAO standard) in hectopascal (hPa)
};


void updateJSON(char* j, struct weather* w)
{
	int fd = open(j, O_RDWR | O_CREAT | O_TRUNC, 0644);
	char *s;
	s = malloc(BSIZE);
	sprintf(s, "{\"timestamp\":%d,\"weather\":{\"temperature\":%.2f,\"qfe\":%.2f,\"qnh\":%.2f,\"humidity_relative\":%d,\"humidity_absolute\":%.2f}}", time(NULL), w->temperature, w->pressure_qfe, w->pressure_qnh, w->humidity_relative, w->humidity_absolute);
	write(fd, s, strlen(s));
	write(fd, "\n", 1);
	close(fd);
};

int qfe2qnh(float qfe, int alt)
{
	return (((alt * 3.208))/30)+qfe;
};

float hrel2habs(int r, float t)
{
	return (6.112*pow(2.71828,((17.67*t)/(243.5+t)))*r*2.1674)/(273.15+t);
};

int main(int argc, char* argv[])
{
	bool d = false; // Decides whether program daemonizes or not
	int tty; // file descriptor for TTY (serial interface)
	int ret;
	int opt;
	int alt = ALTITUDE_DEFAULT; // station altitude in meters
	char* ttyf = TTY_DEFAULT; // TTY filename, argument
	char* jf;
	char buffer[BSIZE];
	fd_set f;
	char *s;
	char *u;
	struct weather w;
	struct timeval t;

	jf = malloc(NAME_MAX);
	sprintf(jf, "%s/%s", getenv("PWD"), JSON_DEFAULT);

	while ((opt = getopt(argc, argv, "a:df:t:")) != -1)
	{
		switch (opt)
		{
			case 'a': // Altitude
				alt = atoi(optarg);
				break;
               		case 'd': // Daemonize
				d = true;
                  		break;
			case 'f': // Output file
				jf = malloc(NAME_MAX);
				strcpy(jf, optarg);
				break;
			case 't': // TTY device
				ttyf = malloc(NAME_MAX);
				strcpy(ttyf, optarg);
				break;
               		default:
                   		fprintf(stderr, "Usage: %s [-a altitude] [-f file] [-t tty] [-d]\n", argv[0]);
                   		exit(EXIT_FAILURE);
               }
	}

	printf("Data will be read from: %s\n", ttyf);
	printf("Output file for weather data: %s\n", jf);
	printf("QNH/QFE conversion will be calculated on station height: %d meters above sea level\n", alt);

	// Opening TTY, initializing weather record, etc.
	tty = open(ttyf, O_RDONLY);
	w.temperature = 0;
	w.humidity_relative = 0;
	w.humidity_absolute = 0;
	w.pressure_qnh = 0;
	w.pressure_qfe = 0;
	FD_ZERO(&f);
	FD_SET(tty, &f);
	t.tv_sec = 50;
	t.tv_usec = 0;

	// Daemonize if requested (-d)
	if(d) { daemon(0, 0); }

	while(1)
	{
		ret = select(tty+1, &f, NULL, NULL, &t);
        	if (ret >= 0) {
            		ret = read(tty, buffer, BSIZE-1);
            		buffer[ret] = '\0';
			if(s = strstr(buffer, MSG_PREFIX_TEMP))
			{
				s = strchr(s, ':');
				s++; s++;
				u = strchr(s, MSG_SUFFIX_TEMP);
				*u = '\0';
				w.temperature = atof(s);
			}
				#ifdef DEBUG
				printf("Temp: %f\n", w.temperature);
				#endif
			if(s = strstr(buffer, MSG_PREFIX_PRESSURE))
			{
				s = strchr(s, ':');
				s++; s++;
				u = strchr(s, MSG_SUFFIX_PRESSURE);
				*u = '\0';
				w.pressure_qfe = (atoi(s)/100);
				w.pressure_qnh = qfe2qnh(w.pressure_qfe, alt);
				#ifdef DEBUG
				printf("QFE: %f\n", w.pressure_qfe);
				printf("QNH: %f\n", w.pressure_qnh);
				#endif
			}
			if(s = strstr(buffer, MSG_PREFIX_HUMIDITY))
			{
				s = strchr(s, ':');
				s++; s++;
				u = strchr(s, MSG_SUFFIX_HUMIDITY);
				*u = '\0';
				w.humidity_relative = atoi(s);
				w.humidity_absolute = hrel2habs(w.humidity_relative, w.temperature);
				#ifdef DEBUG
				printf("Hrel: %d\n", w.humidity_relative);
				printf("Habs: %f\n", w.humidity_absolute);
				#endif
			}
        	}
		updateJSON(jf, &w);
	}
}
