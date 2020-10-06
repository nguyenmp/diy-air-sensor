#include "aqi.h"
#include <assert.h>  // for assert

int pm25_to_aqi(int pm25) {
    assert(pm25 >= 0);
    if (pm25 >= 500) {
        return 500;
    }

    // Max i = 500
    int density_to_aqi_2_5[] = {
        0, 4, 8, 13, 17, 21, 25, 29, 33, 38, 42, 46, 50, 53, 55,
        57, 59, 61, 63, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 87, 89, 91, 93, 95,
        97, 99, 102, 105, 107, 110, 112, 115, 117, 119, 122, 124, 127, 129, 132,
        134, 137, 139, 142, 144, 147, 149, 151, 152, 152, 153, 153, 154, 154, 155,
        155, 156, 156, 157, 157, 158, 158, 159, 160, 160, 161, 161, 162, 162, 163,
        163, 164, 164, 165, 165, 166, 166, 167, 167, 168, 168, 169, 169, 170, 170,
        171, 171, 172, 172, 173, 173, 174, 174, 175, 176, 176, 177, 177, 178, 178,
        179, 179, 180, 180, 181, 181, 182, 182, 183, 183, 184, 184, 185, 185, 186,
        186, 187, 187, 188, 188, 189, 189, 190, 190, 191, 192, 192, 193, 193, 194,
        194, 195, 195, 196, 196, 197, 197, 198, 198, 199, 199, 200, 201, 202, 203,
        204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218,
        219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233,
        234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248,
        249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263,
        264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278,
        279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293,
        294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308,
        309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323,
        324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338,
        339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353,
        354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368,
        369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383,
        384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398,
        399, 400, 401, 402, 403, 403, 404, 405, 405, 406, 407, 407, 408, 409, 409,
        410, 411, 411, 412, 413, 413, 414, 415, 415, 416, 417, 417, 418, 419, 419,
        420, 420, 421, 422, 422, 423, 424, 424, 425, 426, 426, 427, 428, 428, 429,
        430, 430, 431, 432, 432, 433, 434, 434, 435, 436, 436, 437, 438, 438, 439,
        440, 440, 441, 442, 442, 443, 444, 444, 445, 446, 446, 447, 448, 448, 449,
        450, 450, 451, 452, 452, 453, 454, 454, 455, 455, 456, 457, 457, 458, 459,
        459, 460, 461, 461, 462, 463, 463, 464, 465, 465, 466, 467, 467, 468, 469,
        469, 470, 471, 471, 472, 473, 473, 474, 475, 475, 476, 477, 477, 478, 479,
        479, 480, 481, 481, 482, 483, 483, 484, 485, 485, 486, 487, 487, 488, 489,
        489, 490, 490, 491, 492, 492, 493, 494, 494, 495, 496, 496, 497, 498, 498,
        499, 500
    };
    return density_to_aqi_2_5[pm25];
}

int pm10_to_aqi(int pm10) {
    assert(pm10 >= 0);
    if (pm10 >= 604) {
        return 500;
    }

    // Max i = 604
    int density_to_aqi_10[] = {
        0, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 51,
        52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60,
        61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69,
        70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78,
        79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87,
        88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96,
        97, 97, 98, 98, 99, 99, 100, 100, 101, 101, 102, 102, 103, 103, 104, 104,
        105, 105, 106, 106, 107, 107, 108, 108, 109, 109, 110, 110, 111, 111,
        112, 112, 113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 118,
        119, 119, 120, 120, 121, 121, 122, 122, 123, 123, 124, 124, 125, 125,
        126, 126, 127, 127, 128, 128, 129, 129, 130, 130, 131, 131, 132, 132,
        133, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 138, 139, 139,
        140, 140, 141, 141, 142, 142, 143, 143, 144, 144, 145, 145, 146, 146,
        147, 147, 148, 148, 149, 149, 150, 150, 151, 151, 152, 152, 153, 153, 154,
        154, 155, 155, 156, 156, 157, 157, 158, 158, 159, 159, 160, 160, 161, 161,
        162, 162, 163, 163, 164, 164, 165, 165, 166, 166, 167, 167, 168, 168, 169,
        169, 170, 170, 171, 171, 172, 172, 173, 173, 174, 174, 175, 175, 176, 176,
        177, 177, 178, 178, 179, 179, 180, 180, 181, 181, 182, 182, 183, 183, 184,
        184, 185, 185, 186, 186, 187, 187, 188, 188, 189, 189, 190, 190, 191, 191,
        192, 192, 193, 193, 194, 194, 195, 195, 196, 196, 197, 197, 198, 198, 199,
        199, 200, 200, 201, 202, 204, 205, 207, 208, 210, 211, 212, 214, 215, 217,
        218, 220, 221, 223, 224, 225, 227, 228, 230, 231, 233, 234, 235, 237, 238,
        240, 241, 243, 244, 245, 247, 248, 250, 251, 253, 254, 256, 257, 258, 260,
        261, 263, 264, 266, 267, 268, 270, 271, 273, 274, 276, 277, 278, 280, 281,
        283, 284, 286, 287, 289, 290, 291, 293, 294, 296, 297, 299, 300, 301, 302,
        304, 305, 306, 307, 309, 310, 311, 312, 314, 315, 316, 317, 319, 320, 321,
        322, 324, 325, 326, 327, 329, 330, 331, 332, 334, 335, 336, 337, 339, 340,
        341, 342, 344, 345, 346, 347, 349, 350, 351, 352, 354, 355, 356, 357, 359,
        360, 361, 362, 364, 365, 366, 367, 369, 370, 371, 372, 374, 375, 376, 377,
        379, 380, 381, 382, 384, 385, 386, 387, 389, 390, 391, 392, 394, 395, 396,
        397, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412,
        413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427,
        428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442,
        443, 444, 445, 446, 447, 448, 449, 450, 451, 452, 453, 454, 455, 456, 457,
        458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472,
        473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487,
        488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500
    };
    return density_to_aqi_10[pm10];
}

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
