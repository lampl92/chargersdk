/*
 * Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

//#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "minmea.h"
#include "retarget.h"
#include "bsp_uart.h"

extern     int gps_handle;
int minmea_main(void)
{
    char line[1024];

    int rb;


    while (1)
    {   
        memset(line, 0, sizeof(line));
        rb = uart_read_line(gps_handle, (uint8_t *)line, sizeof(line), 1000);
        if (rb > 0)
        {
            printf_safe("%s\n", line);
            switch (minmea_sentence_id(line, true))
            {
            case MINMEA_SENTENCE_RMC:
                {
                    struct minmea_sentence_rmc frame;
                    if (minmea_parse_rmc(&frame, line))
                    {
                        printf_safe("$xxRMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n",
                            frame.latitude.value,
                            frame.latitude.scale,
                            frame.longitude.value,
                            frame.longitude.scale,
                            frame.speed.value,
                            frame.speed.scale);
                        printf_safe("$xxRMC fixed-point coordinates and speed scaled to three decimal places: (%d,%d) %d\n",
                            minmea_rescale(&frame.latitude, 1000),
                            minmea_rescale(&frame.longitude, 1000),
                            minmea_rescale(&frame.speed, 1000));
                        printf_safe("$xxRMC floating point degree coordinates and speed: (%f,%f) %f\n",
                            minmea_tocoord(&frame.latitude),
                            minmea_tocoord(&frame.longitude),
                            minmea_tofloat(&frame.speed));
                        printf_safe("$xxRMC data %02d-%02d-%02d, %02d:%02d:%02d\n", 
                            frame.date.year,
                            frame.date.month,
                            frame.date.day,
                            frame.time.hours,
                            frame.time.minutes,
                            frame.time.seconds);
                    }
                    else
                    {
                        printf_safe("$xxRMC sentence is not parsed\n");
                    }
                }
                break;

            case MINMEA_SENTENCE_GGA:
                {
                    struct minmea_sentence_gga frame;
                    if (minmea_parse_gga(&frame, line))
                    {
                        printf_safe("$xxGGA: fix quality: %d\n", frame.fix_quality);
                    }
                    else
                    {
                        printf_safe("$xxGGA sentence is not parsed\n");
                    }
                }
                break;

            case MINMEA_SENTENCE_GST:
                {
                    struct minmea_sentence_gst frame;
                    if (minmea_parse_gst(&frame, line))
                    {
                        printf_safe("$xxGST: raw latitude,longitude and altitude error deviation: (%d/%d,%d/%d,%d/%d)\n",
                            frame.latitude_error_deviation.value,
                            frame.latitude_error_deviation.scale,
                            frame.longitude_error_deviation.value,
                            frame.longitude_error_deviation.scale,
                            frame.altitude_error_deviation.value,
                            frame.altitude_error_deviation.scale);
                        printf_safe("$xxGST fixed point latitude,longitude and altitude error deviation"
                                             " scaled to one decimal place: (%d,%d,%d)\n",
                            minmea_rescale(&frame.latitude_error_deviation, 10),
                            minmea_rescale(&frame.longitude_error_deviation, 10),
                            minmea_rescale(&frame.altitude_error_deviation, 10));
                        printf_safe("$xxGST floating point degree latitude, longitude and altitude error deviation: (%f,%f,%f)",
                            minmea_tofloat(&frame.latitude_error_deviation),
                            minmea_tofloat(&frame.longitude_error_deviation),
                            minmea_tofloat(&frame.altitude_error_deviation));
                    }
                    else
                    {
                        printf_safe("$xxGST sentence is not parsed\n");
                    }
                }
                break;

            case MINMEA_SENTENCE_GSV:
                {
                    struct minmea_sentence_gsv frame;
                    if (minmea_parse_gsv(&frame, line))
                    {
                        printf_safe("$xxGSV: message %d of %d\n", frame.msg_nr, frame.total_msgs);
                        printf_safe("$xxGSV: sattelites in view: %d\n", frame.total_sats);
                        for (int i = 0; i < 4; i++)
                            printf_safe("$xxGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
                                frame.sats[i].nr,
                                frame.sats[i].elevation,
                                frame.sats[i].azimuth,
                                frame.sats[i].snr);
                    }
                    else
                    {
                        printf_safe("$xxGSV sentence is not parsed\n");
                    }
                }
                break;

            case MINMEA_SENTENCE_VTG:
                {
                    struct minmea_sentence_vtg frame;
                    if (minmea_parse_vtg(&frame, line))
                    {
                        printf_safe("$xxVTG: true track degrees = %f\n",
                            minmea_tofloat(&frame.true_track_degrees));
                        printf_safe("        magnetic track degrees = %f\n",
                            minmea_tofloat(&frame.magnetic_track_degrees));
                        printf_safe("        speed knots = %f\n",
                            minmea_tofloat(&frame.speed_knots));
                        printf_safe("        speed kph = %f\n",
                            minmea_tofloat(&frame.speed_kph));
                    }
                    else
                    {
                        printf_safe("$xxVTG sentence is not parsed\n");
                    }
                }
                break;

            case MINMEA_SENTENCE_ZDA:
                {
                    struct minmea_sentence_zda frame;
                    if (minmea_parse_zda(&frame, line))
                    {
                        printf_safe("$xxZDA: %d:%d:%d %02d.%02d.%d UTC%+03d:%02d\n",
                            frame.time.hours,
                            frame.time.minutes,
                            frame.time.seconds,
                            frame.date.day,
                            frame.date.month,
                            frame.date.year,
                            frame.hour_offset,
                            frame.minute_offset);
                    }
                    else
                    {
                        printf_safe("$xxZDA sentence is not parsed\n");
                    }
                }
                break;

            case MINMEA_INVALID:
                {
                    printf_safe("$xxxxx sentence is not valid\n");
                }
                break;

            default:
                {
                    printf_safe("$xxxxx sentence is not parsed\n");
                }
                break;
            }
        }
    }
       

    return 0;
}

/* vim: set ts=4 sw=4 et: */
