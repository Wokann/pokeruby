@ 0 ==
@ 1 !=
@ 2 <
@ 3 >
@ 4 &=

	.macro calculatedamage
	critical
	atk5
	atk6
	atk7
	.endm


	.macro attackcanceler
	.byte B_SCR_OP_ATTACKCANCELER
	.endm

	.macro accuracycheck address, param1
	.byte B_SCR_OP_ACCURACYCHECK
	.4byte \address
	.2byte \param1
	.endm

	.macro attackstring
	.byte B_SCR_OP_ATTACKSTRING
	.endm

	.macro ppreduce
	.byte B_SCR_OP_PPREDUCE
	.endm

	.macro critcalc
	.byte B_SCR_OP_CRITCALC
	.endm

	.macro damagecalc
	.byte B_SCR_OP_DAMAGECALC
	.endm

	.macro typecalc
	.byte B_SCR_OP_TYPECALC
	.endm

	.macro adjustnormaldamage
	.byte B_SCR_OP_ADJUSTNORMALDAMAGE
	.endm

	.macro adjustnormaldamage2
	.byte B_SCR_OP_ADJUSTNORMALDAMAGE2
	.endm

	.macro attackanimation
	.byte B_SCR_OP_ATTACKANIMATION
	.endm

	.macro waitanimation
	.byte B_SCR_OP_WAITANIMATION
	.endm

	.macro healthbarupdate bank
	.byte B_SCR_OP_HEALTHBARUPDATE
	.byte \bank
	.endm

	.macro datahpupdate bank
	.byte B_SCR_OP_DATAHPUPDATE
	.byte \bank
	.endm

	.macro critmessage
	.byte B_SCR_OP_CRITMESSAGE
	.endm

	.macro effectivenesssound
	.byte B_SCR_OP_EFFECTIVENESSSOUND
	.endm

	.macro resultmessage
	.byte B_SCR_OP_RESULTMESSAGE
	.endm

	.macro printstring string
	.byte B_SCR_OP_PRINTSTRING
	.2byte \string
	.endm

	.macro printselectionstring string
	.byte B_SCR_OP_PRINTSELECTIONSTRING
	.2byte \string
	.endm

	.macro waitmessage delay
	.byte B_SCR_OP_WAITMESSAGE
	.2byte \delay
	.endm

	.macro printfromtable table
	.byte B_SCR_OP_PRINTFROMTABLE
	.4byte \table
	.endm

	.macro printselectionstringfromtable table
	.byte B_SCR_OP_PRINTSELECTIONSTRINGFROMTABLE
	.4byte \table
	.endm

	.macro seteffectwithchance
	.byte B_SCR_OP_SETEFFECTWITHCHANCE
	.endm

	.macro seteffectprimary
	.byte B_SCR_OP_SETEFFECTPRIMARY
	.endm

	.macro seteffectsecondary
	.byte B_SCR_OP_SETEFFECTSECONDARY
	.endm

	.macro clearstatusfromeffect bank
	.byte B_SCR_OP_CLEARSTATUSFROMEFFECT
	.byte \bank
	.endm
	.macro tryfaintmon battler:req
	.byte B_SCR_OP_TRYFAINTMON
	.byte \battler
	.byte FALSE
	.4byte NULL
	.endm

	.macro tryfaintmon_spikes battler:req, ptr:req
	.byte B_SCR_OP_TRYFAINTMON
	.byte \battler
	.byte TRUE
	.4byte \ptr
	.endm

	.macro dofaintanimation param1
	.byte B_SCR_OP_DOFAINTANIMATION
	.byte \param1
	.endm

	.macro cleareffectsonfaint bank
	.byte B_SCR_OP_CLEAREFFECTSONFAINT
	.byte \bank
	.endm

	.macro jumpifstatus bank, status, address
	.byte B_SCR_OP_JUMPIFSTATUS
	.byte \bank
	.4byte \status
	.4byte \address
	.endm

	.macro jumpifstatus2 bank, status, address
	.byte B_SCR_OP_JUMPIFSTATUS2
	.byte \bank
	.4byte \status
	.4byte \address
	.endm

	.macro jumpifability bank, ability, address
	.byte B_SCR_OP_JUMPIFABILITY
	.byte \bank
	.byte \ability
	.4byte \address
	.endm

	.macro jumpifsideaffecting bank, status, address
	.byte B_SCR_OP_JUMPIFSIDEAFFECTING
	.byte \bank
	.2byte \status
	.4byte \address
	.endm

	.macro jumpifstat bank, flag, quantity, statid, address
	.byte B_SCR_OP_JUMPIFSTAT
	.byte \bank
	.byte \flag
	.byte \quantity
	.byte \statid
	.4byte \address
	.endm

	.macro jumpifstatus3condition bank, mask, status, address
	.byte B_SCR_OP_JUMPIFSTATUS3CONDITION
	.byte \bank
	.4byte \mask
	.byte \status
	.4byte \address
	.endm

	.macro jumpiftype bank, type, address
	.byte B_SCR_OP_JUMPIFTYPE
	.byte \bank
	.byte \type
	.4byte \address
	.endm

	.macro getexp bank
	.byte B_SCR_OP_GETEXP
	.byte \bank
	.endm

	.macro checkteamslost ptr
	.byte B_SCR_OP_CHECKTEAMSLOST
	.4byte \ptr
	.endm

	.macro movevaluescleanup
	.byte B_SCR_OP_MOVEVALUESCLEANUP
	.endm

	.macro setmultihit param1
	.byte B_SCR_OP_SETMULTIHIT
	.byte \param1
	.endm

	.macro decrementmultihit address
	.byte B_SCR_OP_DECREMENTMULTIHIT
	.4byte \address
	.endm

	.macro goto address
	.byte B_SCR_OP_GOTO
	.4byte \address
	.endm

	.macro jumpifbyte ifflag, checkaddr, compare, address
	.byte B_SCR_OP_JUMPIFBYTE
	.byte \ifflag
	.4byte \checkaddr
	.byte \compare
	.4byte \address
	.endm

	.macro jumpifhalfword ifflag, checkaddr, compare, address
	.byte B_SCR_OP_JUMPIFHALFWORD
	.byte \ifflag
	.4byte \checkaddr
	.2byte \compare
	.4byte \address
	.endm

	.macro jumpifword ifflag, checkaddr, compare, address
	.byte B_SCR_OP_JUMPIFWORD
	.byte \ifflag
	.4byte \checkaddr
	.4byte \compare
	.4byte \address
	.endm

	.macro jumpifarrayequal mem1, mem2, size, address
	.byte B_SCR_OP_JUMPIFARRAYEQUAL
	.4byte \mem1
	.4byte \mem2
	.byte \size
	.4byte \address
	.endm

	.macro jumpifarraynotequal mem1, mem2, size, address
	.byte B_SCR_OP_JUMPIFARRAYNOTEQUAL
	.4byte \mem1
	.4byte \mem2
	.byte \size
	.4byte \address
	.endm

	.macro setbyte pointer, value
	.byte B_SCR_OP_SETBYTE
	.4byte \pointer
	.byte \value
	.endm

	.macro addbyte pointer, value
	.byte B_SCR_OP_ADDBYTE
	.4byte \pointer
	.byte \value
	.endm

	.macro subbyte pointer, value
	.byte B_SCR_OP_SUBBYTE
	.4byte \pointer
	.byte \value
	.endm

	.macro copyarray destination, source, size
	.byte B_SCR_OP_COPYARRAY
	.4byte \destination
	.4byte \source
	.byte \size
	.endm

	.macro copyarraywithindex param1, param2, param3, byte
	.byte B_SCR_OP_COPYARRAYWITHINDEX
	.4byte \param1
	.4byte \param2
	.4byte \param3
	.byte \byte
	.endm

	.macro orbyte pointer, value
	.byte B_SCR_OP_ORBYTE
	.4byte \pointer
	.byte \value
	.endm

	.macro orhalfword pointer, value
	.byte B_SCR_OP_ORHALFWORD
	.4byte \pointer
	.2byte \value
	.endm

	.macro orword pointer, value
	.byte B_SCR_OP_ORWORD
	.4byte \pointer
	.4byte \value
	.endm

	.macro bicbyte pointer, value
	.byte B_SCR_OP_BICBYTE
	.4byte \pointer
	.byte \value
	.endm

	.macro bichalfword pointer, value
	.byte B_SCR_OP_BICHALFWORD
	.4byte \pointer
	.2byte \value
	.endm

	.macro bicword pointer, value
	.byte B_SCR_OP_BICWORD
	.4byte \pointer
	.4byte \value
	.endm

	.macro pause pause_duration
	.byte B_SCR_OP_PAUSE
	.2byte \pause_duration
	.endm

	.macro waitstate
	.byte B_SCR_OP_WAITSTATE
	.endm

	.macro healthbar_update bank
	.byte B_SCR_OP_HEALTHBAR_UPDATE
	.byte \bank
	.endm

	.macro return
	.byte B_SCR_OP_RETURN
	.endm

	.macro end
	.byte B_SCR_OP_END
	.endm

	.macro end2
	.byte B_SCR_OP_END2
	.endm

	.macro end3
	.byte B_SCR_OP_END3
	.endm

	.macro jumpifaffectedbyprotect address
	.byte B_SCR_OP_JUMPIFAFFECTEDBYPROTECT
	.4byte \address
	.endm

	.macro call address
	.byte B_SCR_OP_CALL
	.4byte \address
	.endm

	.macro jumpiftype2 bank, type, address
	.byte B_SCR_OP_JUMPIFTYPE2
	.byte \bank
	.byte \type
	.4byte \address
	.endm

	.macro jumpifabilitypresent ability, address
	.byte B_SCR_OP_JUMPIFABILITYPRESENT
	.byte \ability
	.4byte \address
	.endm

	.macro endselectionscript
	.byte B_SCR_OP_ENDSELECTIONSCRIPT
	.endm

	.macro playanimation bank, animation, var_address
	.byte B_SCR_OP_PLAYANIMATION
	.byte \bank
	.byte \animation
	.4byte \var_address
	.endm

	.macro playanimation_var battler, animType, arg
	.byte B_SCR_OP_PLAYANIMATION_VAR
	.byte \battler
	.4byte \animType
	.4byte \arg
	.endm

	.macro setgraphicalstatchangevalues
	.byte B_SCR_OP_SETGRAPHICALSTATCHANGEVALUES
	.endm

	.macro playstatchangeanimation bank, color, byte
	.byte B_SCR_OP_PLAYSTATCHANGEANIMATION
	.byte \bank
	.byte \color
	.byte \byte
	.endm

	.macro moveend endMode, endState
	.byte B_SCR_OP_MOVEEND
	.byte \endMode
	.byte \endState
	.endm

