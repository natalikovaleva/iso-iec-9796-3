#pragma once

static const long comb_window = 8;
static const long comb_idx[] = 
{
    3, 2, 1,
    5, 4, 1,
    6, 4, 2,
    7, 4, 3,
    9, 8, 1,
    10, 8, 2,
    11, 8, 3,
    12, 8, 4,
    13, 8, 5,
    14, 8, 6,
    15, 8, 7,
    17, 16, 1,
    18, 16, 2,
    19, 16, 3,
    20, 16, 4,
    21, 16, 5,
    22, 16, 6,
    23, 16, 7,
    24, 16, 8,
    25, 16, 9,
    26, 16, 10,
    27, 16, 11,
    28, 16, 12,
    29, 16, 13,
    30, 16, 14,
    31, 16, 15,
    33, 32, 1,
    34, 32, 2,
    35, 32, 3,
    36, 32, 4,
    37, 32, 5,
    38, 32, 6,
    39, 32, 7,
    40, 32, 8,
    41, 32, 9,
    42, 32, 10,
    43, 32, 11,
    44, 32, 12,
    45, 32, 13,
    46, 32, 14,
    47, 32, 15,
    48, 32, 16,
    49, 32, 17,
    50, 32, 18,
    51, 32, 19,
    52, 32, 20,
    53, 32, 21,
    54, 32, 22,
    55, 32, 23,
    56, 32, 24,
    57, 32, 25,
    58, 32, 26,
    59, 32, 27,
    60, 32, 28,
    61, 32, 29,
    62, 32, 30,
    63, 32, 31,
    65, 64, 1,
    66, 64, 2,
    67, 64, 3,
    68, 64, 4,
    69, 64, 5,
    70, 64, 6,
    71, 64, 7,
    72, 64, 8,
    73, 64, 9,
    74, 64, 10,
    75, 64, 11,
    76, 64, 12,
    77, 64, 13,
    78, 64, 14,
    79, 64, 15,
    80, 64, 16,
    81, 64, 17,
    82, 64, 18,
    83, 64, 19,
    84, 64, 20,
    85, 64, 21,
    86, 64, 22,
    87, 64, 23,
    88, 64, 24,
    89, 64, 25,
    90, 64, 26,
    91, 64, 27,
    92, 64, 28,
    93, 64, 29,
    94, 64, 30,
    95, 64, 31,
    96, 64, 32,
    97, 64, 33,
    98, 64, 34,
    99, 64, 35,
    100, 64, 36,
    101, 64, 37,
    102, 64, 38,
    103, 64, 39,
    104, 64, 40,
    105, 64, 41,
    106, 64, 42,
    107, 64, 43,
    108, 64, 44,
    109, 64, 45,
    110, 64, 46,
    111, 64, 47,
    112, 64, 48,
    113, 64, 49,
    114, 64, 50,
    115, 64, 51,
    116, 64, 52,
    117, 64, 53,
    118, 64, 54,
    119, 64, 55,
    120, 64, 56,
    121, 64, 57,
    122, 64, 58,
    123, 64, 59,
    124, 64, 60,
    125, 64, 61,
    126, 64, 62,
    127, 64, 63,
    129, 128, 1,
    130, 128, 2,
    131, 128, 3,
    132, 128, 4,
    133, 128, 5,
    134, 128, 6,
    135, 128, 7,
    136, 128, 8,
    137, 128, 9,
    138, 128, 10,
    139, 128, 11,
    140, 128, 12,
    141, 128, 13,
    142, 128, 14,
    143, 128, 15,
    144, 128, 16,
    145, 128, 17,
    146, 128, 18,
    147, 128, 19,
    148, 128, 20,
    149, 128, 21,
    150, 128, 22,
    151, 128, 23,
    152, 128, 24,
    153, 128, 25,
    154, 128, 26,
    155, 128, 27,
    156, 128, 28,
    157, 128, 29,
    158, 128, 30,
    159, 128, 31,
    160, 128, 32,
    161, 128, 33,
    162, 128, 34,
    163, 128, 35,
    164, 128, 36,
    165, 128, 37,
    166, 128, 38,
    167, 128, 39,
    168, 128, 40,
    169, 128, 41,
    170, 128, 42,
    171, 128, 43,
    172, 128, 44,
    173, 128, 45,
    174, 128, 46,
    175, 128, 47,
    176, 128, 48,
    177, 128, 49,
    178, 128, 50,
    179, 128, 51,
    180, 128, 52,
    181, 128, 53,
    182, 128, 54,
    183, 128, 55,
    184, 128, 56,
    185, 128, 57,
    186, 128, 58,
    187, 128, 59,
    188, 128, 60,
    189, 128, 61,
    190, 128, 62,
    191, 128, 63,
    192, 128, 64,
    193, 128, 65,
    194, 128, 66,
    195, 128, 67,
    196, 128, 68,
    197, 128, 69,
    198, 128, 70,
    199, 128, 71,
    200, 128, 72,
    201, 128, 73,
    202, 128, 74,
    203, 128, 75,
    204, 128, 76,
    205, 128, 77,
    206, 128, 78,
    207, 128, 79,
    208, 128, 80,
    209, 128, 81,
    210, 128, 82,
    211, 128, 83,
    212, 128, 84,
    213, 128, 85,
    214, 128, 86,
    215, 128, 87,
    216, 128, 88,
    217, 128, 89,
    218, 128, 90,
    219, 128, 91,
    220, 128, 92,
    221, 128, 93,
    222, 128, 94,
    223, 128, 95,
    224, 128, 96,
    225, 128, 97,
    226, 128, 98,
    227, 128, 99,
    228, 128, 100,
    229, 128, 101,
    230, 128, 102,
    231, 128, 103,
    232, 128, 104,
    233, 128, 105,
    234, 128, 106,
    235, 128, 107,
    236, 128, 108,
    237, 128, 109,
    238, 128, 110,
    239, 128, 111,
    240, 128, 112,
    241, 128, 113,
    242, 128, 114,
    243, 128, 115,
    244, 128, 116,
    245, 128, 117,
    246, 128, 118,
    247, 128, 119,
    248, 128, 120,
    249, 128, 121,
    250, 128, 122,
    251, 128, 123,
    252, 128, 124,
    253, 128, 125,
    254, 128, 126,
    255, 128, 127,
    257, 256, 1
};
