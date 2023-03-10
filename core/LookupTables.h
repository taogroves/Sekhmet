//
// Created by Tao G on 2/7/2023.
//

#ifndef CHESS_ENGINE_LOOKUPTABLES_H
#define CHESS_ENGINE_LOOKUPTABLES_H

#endif //CHESS_ENGINE_LOOKUPTABLES_H


#include <unordered_map>

namespace LookupTables {

    const U64 whitePawnAttacks[64] = {
            0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 2UL, 5UL, 10UL, 20UL, 40UL, 80UL, 160UL,
            64UL, 512UL, 1280UL, 2560UL, 5120UL, 10240UL, 20480UL, 40960UL,
            16384UL, 131072UL, 327680UL, 655360UL, 1310720UL, 2621440UL, 5242880UL, 10485760UL,
            4194304UL, 33554432UL, 83886080UL, 167772160UL, 335544320UL, 671088640UL, 1342177280UL, 2684354560UL,
            1073741824UL, 8589934592UL, 21474836480UL, 42949672960UL, 85899345920UL, 171798691840UL, 343597383680UL,
            687194767360UL,
            274877906944UL, 2199023255552UL, 5497558138880UL, 10995116277760UL, 21990232555520UL, 43980465111040UL,
            87960930222080UL, 175921860444160UL,
            70368744177664UL, 562949953421312UL, 1407374883553280UL, 2814749767106560UL, 5629499534213120UL,
            11258999068426240UL, 22517998136852480UL, 45035996273704960UL,
            18014398509481984UL};

    const U64 blackPawnAttacks[64] = {
            512UL, 1280UL, 2560UL, 5120UL, 10240UL, 20480UL, 40960UL,
            16384UL, 131072UL, 327680UL, 655360UL, 1310720UL, 2621440UL, 5242880UL, 10485760UL,
            4194304UL, 33554432UL, 83886080UL, 167772160UL, 335544320UL, 671088640UL, 1342177280UL, 2684354560UL,
            1073741824UL, 8589934592UL, 21474836480UL, 42949672960UL, 85899345920UL, 171798691840UL, 343597383680UL,
            687194767360UL,
            274877906944UL, 2199023255552UL, 5497558138880UL, 10995116277760UL, 21990232555520UL, 43980465111040UL,
            87960930222080UL, 175921860444160UL,
            70368744177664UL, 562949953421312UL, 1407374883553280UL, 2814749767106560UL, 5629499534213120UL,
            11258999068426240UL, 22517998136852480UL, 45035996273704960UL,
            18014398509481984UL, 144115188075855872UL, 360287970189639680UL, 720575940379279360UL,
            1441151880758558720UL, 2882303761517117440UL, 5764607523034234880UL, 11529215046068469760UL,
            4611686018427387904UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL};

    const U64 rookMasks[64] = {282578800148862UL, 565157600297596UL, 1130315200595066UL, 2260630401190006UL,
                               4521260802379886UL, 9042521604759646UL, 18085043209519166UL, 36170086419038334UL,
                               282578800180736UL, 565157600328704UL, 1130315200625152UL, 2260630401218048UL,
                               4521260802403840UL, 9042521604775424UL, 18085043209518592UL, 36170086419037696UL,
                               282578808340736UL, 565157608292864UL, 1130315208328192UL, 2260630408398848UL,
                               4521260808540160UL, 9042521608822784UL, 18085043209388032UL, 36170086418907136UL,
                               282580897300736UL, 565159647117824UL, 1130317180306432UL, 2260632246683648UL,
                               4521262379438080UL, 9042522644946944UL, 18085043175964672UL, 36170086385483776UL,
                               283115671060736UL, 565681586307584UL, 1130822006735872UL, 2261102847592448UL,
                               4521664529305600UL, 9042787892731904UL, 18085034619584512UL, 36170077829103616UL,
                               420017753620736UL, 699298018886144UL, 1260057572672512UL, 2381576680245248UL,
                               4624614895390720UL, 9110691325681664UL, 18082844186263552UL, 36167887395782656UL,
                               35466950888980736UL, 34905104758997504UL, 34344362452452352UL, 33222877839362048UL,
                               30979908613181440UL, 26493970160820224UL, 17522093256097792UL, 35607136465616896UL,
                               9079539427579068672UL, 8935706818303361536UL, 8792156787827803136UL,
                               8505056726876686336UL, 7930856604974452736UL, 6782456361169985536UL,
                               4485655873561051136UL, 9115426935197958144UL};