@ Helper macros for moveend command

@ All cases
	.macro moveendall
	setbyte sMOVEEND_STATE, 0
	moveend 0, 0
	.endm

@ Chosen case
	.macro moveendcase case
	setbyte sMOVEEND_STATE, \case
	moveend 1, 0
	.endm

@ All cases from (inclusive)
	.macro moveendfrom from
	setbyte sMOVEEND_STATE, \from
	moveend 0, 0
	.endm

@ All cases from 0 to (not inclusive)
	.macro moveendto to
	setbyte sMOVEEND_STATE, 0
	moveend 2, \to
	.endm

@ Cases from (inclusive) to (not inclusive)
	.macro moveendfromto from, to
	setbyte sMOVEEND_STATE, \from
	moveend 2, \to
	.endm

	.macro typecalc2
	.byte B_SCR_OP_TYPECALC2
	.endm

	.macro returnatktoball
	.byte B_SCR_OP_RETURNATKTOBALL
	.endm

	.macro getswitchedmondata bank
	.byte B_SCR_OP_GETSWITCHEDMONDATA
	.byte \bank
	.endm

	.macro switchindataupdate bank
	.byte B_SCR_OP_SWITCHINDATAUPDATE
	.byte \bank
	.endm

	.macro switchinanim bank, byte
	.byte B_SCR_OP_SWITCHINANIM
	.byte \bank
	.byte \byte
	.endm

	.macro jumpifcantswitch bank, address
	.byte B_SCR_OP_JUMPIFCANTSWITCH
	.byte \bank
	.4byte \address
	.endm

	.macro openpartyscreen bank, address
	.byte B_SCR_OP_OPENPARTYSCREEN
	.byte \bank
	.4byte \address
	.endm

	.macro switchhandleorder bank, param2
	.byte B_SCR_OP_SWITCHHANDLEORDER
	.byte \bank
	.byte \param2
	.endm

	.macro switchineffects bank
	.byte B_SCR_OP_SWITCHINEFFECTS
	.byte \bank
	.endm

	.macro trainerslidein bank
	.byte B_SCR_OP_TRAINERSLIDEIN
	.byte \bank
	.endm

	.macro playse word
	.byte B_SCR_OP_PLAYSE
	.2byte \word
	.endm

	.macro fanfare int
	.byte B_SCR_OP_FANFARE
	.2byte \int
	.endm

	.macro playfaintcry bank_or_side
	.byte B_SCR_OP_PLAYFAINTCRY
	.byte \bank_or_side
	.endm

	.macro endlinkbattle
	.byte B_SCR_OP_ENDLINKBATTLE
	.endm

	.macro returntoball bank
	.byte B_SCR_OP_RETURNTOBALL
	.byte \bank
	.endm

	.macro handlelearnnewmove param1, param2, bank_maybe
	.byte B_SCR_OP_HANDLELEARNNEWMOVE
	.4byte \param1
	.4byte \param2
	.byte \bank_maybe
	.endm

	.macro yesnoboxlearnmove address
	.byte B_SCR_OP_YESNOBOXLEARNMOVE
	.4byte \address
	.endm

	.macro yesnoboxstoplearningmove address
	.byte B_SCR_OP_YESNOBOXSTOPLEARNINGMOVE
	.4byte \address
	.endm

	.macro hitanimation bank
	.byte B_SCR_OP_HITANIMATION
	.byte \bank
	.endm

	.macro getmoneyreward
	.byte B_SCR_OP_GETMONEYREWARD
	.endm

	.macro updatebattlermoves battler
	.byte B_SCR_OP_UPDATEBATTLERMOVES
	.byte \battler
	.endm

	.macro swapattackerwithtarget
	.byte B_SCR_OP_SWAPATTACKERWITHTARGET
	.endm

	.macro incrementgamestat byte
	.byte B_SCR_OP_INCREMENTGAMESTAT
	.byte \byte
	.endm

	.macro drawpartystatussummary bank_or_side
	.byte B_SCR_OP_DRAWPARTYSTATUSSUMMARY
	.byte \bank_or_side
	.endm

	.macro hidepartystatussummary battler
	.byte B_SCR_OP_HIDEPARTYSTATUSSUMMARY
	.byte \battler
	.endm

	.macro jumptocalledmove notChosenMove
	.byte B_SCR_OP_JUMPTOCALLEDMOVE
	.byte \notChosenMove
	.endm

	.macro statusanimation bank
	.byte B_SCR_OP_STATUSANIMATION
	.byte \bank
	.endm

	.macro status2animation bank_or_side, address
	.byte B_SCR_OP_STATUS2ANIMATION
	.byte \bank_or_side
	.4byte \address
	.endm

	.macro chosenstatusanimation bank_or_side, bank_or_side2, address
	.byte B_SCR_OP_CHOSENSTATUSANIMATION
	.byte \bank_or_side
	.byte \bank_or_side2
	.4byte \address
	.endm

	.macro yesnobox
	.byte B_SCR_OP_YESNOBOX
	.endm

	.macro cancelallactions
	.byte B_SCR_OP_CANCELALLACTIONS
	.endm

	.macro adjustsetdamage
	.byte B_SCR_OP_ADJUSTSETDAMAGE
	.endm

	.macro removeitem bank
	.byte B_SCR_OP_REMOVEITEM
	.byte \bank
	.endm

	.macro atknameinbuff1
	.byte B_SCR_OP_ATKNAMEINBUFF1
	.endm

	.macro drawlvlupbox
	.byte B_SCR_OP_DRAWLVLUPBOX
	.endm

	.macro resetsentmonsvalue
	.byte B_SCR_OP_RESETSENTMONSVALUE
	.endm

	.macro setatktoplayer0
	.byte B_SCR_OP_SETATKTOPLAYER0
	.endm

	.macro makevisible bank
	.byte B_SCR_OP_MAKEVISIBLE
	.byte \bank
	.endm

	.macro recordlastability bank
	.byte B_SCR_OP_RECORDLASTABILITY
	.byte \bank
	.endm

	.macro buffermovetolearn
	.byte B_SCR_OP_BUFFERMOVETOLEARN
	.endm

	.macro jumpifplayerran address
	.byte B_SCR_OP_JUMPIFPLAYERRAN
	.4byte \address
	.endm

	.macro hpthresholds bank
	.byte B_SCR_OP_HPTHRESHOLDS
	.byte \bank
	.endm

	.macro hpthresholds2 bank
	.byte B_SCR_OP_HPTHRESHOLDS2
	.byte \bank
	.endm

	.macro useitemonopponent
	.byte B_SCR_OP_USEITEMONOPPONENT
	.endm

	.macro various bank, byte
	.byte B_SCR_OP_VARIOUS
	.byte \bank
	.byte \byte
	.endm

	.macro setprotectlike
	.byte B_SCR_OP_SETPROTECTLIKE
	.endm

	.macro tryexplosion
	.byte B_SCR_OP_TRYEXPLOSION
	.endm

	.macro setatkhptozero
	.byte B_SCR_OP_SETATKHPTOZERO
	.endm

	.macro jumpifnexttargetvalid address
	.byte B_SCR_OP_JUMPIFNEXTTARGETVALID
	.4byte \address
	.endm

	.macro tryhealhalfhealth address, byte
	.byte B_SCR_OP_TRYHEALHALFHEALTH
	.4byte \address
	.byte \byte
	.endm

	.macro trymirrormove
	.byte B_SCR_OP_TRYMIRRORMOVE
	.endm

	.macro setrain
	.byte B_SCR_OP_SETRAIN
	.endm

	.macro setreflect
	.byte B_SCR_OP_SETREFLECT
	.endm

	.macro setseeded
	.byte B_SCR_OP_SETSEEDED
	.endm

	.macro manipulatedamage id
	.byte B_SCR_OP_MANIPULATEDAMAGE
	.byte \id
	.endm

	.macro trysetrest address
	.byte B_SCR_OP_TRYSETREST
	.4byte \address
	.endm

	.macro jumpifnotfirstturn address
	.byte B_SCR_OP_JUMPIFNOTFIRSTTURN
	.4byte \address
	.endm

	.macro nop
	.byte B_SCR_OP_NOP
	.endm

	.macro jumpifcantmakeasleep address
	.byte B_SCR_OP_JUMPIFCANTMAKEASLEEP
	.4byte \address
	.endm

	.macro stockpile
	.byte B_SCR_OP_STOCKPILE
	.endm

	.macro stockpiletobasedamage address
	.byte B_SCR_OP_STOCKPILETOBASEDAMAGE
	.4byte \address
	.endm

	.macro stockpiletohpheal address
	.byte B_SCR_OP_STOCKPILETOHPHEAL
	.4byte \address
	.endm

	.macro negativedamage
	.byte B_SCR_OP_NEGATIVEDAMAGE
	.endm

	.macro statbuffchange target, address
	.byte B_SCR_OP_STATBUFFCHANGE
	.byte \target
	.4byte \address
	.endm

	.macro normalisebuffs
	.byte B_SCR_OP_NORMALISEBUFFS
	.endm

	.macro setbide
	.byte B_SCR_OP_SETBIDE
	.endm

	.macro confuseifrepeatingattackends
	.byte B_SCR_OP_CONFUSEIFREPEATINGATTACKENDS
	.endm

	.macro setmultihitcounter count
	.byte B_SCR_OP_SETMULTIHITCOUNTER
	.byte \count
	.endm

	.macro initmultihitstring
	.byte B_SCR_OP_INITMULTIHITSTRING
	.endm

	.macro forcerandomswitch address
	.byte B_SCR_OP_FORCERANDOMSWITCH
	.4byte \address
	.endm

	.macro tryconversiontypechange address
	.byte B_SCR_OP_TRYCONVERSIONTYPECHANGE
	.4byte \address
	.endm

	.macro givepaydaymoney
	.byte B_SCR_OP_GIVEPAYDAYMONEY
	.endm

	.macro setlightscreen
	.byte B_SCR_OP_SETLIGHTSCREEN
	.endm

	.macro tryKO address
	.byte B_SCR_OP_TRYKO
	.4byte \address
	.endm

	.macro damagetohalftargethp
	.byte B_SCR_OP_DAMAGETOHALFTARGETHP
	.endm

	.macro setsandstorm
	.byte B_SCR_OP_SETSANDSTORM
	.endm

	.macro weatherdamage
	.byte B_SCR_OP_WEATHERDAMAGE
	.endm

	.macro tryinfatuating address
	.byte B_SCR_OP_TRYINFATUATING
	.4byte \address
	.endm

	.macro updatestatusicon byte
	.byte B_SCR_OP_UPDATESTATUSICON
	.byte \byte
	.endm

	.macro setmist
	.byte B_SCR_OP_SETMIST
	.endm

	.macro setfocusenergy
	.byte B_SCR_OP_SETFOCUSENERGY
	.endm

	.macro transformdataexecution
	.byte B_SCR_OP_TRANSFORMDATAEXECUTION
	.endm

	.macro setsubstitute
	.byte B_SCR_OP_SETSUBSTITUTE
	.endm

	.macro mimicattackcopy address
	.byte B_SCR_OP_MIMICATTACKCOPY
	.4byte \address
	.endm

	.macro metronome
	.byte B_SCR_OP_METRONOME
	.endm

	.macro dmgtolevel
	.byte B_SCR_OP_DMGTOLEVEL
	.endm

	.macro psywavedamageeffect
	.byte B_SCR_OP_PSYWAVEDAMAGEEFFECT
	.endm

	.macro counterdamagecalculator address
	.byte B_SCR_OP_COUNTERDAMAGECALCULATOR
	.4byte \address
	.endm

	.macro mirrorcoatdamagecalculator address
	.byte B_SCR_OP_MIRRORCOATDAMAGECALCULATOR
	.4byte \address
	.endm

	.macro disablelastusedattack address
	.byte B_SCR_OP_DISABLELASTUSEDATTACK
	.4byte \address
	.endm

	.macro trysetencore address
	.byte B_SCR_OP_TRYSETENCORE
	.4byte \address
	.endm

	.macro painsplitdmgcalc address
	.byte B_SCR_OP_PAINSPLITDMGCALC
	.4byte \address
	.endm

	.macro settypetorandomresistance address
	.byte B_SCR_OP_SETTYPETORANDOMRESISTANCE
	.4byte \address
	.endm

	.macro setalwayshitflag
	.byte B_SCR_OP_SETALWAYSHITFLAG
	.endm

	.macro copymovepermanently address
	.byte B_SCR_OP_COPYMOVEPERMANENTLY
	.4byte \address
	.endm

	.macro trychoosesleeptalkmove address
	.byte B_SCR_OP_TRYCHOOSESLEEPTALKMOVE
	.4byte \address
	.endm

	.macro setdestinybond
	.byte B_SCR_OP_SETDESTINYBOND
	.endm

	.macro trysetdestinybondtohappen
	.byte B_SCR_OP_TRYSETDESTINYBONDTOHAPPEN
	.endm

	.macro remaininghptopower
	.byte B_SCR_OP_REMAININGHPTOPOWER
	.endm

	.macro tryspiteppreduce address
	.byte B_SCR_OP_TRYSPITEPPREDUCE
	.4byte \address
	.endm

	.macro healpartystatus
	.byte B_SCR_OP_HEALPARTYSTATUS
	.endm

	.macro cursetarget address
	.byte B_SCR_OP_CURSETARGET
	.4byte \address
	.endm

	.macro trysetspikes address
	.byte B_SCR_OP_TRYSETSPIKES
	.4byte \address
	.endm

	.macro setforesight
	.byte B_SCR_OP_SETFORESIGHT
	.endm

	.macro trysetperishsong address
	.byte B_SCR_OP_TRYSETPERISHSONG
	.4byte \address
	.endm

	.macro rolloutdamagecalculation
	.byte B_SCR_OP_ROLLOUTDAMAGECALCULATION
	.endm

	.macro jumpifconfusedandstatmaxed bank, address
	.byte B_SCR_OP_JUMPIFCONFUSEDANDSTATMAXED
	.byte \bank
	.4byte \address
	.endm

	.macro furycuttercalc
	.byte B_SCR_OP_FURYCUTTERCALC
	.endm

	.macro friendshiptodamagecalculation
	.byte B_SCR_OP_FRIENDSHIPTODAMAGECALCULATION
	.endm

	.macro presentdamagecalculation
	.byte B_SCR_OP_PRESENTDAMAGECALCULATION
	.endm

	.macro setsafeguard
	.byte B_SCR_OP_SETSAFEGUARD
	.endm

	.macro magnitudedamagecalculation
	.byte B_SCR_OP_MAGNITUDEDAMAGECALCULATION
	.endm

	.macro jumpifnopursuitswitchdmg address
	.byte B_SCR_OP_JUMPIFNOPURSUITSWITCHDMG
	.4byte \address
	.endm

	.macro setsunny
	.byte B_SCR_OP_SETSUNNY
	.endm

	.macro maxattackhalvehp address
	.byte B_SCR_OP_MAXATTACKHALVEHP
	.4byte \address
	.endm

	.macro copyfoestats address
	.byte B_SCR_OP_COPYFOESTATS
	.4byte \address
	.endm

	.macro rapidspinfree
	.byte B_SCR_OP_RAPIDSPINFREE
	.endm

	.macro setdefensecurlbit
	.byte B_SCR_OP_SETDEFENSECURLBIT
	.endm

	.macro recoverbasedonsunlight address
	.byte B_SCR_OP_RECOVERBASEDONSUNLIGHT
	.4byte \address
	.endm

	.macro hiddenpowercalc
	.byte B_SCR_OP_HIDDENPOWERCALC
	.endm

	.macro selectfirstvalidtarget
	.byte B_SCR_OP_SELECTFIRSTVALIDTARGET
	.endm

	.macro trysetfutureattack address
	.byte B_SCR_OP_TRYSETFUTUREATTACK
	.4byte \address
	.endm

	.macro trydobeatup address1, address2
	.byte B_SCR_OP_TRYDOBEATUP
	.4byte \address1
	.4byte \address2
	.endm

	.macro setsemiinvulnerablebit
	.byte B_SCR_OP_SETSEMIINVULNERABLEBIT
	.endm

	.macro clearsemiinvulnerablebit
	.byte B_SCR_OP_CLEARSEMIINVULNERABLEBIT
	.endm

	.macro setminimize
	.byte B_SCR_OP_SETMINIMIZE
	.endm

	.macro sethail
	.byte B_SCR_OP_SETHAIL
	.endm

	.macro trymemento ptr
	.byte B_SCR_OP_TRYMEMENTO
	.4byte \ptr
	.endm

	.macro setforcedtarget
	.byte B_SCR_OP_SETFORCEDTARGET
	.endm

	.macro setcharge
	.byte B_SCR_OP_SETCHARGE
	.endm

	.macro callenvironmentattack
	.byte B_SCR_OP_CALLENVIRONMENTATTACK
	.endm

	.macro cureifburnedparalyzedorpoisoned ptr
	.byte B_SCR_OP_CUREIFBURNEDPARALYZEDORPOISONED
	.4byte \ptr
	.endm

	.macro settorment address
	.byte B_SCR_OP_SETTORMENT
	.4byte \address
	.endm

	.macro jumpifnodamage address
	.byte B_SCR_OP_JUMPIFNODAMAGE
	.4byte \address
	.endm

	.macro settaunt address
	.byte B_SCR_OP_SETTAUNT
	.4byte \address
	.endm

	.macro trysethelpinghand address
	.byte B_SCR_OP_TRYSETHELPINGHAND
	.4byte \address
	.endm

	.macro tryswapitems address
	.byte B_SCR_OP_TRYSWAPITEMS
	.4byte \address
	.endm

	.macro trycopyability address
	.byte B_SCR_OP_TRYCOPYABILITY
	.4byte \address
	.endm

	.macro trywish byte, address
	.byte B_SCR_OP_TRYWISH
	.byte \byte
	.4byte \address
	.endm

	.macro trysetroots address
	.byte B_SCR_OP_TRYSETROOTS
	.4byte \address
	.endm

	.macro doubledamagedealtifdamaged
	.byte B_SCR_OP_DOUBLEDAMAGEDEALTIFDAMAGED
	.endm

	.macro setyawn address
	.byte B_SCR_OP_SETYAWN
	.4byte \address
	.endm

	.macro setdamagetohealthdifference address
	.byte B_SCR_OP_SETDAMAGETOHEALTHDIFFERENCE
	.4byte \address
	.endm

	.macro scaledamagebyhealthratio
	.byte B_SCR_OP_SCALEDAMAGEBYHEALTHRATIO
	.endm

	.macro tryswapabilities address
	.byte B_SCR_OP_TRYSWAPABILITIES
	.4byte \address
	.endm

	.macro tryimprison ptr
	.byte B_SCR_OP_TRYIMPRISON
	.4byte \ptr
	.endm

	.macro trysetgrudge address
	.byte B_SCR_OP_TRYSETGRUDGE
	.4byte \address
	.endm

	.macro weightdamagecalculation
	.byte B_SCR_OP_WEIGHTDAMAGECALCULATION
	.endm

	.macro assistattackselect address
	.byte B_SCR_OP_ASSISTATTACKSELECT
	.4byte \address
	.endm

	.macro trysetmagiccoat address
	.byte B_SCR_OP_TRYSETMAGICCOAT
	.4byte \address
	.endm

	.macro trysetsnatch address
	.byte B_SCR_OP_TRYSETSNATCH
	.4byte \address
	.endm

	.macro trygetintimidatetarget address
	.byte B_SCR_OP_TRYGETINTIMIDATETARGET
	.4byte \address
	.endm

	.macro switchoutabilities bank
	.byte B_SCR_OP_SWITCHOUTABILITIES
	.byte \bank
	.endm

	.macro jumpifhasnohp bank, address
	.byte B_SCR_OP_JUMPIFHASNOHP
	.byte \bank
	.4byte \address
	.endm

	.macro getsecretpowereffect
	.byte B_SCR_OP_GETSECRETPOWEREFFECT
	.endm

	.macro pickup
	.byte B_SCR_OP_PICKUP
	.endm

	.macro docastformchangeanimation
	.byte B_SCR_OP_DOCASTFORMCHANGEANIMATION
	.endm

	.macro trycastformdatachange
	.byte B_SCR_OP_TRYCASTFORMDATACHANGE
	.endm

	.macro settypebasedhalvers address
	.byte B_SCR_OP_SETTYPEBASEDHALVERS
	.4byte \address
	.endm

	.macro setweatherballtype
	.byte B_SCR_OP_SETWEATHERBALLTYPE
	.endm

	.macro tryrecycleitem address
	.byte B_SCR_OP_TRYRECYCLEITEM
	.4byte \address
	.endm

	.macro settypetoenvironment address
	.byte B_SCR_OP_SETTYPETOENVIRONMENT
	.4byte \address
	.endm

	.macro pursuitdoubles ptr
	.byte B_SCR_OP_PURSUITDOUBLES
	.4byte \ptr
	.endm

	.macro snatchsetbattlers
	.byte B_SCR_OP_SNATCHSETBATTLERS
	.endm

	.macro removelightscreenreflect
	.byte B_SCR_OP_REMOVELIGHTSCREENREFLECT
	.endm

	.macro handleballthrow
	.byte B_SCR_OP_HANDLEBALLTHROW
	.endm

	.macro givecaughtmon
	.byte B_SCR_OP_GIVECAUGHTMON
	.endm

	.macro trysetcaughtmondexflags address
	.byte B_SCR_OP_TRYSETCAUGHTMONDEXFLAGS
	.4byte \address
	.endm

	.macro displaydexinfo
	.byte B_SCR_OP_DISPLAYDEXINFO
	.endm

	.macro trygivecaughtmonnick address
	.byte B_SCR_OP_TRYGIVECAUGHTMONNICK
	.4byte \address
	.endm

	.macro subattackerhpbydmg
	.byte B_SCR_OP_SUBATTACKERHPBYDMG
	.endm

	.macro removeattackerstatus1
	.byte B_SCR_OP_REMOVEATTACKERSTATUS1
	.endm

	.macro finishaction
	.byte B_SCR_OP_FINISHACTION
	.endm

	.macro finishturn
	.byte B_SCR_OP_FINISHTURN
	.endm

