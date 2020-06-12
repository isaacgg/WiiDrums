/*
 *	wiiuse
 *
 *	Written By:
 *		Michael Laforest	< para >
 *		Email: < thepara (--AT--) g m a i l [--DOT--] com >
 *
 *	Copyright 2006-2007
 *
 *	This file is part of wiiuse.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	$Header$
 *
 */

/**
 *	@file
 *
 *	@brief Example using the wiiuse API.
 *
 *	This file is an example of how to use the wiiuse library.
 */
#include "./drum_sounds.h"

#include <stdio.h>  /* for printf */
#include "wiiuse.h" /* for wiimote_t, classic_ctrl_t, etc */

#ifndef WIIUSE_WIN32
#include <unistd.h> /* for usleep */
#endif

//#include <winmm.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


#define MAX_WIIMOTES 1

/**
 *	@brief Callback that handles an event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *
 *	This function is called automatically by the wiiuse library when an
 *	event occurs on the specified wiimote.
 */

union
{
    unsigned long word;
    unsigned char data[4];
} message;

union
{
    int green;
    int blue;
    int red;
    int yellow_open;
    int yellow_close;
    int orange;
    int pedal;
    int hihat_pedal;
} drumkit;

void handle_event(struct wiimote_t *wm, HMIDIOUT *device)
{
    int flag = 0;
    message.data[0] = 0x99; // MIDI note-on message (requires to data bytes) channel 1
    // message.data[1] = 38;  // MIDI note-on message: Key number (60 = middle C)
    // message.data[2] = 100; // MIDI note-on message: Key velocity (100 = loud)
    message.data[3] = 0;    // Unused parameter

        /* guitar hero 3 guitar */
        struct guitar_hero_drums_t *ghdrums = (guitar_hero_drums_t *)&wm->exp.ghdrums;

        // if (IS_PRESSED(ghdrums, GUITAR_HERO_DRUMS_BUTTON_STRUM_UP)) {
        // 	printf("DRUMS: Strum Up pressed\n");
        // }
        message.data[2] = 100;
        //+(int)ghdrums->whammy_bar;
        if (IS_DRUM_HIT(ghdrums, GUITAR_HERO_DRUMS_BUTTON_PEDAL))
        {
            if (((int)ghdrums->whammy_bar % 2) == 1)
            {
                message.data[1] = drumkit.hihat_pedal;
                message.data[2] = 120;
                midiOutShortMsg(device, message.word);
            } else
            {
                message.data[1] = drumkit.pedal;
                message.data[2] = 120;
                midiOutShortMsg(device, message.word);
            }
        }
        if (IS_DRUM_HIT(ghdrums, GUITAR_HERO_DRUMS_BUTTON_YELLOW))
        {
            if (IS_HIHAT_OPEN(ghdrums))
            {
                message.data[1] = drumkit.yellow_open;
                midiOutShortMsg(device, message.word);
            } else
            {
                message.data[1] = drumkit.yellow_close;
                midiOutShortMsg(device, message.word);
            }
        }
        if (IS_DRUM_HIT(ghdrums, GUITAR_HERO_DRUMS_BUTTON_GREEN))
        {
            message.data[1] = drumkit.green;
            midiOutShortMsg(device, message.word);
            Sleep(200);
        }
        if (IS_DRUM_HIT(ghdrums, GUITAR_HERO_DRUMS_BUTTON_BLUE))
        {
            message.data[1] = drumkit.blue;
            midiOutShortMsg(device, message.word);
        }
        if (IS_DRUM_HIT(ghdrums, GUITAR_HERO_DRUMS_BUTTON_RED))
        {
            message.data[1] = drumkit.red;
            midiOutShortMsg(device, message.word);
        }
        if (IS_DRUM_HIT(ghdrums, GUITAR_HERO_DRUMS_BUTTON_ORANGE))
        {
            message.data[1] = drumkit.orange;
            midiOutShortMsg(device, message.word);
        }

           if (flag != MMSYSERR_NOERROR)
        {
            printf("Error opening MIDI Output.\n");
            return 1;
        }
        // if (IS_DRUM_HIT(ghdrums, GUITAR_HERO_DRUMS_BUTTON_PLUS))
        // {
        //     printf("DRUMS: Plus pressed\n");
        // }
        // if (IS_DRUM_HIT(ghdrums, GUITAR_HERO_DRUMS_BUTTON_MINUS))
        // {
        //     printf("DRUMS: Minus pressed\n");
        // }

        // printf("DRUMS whammy bar:          %f\n", ghdrums->whammy_bar);
        // printf("DRUMS joystick angle:      %f\n", ghdrums->js.ang);
        // printf("DRUMS joystick magnitude:  %f\n", ghdrums->js.mag);
}

