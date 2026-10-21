	.macro loadspritegfx tag
	.byte 0x00
	.2byte \tag
	.endm

	.macro unloadspritegfx tag
	.byte 0x01
	.2byte \tag
	.endm

	.macro createsprite template, anim_battler, subpriority_offset, argv:vararg
    .byte 0x02
    .4byte \template
    .if \anim_battler == ANIM_TARGET
    .byte ANIMSPRITE_IS_TARGET | (\subpriority_offset & 0x7F)
    .else
    .byte (\subpriority_offset & 0x7F)
    .endif
    .byte (.Lsprite_\@_2 - .Lsprite_\@_1) / 2
.Lsprite_\@_1:
    .2byte \argv
.Lsprite_\@_2:
	.endm

	.macro createvisualtask addr, priority, argv:vararg
	.byte 0x03
	.4byte \addr
	.byte \priority
	.byte (.Lcreatetask_\@_2 - .Lcreatetask_\@_1) / 2
.Lcreatetask_\@_1:
	.2byte \argv
.Lcreatetask_\@_2:
	.endm

	.macro delay frames
	.byte 0x04
	.byte \frames
	.endm

	.macro waitforvisualfinish
	.byte 0x05
	.endm

	.macro nop
	.byte 0x06
	.endm

	.macro nop2
	.byte 0x07
	.endm

	.macro end
	.byte 0x08
	.endm

	.macro playse se
	.byte 0x09
	.2byte \se
	.endm

	.macro monbg battler
	.byte 0x0A
	.byte \battler
	.endm

	.macro clearmonbg battler
	.byte 0x0B
	.byte \battler
	.endm

	.macro setalpha eva, evb
	.byte 0x0C
	.2byte ((\evb) << 8) | (\eva)
	.endm

	.macro blendoff
	.byte 0x0D
	.endm

	.macro call ptr
	.byte 0x0E
	.4byte \ptr
	.endm

	.macro return
	.byte 0x0F
	.endm

	.macro setarg argId, value
	.byte 0x10
	.byte \argId
	.2byte \value
	.endm

	.macro choosetwoturnanim ptr1, ptr2
	.byte 0x11
	.4byte \ptr1
	.4byte \ptr2
	.endm

	.macro jumpifmoveturn value, ptr
	.byte 0x12
	.byte \value
	.4byte \ptr
	.endm

	.macro goto ptr
	.byte 0x13
	.4byte \ptr
	.endm

	.macro fadetobg bg
	.byte 0x14
	.byte \bg
	.endm

	.macro restorebg
	.byte 0x15
	.endm

	.macro waitbgfadeout
	.byte 0x16
	.endm

	.macro waitbgfadein
	.byte 0x17
	.endm

	.macro changebg bg
	.byte 0x18
	.byte \bg
	.endm

	.macro playsewithpan se, pan
	.byte 0x19
	.2byte \se
	.byte \pan
	.endm

	.macro setpan pan
	.byte 0x1A
	.byte \pan
	.endm

	.macro panse se, currentPan, targetPan, incrementPan, delay
	.byte 0x1B
	.2byte \se
	.byte \currentPan
	.byte \targetPan
	.byte \incrementPan
	.byte \delay
	.endm

	.macro loopsewithpan se, pan, wait, times
	.byte 0x1C
	.2byte \se
	.byte \pan
	.byte \wait
	.byte \times
	.endm

	.macro waitplaysewithpan se, pan, wait
	.byte 0x1D
	.2byte \se
	.byte \pan
	.byte \wait
	.endm

	.macro setbldcnt bldcnt
	.byte 0x1E
	.2byte \bldcnt
	.endm

	.macro createsoundtask addr, argv:vararg
	.byte 0x1F
	.4byte \addr
	.byte (.Lcreatetask_1F_\@_2 - .Lcreatetask_1F_\@_1) / 2
.Lcreatetask_1F_\@_1:
	.2byte \argv
.Lcreatetask_1F_\@_2:
	.endm

	.macro waitsound
	.byte 0x20
	.endm

	.macro jumpargeq argId, value, ptr
	.byte 0x21
	.byte \argId
	.2byte \value
	.4byte \ptr
	.endm

	.macro monbg_static battler
	.byte 0x22
	.byte \battler
	.endm

	.macro clearmonbg_static battler
	.byte 0x23
	.byte \battler
	.endm

	.macro jumpifcontest ptr
	.byte 0x24
	.4byte \ptr
	.endm

	.macro fadetobgfromset bgOpponent, bgPlayer, bgContest
	.byte 0x25
	.byte \bgOpponent
	.byte \bgPlayer
	.byte \bgContest
	.endm

	.macro panse_adjustnone se, currentPan, targetPan, incrementPan, delay
	.byte 0x26
	.2byte \se
	.byte \currentPan
	.byte \targetPan
	.byte \incrementPan
	.byte \delay
	.endm

	.macro panse_adjustall se, currentPan, targetPan, incrementPan, delay
	.byte 0x27
	.2byte \se
	.byte \currentPan
	.byte \targetPan
	.byte \incrementPan
	.byte \delay
	.endm

	.macro splitbgprio battler
	.byte 0x28
	.byte \battler
	.endm

	.macro splitbgprio_all
	.byte 0x29
	.endm

	.macro splitbgprio_foes battler
	.byte 0x2A
	.byte \battler
	.endm

	.macro invisible battler
	.byte 0x2B
	.byte \battler
	.endm

	.macro visible battler
	.byte 0x2C
	.byte \battler
	.endm

	.macro teamattack_moveback battler
	.byte 0x2D
	.byte \battler
	.endm

	.macro teamattack_movefwd battler
	.byte 0x2E
	.byte \battler
	.endm

	.macro stopsound
	.byte 0x2F
	.endm