@ various command changed to more readable macros
	.macro cancelmultiturnmoves battler:req
	various \battler, VARIOUS_CANCEL_MULTI_TURN_MOVES
	.endm

	.macro setmagiccoattarget battler:req
	various \battler, VARIOUS_SET_MAGIC_COAT_TARGET
	.endm

	.macro getifcantrunfrombattle battler:req
	various \battler, VARIOUS_IS_RUNNING_IMPOSSIBLE
	.endm

	.macro getmovetarget battler:req
	various \battler, VARIOUS_GET_MOVE_TARGET
	.endm

	.macro getbattlerfainted battler:req
	various \battler, VARIOUS_GET_BATTLER_FAINTED
	.endm

	.macro resetintimidatetracebits battler:req
	various \battler, VARIOUS_RESET_INTIMIDATE_TRACE_BITS
	.endm

	.macro updatechoicemoveonlvlup battler:req
	various \battler, VARIOUS_UPDATE_CHOICE_MOVE_ON_LVL_UP
	.endm

	.macro forfeityesnobox battler:req
	various \battler, VARIOUS_EMIT_YESNOBOX
	.endm

	.macro waitcry battler:req
	various \battler, VARIOUS_WAIT_CRY
	.endm

	.macro returnopponentmon1toball battler:req
	various \battler, VARIOUS_RETURN_OPPONENT_MON1
	.endm

	.macro returnopponentmon2toball battler:req
	various \battler, VARIOUS_RETURN_OPPONENT_MON2
	.endm

	.macro setoutcomeonteleport battler:req
	various \battler, VARIOUS_SET_TELEPORT_OUTCOME
	.endm

	.macro playtrainerdefeatbgm battler:req
	various \battler, VARIOUS_PLAY_TRAINER_DEFEATED_MUSIC
	.endm