    const U64 bishopMasks[64] = {18049651735527936UL, 70506452091904UL, 275415828992UL, 1075975168UL, 38021120UL,
                                 8657588224UL, 2216338399232UL, 567382630219776UL,
                                 9024825867763712UL, 18049651735527424UL, 70506452221952UL, 275449643008UL,
                                 9733406720UL, 2216342585344UL, 567382630203392UL, 1134765260406784UL,
                                 4512412933816832UL, 9024825867633664UL, 18049651768822272UL, 70515108615168UL,
                                 2491752130560UL, 567383701868544UL, 1134765256220672UL, 2269530512441344UL,
                                 2256206450263040UL, 4512412900526080UL, 9024834391117824UL, 18051867805491712UL,
                                 637888545440768UL, 1135039602493440UL, 2269529440784384UL, 4539058881568768UL,
                                 1128098963916800UL, 2256197927833600UL, 4514594912477184UL, 9592139778506752UL,
                                 19184279556981248UL, 2339762086609920UL, 4538784537380864UL, 9077569074761728UL,
                                 562958610993152UL, 1125917221986304UL, 2814792987328512UL, 5629586008178688UL,
                                 11259172008099840UL, 22518341868716544UL, 9007336962655232UL, 18014673925310464UL,
                                 2216338399232UL, 4432676798464UL, 11064376819712UL, 22137335185408UL, 44272556441600UL,
                                 87995357200384UL, 35253226045952UL, 70506452091904UL,
                                 567382630219776UL, 1134765260406784UL, 2832480465846272UL, 5667157807464448UL,
                                 11333774449049600UL, 22526811443298304UL, 9024825867763712UL, 18049651735527936UL};

    const unsigned long long knightMasks[64] = {132096UL, 329728UL, 659712UL, 1319424UL, 2638848UL, 5277696UL,
                                                10489856UL, 4202496UL,
                                                33816580UL, 84410376UL, 168886289UL, 337772578UL, 675545156UL,
                                                1351090312UL, 2685403152UL, 1075839008UL,
                                                8657044482UL, 21609056261UL, 43234889994UL, 86469779988UL,
                                                172939559976UL, 345879119952UL, 687463207072UL, 275414786112UL,
                                                2216203387392UL, 5531918402816UL, 11068131838464UL, 22136263676928UL,
                                                44272527353856UL, 88545054707712UL, 175990581010432UL, 70506185244672UL,
                                                567348067172352UL, 1416171111120896UL, 2833441750646784UL,
                                                5666883501293568UL, 11333767002587136UL, 22667534005174272UL,
                                                45053588738670592UL, 18049583422636032UL,
                                                145241105196122112UL, 362539804446949376UL, 725361088165576704UL,
                                                1450722176331153408UL, 2901444352662306816UL, 5802888705324613632UL,
                                                11533718717099671552UL, 4620693356194824192UL,
                                                288234782788157440UL, 576469569871282176UL, 1224997833292120064UL,
                                                2449995666584240128UL, 4899991333168480256UL, 9799982666336960512UL,
                                                1152939783987658752UL, 2305878468463689728UL,
                                                1128098930098176UL, 2257297371824128UL, 4796069720358912UL,
                                                9592139440717824UL, 19184278881435648UL, 38368557762871296UL,
                                                4679521487814656UL, 9077567998918656UL};

