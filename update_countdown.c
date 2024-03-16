/*
 * Code based totally on Giacomo Ferretti's incredible work.
 * The changes you will find compared to his codes have been
 * made only for personal convenience and ease of use which
 * I hope will help you too. 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <nfc/nfc.h>
#include <inttypes.h>
#include "logging.h"
#include "nfc_utils.h"

static void print_usage(const char *executable) {
    printf("Usage: %s [-h] [-v] [-y]\n", executable);
    printf("\nOptions:\n");
    printf("  -h           show this help message\n");
    printf("  -v           enable verbose - print debugging data\n");
    printf("  -y           answer YES to all questions\n");
}

int main(int argc, char *argv[], char *envp[]) {
    bool skip_confirmation = false;

    // Parse options
    int opt = 0;
    while ((opt = getopt(argc, argv, "hvy")) != -1) {
        switch (opt) {
            case 'v':
                set_verbose(true);
                break;
            case 'y':
                skip_confirmation = true;
                break;
            default:
            case 'h':
                print_usage(argv[0]);
                exit(0);
        }
    }

    // Initialize NFC
    nfc_context *context = NULL;
    nfc_device *reader = NULL;
    nfc_init(&context);
    if (context == NULL) {
        lerror("Unable to init libnfc. Exiting...\n");
        exit(1);
    }

    // Display libnfc version
    lverbose("libnfc version: %s\n", nfc_version());

    // Search for readers
    lverbose("Searching for readers... ");
    nfc_connstring connstrings[MAX_DEVICE_COUNT] = {};
    size_t num_readers = nfc_list_devices(context, connstrings, MAX_DEVICE_COUNT);
    lverbose("found %zu.\n", num_readers);

    // Check if no readers are available
    if (num_readers == 0) {
        lerror("No readers available. Exiting...\n");
        close_nfc(context, reader);
        exit(1);
    }

    // Print out readers
    for (unsigned int i = 0; i < num_readers; i++) {
        if (i == num_readers - 1) {
            lverbose("└── ");
        } else {
            lverbose("├── ");
        }
        lverbose("[%d] %s\n", i, connstrings[i]);
    }
    lverbose("Opening %s...\n", connstrings[0]);

    // Open first reader
    reader = nfc_open(context, connstrings[0]);
    if (reader == NULL) {
        lerror("Unable to open NFC device. Exiting...\n");
        close_nfc(context, reader);
        exit(1);
    }

    // Set opened NFC device to initiator mode
    if (nfc_initiator_init(reader) < 0) {
        lerror("nfc_initiator_init => %s\n", nfc_strerror(reader));
        close_nfc(context, reader);
        exit(1);
    }

    lverbose("NFC reader: %s\n", nfc_device_get_name(reader));

    nfc_target target_key[MAX_TARGET_COUNT];

    /*
     * This is a known bug from libnfc.
     * To read ISO14443B2SR you have to initiate first ISO14443B to configure internal registers.
     *
     * https://github.com/nfc-tools/libnfc/issues/436#issuecomment-326686914
     */
    lverbose("Searching for ISO14443B targets... found %d.\n", nfc_initiator_list_passive_targets(reader, nmISO14443B, target_key, MAX_TARGET_COUNT));

    lverbose("Searching for ISO14443B2SR targets...");
    int ISO14443B2SR_targets = nfc_initiator_list_passive_targets(reader, nmISO14443B2SR, target_key, MAX_TARGET_COUNT);
    lverbose(" found %d.\n", ISO14443B2SR_targets);

    // Check for tags
    if (ISO14443B2SR_targets == 0) {
        printf("Waiting for tag...\n");

        // Infinite select for tag
        if (nfc_initiator_select_passive_target(reader, nmISO14443B2SR, NULL, 0, target_key) <= 0) {
            lerror("nfc_initiator_select_passive_target => %s\n", nfc_strerror(reader));
            close_nfc(context, reader);
            exit(1);
        }
    }

    // Read necessary blocks
    uint32_t Current_CDown_block = 0;
    uint32_t Lowered_CDown_block = 0;

    lverbose("Reading 6 blocks...\n");

        uint8_t block_bytes[4] = {};
        uint8_t block_bytes_read = nfc_srix_read_block(reader, block_bytes, 5);

        Current_CDown_block = (block_bytes[0]) << 24u | block_bytes[1] << 16u | block_bytes[2] << 8u | block_bytes[3];

        //find the value to lower down.
        for (uint8_t i = 0; i < 5; i++) {
            if (block_bytes[i] != 0xFF && block_bytes[0] != 0xFF){
                block_bytes[i]=block_bytes[i]-1;
            }
        }

        // Check for errors
        if (block_bytes_read != 4) {
            lerror("Error while reading block %d. Exiting...\n", 5);
            lverbose("Received %d bytes instead of 4.\n", block_bytes_read);
            close_nfc(context, reader);
            exit(1);
        }

        Lowered_CDown_block = (block_bytes[0]) << 24u | block_bytes[1] << 16u | block_bytes[2] << 8u | block_bytes[3];
        printf("[05] %08X -> %08X\n",Current_CDown_block, Lowered_CDown_block);
        printf("CDown uses available: %u\n", Lowered_CDown_block >> 21u);

    // Ask for confirmation
    if (!skip_confirmation) {
        printf("This action is irreversible.\n");
        printf("Are you sure? [Y/N] ");
        char c = 'n';
        scanf(" %c", &c);
        if (c != 'Y' && c != 'y') {
            printf("Exiting...\n");
            exit(0);
        }
    }
    // Write Block 06 first to trigger an Auto erase cycle
    nfc_write_block(reader, Lowered_CDown_block, 0x05);

    // Close NFC
    close_nfc(context, reader);

    return 0;
}