@ helpful macros
	.macro setstatchanger stat, stages, down
	setbyte sSTATCHANGER \stat | \stages << 4 | \down << 7
	.endm
	
	.macro setmoveeffect effect
	setbyte cEFFECT_CHOOSER \effect
	.endm
	
	.macro chosenstatus1animation bank, status
	chosenstatusanimation \bank 0x0 \status
	.endm
	
	.macro chosenstatus2animation bank, status
	chosenstatusanimation \bank, 0x1, \status
	.endm
	
	.macro sethword dst, value
	setbyte \dst, \value & 0xFF
	setbyte \dst + 1, (\value >> 8) & 0xFF
	.endm
	
	.macro setword dst, value
	setbyte \dst, \value & 0xFF
	setbyte \dst + 1, (\value >> 8) & 0xFF
	setbyte \dst + 2, (\value >> 16) & 0xFF
	setbyte \dst + 3, (\value >> 24) & 0xFF
	.endm
	
	.macro copybyte dst, src
	copyarray \dst, \src, 0x1
	.endm
	
	.macro copyhword dst, src
	copyarray \dst, \src, 0x2
	.endm
	
	.macro copyword dst, src
	copyarray \dst, \src, 0x4
	.endm
	
	.macro jumpifbytenotequal byte1, byte2, jumpptr
	jumpifarraynotequal \byte1, \byte2, 0x1, \jumpptr
	.endm
	
	.macro jumpifbyteequal byte1, byte2, jumpptr
	jumpifarrayequal \byte1, \byte2, 0x1, \jumpptr
	.endm
	
	.macro jumpifmove move, jumpptr
	jumpifhalfword EQUAL, gCurrentMove, \move, \jumpptr
	.endm

	.macro jumpifnotmove move, jumpptr
	jumpifhalfword NOT_EQUAL, gCurrentMove, \move, \jumpptr
	.endm
	
	.macro jumpifstatus3 bank, status, jumpptr
	jumpifstatus3condition \bank, \status, 0x0, \jumpptr
	.endm
	
	.macro jumpifnostatus3 bank, status, jumpptr
	jumpifstatus3condition \bank, \status, 0x1, \jumpptr
	.endm
	
	.macro jumpifmovehadnoeffect jumpptr
	jumpifbyte COMMON_BITS, gMoveResultFlags, MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE | MOVE_RESULT_FAILED, \jumpptr
	.endm
	
	.macro jumpifbattletype flags, jumpptr
	jumpifhalfword COMMON_BITS, gBattleTypeFlags, \flags, \jumpptr
	.endm
	
	.macro jumpifnotbattletype flags, jumpptr
	jumpifhalfword NO_COMMON_BITS, gBattleTypeFlags, \flags, \jumpptr
	.endm