    const unsigned long long kingMasks[64] = {770UL, 1797UL, 3594UL, 7188UL, 14376UL, 28752UL, 57504UL, 49216UL,
                                              197123UL, 460039UL, 920078UL, 1840156UL, 3680312UL, 7360624UL, 14721248UL,
                                              12599488UL,
                                              50463488UL, 117769984UL, 235539968UL, 471079936UL, 942159872UL,
                                              1884319744UL, 3768639488UL, 3225468928UL,
                                              12918652928UL, 30149115904UL, 60298231808UL, 120596463616UL,
                                              241192927232UL, 482385854464UL, 964771708928UL, 825720045568UL,
                                              3307175149568UL, 7718173671424UL, 15436347342848UL, 30872694685696UL,
                                              61745389371392UL, 123490778742784UL, 246981557485568UL, 211384331665408UL,
                                              846636838289408UL, 1975852459884544UL, 3951704919769088UL,
                                              7903409839538176UL, 15806819679076352UL, 31613639358152704UL,
                                              63227278716305408UL, 54114388906344448UL,
                                              216739030602088448UL, 505818229730443264UL, 1011636459460886528UL,
                                              2023272918921773056UL, 4046545837843546112UL, 8093091675687092224UL,
                                              16186183351374184448UL, 13853283560024178688UL,
                                              144959613005987840UL, 362258295026614272UL, 724516590053228544UL,
                                              1449033180106457088UL, 2898066360212914176UL, 5796132720425828352UL,
                                              11592265440851656704UL, 4665729213955833856UL};

    const unsigned long long rookMagics[64] = {
            0xa8002c000108020ULL, 0x6c00049b0002001ULL, 0x100200010090040ULL, 0x2480041000800801ULL,
            0x280028004000800ULL,
            0x900410008040022ULL, 0x280020001001080ULL, 0x2880002041000080ULL, 0xa000800080400034ULL,
            0x4808020004000ULL,
            0x2290802004801000ULL, 0x411000d00100020ULL, 0x402800800040080ULL, 0xb000401004208ULL,
            0x2409000100040200ULL,
            0x1002100004082ULL, 0x22878001e24000ULL, 0x1090810021004010ULL, 0x801030040200012ULL, 0x500808008001000ULL,
            0xa08018014000880ULL, 0x8000808004000200ULL, 0x201008080010200ULL, 0x801020000441091ULL, 0x800080204005ULL,
            0x1040200040100048ULL, 0x120200402082ULL, 0xd14880480100080ULL, 0x12040280080080ULL, 0x100040080020080ULL,
            0x9020010080800200ULL, 0x813241200148449ULL, 0x491604001800080ULL, 0x100401000402001ULL,
            0x4820010021001040ULL,
            0x400402202000812ULL, 0x209009005000802ULL, 0x810800601800400ULL, 0x4301083214000150ULL,
            0x204026458e001401ULL,
            0x40204000808000ULL, 0x8001008040010020ULL, 0x8410820820420010ULL, 0x1003001000090020ULL,
            0x804040008008080ULL,
            0x12000810020004ULL, 0x1000100200040208ULL, 0x430000a044020001ULL, 0x280009023410300ULL,
            0xe0100040002240ULL,
            0x200100401700ULL, 0x2244100408008080ULL, 0x8000400801980ULL, 0x2000810040200ULL, 0x8010100228810400ULL,
            0x2000009044210200ULL, 0x4080008040102101ULL, 0x40002080411d01ULL, 0x2005524060000901ULL,
            0x502001008400422ULL,
            0x489a000810200402ULL, 0x1004400080a13ULL, 0x4000011008020084ULL, 0x26002114058042ULL
    };