/**
 *	@brief Callback that handles a read event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *	@param data		Pointer to the filled data block.
 *	@param len		Length in bytes of the data block.
 *
 *	This function is called automatically by the wiiuse library when
 *	the wiimote has returned the full data requested by a previous
 *	call to wiiuse_read_data().
 *
 *	You can read data on the wiimote, such as Mii data, if
 *	you know the offset address and the length.
 *
 *	The \a data pointer was specified on the call to wiiuse_read_data().
 *	At the time of this function being called, it is not safe to deallocate
 *	this buffer.
 */
void handle_read(struct wiimote_t *wm, byte *data, unsigned short len)
{
    int i = 0;

    printf("\n\n--- DATA READ [wiimote id %i] ---\n", wm->unid);
    printf("finished read of size %i\n", len);
    for (; i < len; ++i)
    {
        if (!(i % 16))
        {
            printf("\n");
        }
        printf("%x ", data[i]);
    }
    printf("\n\n");
}

/**
 *	@brief Callback that handles a controller status event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *	@param attachment		Is there an attachment? (1 for yes, 0 for no)
 *	@param speaker			Is the speaker enabled? (1 for yes, 0 for no)
 *	@param ir				Is the IR support enabled? (1 for yes, 0 for no)
 *	@param led				What LEDs are lit.
 *	@param battery_level	Battery level, between 0.0 (0%) and 1.0 (100%).
 *
 *	This occurs when either the controller status changed
 *	or the controller status was requested explicitly by
 *	wiiuse_status().
 *
 *	One reason the status can change is if the nunchuk was
 *	inserted or removed from the expansion port.
 */
void handle_ctrl_status(struct wiimote_t *wm)
{
    printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

    printf("attachment:      %i\n", wm->exp.type);
    printf("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
    printf("ir:              %i\n", WIIUSE_USING_IR(wm));
    printf("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2),
           WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
    printf("battery:         %f %%\n", wm->battery_level);
}

/**
 *	@brief Callback that handles a disconnection event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *
 *	This can happen if the POWER button is pressed, or
 *	if the connection is interrupted.
 */
void handle_disconnect(wiimote *wm) { printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid); }

void test(struct wiimote_t *wm, byte *data, unsigned short len)
{
    printf("test: %i [%x %x %x %x]\n", len, data[0], data[1], data[2], data[3]);
}

short any_wiimote_connected(wiimote **wm, int wiimotes)
{
    int i;
    if (!wm)
    {
        return 0;
    }

    for (i = 0; i < wiimotes; i++)
    {
        if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i]))
        {
            return 1;
        }
    }

    return 0;
}

/**
 *	@brief main()
 *
 *	Connect to up to two wiimotes and print any events
 *	that occur on either device.
 */
