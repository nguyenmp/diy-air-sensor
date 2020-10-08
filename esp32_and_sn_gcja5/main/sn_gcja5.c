#include "sn_gcja5.h"
#include "aqi.h"
#include <assert.h>

/* Assumes 32 bytes, https://b2b-api.panasonic.eu/file_stream/pids/fileversion/8814 */
void parse_uart(uint8_t data[])
{
    printf("Raw decimal values: ");
    for (int index = 0; index < 32; index++) {
        printf("0d%d", data[index]);
    }
    printf("\n");

    // STX fixed value
    assert(data[0] == 2);

    // PM values (ug/m3)
    int pm_1_0 = data[1] + (data[2] << 8) + (data[3] << 16) + (data[4] << 24);
    int pm_2_5 = data[5] + (data[6] << 8) + (data[7] << 16) + (data[8] << 24);
    int pm_10 = data[9] + (data[10] << 8) + (data[11] << 16) + (data[12] << 24);

    // Particle counts (#, baseless)
    int pc_0_5 = data[13] + (data[14] << 8);
    int pc_1_0 = data[15] + (data[16] << 8);
    int pc_2_5 = data[17] + (data[18] << 8);
    assert(data[19] == 0);
    assert(data[20] == 0);
    int pc_5_0 = data[21] + (data[22] << 8);
    int pc_7_5 = data[23] + (data[24] << 8);
    int pc_10 = data[25] + (data[26] << 8);
    assert(data[27] == 0);
    assert(data[28] == 0);

    // Status
    int status = data[29];

    // XOR between data 2~30
    int fcc = data[30];
    int xor = 0;
    for (int i = 1; i < 30; i++) {
        xor ^= data[i];
    }
    assert(fcc == xor);

    // EXT fixed value
    assert(data[31] == 3);

    // Print human values
    printf("AQI 2.5: %d, AQI 10: %d\n", pm25_to_aqi(pm_2_5), pm10_to_aqi(pm_10));
    printf("PM 1.0: %d, PM 2.5: %d PM 10: %d\n", pm_1_0, pm_2_5, pm_10);
    printf(
        "PC 0.5: %d, PC 1.0: %d, PC 2.5: %d, PC 5.0: %d, PC 7.5: %d, PC 10: %d\n",
        pc_0_5, pc_1_0, pc_2_5, pc_5_0, pc_7_5, pc_10
    );
    printf("Status: %d\n", status);
    fflush(stdout);
}