    const unsigned long long bishopMagics[64] = {
            0x89a1121896040240ULL, 0x2004844802002010ULL, 0x2068080051921000ULL, 0x62880a0220200808ULL,
            0x4042004000000ULL,
            0x100822020200011ULL, 0xc00444222012000aULL, 0x28808801216001ULL, 0x400492088408100ULL,
            0x201c401040c0084ULL,
            0x840800910a0010ULL, 0x82080240060ULL, 0x2000840504006000ULL, 0x30010c4108405004ULL, 0x1008005410080802ULL,
            0x8144042209100900ULL, 0x208081020014400ULL, 0x4800201208ca00ULL, 0xf18140408012008ULL,
            0x1004002802102001ULL,
            0x841000820080811ULL, 0x40200200a42008ULL, 0x800054042000ULL, 0x88010400410c9000ULL, 0x520040470104290ULL,
            0x1004040051500081ULL, 0x2002081833080021ULL, 0x400c00c010142ULL, 0x941408200c002000ULL,
            0x658810000806011ULL,
            0x188071040440a00ULL, 0x4800404002011c00ULL, 0x104442040404200ULL, 0x511080202091021ULL, 0x4022401120400ULL,
            0x80c0040400080120ULL, 0x8040010040820802ULL, 0x480810700020090ULL, 0x102008e00040242ULL,
            0x809005202050100ULL,
            0x8002024220104080ULL, 0x431008804142000ULL, 0x19001802081400ULL, 0x200014208040080ULL,
            0x3308082008200100ULL,
            0x41010500040c020ULL, 0x4012020c04210308ULL, 0x208220a202004080ULL, 0x111040120082000ULL,
            0x6803040141280a00ULL,
            0x2101004202410000ULL, 0x8200000041108022ULL, 0x21082088000ULL, 0x2410204010040ULL, 0x40100400809000ULL,
            0x822088220820214ULL, 0x40808090012004ULL, 0x910224040218c9ULL, 0x402814422015008ULL, 0x90014004842410ULL,
            0x1000042304105ULL, 0x10008830412a00ULL, 0x2520081090008908ULL, 0x40102000a0a60140ULL,
    };

    const int rookIndexBits[64] = {
            12, 11, 11, 11, 11, 11, 11, 12,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            12, 11, 11, 11, 11, 11, 11, 12
    };

    const int bishopIndexBits[64] = {
            6, 5, 5, 5, 5, 5, 5, 6,
            5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5,
            6, 5, 5, 5, 5, 5, 5, 6
    };

    const std::unordered_map<int, int> dirToIndex = {
            // {1, 7, 8, 9, -1, -7, -8, -9}
            {1,  0},
            {7,  1},
            {8,  2},
            {9,  3},
            {-1, 4},
            {-7, 5},
            {-8, 6},
            {-9, 7}
    };


    const int numSquaresToEdge[64][8] =
            {{7, 0, 7, 7, 0, 0, 0, 0},
             {6, 1, 7, 6, 1, 0, 0, 0},
             {5, 2, 7, 5, 2, 0, 0, 0},
             {4, 3, 7, 4, 3, 0, 0, 0},
             {3, 4, 7, 3, 4, 0, 0, 0},
             {2, 5, 7, 2, 5, 0, 0, 0},
             {1, 6, 7, 1, 6, 0, 0, 0},
             {0, 7, 7, 0, 7, 0, 0, 0},
             {7, 0, 6, 6, 0, 1, 1, 0},
             {6, 1, 6, 6, 1, 1, 1, 1},
             {5, 2, 6, 5, 2, 1, 1, 1},
             {4, 3, 6, 4, 3, 1, 1, 1},
             {3, 4, 6, 3, 4, 1, 1, 1},
             {2, 5, 6, 2, 5, 1, 1, 1},
             {1, 6, 6, 1, 6, 1, 1, 1},
             {0, 6, 6, 0, 7, 0, 1, 1},
             {7, 0, 5, 5, 0, 2, 2, 0},
             {6, 1, 5, 5, 1, 2, 2, 1},
             {5, 2, 5, 5, 2, 2, 2, 2},
             {4, 3, 5, 4, 3, 2, 2, 2},
             {3, 4, 5, 3, 4, 2, 2, 2},
             {2, 5, 5, 2, 5, 2, 2, 2},
             {1, 5, 5, 1, 6, 1, 2, 2},
             {0, 5, 5, 0, 7, 0, 2, 2},
             {7, 0, 4, 4, 0, 3, 3, 0},
             {6, 1, 4, 4, 1, 3, 3, 1},
             {5, 2, 4, 4, 2, 3, 3, 2},
             {4, 3, 4, 4, 3, 3, 3, 3},
             {3, 4, 4, 3, 4, 3, 3, 3},
             {2, 4, 4, 2, 5, 2, 3, 3},
             {1, 4, 4, 1, 6, 1, 3, 3},
             {0, 4, 4, 0, 7, 0, 3, 3},
             {7, 0, 3, 3, 0, 4, 4, 0},
             {6, 1, 3, 3, 1, 4, 4, 1},
             {5, 2, 3, 3, 2, 4, 4, 2},
             {4, 3, 3, 3, 3, 4, 4, 3},
             {3, 3, 3, 3, 4, 3, 4, 4},
             {2, 3, 3, 2, 5, 2, 4, 4},
             {1, 3, 3, 1, 6, 1, 4, 4},
             {0, 3, 3, 0, 7, 0, 4, 4},
             {7, 0, 2, 2, 0, 5, 5, 0},
             {6, 1, 2, 2, 1, 5, 5, 1},
             {5, 2, 2, 2, 2, 5, 5, 2},
             {4, 2, 2, 2, 3, 4, 5, 3},
             {3, 2, 2, 2, 4, 3, 5, 4},
             {2, 2, 2, 2, 5, 2, 5, 5},
             {1, 2, 2, 1, 6, 1, 5, 5},
             {0, 2, 2, 0, 7, 0, 5, 5},
             {7, 0, 1, 1, 0, 6, 6, 0},
             {6, 1, 1, 1, 1, 6, 6, 1},
             {5, 1, 1, 1, 2, 5, 6, 2},
             {4, 1, 1, 1, 3, 4, 6, 3},
             {3, 1, 1, 1, 4, 3, 6, 4},
             {2, 1, 1, 1, 5, 2, 6, 5},
             {1, 1, 1, 1, 6, 1, 6, 6},
             {0, 1, 1, 0, 7, 0, 6, 6},
             {7, 0, 0, 0, 0, 7, 7, 0},
             {6, 0, 0, 0, 1, 6, 7, 1},
             {5, 0, 0, 0, 2, 5, 7, 2},
             {4, 0, 0, 0, 3, 4, 7, 3},
             {3, 0, 0, 0, 4, 3, 7, 4},
             {2, 0, 0, 0, 5, 2, 7, 5},
             {1, 0, 0, 0, 6, 1, 7, 6},
             {0, 0, 0, 0, 7, 0, 7, 7}};

