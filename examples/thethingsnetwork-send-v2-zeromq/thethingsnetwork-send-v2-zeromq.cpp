/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/
#include <lmic.h>
#include <hal/hal.h>
#include "Helper.cpp"
#include "ZeroMQ.cpp"

#include <iostream>
#include <chrono>
#include <thread>

static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 13,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 14,
    // LoRa mode
    // DIO0: TxDone and RxDone
    // DIO1: RxTimeout

    // FSK mode
    // DIO0: PayloadReady and PacketSent
    // DIO2: TimeOut
    .dio = {6, 7, LMIC_UNUSED_PIN},
};

void do_send(osjob_t *j)
{
    fprintf(stdout, "do_send\n");
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
        fprintf(stdout, "OP_TXRXPEND, not sending\n");
    }
    else
    {
        while (currentData == "")
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        printf("Sending: ");

        std::cout << currentData << std::endl;
        LMIC_setTxData2(1, (xref2u1_t)currentData.c_str(), currentData.length(), 0);
        fprintf(stdout, "Packet queued\n");
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent(ev_t ev)
{
    fprintf(stdout, "%d: ", os_getTime());
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        fprintf(stdout, "EV_SCAN_TIMEOUT\n");
        break;
    case EV_BEACON_FOUND:
        fprintf(stdout, "EV_BEACON_FOUND\n");
        break;
    case EV_BEACON_MISSED:
        fprintf(stdout, "EV_BEACON_MISSED\n");
        break;
    case EV_BEACON_TRACKED:
        fprintf(stdout, "EV_BEACON_TRACKED\n");
        break;
    case EV_JOINING:
        fprintf(stdout, "EV_JOINING\n");
        break;
    case EV_JOINED:
        fprintf(stdout, "EV_JOINED\n");
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            fprintf(stdout, "netid: ");
            // Serial.println(netid, DEC);
            fprintf(stdout, "devaddr: ");
            // Serial.println(devaddr, HEX);
            fprintf(stdout, "AppSKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i)
            {
                if (i != 0)
                    fprintf(stdout, "-");
                printHex2(artKey[i]);
            }
            // Serial.println("");
            fprintf(stdout, "NwkSKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i)
            {
                if (i != 0)
                    fprintf(stdout, "-");
                printHex2(nwkKey[i]);
            }
            // Serial.println();
        }
        // Disable link check validation (automatically enabled
        // during join, but because slow data rates change max TX
        // size, we don't use it in this example.
        LMIC_setLinkCheckMode(0);
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_RFU1:
    ||     fprintf(stdout, "EV_RFU1\n");
    ||     break;
    */
    case EV_JOIN_FAILED:
        fprintf(stdout, "EV_JOIN_FAILED\n");
        break;
    case EV_REJOIN_FAILED:
        fprintf(stdout, "EV_REJOIN_FAILED\n");
        break;
    case EV_TXCOMPLETE:
        fprintf(stdout, "EV_TXCOMPLETE (includes waiting for RX windows)");
        if (LMIC.txrxFlags & TXRX_ACK)
            fprintf(stdout, "Received ack\n");
        if (LMIC.dataLen)
            fprintf(stdout, "Received %d bytes of payload\n", LMIC.dataLen);

        // Clear string
        currentData.clear();

        // Schedule next transmission
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
        break;
    case EV_LOST_TSYNC:
        fprintf(stdout, "EV_LOST_TSYNC\n");
        break;
    case EV_RESET:
        fprintf(stdout, "EV_RESET\n");
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        fprintf(stdout, "EV_RXCOMPLETE\n");
        break;
    case EV_LINK_DEAD:
        fprintf(stdout, "EV_LINK_DEAD\n");
        break;
    case EV_LINK_ALIVE:
        fprintf(stdout, "EV_LINK_ALIVE\n");
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_SCAN_FOUND:
    ||    fprintf(stdout, "EV_SCAN_FOUND\n");
    ||    break;
    */
    case EV_TXSTART:
        fprintf(stdout, "EV_TXSTART\n");
        break;
    case EV_TXCANCELED:
        fprintf(stdout, "EV_TXCANCELED\n");
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        fprintf(stdout, "EV_JOIN_TXCOMPLETE: no JoinAccept\n");
        break;

    default:
        fprintf(stdout, "Unknown event: %d", (unsigned)ev);
        // Serial.println((unsigned) ev);
        break;
    }
}

void setup()
{
    fprintf(stdout, "Starting...\n");
    // Setup ZeroMQ
    setupZeroMQ();

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    LMIC_setupBand(4, 30, 30);

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop()
{
    os_runloop();
}

int main()
{
    setup();
    loop();
    return 0;
}