int main(int argc, char **argv)
{
    HMIDIOUT device; // MIDI device interface for sending MIDI output

    int midiport  = 0;   // select which MIDI output port to open
    int flag;            // monitor the status of returning functions
    // Open the MIDI output port
    flag = midiOutOpen(&device, midiport, 0, 0, CALLBACK_NULL);
    if (flag != MMSYSERR_NOERROR)
    {
        printf("Error opening MIDI Output.\n");
        return 1;
    }
    
    int config_done = 0;

    drumkit.green        = LowMidTom;
    drumkit.blue         = HighMidTom;
    drumkit.red          = AcousticSnare;
    drumkit.hihat_pedal  = PedalHighHat;
    drumkit.yellow_open  = OpenHighHat;
    drumkit.yellow_close = ClosedHighHat;
    drumkit.orange       = CrashCymbal1;
    drumkit.pedal        = AcousticBassDrum;

    /*Old school menu*/
    int option;
    int button_option;
    int sound_option;
    while (config_done == 0)
    {
        printf("Select an option:\n");
        printf("(1) Play\n");
        printf("(2) Config sounds\n");
        printf("(3) Exit\n");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            config_done = 1;
            break;
        case 2:
            printf("Select a button:\n");
            printf("(1) green\n");
            printf("(2) blue\n");
            printf("(3) red\n");
            printf("(4) yellow_open\n");
            printf("(5) yellow_close\n");
            printf("(6) orange\n");
            printf("(7) hihat pedal\n");
            printf("(8) bass pedal\n");
			printf("Enter anything else to skip");
            scanf("%d", &button_option);
            if ((button_option > 0) &( button_option <= 8))
            {
                printf("Select a sound\n");
                scanf("%d", &sound_option);
                switch (button_option)
                {
                case 1:
                    drumkit.green = sound_option;
                    break;
                case 2:
                    drumkit.blue = sound_option;
                    break;
                case 3:
                    drumkit.red = sound_option;
                    break;
                case 4:
                    drumkit.yellow_open = sound_option;
                    break;
                case 5:
                    drumkit.yellow_close = sound_option;
                    break;
                case 6:
                    drumkit.orange = sound_option;
                    break;
                case 7:
                    drumkit.hihat_pedal = sound_option;
                    break;
                case 8:
                    drumkit.pedal = sound_option;
                    break;
                default:
                    break;
                }
            }
            break;
        case 3:
            //// turn any MIDI notes currently playing:
            midiOutReset(device);
            //// Remove any data in MIDI device and close the MIDI Output port
            midiOutClose(device);
            exit(0);
        default:
            printf("Option not valid, just type 1, 2 or 3");
            break;
        }
    }



    message.data[0] = 0x99; // MIDI note-on message (requires to data bytes) channel 1
    message.data[1] = 38;   // MIDI note-on message: Key number (60 = middle C)
    message.data[2] = 100;  // MIDI note-on message: Key velocity (100 = loud)
    message.data[3] = 0;    // Unused parameter

    midiOutShortMsg(device, message.word);


#ifndef WIIUSE_WIN32
    usleep(200000);
#else
    Sleep(200);
#endif

    wiimote **wiimotes;
    int found, connected;

    /*
     *	Initialize an array of wiimote objects.
     *
     *	The parameter is the number of wiimotes I want to create.
     */
    wiimotes = wiiuse_init(MAX_WIIMOTES);

    /*
     *	Find wiimote devices
     *
     *	Now we need to find some wiimotes.
     *	Give the function the wiimote array we created, and tell it there
     *	are MAX_WIIMOTES wiimotes we are interested in.
     *
     *	Set the timeout to be 5 seconds.
     *
     *	This will return the number of actual wiimotes that are in discovery mode.
     */
    found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);
    if (!found)
    {
        printf("No wiimotes found.\n");
        return 0;
    }

    /*
     *	Connect to the wiimotes
     *
     *	Now that we found some wiimotes, connect to them.
     *	Give the function the wiimote array and the number
     *	of wiimote devices we found.
     *
     *	This will return the number of established connections to the found wiimotes.
     */
    connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
    if (connected)
    {
        printf("Connected to %i wiimotes (of %i found).\n", connected, found);
    } else
    {
        printf("Failed to connect to any wiimote.\n");
        return 0;
    }

    /*
     *	Now set the LEDs and rumble for a second so it's easy
     *	to tell which wiimotes are connected (just like the wii does).
     */
    //wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
    //wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_2);
    //wiiuse_set_leds(wiimotes[2], WIIMOTE_LED_3);
    //wiiuse_set_leds(wiimotes[3], WIIMOTE_LED_4);
    // wiiuse_rumble(wiimotes[0], 1);
    // wiiuse_rumble(wiimotes[1], 1);

#ifndef WIIUSE_WIN32
    usleep(200000);
#else
    Sleep(200);
