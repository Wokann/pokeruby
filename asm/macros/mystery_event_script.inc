	@ These values match the Mystery Event command table and C enum.
	.set MEVENT_SCRIPT_CMD_NOP,                    0x00
	.set MEVENT_SCRIPT_CMD_CHECK_COMPAT,           0x01
	.set MEVENT_SCRIPT_CMD_END,                    0x02
	.set MEVENT_SCRIPT_CMD_SET_MSG,                0x03
	.set MEVENT_SCRIPT_CMD_SET_STATUS,             0x04
	.set MEVENT_SCRIPT_CMD_RUN_SCRIPT,             0x05
	.set MEVENT_SCRIPT_CMD_INIT_RAM_SCRIPT,        0x06
	.set MEVENT_SCRIPT_CMD_SET_ENIGMA_BERRY,       0x07
	.set MEVENT_SCRIPT_CMD_GIVE_RIBBON,            0x08
	.set MEVENT_SCRIPT_CMD_GIVE_NATIONAL_DEX,      0x09
	.set MEVENT_SCRIPT_CMD_ADD_RARE_WORD,          0x0A
	.set MEVENT_SCRIPT_CMD_SET_RECORD_MIXING_GIFT, 0x0B
	.set MEVENT_SCRIPT_CMD_GIVE_POKEMON,           0x0C
	.set MEVENT_SCRIPT_CMD_ADD_TRAINER,            0x0D
	.set MEVENT_SCRIPT_CMD_ENABLE_RESET_RTC,       0x0E
	.set MEVENT_SCRIPT_CMD_CHECKSUM,               0x0F
	.set MEVENT_SCRIPT_CMD_CRC,                    0x10

	.set MEVENT_STATUS_LOAD_OK,    0
	.set MEVENT_STATUS_LOAD_ERROR, 1
	.set MEVENT_STATUS_SUCCESS,    2
	.set MEVENT_STATUS_FAILURE,    3
	.set MEVENT_STATUS_FF,         0xFF

	.set MEVENT_GAME_MASK_RUBY_SAPPHIRE, 0x4
	.set MEVENT_CHECK_VALUE_UNSET,       0

	.macro me_nop
	.byte MEVENT_SCRIPT_CMD_NOP
	.endm

	.macro me_checkcompat scriptBase, languageMask1, languageMask2, gameMask, versionMask
	.byte MEVENT_SCRIPT_CMD_CHECK_COMPAT
	.4byte \scriptBase
	.2byte \languageMask1
	.4byte \languageMask2
	.2byte \gameMask
	.4byte \versionMask
	.endm

	.macro me_end
	.byte MEVENT_SCRIPT_CMD_END
	.endm

	.macro me_setmsg status, message
	.byte MEVENT_SCRIPT_CMD_SET_MSG
	.byte \status
	.4byte \message
	.endm

	.macro me_setstatus status
	.byte MEVENT_SCRIPT_CMD_SET_STATUS
	.byte \status
	.endm

	.macro me_runscript script
	.byte MEVENT_SCRIPT_CMD_RUN_SCRIPT
	.4byte \script
	.endm

	.macro me_initramscript mapId, objectLocalId, script, scriptEnd
	.byte MEVENT_SCRIPT_CMD_INIT_RAM_SCRIPT
	map \mapId
	.byte \objectLocalId
	.4byte \script, \scriptEnd
	.endm

	.macro me_setenigmaberry berryData
	.byte MEVENT_SCRIPT_CMD_SET_ENIGMA_BERRY
	.4byte \berryData
	.endm

	.macro me_giveribbon index, ribbonId
	.byte MEVENT_SCRIPT_CMD_GIVE_RIBBON
	.byte \index, \ribbonId
	.endm

	.macro me_givenationaldex
	.byte MEVENT_SCRIPT_CMD_GIVE_NATIONAL_DEX
	.endm

	.macro me_addrareword trendySayingId
	.byte MEVENT_SCRIPT_CMD_ADD_RARE_WORD
	.byte \trendySayingId
	.endm

	.macro me_setrecordmixinggift unk, quantity, itemId
	.byte MEVENT_SCRIPT_CMD_SET_RECORD_MIXING_GIFT
	.byte \unk, \quantity
	.2byte \itemId
	.endm

	.macro me_givepokemon pokemonData
	.byte MEVENT_SCRIPT_CMD_GIVE_POKEMON
	.4byte \pokemonData
	.endm

	.macro me_addtrainer trainerData
	.byte MEVENT_SCRIPT_CMD_ADD_TRAINER
	.4byte \trainerData
	.endm

	.macro me_enableresetrtc
	.byte MEVENT_SCRIPT_CMD_ENABLE_RESET_RTC
	.endm

	.macro me_checksum expectedChecksum, data, dataEnd
	.byte MEVENT_SCRIPT_CMD_CHECKSUM
	.4byte \expectedChecksum, \data, \dataEnd
	.endm

	.macro me_crc expectedCrc, data, dataEnd
	.byte MEVENT_SCRIPT_CMD_CRC
	.4byte \expectedCrc, \data, \dataEnd
	.endm