    const short pieceSquareValues[6][64] = {
            // white pawn
            {
                    0,   0,   0,   0,    0,   0,    0,   0,
                    78,  83, 86,  73,  102, 82, 85, 90,
                    7,   29, 21,  44, 40,  31,  44, 7,
                    -17, 16,  -2, 15, 14,  0,  15,  -13,
                    -26, 3,   10,  9,   6,   1,   0,   -23,
                    -22, 9,   5,   -11, -10, -2,  3,   -19,
                    -31, 8,   -7,  -37, -36, -14, 3,   -31,
                    0,   0,   0,   0,   0,   0,   0,   0
            },
            // white knight
            {
                    -66, -53, -75, -75,  -10, -55,  -58, -70,
                    -3,  -6, 100, -36, 4,   62, -4, -14,
                    10,  67, 1,   74, 73,  27,  62, -2,
                    24,  24,  45, 37, 33,  41, 25,  17,
                    -1,  5,   31,  21,  22,  35,  2,   0,
                    -18, 10,  13,  22,  18,  15,  11,  -14,
                    -23, -15, 2,   0,   2,   0,   -23, -20,
                    -74, -23, -26, -24, -19, -35, -22, -69
            },
            // white bishop
            {
                    -59, -78, -82, -76,  -23, -107, -37, -50,
                    -11, 20, 35,  -42, -39, 31, 2,  -22,
                    -9,  39, -32, 41, 52,  -10, 28, -14,
                    25,  17,  20, 34, 26,  25, 15,  10,
                    13,  10,  17,  23,  17,  16,  0,   7,
                    14,  25,  24,  15,  8,   25,  20,  15,
                    19,  20,  11,  6,   7,   6,   20,  16,
                    -7,  2,   -15, -12, -14, -15, -10, -10
            },
            // white rook
            {
                    35,  29,  33,  4,    37,  33,   56,  50,
                    55,  29, 56,  67,  55,  62, 34, 60,
                    19,  35, 28,  33, 45,  27,  25, 15,
                    0,   5,   16, 13, 18,  -4, -9,  -6,
                    -28, -35, -16, -21, -13, -29, -46, -30,
                    -42, -28, -42, -25, -25, -35, -26, -46,
                    -53, -38, -31, -26, -29, -43, -44, -53,
                    -30, -24, -18, 5,   -2,  -18, -31, -32
            },
            // white queen
            {
                    6,   1,   -8,  -104, 69,  24,   88,  26,
                    14,  32, 60,  -10, 20,  76, 57, 24,
                    -2,  43, 32,  60, 72,  63,  43, 2,
                    1,   -16, 22, 17, 25,  20, -13, -6,
                    -14, -15, -2,  -5,  -1,  -10, -20, -22,
                    -30, -6,  -13, -11, -16, -11, -16, -27,
                    -36, -18, 0,   -19, -15, -15, -21, -38,
                    -39, -30, -31, -13, -31, -36, -34, -42
            },
            // white king
            {
                    4,   54,  47,  -99,  -99, 60,   83,  -62,
                    -32, 10, 55,  56,  56,  55, 10, 3,
                    -62, 12, -57, 44, -67, 28,  37, -31,
                    -55, 50,  11, -4, -19, 13, 0,   -49,
                    -55, -43, -52, -28, -51, -47, -8,  -50,
                    -47, -42, -43, -79, -64, -32, -29, -32,
                    -4,  3,   -14, -50, -57, -18, 13,  4,
                    17,  30,  -3,  -14, 6,   -1,  40,  18
            }
    };

