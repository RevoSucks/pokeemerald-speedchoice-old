	.set TRUE, 1
	.set FALSE, 0

	.set NULL, 0

	.set SPRITE_SIZE_8x8,   (OAM_SIZE_0 >> 28) | (OAM_SQUARE >> 14)
	.set SPRITE_SIZE_16x16, (OAM_SIZE_1 >> 28) | (OAM_SQUARE >> 14)
	.set SPRITE_SIZE_32x32, (OAM_SIZE_2 >> 28) | (OAM_SQUARE >> 14)
	.set SPRITE_SIZE_64x64, (OAM_SIZE_3 >> 28) | (OAM_SQUARE >> 14)

	.set SPRITE_SIZE_16x8,  (OAM_SIZE_0 >> 28) | (OAM_H_RECTANGLE >> 14)
	.set SPRITE_SIZE_32x8,  (OAM_SIZE_1 >> 28) | (OAM_H_RECTANGLE >> 14)
	.set SPRITE_SIZE_32x16, (OAM_SIZE_2 >> 28) | (OAM_H_RECTANGLE >> 14)
	.set SPRITE_SIZE_64x32, (OAM_SIZE_3 >> 28) | (OAM_H_RECTANGLE >> 14)

	.set SPRITE_SIZE_8x16,  (OAM_SIZE_0 >> 28) | (OAM_V_RECTANGLE >> 14)
	.set SPRITE_SIZE_8x32,  (OAM_SIZE_1 >> 28) | (OAM_V_RECTANGLE >> 14)
	.set SPRITE_SIZE_16x32, (OAM_SIZE_2 >> 28) | (OAM_V_RECTANGLE >> 14)
	.set SPRITE_SIZE_32x64, (OAM_SIZE_3 >> 28) | (OAM_V_RECTANGLE >> 14)

	.set OBJ_IMAGE_ANIM_H_FLIP, 1 << 6
	.set OBJ_IMAGE_ANIM_V_FLIP, 1 << 7

    @ README: ensure that these values are aligned with the enum in speedchoice.h!
    @ 0 is for the preset value in the enum. DO NOT USE THIS VALUE!
	.set EXPMATH, 1
	.set PLOTLESS, 2
	.set INSTANTTEXT, 3
	.set SPINNERS, 4
	.set MAXVISION, 5
	.set NERFROXANNE, 6
	.set SUPERBIKE, 7
	.set NEWWILDENC, 8
	.set EARLYFLY, 9
	.set RUN_EVERYWHERE, 10
	.set MEME_ISLAND, 11
	.set BETTER_MARTS, 12
	.set GOOD_EARLY_WILDS, 13
	.set EARLYSURF, 14
	.set NICE_PARTY_MENU, 15

    @ -------------
    @ option enums
    @ -------------

    @ EXPMATH
    .set EXP_KEEP, 0
    .set EXP_BW, 1
    .set EXP_NONE, 2

    @ PLOTLESS
    .set PLOT_SEMI, 0
    .set PLOT_KEEP, 1
    .set PLOT_FULL, 2

    @ INSTANT TEXT
    .set IT_ON, 0
    .set IT_OFF, 1

    @ SPINNERS
    .set SPIN_NERF, 0
    .set SPIN_KEEP, 1
    .set SPIN_HELL, 2
    .set SPIN_WHY, 3

    @ MAX VISION
    .set MAX_OFF, 0
    .set MAX_SANE, 1
    .set MAX_HELL, 2

    @ NERF ROXANNE
    .set NERF_YES, 0
    .set NERF_NO, 1

    @ SUPER BIKE
    .set BIKE_ON, 0
    .set BIKE_OFF, 1

    @ NEW WILD ENC
    .set NEW_ON, 0
    .set NEW_OFF, 1

    @ EARLY FLY
    .set FLY_YES, 0
    .set FLY_NO, 1

    @ RUN EVERYWHERE
    .set RUN_ON, 0
    .set RUN_OFF, 1

    @ MEME ISLAND
    .set MEME_BIG, 0 @ dot net
    .set MEME_SMALL, 1 @ you're banned

    @ BETTER MARTS
    .set MARTS_ON, 0
    .set MARTS_OFF, 1

    @ GOOD EARLY WILDS
    .set GOOD_OFF, 0
    .set GOOD_STATIC, 1
    .set GOOD_RAND, 2

    @ EARLY SURF
    .set SURF_ON, 0
    .set SURF_OFF, 1

    @ NICE_PARTY_MENU
    .set NICE_PARTY_MENU_ON, 0
    .set NICE_PARTY_MENU_OFF, 1
    