#endif

    /*
     *	Maybe I'm interested in the battery power of the 0th
     *	wiimote.  This should be WIIMOTE_ID_1 but to be sure
     *	you can get the wiimote associated with WIIMOTE_ID_1
     *	using the wiiuse_get_by_id() function.
     *
     *	A status request will return other things too, like
     *	if any expansions are plugged into the wiimote or
     *	what LEDs are lit.
     */
    /* wiiuse_status(wiimotes[0]); */

    /*
     *	This is the main loop
     *
     *	wiiuse_poll() needs to be called with the wiimote array
     *	and the number of wiimote structures in that array
     *	(it doesn't matter if some of those wiimotes are not used
     *	or are not connected).
     *
     *	This function will set the event flag for each wiimote
     *	when the wiimote has things to report.
     */
    wiimotes[0]->exp.type = EXP_GUITAR_HERO_DRUMS;
    while (any_wiimote_connected(wiimotes, MAX_WIIMOTES))
    {
        if (wiiuse_poll(wiimotes, MAX_WIIMOTES))
        {
            /*
             *	This happens if something happened on any wiimote.
             *	So go through each one and check if anything happened.
             */
            int i = 0;
            for (; i < MAX_WIIMOTES; ++i)
            {
                switch (wiimotes[i]->event)
                {
                case WIIUSE_EVENT:
                    /* a generic event occurred */
                    handle_event(wiimotes[i], device);
                    break;

                case WIIUSE_STATUS:
                    /* a status event occurred */
                    handle_ctrl_status(wiimotes[i]);
                    break;

                case WIIUSE_DISCONNECT:
                case WIIUSE_UNEXPECTED_DISCONNECT:
                    /* the wiimote disconnected */
                    handle_disconnect(wiimotes[i]);
                    break;

                case WIIUSE_READ_DATA:
                    /*
                     *	Data we requested to read was returned.
                     *	Take a look at wiimotes[i]->read_req
                     *	for the data.
                     */
                    break;

                case WIIUSE_NUNCHUK_INSERTED:
                    /*
                     *	a nunchuk was inserted
                     *	This is a good place to set any nunchuk specific
                     *	threshold values.  By default they are the same
                     *	as the wiimote.
                     */
                    /* wiiuse_set_nunchuk_orient_threshold((struct
                     * nunchuk_t*)&wiimotes[i]->exp.nunchuk, 90.0f); */
                    /* wiiuse_set_nunchuk_accel_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 100);
                     */
                    printf("Nunchuk inserted.\n");
                    break;

                case WIIUSE_CLASSIC_CTRL_INSERTED:
                    printf("Classic controller inserted.\n");
                    break;

                case WIIUSE_WII_BOARD_CTRL_INSERTED:
                    printf("Balance board controller inserted.\n");
                    break;

                case WIIUSE_GUITAR_HERO_DRUMS_CTRL_INSERTED:
                    /* some expansion was inserted */
                    handle_ctrl_status(wiimotes[i]);
                    printf("Guitar Hero Drums controller inserted.\n");
                    break;

                case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
                    /* some expansion was inserted */
                    handle_ctrl_status(wiimotes[i]);
                    printf("Guitar Hero 3 controller inserted.\n");
                    break;

                case WIIUSE_MOTION_PLUS_ACTIVATED:
                    printf("Motion+ was activated\n");
                    break;

                case WIIUSE_NUNCHUK_REMOVED:
                case WIIUSE_CLASSIC_CTRL_REMOVED:
                case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
                case WIIUSE_GUITAR_HERO_DRUMS_CTRL_REMOVED:
                case WIIUSE_WII_BOARD_CTRL_REMOVED:
                case WIIUSE_MOTION_PLUS_REMOVED:
                    /* some expansion was removed */
                    handle_ctrl_status(wiimotes[i]);
                    printf("An expansion was removed.\n");
                    break;

                default:
                    break;
                }
            }
        }
    }

    /*
     *	Disconnect the wiimotes
     */
    wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
    //// turn any MIDI notes currently playing:
    midiOutReset(device);

    //   // Remove any data in MIDI device and close the MIDI Output port
    midiOutClose(device);
    return 0;
}