    const short pieceSquareValuesEndGame[6][64] = {
            // white pawn
            {
                    0, 0, 0, 0, 0, 0, 0, 0,
                    80, 80, 80, 80, 80, 80, 80, 80,
                    60, 60, 60, 60, 60, 60, 60, 60,
                    40, 40, 40, 40, 40, 40, 40, 40,
                    20, 20, 20, 20, 20, 20, 20, 20,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    -20, -20, -20, -20, -20, -20, -20, -20,
                    0, 0, 0, 0, 0, 0, 0, 0
            },
            // white knight
            {
                    -50, -40, -30, -30, -30, -30, -40, -50,
                    -40, -20, 0, 0, 0, 0, -20, -40,
                    -30, 0, 10, 15, 15, 10, 0, -30,
                    -30, 5, 15, 20, 20, 15, 5, -30,
                    -30, 0, 15, 20, 20, 15, 0, -30,
                    -30, 5, 10, 15, 15, 10, 5, -30,
                    -40, -20, 0, 5, 5, 0, -20, -40,
                    -50, -40, -30, -30, -30, -30, -40, -50
            },
            // white bishop
            {
                    -20, -10, -10, -10, -10, -10, -10, -20,
                    -10, 0, 0, 0, 0, 0, 0, -10,
                    -10, 0, 5, 10, 10, 5, 0, -10,
                    -10, 5, 5, 10, 10, 5, 5, -10,
                    -10, 0, 10, 10, 10, 10, 0, -10,
                    -10, 10, 10, 10, 10, 10, 10, -10,
                    -10, 5, 0, 0, 0, 0, 5, -10,
                    -20, -10, -10, -10, -10, -10, -10, -20
            },
            // white rook
            {
                    0, 0, 0, 0, 0, 0, 0, 0,
                    -5, 0, 0, 0, 0, 0, 0, -5,
                    -5, 0, 0, 0, 0, 0, 0, -5,
                    -5, 0, 0, 0, 0, 0, 0, -5,
                    -5, 0, 0, 0, 0, 0, 0, -5,
                    -5, 0, 0, 0, 0, 0, 0, -5,
                    -5, 0, 0, 0, 0, 0, 0, -5,
                    0, 0, 0, 0, 0, 0, 0, 0
            },
            // white queen
            {
                    -20, -10, -10, -5, -5, -10, -10, -20,
                    -10, 0, 0, 0, 0, 0, 0, -10,
                    -10, 0, 5, 5, 5, 5, 0, -10,
                    -5, 0, 5, 5, 5, 5, 0, -5,
                    0, 0, 5, 5, 5, 5, 0, -5,
                    -10, 5, 5, 5, 5, 5, 0, -10,
                    -10, 0, 5, 0, 0, 0, 0, -10,
                    -20, -10, -10, -5, -5, -10, -10, -20
            },
            // white king
            {
                    -50, -40, -30, -20, -20, -30, -40, -50,
                    -30, -20, -10, 0, 0, -10, -20, -30,
                    -30, -10, 20, 30, 30, 20, -10, -30,
                    -30, -10, 30, 40, 40, 30, -10, -30,
                    -30, -10, 30, 40, 40, 30, -10, -30,
                    -30, -10, 20, 30, 30, 20, -10, -30,
                    -30, -30, 0, 0, 0, 0, -30, -30,
                    -50, -30, -30, -30, -30, -30, -30, -50
            }
    };

    const U64 passedPawnMasksW[64] = {
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x3, 0x7, 0xe, 0x1c, 0x38, 0x70, 0xe0, 0xc0,
            0x303, 0x707, 0xe0e, 0x1c1c, 0x3838, 0x7070, 0xe0e0, 0xc0c0,
            0x30303, 0x70707, 0xe0e0e, 0x1c1c1c, 0x383838, 0x707070, 0xe0e0e0, 0xc0c0c0,
            0x3030303, 0x7070707, 0xe0e0e0e, 0x1c1c1c1c, 0x38383838, 0x70707070, 0xe0e0e0e0, 0xc0c0c0c0,
            0x303030303, 0x707070707, 0xe0e0e0e0e, 0x1c1c1c1c1c, 0x3838383838, 0x7070707070, 0xe0e0e0e0e0, 0xc0c0c0c0c0,
            0x30303030303, 0x70707070707, 0xe0e0e0e0e0e, 0x1c1c1c1c1c1c, 0x383838383838, 0x707070707070, 0xe0e0e0e0e0e0, 0xc0c0c0c0c0c0,
            0x3030303030303, 0x7070707070707, 0xe0e0e0e0e0e0e, 0x1c1c1c1c1c1c1c, 0x38383838383838, 0x70707070707070, 0xe0e0e0e0e0e0e0, 0xc0c0c0c0c0c0c0
    };

    const U64 passedPawnMasksB[64] = {
            0x303030303030300, 0x707070707070700, 0xe0e0e0e0e0e0e00, 0x1c1c1c1c1c1c1c00, 0x3838383838383800, 0x7070707070707000, 0xe0e0e0e0e0e0e000, 0xc0c0c0c0c0c0c000,
            0x303030303030000, 0x707070707070000, 0xe0e0e0e0e0e0000, 0x1c1c1c1c1c1c0000, 0x3838383838380000, 0x7070707070700000, 0xe0e0e0e0e0e00000, 0xc0c0c0c0c0c00000,
            0x303030303000000, 0x707070707000000, 0xe0e0e0e0e000000, 0x1c1c1c1c1c000000, 0x3838383838000000, 0x7070707070000000, 0xe0e0e0e0e0000000, 0xc0c0c0c0c0000000,
            0x303030300000000, 0x707070700000000, 0xe0e0e0e00000000, 0x1c1c1c1c00000000, 0x3838383800000000, 0x7070707000000000, 0xe0e0e0e000000000, 0xc0c0c0c000000000,
            0x303030000000000, 0x707070000000000, 0xe0e0e0000000000, 0x1c1c1c0000000000, 0x3838380000000000, 0x7070700000000000, 0xe0e0e00000000000, 0xc0c0c00000000000,
            0x303000000000000, 0x707000000000000, 0xe0e000000000000, 0x1c1c000000000000, 0x3838000000000000, 0x7070000000000000, 0xe0e0000000000000, 0xc0c0000000000000,
            0x300000000000000, 0x700000000000000, 0xe00000000000000, 0x1c00000000000000, 0x3800000000000000, 0x7000000000000000, 0xe000000000000000, 0xc000000000000000,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    };

    const U64 isolatedPawnMasks[8] = {0x202020202020202, 0x505050505050505, 0xa0a0a0a0a0a0a0a, 0x1414141414141414, 0x2828282828282828, 0x5050505050505050, 0xa0a0a0a0a0a0a0a0, 0x4040404040404040};
}