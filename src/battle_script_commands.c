#include "global.h"
#include "constants/trainers.h"
#include "constants/rgb.h"
#include "battle_scripts.h"
#include "pokedex.h"
#include "reshow_battle_screen.h"
#include "battle.h"
#include "battle_ai_script_commands.h"
#include "battle_main.h"
#include "battle_gfx_sfx_util.h"
#include "battle_anim.h"
#include "battle_message.h"
#include "constants/battle_string_ids.h"
#include "battle_script_commands.h"
#include "battle_interface.h"
#include "money.h"
#include "overworld.h"
#include "menu_cursor.h"
#include "battle_util.h"
#include "constants/battle_move_effects.h"
#include "constants/moves.h"
#include "constants/map_types.h"
#include "constants/abilities.h"
#include "item.h"
#include "mail.h"
#include "constants/items.h"
#include "data.h"
#include "constants/hold_effects.h"
#include "random.h"
#include "battle_controllers.h"
#include "constants/species.h"
#include "pokemon.h"
#include "party_menu.h"
#include "text.h"
#include "palette.h"
#include "main.h"
#include "constants/songs.h"
#include "sound.h"
#include "task.h"
#include "decompress.h"
#include "string_util.h"
#include "strings.h"
#include "pokemon_summary_screen.h"
#include "naming_screen.h"
#include "ewram.h"
#include "util.h"
//extern needed variables
extern u8 gUnknown_02023A14_50;
extern s32 gBattleMoveDamage;
extern u16 gBattleTypeFlags;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[MAX_BATTLERS_COUNT];
extern u8 gBattleOutcome;
extern u8 gBattleEnvironment;
extern u16 gTrainerBattleOpponent_A;
extern u8 gBattlerAttacker;
extern u8 gBattlerTarget;
extern const u8* gBattlescriptCurrInstr;
extern u8 gBattleCommunication[];
extern s32 gHpDealt;
extern u16 gExpShareExp;
extern struct Window gWindowTemplate_Contest_MoveDescription;
extern u8 gPlayerPartyCount;
extern u16 gMoveToLearn; //move to learn
extern u8* gBattleScriptsForMoveEffects[];
extern u8 gBattlerInMenuId;


//extern functions
static bool8 IsTwoTurnsMove(u16 move);
static void TrySetDestinyBondToHappen(void);
static void CheckWonderGuardAndLevitate(void);
static u8 AttacksThisTurn(u8 battler, u16 move); //Note: returns 1 if it's a charging turn, otherwise 2
void UndoEffectsAfterFainting(void);
u8 GetBattlerMultiplayerId(u8 id);
u16 GetLinkTrainerFlankId(u8 linkPlayerId);
//MonTryLearningNewMove teach poke a move

//extern BattleScripts


// read via orr
#define BSScriptRead32(ptr) ((ptr)[0] | (ptr)[1] << 8 | (ptr)[2] << 16 | (ptr)[3] << 24)
#define BSScriptRead16(ptr) ((ptr)[0] | ((ptr)[1] << 8))
#define BSScriptReadPtr(ptr) ((void *)BSScriptRead32(ptr))

#define DEFENDER_IS_PROTECTED ((gProtectStructs[gBattlerTarget].protected && gBattleMoves[gCurrentMove].flags & FLAG_PROTECT_AFFECTED))


#define uBYTE0_16(value)(( (u8) (((u16)(value) & (0x000000FF)) >> 0x00)))
#define uBYTE1_16(value)(( (u8) (((u16)(value) & (0x0000FF00)) >> 0x08)))

#define uBYTE0_32(value)(( (u8) (((u32)(value) & (0x000000FF)) >> 0x00)))
#define uBYTE1_32(value)(( (u8) (((u32)(value) & (0x0000FF00)) >> 0x08)))
#define uBYTE2_32(value)(( (u8) (((u32)(value) & (0x00FF0000)) >> 0x10)))
#define uBYTE3_32(value)(( (u8) (((u32)(value) & (0xFF000000)) >> 0x18)))

#define sBYTE0_16(value)(( (u8) (((s16)(value) & (0x000000FF)) >> 0x00)))
#define sBYTE1_16(value)(( (u8) (((s16)(value) & (0x0000FF00)) >> 0x08)))

#define sBYTE0_32(value)(( (u8) (((s32)(value) & (0x000000FF)) >> 0x00)))
#define sBYTE1_32(value)(( (u8) (((s32)(value) & (0x0000FF00)) >> 0x08)))
#define sBYTE2_32(value)(( (u8) (((s32)(value) & (0x00FF0000)) >> 0x10)))
#define sBYTE3_32(value)(( (u8) (((s32)(value) & (0xFF000000)) >> 0x18)))

#define RecordAbilitySetField6(ability, fieldValue) \
(gLastUsedAbility = ability, gBattleCommunication[MISS_TYPE] = fieldValue, RecordAbilityBattle(gBattlerTarget, ability))

static void atk00_attackcanceler(void);
static void Cmd_accuracycheck(void);
static void Cmd_attackstring(void);
static void Cmd_ppreduce(void);
static void Cmd_critcalc(void);
static void Cmd_damagecalc(void);
static void Cmd_typecalc(void);
static void Cmd_adjustnormaldamage(void);
static void Cmd_adjustnormaldamage2(void);
static void atk09_attackanimation(void);
static void Cmd_waitanimation(void);
static void atk0B_healthbarupdate(void);
static void Cmd_datahpupdate(void);
static void Cmd_critmessage(void);
static void Cmd_effectivenesssound(void);
static void Cmd_resultmessage(void);
static void Cmd_printstring(void);
static void Cmd_printselectionstring(void);
static void Cmd_waitmessage(void);
static void Cmd_printfromtable(void);
static void Cmd_printselectionstringfromtable(void);
static void atk15_seteffectwithchance(void);
static void Cmd_seteffectprimary(void);
static void Cmd_seteffectsecondary(void);
static void Cmd_clearstatusfromeffect(void);
static void atk19_tryfaintmon(void);
static void Cmd_dofaintanimation(void);
static void atk1B_cleareffectsonfaint(void);
static void Cmd_jumpifstatus(void);
static void Cmd_jumpifstatus2(void);
static void Cmd_jumpifability(void);
static void Cmd_jumpifsideaffecting(void);
static void Cmd_jumpifstat(void);
static void Cmd_jumpifstatus3condition(void);
static void Cmd_jumpiftype(void);
static void atk23_getexp(void);
static void atk24(void);
static void Cmd_movevaluescleanup(void);
static void Cmd_setmultihit(void);
static void Cmd_decrementmultihit(void);
static void Cmd_goto(void);
static void Cmd_jumpifbyte(void);
static void Cmd_jumpifhalfword(void);
static void Cmd_jumpifword(void);
static void Cmd_jumpifarrayequal(void);
static void Cmd_jumpifarraynotequal(void);
static void Cmd_setbyte(void);
static void Cmd_addbyte(void);
static void Cmd_subbyte(void);
static void Cmd_copyarray(void);
static void Cmd_copyarraywithindex(void);
static void Cmd_orbyte(void);
static void Cmd_orhalfword(void);
static void Cmd_orword(void);
static void Cmd_bicbyte(void);
static void Cmd_bichalfword(void);
static void Cmd_bicword(void);
static void Cmd_pause(void);
static void Cmd_waitstate(void);
static void Cmd_healthbar_update(void);
static void Cmd_return(void);
static void atk3D_end(void);
static void Cmd_end2(void);
static void Cmd_end3(void);
static void Cmd_jumpifaffectedbyprotect(void);
static void Cmd_call(void);
static void Cmd_jumpiftype2(void);
static void Cmd_jumpifabilitypresent(void);
static void Cmd_endselectionscript(void);
static void Cmd_playanimation(void);
static void Cmd_playanimation_var(void);
static void Cmd_setgraphicalstatchangevalues(void);
static void Cmd_playstatchangeanimation(void);
void atk49_moveend(void);
static void Cmd_typecalc2(void);
static void Cmd_returnatktoball(void);
static void Cmd_getswitchedmondata(void);
static void atk4D_switchindataupdate(void);
static void atk4E_switchinanim(void);
static void atk4F_jumpifcantswitch(void);
static void atk50_openpartyscreen(void);
static void atk51_switchhandleorder(void);
static void atk52_switchineffects(void);
static void atk53_trainerslidein(void);
static void Cmd_playse(void);
static void atk55_fanfare(void);
static void Cmd_playfaintcry(void);
static void atk57(void);
static void Cmd_returntoball(void);
static void Cmd_handlelearnnewmove(void);
static void atk5A_yesnoboxlearnmove(void);
static void atk5B_yesnoboxstoplearningmove(void);
static void Cmd_hitanimation(void);
static void atk5D_getmoneyreward(void);
static void Cmd_updatebattlermoves(void);
static void Cmd_swapattackerwithtarget(void);
static void Cmd_incrementgamestat(void);
static void Cmd_drawpartystatussummary(void);
static void Cmd_hidepartystatussummary(void);
static void Cmd_jumptocalledmove(void);
static void Cmd_statusanimation(void);
static void Cmd_status2animation(void);
static void Cmd_chosenstatusanimation(void);
static void atk67_yesnobox(void);
static void Cmd_cancelallactions(void);
static void Cmd_adjustsetdamage(void);
static void Cmd_removeitem(void);
static void Cmd_atknameinbuff1(void);
static void atk6C_drawlvlupbox(void);
static void Cmd_resetsentmonsvalue(void);
static void Cmd_setatktoplayer0(void);
static void Cmd_makevisible(void);
static void Cmd_recordlastability(void);
static void Cmd_buffermovetolearn(void);
static void Cmd_jumpifplayerran(void);
static void Cmd_hpthresholds(void);
static void Cmd_hpthresholds2(void);
static void Cmd_useitemonopponent(void);
static void atk76_various(void);
static void atk77_setprotectlike(void);
static void Cmd_tryexplosion(void);
static void Cmd_setatkhptozero(void);
static void Cmd_jumpifnexttargetvalid(void);
static void Cmd_tryhealhalfhealth(void);
static void atk7C_trymirrormove(void);
static void Cmd_setrain(void);
static void atk7E_setreflect(void);
static void Cmd_setseeded(void);
static void Cmd_manipulatedamage(void);
static void Cmd_trysetrest(void);
static void Cmd_jumpifnotfirstturn(void);
static void Cmd_nop(void);
static void Cmd_jumpifcantmakeasleep(void);
static void Cmd_stockpile(void);
static void Cmd_stockpiletobasedamage(void);
static void Cmd_stockpiletohpheal(void);
static void Cmd_negativedamage(void);
static u8 ChangeStatBuffs(s8 statValue, u8 statId, u8 flags, const u8 *BS_ptr);
static void Cmd_statbuffchange(void);
static void Cmd_normalisebuffs(void);
static void Cmd_setbide(void);
static void Cmd_confuseifrepeatingattackends(void);
static void Cmd_setmultihitcounter(void);
static void Cmd_initmultihitstring(void);
static void atk8F_forcerandomswitch(void);
static void Cmd_tryconversiontypechange(void);
static void atk91_givepaydaymoney(void);
static void atk92_setlightscreen(void);
static void atk93_tryKO(void);
static void Cmd_damagetohalftargethp(void);
static void Cmd_setsandstorm(void);
static void Cmd_weatherdamage(void);
static void atk97_tryinfatuating(void);
static void Cmd_updatestatusicon(void);
static void atk99_setmist(void);
static void Cmd_setfocusenergy(void);
static void Cmd_transformdataexecution(void);
static void Cmd_setsubstitute(void);
static void Cmd_mimicattackcopy(void);
static void atk9E_metronome(void);
static void Cmd_dmgtolevel(void);
static void Cmd_psywavedamageeffect(void);
static void Cmd_counterdamagecalculator(void);
static void Cmd_mirrorcoatdamagecalculator(void);
static void Cmd_disablelastusedattack(void);
static void Cmd_trysetencore(void);
static void Cmd_painsplitdmgcalc(void);
static void atkA6_settypetorandomresistance(void);
static void Cmd_setalwayshitflag(void);
static void Cmd_copymovepermanently(void);
static void Cmd_trychoosesleeptalkmove(void);
static void Cmd_setdestinybond(void);
static void Cmd_trysetdestinybondtohappen(void);
static void Cmd_remaininghptopower(void);
static void atkAD_tryspiteppreduce(void);
static void atkAE_healpartystatus(void);
static void atkAF_cursetarget(void);
static void Cmd_trysetspikes(void);
static void Cmd_setforesight(void);
static void Cmd_trysetperishsong(void);
static void Cmd_rolloutdamagecalculation(void);
static void Cmd_jumpifconfusedandstatmaxed(void);
static void Cmd_furycuttercalc(void);
static void Cmd_friendshiptodamagecalculation(void);
static void atkB7_presentdamagecalculation(void);
static void atkB8_setsafeguard(void);
static void Cmd_magnitudedamagecalculation(void);
static void atkBA_jumpifnopursuitswitchdmg(void);
static void Cmd_setsunny(void);
static void Cmd_maxattackhalvehp(void);
static void Cmd_copyfoestats(void);
static void atkBE_rapidspinfree(void);
static void Cmd_setdefensecurlbit(void);
static void Cmd_recoverbasedonsunlight(void);
static void Cmd_hiddenpowercalc(void);
static void Cmd_selectfirstvalidtarget(void);
static void atkC3_trysetfutureattack(void);
static void Cmd_trydobeatup(void);
static void Cmd_setsemiinvulnerablebit(void);
static void Cmd_clearsemiinvulnerablebit(void);
static void Cmd_setminimize(void);
static void Cmd_sethail(void);
static void Cmd_trymemento(void);
static void Cmd_setforcedtarget(void);
static void Cmd_setcharge(void);
static void Cmd_callenvironmentattack(void);
static void Cmd_cureifburnedparalyzedorpoisoned(void);
static void Cmd_settorment(void);
static void Cmd_jumpifnodamage(void);
static void Cmd_settaunt(void);
static void Cmd_trysethelpinghand(void);
static void atkD2_tryswapitems(void);
static void Cmd_trycopyability(void);
static void Cmd_trywish(void);
static void Cmd_trysetroots(void);
static void Cmd_doubledamagedealtifdamaged(void);
static void Cmd_setyawn(void);
static void Cmd_setdamagetohealthdifference(void);
static void Cmd_scaledamagebyhealthratio(void);
static void Cmd_tryswapabilities(void);
static void atkDB_tryimprison(void);
static void Cmd_trysetgrudge(void);
static void Cmd_weightdamagecalculation(void);
static void Cmd_assistattackselect(void);
static void Cmd_trysetmagiccoat(void);
static void Cmd_trysetsnatch(void);
static void Cmd_trygetintimidatetarget(void);
static void Cmd_switchoutabilities(void);
static void Cmd_jumpifhasnohp(void);
static void Cmd_getsecretpowereffect(void);
static void atkE5_pickup(void);
static void Cmd_docastformchangeanimation(void);
static void Cmd_trycastformdatachange(void);
static void Cmd_settypebasedhalvers(void);
static void Cmd_setweatherballtype(void);
static void Cmd_tryrecycleitem(void);
static void Cmd_settypetoenvironment(void);
static void Cmd_pursuitdoubles(void);
static void Cmd_snatchsetbattlers(void);
static void Cmd_removelightscreenreflect(void);
void atkEF_handleballthrow(void);
static void atkF0_givecaughtmon(void);
static void atkF1_trysetcaughtmondexflags(void);
static void atkF2_displaydexinfo(void);
static void atkF3_trygivecaughtmonnick(void);
static void Cmd_subattackerhpbydmg(void);
static void Cmd_removeattackerstatus1(void);
static void Cmd_finishaction(void);
static void Cmd_finishturn(void);

void (* const gBattleScriptingCommandsTable[])(void) =
{
    [B_SCR_OP_ATTACKCANCELER]                     = atk00_attackcanceler,
    [B_SCR_OP_ACCURACYCHECK]                      = Cmd_accuracycheck,
    [B_SCR_OP_ATTACKSTRING]                       = Cmd_attackstring,
    [B_SCR_OP_PPREDUCE]                           = Cmd_ppreduce,
    [B_SCR_OP_CRITCALC]                           = Cmd_critcalc,
    [B_SCR_OP_DAMAGECALC]                         = Cmd_damagecalc,
    [B_SCR_OP_TYPECALC]                           = Cmd_typecalc,
    [B_SCR_OP_ADJUSTNORMALDAMAGE]                 = Cmd_adjustnormaldamage,
    [B_SCR_OP_ADJUSTNORMALDAMAGE2]                = Cmd_adjustnormaldamage2,
    [B_SCR_OP_ATTACKANIMATION]                    = atk09_attackanimation,
    [B_SCR_OP_WAITANIMATION]                      = Cmd_waitanimation,
    [B_SCR_OP_HEALTHBARUPDATE]                    = atk0B_healthbarupdate,
    [B_SCR_OP_DATAHPUPDATE]                       = Cmd_datahpupdate,
    [B_SCR_OP_CRITMESSAGE]                        = Cmd_critmessage,
    [B_SCR_OP_EFFECTIVENESSSOUND]                 = Cmd_effectivenesssound,
    [B_SCR_OP_RESULTMESSAGE]                      = Cmd_resultmessage,
    [B_SCR_OP_PRINTSTRING]                        = Cmd_printstring,
    [B_SCR_OP_PRINTSELECTIONSTRING]               = Cmd_printselectionstring,
    [B_SCR_OP_WAITMESSAGE]                        = Cmd_waitmessage,
    [B_SCR_OP_PRINTFROMTABLE]                     = Cmd_printfromtable,
    [B_SCR_OP_PRINTSELECTIONSTRINGFROMTABLE]      = Cmd_printselectionstringfromtable,
    [B_SCR_OP_SETEFFECTWITHCHANCE]                = atk15_seteffectwithchance,
    [B_SCR_OP_SETEFFECTPRIMARY]                   = Cmd_seteffectprimary,
    [B_SCR_OP_SETEFFECTSECONDARY]                 = Cmd_seteffectsecondary,
    [B_SCR_OP_CLEARSTATUSFROMEFFECT]              = Cmd_clearstatusfromeffect,
    [B_SCR_OP_TRYFAINTMON]                        = atk19_tryfaintmon,
    [B_SCR_OP_DOFAINTANIMATION]                   = Cmd_dofaintanimation,
    [B_SCR_OP_CLEAREFFECTSONFAINT]                = atk1B_cleareffectsonfaint,
    [B_SCR_OP_JUMPIFSTATUS]                       = Cmd_jumpifstatus,
    [B_SCR_OP_JUMPIFSTATUS2]                      = Cmd_jumpifstatus2,
    [B_SCR_OP_JUMPIFABILITY]                      = Cmd_jumpifability,
    [B_SCR_OP_JUMPIFSIDEAFFECTING]                = Cmd_jumpifsideaffecting,
    [B_SCR_OP_JUMPIFSTAT]                         = Cmd_jumpifstat,
    [B_SCR_OP_JUMPIFSTATUS3CONDITION]             = Cmd_jumpifstatus3condition,
    [B_SCR_OP_JUMPIFTYPE]                         = Cmd_jumpiftype,
    [B_SCR_OP_GETEXP]                             = atk23_getexp,
    [B_SCR_OP_CHECKTEAMSLOST]                     = atk24,
    [B_SCR_OP_MOVEVALUESCLEANUP]                  = Cmd_movevaluescleanup,
    [B_SCR_OP_SETMULTIHIT]                        = Cmd_setmultihit,
    [B_SCR_OP_DECREMENTMULTIHIT]                  = Cmd_decrementmultihit,
    [B_SCR_OP_GOTO]                               = Cmd_goto,
    [B_SCR_OP_JUMPIFBYTE]                         = Cmd_jumpifbyte,
    [B_SCR_OP_JUMPIFHALFWORD]                     = Cmd_jumpifhalfword,
    [B_SCR_OP_JUMPIFWORD]                         = Cmd_jumpifword,
    [B_SCR_OP_JUMPIFARRAYEQUAL]                   = Cmd_jumpifarrayequal,
    [B_SCR_OP_JUMPIFARRAYNOTEQUAL]                = Cmd_jumpifarraynotequal,
    [B_SCR_OP_SETBYTE]                            = Cmd_setbyte,
    [B_SCR_OP_ADDBYTE]                            = Cmd_addbyte,
    [B_SCR_OP_SUBBYTE]                            = Cmd_subbyte,
    [B_SCR_OP_COPYARRAY]                          = Cmd_copyarray,
    [B_SCR_OP_COPYARRAYWITHINDEX]                 = Cmd_copyarraywithindex,
    [B_SCR_OP_ORBYTE]                             = Cmd_orbyte,
    [B_SCR_OP_ORHALFWORD]                         = Cmd_orhalfword,
    [B_SCR_OP_ORWORD]                             = Cmd_orword,
    [B_SCR_OP_BICBYTE]                            = Cmd_bicbyte,
    [B_SCR_OP_BICHALFWORD]                        = Cmd_bichalfword,
    [B_SCR_OP_BICWORD]                            = Cmd_bicword,
    [B_SCR_OP_PAUSE]                              = Cmd_pause,
    [B_SCR_OP_WAITSTATE]                          = Cmd_waitstate,
    [B_SCR_OP_HEALTHBAR_UPDATE]                   = Cmd_healthbar_update,
    [B_SCR_OP_RETURN]                             = Cmd_return,
    [B_SCR_OP_END]                                = atk3D_end,
    [B_SCR_OP_END2]                               = Cmd_end2,
    [B_SCR_OP_END3]                               = Cmd_end3,
    [B_SCR_OP_JUMPIFAFFECTEDBYPROTECT]            = Cmd_jumpifaffectedbyprotect,
    [B_SCR_OP_CALL]                               = Cmd_call,
    [B_SCR_OP_JUMPIFTYPE2]                        = Cmd_jumpiftype2,
    [B_SCR_OP_JUMPIFABILITYPRESENT]               = Cmd_jumpifabilitypresent,
    [B_SCR_OP_ENDSELECTIONSCRIPT]                 = Cmd_endselectionscript,
    [B_SCR_OP_PLAYANIMATION]                      = Cmd_playanimation,
    [B_SCR_OP_PLAYANIMATION_VAR]                  = Cmd_playanimation_var,
    [B_SCR_OP_SETGRAPHICALSTATCHANGEVALUES]       = Cmd_setgraphicalstatchangevalues,
    [B_SCR_OP_PLAYSTATCHANGEANIMATION]            = Cmd_playstatchangeanimation,
    [B_SCR_OP_MOVEEND]                            = atk49_moveend,
    [B_SCR_OP_TYPECALC2]                          = Cmd_typecalc2,
    [B_SCR_OP_RETURNATKTOBALL]                    = Cmd_returnatktoball,
    [B_SCR_OP_GETSWITCHEDMONDATA]                 = Cmd_getswitchedmondata,
    [B_SCR_OP_SWITCHINDATAUPDATE]                 = atk4D_switchindataupdate,
    [B_SCR_OP_SWITCHINANIM]                       = atk4E_switchinanim,
    [B_SCR_OP_JUMPIFCANTSWITCH]                   = atk4F_jumpifcantswitch,
    [B_SCR_OP_OPENPARTYSCREEN]                    = atk50_openpartyscreen,
    [B_SCR_OP_SWITCHHANDLEORDER]                  = atk51_switchhandleorder,
    [B_SCR_OP_SWITCHINEFFECTS]                    = atk52_switchineffects,
    [B_SCR_OP_TRAINERSLIDEIN]                     = atk53_trainerslidein,
    [B_SCR_OP_PLAYSE]                             = Cmd_playse,
    [B_SCR_OP_FANFARE]                            = atk55_fanfare,
    [B_SCR_OP_PLAYFAINTCRY]                       = Cmd_playfaintcry,
    [B_SCR_OP_ENDLINKBATTLE]                      = atk57,
    [B_SCR_OP_RETURNTOBALL]                       = Cmd_returntoball,
    [B_SCR_OP_HANDLELEARNNEWMOVE]                 = Cmd_handlelearnnewmove,
    [B_SCR_OP_YESNOBOXLEARNMOVE]                  = atk5A_yesnoboxlearnmove,
    [B_SCR_OP_YESNOBOXSTOPLEARNINGMOVE]           = atk5B_yesnoboxstoplearningmove,
    [B_SCR_OP_HITANIMATION]                       = Cmd_hitanimation,
    [B_SCR_OP_GETMONEYREWARD]                     = atk5D_getmoneyreward,
    [B_SCR_OP_UPDATEBATTLERMOVES]                 = Cmd_updatebattlermoves,
    [B_SCR_OP_SWAPATTACKERWITHTARGET]             = Cmd_swapattackerwithtarget,
    [B_SCR_OP_INCREMENTGAMESTAT]                  = Cmd_incrementgamestat,
    [B_SCR_OP_DRAWPARTYSTATUSSUMMARY]             = Cmd_drawpartystatussummary,
    [B_SCR_OP_HIDEPARTYSTATUSSUMMARY]             = Cmd_hidepartystatussummary,
    [B_SCR_OP_JUMPTOCALLEDMOVE]                   = Cmd_jumptocalledmove,
    [B_SCR_OP_STATUSANIMATION]                    = Cmd_statusanimation,
    [B_SCR_OP_STATUS2ANIMATION]                   = Cmd_status2animation,
    [B_SCR_OP_CHOSENSTATUSANIMATION]              = Cmd_chosenstatusanimation,
    [B_SCR_OP_YESNOBOX]                           = atk67_yesnobox,
    [B_SCR_OP_CANCELALLACTIONS]                   = Cmd_cancelallactions,
    [B_SCR_OP_ADJUSTSETDAMAGE]                    = Cmd_adjustsetdamage,
    [B_SCR_OP_REMOVEITEM]                         = Cmd_removeitem,
    [B_SCR_OP_ATKNAMEINBUFF1]                     = Cmd_atknameinbuff1,
    [B_SCR_OP_DRAWLVLUPBOX]                       = atk6C_drawlvlupbox,
    [B_SCR_OP_RESETSENTMONSVALUE]                 = Cmd_resetsentmonsvalue,
    [B_SCR_OP_SETATKTOPLAYER0]                    = Cmd_setatktoplayer0,
    [B_SCR_OP_MAKEVISIBLE]                        = Cmd_makevisible,
    [B_SCR_OP_RECORDLASTABILITY]                  = Cmd_recordlastability,
    [B_SCR_OP_BUFFERMOVETOLEARN]                  = Cmd_buffermovetolearn,
    [B_SCR_OP_JUMPIFPLAYERRAN]                    = Cmd_jumpifplayerran,
    [B_SCR_OP_HPTHRESHOLDS]                       = Cmd_hpthresholds,
    [B_SCR_OP_HPTHRESHOLDS2]                      = Cmd_hpthresholds2,
    [B_SCR_OP_USEITEMONOPPONENT]                  = Cmd_useitemonopponent,
    [B_SCR_OP_VARIOUS]                            = atk76_various,
    [B_SCR_OP_SETPROTECTLIKE]                     = atk77_setprotectlike,
    [B_SCR_OP_TRYEXPLOSION]                       = Cmd_tryexplosion,
    [B_SCR_OP_SETATKHPTOZERO]                     = Cmd_setatkhptozero,
    [B_SCR_OP_JUMPIFNEXTTARGETVALID]              = Cmd_jumpifnexttargetvalid,
    [B_SCR_OP_TRYHEALHALFHEALTH]                  = Cmd_tryhealhalfhealth,
    [B_SCR_OP_TRYMIRRORMOVE]                      = atk7C_trymirrormove,
    [B_SCR_OP_SETRAIN]                            = Cmd_setrain,
    [B_SCR_OP_SETREFLECT]                         = atk7E_setreflect,
    [B_SCR_OP_SETSEEDED]                          = Cmd_setseeded,
    [B_SCR_OP_MANIPULATEDAMAGE]                   = Cmd_manipulatedamage,
    [B_SCR_OP_TRYSETREST]                         = Cmd_trysetrest,
    [B_SCR_OP_JUMPIFNOTFIRSTTURN]                 = Cmd_jumpifnotfirstturn,
    [B_SCR_OP_NOP]                                = Cmd_nop,
    [B_SCR_OP_JUMPIFCANTMAKEASLEEP]               = Cmd_jumpifcantmakeasleep,
    [B_SCR_OP_STOCKPILE]                          = Cmd_stockpile,
    [B_SCR_OP_STOCKPILETOBASEDAMAGE]              = Cmd_stockpiletobasedamage,
    [B_SCR_OP_STOCKPILETOHPHEAL]                  = Cmd_stockpiletohpheal,
    [B_SCR_OP_NEGATIVEDAMAGE]                     = Cmd_negativedamage,
    [B_SCR_OP_STATBUFFCHANGE]                     = Cmd_statbuffchange,
    [B_SCR_OP_NORMALISEBUFFS]                     = Cmd_normalisebuffs,
    [B_SCR_OP_SETBIDE]                            = Cmd_setbide,
    [B_SCR_OP_CONFUSEIFREPEATINGATTACKENDS]       = Cmd_confuseifrepeatingattackends,
    [B_SCR_OP_SETMULTIHITCOUNTER]                 = Cmd_setmultihitcounter,
    [B_SCR_OP_INITMULTIHITSTRING]                 = Cmd_initmultihitstring,
    [B_SCR_OP_FORCERANDOMSWITCH]                  = atk8F_forcerandomswitch,
    [B_SCR_OP_TRYCONVERSIONTYPECHANGE]            = Cmd_tryconversiontypechange,
    [B_SCR_OP_GIVEPAYDAYMONEY]                    = atk91_givepaydaymoney,
    [B_SCR_OP_SETLIGHTSCREEN]                     = atk92_setlightscreen,
    [B_SCR_OP_TRYKO]                              = atk93_tryKO,
    [B_SCR_OP_DAMAGETOHALFTARGETHP]               = Cmd_damagetohalftargethp,
    [B_SCR_OP_SETSANDSTORM]                       = Cmd_setsandstorm,
    [B_SCR_OP_WEATHERDAMAGE]                      = Cmd_weatherdamage,
    [B_SCR_OP_TRYINFATUATING]                     = atk97_tryinfatuating,
    [B_SCR_OP_UPDATESTATUSICON]                   = Cmd_updatestatusicon,
    [B_SCR_OP_SETMIST]                            = atk99_setmist,
    [B_SCR_OP_SETFOCUSENERGY]                     = Cmd_setfocusenergy,
    [B_SCR_OP_TRANSFORMDATAEXECUTION]             = Cmd_transformdataexecution,
    [B_SCR_OP_SETSUBSTITUTE]                      = Cmd_setsubstitute,
    [B_SCR_OP_MIMICATTACKCOPY]                    = Cmd_mimicattackcopy,
    [B_SCR_OP_METRONOME]                          = atk9E_metronome,
    [B_SCR_OP_DMGTOLEVEL]                         = Cmd_dmgtolevel,
    [B_SCR_OP_PSYWAVEDAMAGEEFFECT]                = Cmd_psywavedamageeffect,
    [B_SCR_OP_COUNTERDAMAGECALCULATOR]            = Cmd_counterdamagecalculator,
    [B_SCR_OP_MIRRORCOATDAMAGECALCULATOR]         = Cmd_mirrorcoatdamagecalculator,
    [B_SCR_OP_DISABLELASTUSEDATTACK]              = Cmd_disablelastusedattack,
    [B_SCR_OP_TRYSETENCORE]                       = Cmd_trysetencore,
    [B_SCR_OP_PAINSPLITDMGCALC]                   = Cmd_painsplitdmgcalc,
    [B_SCR_OP_SETTYPETORANDOMRESISTANCE]          = atkA6_settypetorandomresistance,
    [B_SCR_OP_SETALWAYSHITFLAG]                   = Cmd_setalwayshitflag,
    [B_SCR_OP_COPYMOVEPERMANENTLY]                = Cmd_copymovepermanently,
    [B_SCR_OP_TRYCHOOSESLEEPTALKMOVE]             = Cmd_trychoosesleeptalkmove,
    [B_SCR_OP_SETDESTINYBOND]                     = Cmd_setdestinybond,
    [B_SCR_OP_TRYSETDESTINYBONDTOHAPPEN]          = Cmd_trysetdestinybondtohappen,
    [B_SCR_OP_REMAININGHPTOPOWER]                 = Cmd_remaininghptopower,
    [B_SCR_OP_TRYSPITEPPREDUCE]                   = atkAD_tryspiteppreduce,
    [B_SCR_OP_HEALPARTYSTATUS]                    = atkAE_healpartystatus,
    [B_SCR_OP_CURSETARGET]                        = atkAF_cursetarget,
    [B_SCR_OP_TRYSETSPIKES]                       = Cmd_trysetspikes,
    [B_SCR_OP_SETFORESIGHT]                       = Cmd_setforesight,
    [B_SCR_OP_TRYSETPERISHSONG]                   = Cmd_trysetperishsong,
    [B_SCR_OP_ROLLOUTDAMAGECALCULATION]           = Cmd_rolloutdamagecalculation,
    [B_SCR_OP_JUMPIFCONFUSEDANDSTATMAXED]         = Cmd_jumpifconfusedandstatmaxed,
    [B_SCR_OP_FURYCUTTERCALC]                     = Cmd_furycuttercalc,
    [B_SCR_OP_FRIENDSHIPTODAMAGECALCULATION]      = Cmd_friendshiptodamagecalculation,
    [B_SCR_OP_PRESENTDAMAGECALCULATION]           = atkB7_presentdamagecalculation,
    [B_SCR_OP_SETSAFEGUARD]                       = atkB8_setsafeguard,
    [B_SCR_OP_MAGNITUDEDAMAGECALCULATION]         = Cmd_magnitudedamagecalculation,
    [B_SCR_OP_JUMPIFNOPURSUITSWITCHDMG]           = atkBA_jumpifnopursuitswitchdmg,
    [B_SCR_OP_SETSUNNY]                           = Cmd_setsunny,
    [B_SCR_OP_MAXATTACKHALVEHP]                   = Cmd_maxattackhalvehp,
    [B_SCR_OP_COPYFOESTATS]                       = Cmd_copyfoestats,
    [B_SCR_OP_RAPIDSPINFREE]                      = atkBE_rapidspinfree,
    [B_SCR_OP_SETDEFENSECURLBIT]                  = Cmd_setdefensecurlbit,
    [B_SCR_OP_RECOVERBASEDONSUNLIGHT]             = Cmd_recoverbasedonsunlight,
    [B_SCR_OP_HIDDENPOWERCALC]                    = Cmd_hiddenpowercalc,
    [B_SCR_OP_SELECTFIRSTVALIDTARGET]             = Cmd_selectfirstvalidtarget,
    [B_SCR_OP_TRYSETFUTUREATTACK]                 = atkC3_trysetfutureattack,
    [B_SCR_OP_TRYDOBEATUP]                        = Cmd_trydobeatup,
    [B_SCR_OP_SETSEMIINVULNERABLEBIT]             = Cmd_setsemiinvulnerablebit,
    [B_SCR_OP_CLEARSEMIINVULNERABLEBIT]           = Cmd_clearsemiinvulnerablebit,
    [B_SCR_OP_SETMINIMIZE]                        = Cmd_setminimize,
    [B_SCR_OP_SETHAIL]                            = Cmd_sethail,
    [B_SCR_OP_TRYMEMENTO]                         = Cmd_trymemento,
    [B_SCR_OP_SETFORCEDTARGET]                    = Cmd_setforcedtarget,
    [B_SCR_OP_SETCHARGE]                          = Cmd_setcharge,
    [B_SCR_OP_CALLENVIRONMENTATTACK]              = Cmd_callenvironmentattack,
    [B_SCR_OP_CUREIFBURNEDPARALYZEDORPOISONED]    = Cmd_cureifburnedparalyzedorpoisoned,
    [B_SCR_OP_SETTORMENT]                         = Cmd_settorment,
    [B_SCR_OP_JUMPIFNODAMAGE]                     = Cmd_jumpifnodamage,
    [B_SCR_OP_SETTAUNT]                           = Cmd_settaunt,
    [B_SCR_OP_TRYSETHELPINGHAND]                  = Cmd_trysethelpinghand,
    [B_SCR_OP_TRYSWAPITEMS]                       = atkD2_tryswapitems,
    [B_SCR_OP_TRYCOPYABILITY]                     = Cmd_trycopyability,
    [B_SCR_OP_TRYWISH]                            = Cmd_trywish,
    [B_SCR_OP_TRYSETROOTS]                        = Cmd_trysetroots,
    [B_SCR_OP_DOUBLEDAMAGEDEALTIFDAMAGED]         = Cmd_doubledamagedealtifdamaged,
    [B_SCR_OP_SETYAWN]                            = Cmd_setyawn,
    [B_SCR_OP_SETDAMAGETOHEALTHDIFFERENCE]        = Cmd_setdamagetohealthdifference,
    [B_SCR_OP_SCALEDAMAGEBYHEALTHRATIO]           = Cmd_scaledamagebyhealthratio,
    [B_SCR_OP_TRYSWAPABILITIES]                   = Cmd_tryswapabilities,
    [B_SCR_OP_TRYIMPRISON]                        = atkDB_tryimprison,
    [B_SCR_OP_TRYSETGRUDGE]                       = Cmd_trysetgrudge,
    [B_SCR_OP_WEIGHTDAMAGECALCULATION]            = Cmd_weightdamagecalculation,
    [B_SCR_OP_ASSISTATTACKSELECT]                 = Cmd_assistattackselect,
    [B_SCR_OP_TRYSETMAGICCOAT]                    = Cmd_trysetmagiccoat,
    [B_SCR_OP_TRYSETSNATCH]                       = Cmd_trysetsnatch,
    [B_SCR_OP_TRYGETINTIMIDATETARGET]             = Cmd_trygetintimidatetarget,
    [B_SCR_OP_SWITCHOUTABILITIES]                 = Cmd_switchoutabilities,
    [B_SCR_OP_JUMPIFHASNOHP]                      = Cmd_jumpifhasnohp,
    [B_SCR_OP_GETSECRETPOWEREFFECT]               = Cmd_getsecretpowereffect,
    [B_SCR_OP_PICKUP]                             = atkE5_pickup,
    [B_SCR_OP_DOCASTFORMCHANGEANIMATION]          = Cmd_docastformchangeanimation,
    [B_SCR_OP_TRYCASTFORMDATACHANGE]              = Cmd_trycastformdatachange,
    [B_SCR_OP_SETTYPEBASEDHALVERS]                = Cmd_settypebasedhalvers,
    [B_SCR_OP_SETWEATHERBALLTYPE]                 = Cmd_setweatherballtype,
    [B_SCR_OP_TRYRECYCLEITEM]                     = Cmd_tryrecycleitem,
    [B_SCR_OP_SETTYPETOENVIRONMENT]               = Cmd_settypetoenvironment,
    [B_SCR_OP_PURSUITDOUBLES]                     = Cmd_pursuitdoubles,
    [B_SCR_OP_SNATCHSETBATTLERS]                  = Cmd_snatchsetbattlers,
    [B_SCR_OP_REMOVELIGHTSCREENREFLECT]           = Cmd_removelightscreenreflect,
    [B_SCR_OP_HANDLEBALLTHROW]                    = atkEF_handleballthrow,
    [B_SCR_OP_GIVECAUGHTMON]                      = atkF0_givecaughtmon,
    [B_SCR_OP_TRYSETCAUGHTMONDEXFLAGS]            = atkF1_trysetcaughtmondexflags,
    [B_SCR_OP_DISPLAYDEXINFO]                     = atkF2_displaydexinfo,
    [B_SCR_OP_TRYGIVECAUGHTMONNICK]               = atkF3_trygivecaughtmonnick,
    [B_SCR_OP_SUBATTACKERHPBYDMG]                 = Cmd_subattackerhpbydmg,
    [B_SCR_OP_REMOVEATTACKERSTATUS1]              = Cmd_removeattackerstatus1,
    [B_SCR_OP_FINISHACTION]                       = Cmd_finishaction,
    [B_SCR_OP_FINISHTURN]                         = Cmd_finishturn,
};

struct StatFractions
{
    u8 dividend;
    u8 divisor;
};

static const struct StatFractions sAccuracyStageRatios[] =
{
    { 33, 100}, // -6
    { 36, 100}, // -5
    { 43, 100}, // -4
    { 50, 100}, // -3
    { 60, 100}, // -2
    { 75, 100}, // -1
    {  1,   1}, //  0
    {133, 100}, // +1
    {166, 100}, // +2
    {  2,   1}, // +3
    {233, 100}, // +4
    {133,  50}, // +5
    {  3,   1}, // +6
};

// The chance is 1/N for each stage.
static const u16 sCriticalHitChance[] = {16, 8, 4, 3, 2};

static const u32 sStatusFlagsForMoveEffects[NUM_MOVE_EFFECTS] =
{
    [MOVE_EFFECT_SLEEP]            = STATUS1_SLEEP,
    [MOVE_EFFECT_POISON]           = STATUS1_POISON,
    [MOVE_EFFECT_BURN]             = STATUS1_BURN,
    [MOVE_EFFECT_FREEZE]           = STATUS1_FREEZE,
    [MOVE_EFFECT_PARALYSIS]        = STATUS1_PARALYSIS,
    [MOVE_EFFECT_TOXIC]            = STATUS1_TOXIC_POISON,
    [MOVE_EFFECT_CONFUSION]        = STATUS2_CONFUSION,
    [MOVE_EFFECT_FLINCH]           = STATUS2_FLINCHED,
    [MOVE_EFFECT_UPROAR]           = STATUS2_UPROAR,
    [MOVE_EFFECT_CHARGING]         = STATUS2_MULTIPLETURNS,
    [MOVE_EFFECT_WRAP]             = STATUS2_WRAPPED,
    [MOVE_EFFECT_RECHARGE]         = STATUS2_RECHARGE,
    [MOVE_EFFECT_PREVENT_ESCAPE]   = STATUS2_ESCAPE_PREVENTION,
    [MOVE_EFFECT_NIGHTMARE]        = STATUS2_NIGHTMARE,
    [MOVE_EFFECT_THRASH]           = STATUS2_LOCK_CONFUSE,
};

u8* const sMoveEffectBS_Ptrs[] =
{
    [0]                            = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_SLEEP]            = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_POISON]           = BattleScript_MoveEffectPoison,
    [MOVE_EFFECT_BURN]             = BattleScript_MoveEffectBurn,
    [MOVE_EFFECT_FREEZE]           = BattleScript_MoveEffectFreeze,
    [MOVE_EFFECT_PARALYSIS]        = BattleScript_MoveEffectParalysis,
    [MOVE_EFFECT_TOXIC]            = BattleScript_MoveEffectToxic,
    [MOVE_EFFECT_CONFUSION]        = BattleScript_MoveEffectConfusion,
    [MOVE_EFFECT_FLINCH]           = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_TRI_ATTACK]       = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_UPROAR]           = BattleScript_MoveEffectUproar,
    [MOVE_EFFECT_PAYDAY]           = BattleScript_MoveEffectPayDay,
    [MOVE_EFFECT_CHARGING]         = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_WRAP]             = BattleScript_MoveEffectWrap,
    [MOVE_EFFECT_RECOIL_25]        = BattleScript_MoveEffectRecoil,
    [MOVE_EFFECT_ATK_PLUS_1]       = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_DEF_PLUS_1]       = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_SPD_PLUS_1]       = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_SP_ATK_PLUS_1]    = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_SP_DEF_PLUS_1]    = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_ACC_PLUS_1]       = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_EVS_PLUS_1]       = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_ATK_MINUS_1]      = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_DEF_MINUS_1]      = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_SPD_MINUS_1]      = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_SP_ATK_MINUS_1]   = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_SP_DEF_MINUS_1]   = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_ACC_MINUS_1]      = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_EVS_MINUS_1]      = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_RECHARGE]         = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_RAGE]             = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_STEAL_ITEM]       = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_PREVENT_ESCAPE]   = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_NIGHTMARE]        = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_ALL_STATS_UP]     = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_RAPIDSPIN]        = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_REMOVE_PARALYSIS] = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_ATK_DEF_DOWN]     = BattleScript_MoveEffectSleep,
    [MOVE_EFFECT_RECOIL_33]        = BattleScript_MoveEffectRecoil,
};

const u8 sUnreferencedBitMask1[] = {0, 1, 3, 7, 0xF, 0x1F, 0x3F};

static const u8 sRubyLevelUpStatBoxStats[] =
{
    MON_DATA_MAX_HP, MON_DATA_SPATK, MON_DATA_ATK,
    MON_DATA_SPDEF, MON_DATA_DEF, MON_DATA_SPEED
};

static const u16 sProtectSuccessRates[] = {USHRT_MAX, USHRT_MAX / 2, USHRT_MAX / 4, USHRT_MAX / 8};

#define MIMIC_FORBIDDEN_END             0xFFFE
#define METRONOME_FORBIDDEN_END         0xFFFF
#define ASSIST_FORBIDDEN_END            0xFFFF

static const u16 sMovesForbiddenToCopy[] =
{
    MOVE_METRONOME,
    MOVE_STRUGGLE,
    MOVE_SKETCH,
    MOVE_MIMIC,
    MIMIC_FORBIDDEN_END,
    MOVE_COUNTER,
    MOVE_MIRROR_COAT,
    MOVE_PROTECT,
    MOVE_DETECT,
    MOVE_ENDURE,
    MOVE_DESTINY_BOND,
    MOVE_SLEEP_TALK,
    MOVE_THIEF,
    MOVE_FOLLOW_ME,
    MOVE_SNATCH,
    MOVE_HELPING_HAND,
    MOVE_COVET,
    MOVE_TRICK,
    MOVE_FOCUS_PUNCH,
    METRONOME_FORBIDDEN_END
};

static const u8 sFlailHpScaleToPowerTable[] = //reversal+flail HP thresholds to power
{
    1, 200,
    4, 150,
    9, 100,
    16, 80,
    32, 40,
    48, 20
};

static const u16 sNaturePowerMoves[] =
{
    [BATTLE_ENVIRONMENT_GRASS]      = MOVE_STUN_SPORE,
    [BATTLE_ENVIRONMENT_LONG_GRASS] = MOVE_RAZOR_LEAF,
    [BATTLE_ENVIRONMENT_SAND]       = MOVE_EARTHQUAKE,
    [BATTLE_ENVIRONMENT_UNDERWATER] = MOVE_HYDRO_PUMP,
    [BATTLE_ENVIRONMENT_WATER]      = MOVE_SURF,
    [BATTLE_ENVIRONMENT_POND]       = MOVE_BUBBLE_BEAM,
    [BATTLE_ENVIRONMENT_MOUNTAIN]   = MOVE_ROCK_SLIDE,
    [BATTLE_ENVIRONMENT_CAVE]       = MOVE_SHADOW_BALL,
    [BATTLE_ENVIRONMENT_BUILDING]   = MOVE_SWIFT,
    [BATTLE_ENVIRONMENT_PLAIN]      = MOVE_SWIFT
};

// weight-based damage table for Low Kick
// format: min. weight (hectograms), base power
static const u16 sWeightToDamageTable[] =
{
    100, 20,
    250, 40,
    500, 60,
    1000, 80,
    2000, 100,
    0xFFFF, 0xFFFF
};

static const u16 sPickupItems[] =
{
    ITEM_SUPER_POTION, 30,
    ITEM_FULL_HEAL, 40,
    ITEM_ULTRA_BALL, 50,
    ITEM_RARE_CANDY, 60,
    ITEM_FULL_RESTORE, 70,
    ITEM_REVIVE, 80,
    ITEM_NUGGET, 90,
    ITEM_PROTEIN, 95,
    ITEM_PP_UP, 99,
    ITEM_KINGS_ROCK, 1
};

static const u8 sEnvironmentToType[] =
{
    [BATTLE_ENVIRONMENT_GRASS]      = TYPE_GRASS,
    [BATTLE_ENVIRONMENT_LONG_GRASS] = TYPE_GRASS,
    [BATTLE_ENVIRONMENT_SAND]       = TYPE_GROUND,
    [BATTLE_ENVIRONMENT_UNDERWATER] = TYPE_WATER,
    [BATTLE_ENVIRONMENT_WATER]      = TYPE_WATER,
    [BATTLE_ENVIRONMENT_POND]       = TYPE_WATER,
    [BATTLE_ENVIRONMENT_MOUNTAIN]   = TYPE_ROCK,
    [BATTLE_ENVIRONMENT_CAVE]       = TYPE_ROCK,
    [BATTLE_ENVIRONMENT_BUILDING]   = TYPE_NORMAL,
    [BATTLE_ENVIRONMENT_PLAIN]      = TYPE_NORMAL,
};

static const u8 sBallCatchBonuses[] =
{
    [ITEM_ULTRA_BALL - ITEM_ULTRA_BALL]  = 20,
    [ITEM_GREAT_BALL - ITEM_ULTRA_BALL]  = 15,
    [ITEM_POKE_BALL - ITEM_ULTRA_BALL]   = 10,
    [ITEM_SAFARI_BALL - ITEM_ULTRA_BALL] = 15
};

static void atk00_attackcanceler(void)
{
    s32 i;

    if (gBattleOutcome != 0)
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        return;
    }
    if (gBattleMons[gBattlerAttacker].hp == 0 && !(gHitMarker & HITMARKER_NO_ATTACKSTRING))
    {
        gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
        gBattlescriptCurrInstr = BattleScript_MoveEnd;
        return;
    }
    if (AtkCanceler_UnableToUseMove())
        return;
    if (AbilityBattleEffects(ABILITYEFFECT_MOVES_BLOCK, gBattlerTarget, 0, 0, 0))
        return;
    if (!gBattleMons[gBattlerAttacker].pp[gCurrMovePos] && gCurrentMove != MOVE_STRUGGLE && !(gHitMarker & (HITMARKER_ALLOW_NO_PP | HITMARKER_NO_ATTACKSTRING))
     && !(gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS))
    {
        gBattlescriptCurrInstr = BattleScript_NoPPForMove;
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        return;
    }

    gHitMarker &= ~(HITMARKER_ALLOW_NO_PP);

    if (!(gHitMarker & HITMARKER_OBEYS) && !(gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS))
    {
        i = IsMonDisobedient(); // why use the 'i' variable...?
        switch (i)
        {
        case 0:
            break;
        case 2:
            gHitMarker |= HITMARKER_OBEYS;
            return;
        default:
            gMoveResultFlags |= MOVE_RESULT_MISSED;
            return;
        }
    }

    gHitMarker |= HITMARKER_OBEYS;

    if (gProtectStructs[gBattlerTarget].bounceMove && gBattleMoves[gCurrentMove].flags & FLAG_MAGIC_COAT_AFFECTED)
    {
        PressurePPLose(gBattlerAttacker, gBattlerTarget, MOVE_MAGIC_COAT);
        gProtectStructs[gBattlerTarget].bounceMove = FALSE;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_MagicCoatBounce;
        return;
    }

    for (i = 0; i < gBattlersCount; i++)
    {
        if ((gProtectStructs[gBattlerByTurnOrder[i]].stealMove) && gBattleMoves[gCurrentMove].flags & FLAG_SNATCH_AFFECTED)
        {
            PressurePPLose(gBattlerAttacker, gBattlerByTurnOrder[i], MOVE_SNATCH);
            gProtectStructs[gBattlerByTurnOrder[i]].stealMove = FALSE;
            gBattleStruct->battler = gBattlerByTurnOrder[i];
            BattleScriptPushCursor();
            gBattlescriptCurrInstr = BattleScript_SnatchedMove;
            return;
        }
    }

    if (gSpecialStatuses[gBattlerTarget].lightningRodRedirected)
    {
        gSpecialStatuses[gBattlerTarget].lightningRodRedirected = FALSE;
        gLastUsedAbility = ABILITY_LIGHTNING_ROD;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_TookAttack;
        RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
    }
    else if (DEFENDER_IS_PROTECTED
     && (gCurrentMove != MOVE_CURSE || IS_BATTLER_OF_TYPE(gBattlerAttacker, TYPE_GHOST))
     && ((!IsTwoTurnsMove(gCurrentMove) || (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS))))
    {
        CancelMultiTurnMoves(gBattlerAttacker);
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gLastLandedMoves[gBattlerTarget] = MOVE_NONE;
        gLastHitByType[gBattlerTarget] = 0;
        gBattleCommunication[MISS_TYPE] = B_MSG_PROTECTED;
        gBattlescriptCurrInstr++;
    }
    else
    {
        gBattlescriptCurrInstr++;
    }
}

static void JumpIfMoveFailed(u8 adder, u16 move)
{
    const u8 *BS_ptr = gBattlescriptCurrInstr + adder;
    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
    {
        gLastLandedMoves[gBattlerTarget] = MOVE_NONE;
        gLastHitByType[gBattlerTarget] = 0;
        BS_ptr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        TrySetDestinyBondToHappen();
        if (AbilityBattleEffects(ABILITYEFFECT_ABSORBING, gBattlerTarget, 0, 0, move))
            return;
    }
    gBattlescriptCurrInstr = BS_ptr;
}

static void Cmd_jumpifaffectedbyprotect(void)
{
    if (DEFENDER_IS_PROTECTED)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        JumpIfMoveFailed(5, 0);
        gBattleCommunication[MISS_TYPE] = B_MSG_PROTECTED;
    }
    else
    {
        gBattlescriptCurrInstr += 5;
    }
}

static bool8 JumpIfMoveAffectedByProtect(u16 move)
{
    bool8 affected = FALSE;
    if (DEFENDER_IS_PROTECTED)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        JumpIfMoveFailed(7, move);
        gBattleCommunication[MISS_TYPE] = B_MSG_PROTECTED;
        affected = TRUE;
    }
    return affected;
}

static bool8 AccuracyCalcHelper(u16 move)
{
    if (gStatuses3[gBattlerTarget] & STATUS3_ALWAYS_HITS && gDisableStructs[gBattlerTarget].battlerWithSureHit == gBattlerAttacker)
    {
        JumpIfMoveFailed(7, move);
        return TRUE;
    }

    if (!(gHitMarker & HITMARKER_IGNORE_ON_AIR) && gStatuses3[gBattlerTarget] & STATUS3_ON_AIR)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        JumpIfMoveFailed(7, move);
        return TRUE;
    }

    gHitMarker &= ~HITMARKER_IGNORE_ON_AIR;

    if (!(gHitMarker & HITMARKER_IGNORE_UNDERGROUND) && gStatuses3[gBattlerTarget] & STATUS3_UNDERGROUND)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        JumpIfMoveFailed(7, move);
        return TRUE;
    }

    gHitMarker &= ~HITMARKER_IGNORE_UNDERGROUND;

    if (!(gHitMarker & HITMARKER_IGNORE_UNDERWATER) && gStatuses3[gBattlerTarget] & STATUS3_UNDERWATER)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        JumpIfMoveFailed(7, move);
        return TRUE;
    }

    gHitMarker &= ~HITMARKER_IGNORE_UNDERWATER;

    if ((WEATHER_HAS_EFFECT && (gBattleWeather & B_WEATHER_RAIN) && gBattleMoves[move].effect == EFFECT_THUNDER)
     || (gBattleMoves[move].effect == EFFECT_ALWAYS_HIT || gBattleMoves[move].effect == EFFECT_VITAL_THROW))
    {
        JumpIfMoveFailed(7, move);
        return TRUE;
    }

    return FALSE;
}

static void Cmd_accuracycheck(void)
{
    u16 move = T2_READ_16(gBattlescriptCurrInstr + 5);

    if (move == NO_ACC_CALC || move == NO_ACC_CALC_CHECK_LOCK_ON)
    {
        if (gStatuses3[gBattlerTarget] & STATUS3_ALWAYS_HITS && move == NO_ACC_CALC_CHECK_LOCK_ON && gDisableStructs[gBattlerTarget].battlerWithSureHit == gBattlerAttacker)
            gBattlescriptCurrInstr += 7;
        else if (gStatuses3[gBattlerTarget] & (STATUS3_ON_AIR | STATUS3_UNDERGROUND | STATUS3_UNDERWATER))
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        else if (!JumpIfMoveAffectedByProtect(ACC_CURR_MOVE))
            gBattlescriptCurrInstr += 7;
    }
    else
    {
        u8 type, moveAcc, holdEffect, param;
        s8 buff;
        u16 calc;

        if (move == ACC_CURR_MOVE)
            move = gCurrentMove;

        GET_MOVE_TYPE(move, type);

        if (JumpIfMoveAffectedByProtect(move))
            return;
        if (AccuracyCalcHelper(move))
            return;

        if (gBattleMons[gBattlerTarget].status2 & STATUS2_FORESIGHT)
        {
            u8 acc = gBattleMons[gBattlerAttacker].statStages[STAT_ACC];
            buff = acc;
        }
        else
        {
            u8 acc = gBattleMons[gBattlerAttacker].statStages[STAT_ACC];
            buff = acc + DEFAULT_STAT_STAGE - gBattleMons[gBattlerTarget].statStages[STAT_EVASION];
        }

        if (buff < MIN_STAT_STAGE)
            buff = MIN_STAT_STAGE;
        if (buff > MAX_STAT_STAGE)
            buff = MAX_STAT_STAGE;

        moveAcc = gBattleMoves[move].accuracy;
        // check Thunder on sunny weather
        if (WEATHER_HAS_EFFECT && gBattleWeather & B_WEATHER_SUN && gBattleMoves[move].effect == EFFECT_THUNDER)
            moveAcc = 50;

        calc = sAccuracyStageRatios[buff].dividend * moveAcc;
        calc /= sAccuracyStageRatios[buff].divisor;

        if (gBattleMons[gBattlerAttacker].ability == ABILITY_COMPOUND_EYES)
            calc = (calc * 130) / 100; // 1.3 compound eyes boost
        if (WEATHER_HAS_EFFECT && gBattleMons[gBattlerTarget].ability == ABILITY_SAND_VEIL && gBattleWeather & B_WEATHER_SANDSTORM)
            calc = (calc * 80) / 100; // 1.2 sand veil loss;
        if (gBattleMons[gBattlerAttacker].ability == ABILITY_HUSTLE && IS_TYPE_PHYSICAL(type))
            calc = (calc * 80) / 100; // 1.2 hustle loss;

        if (gBattleMons[gBattlerTarget].item == ITEM_ENIGMA_BERRY)
        {
            holdEffect = gEnigmaBerries[gBattlerTarget].holdEffect;
            param = gEnigmaBerries[gBattlerTarget].holdEffectParam;
        }
        else
        {
            holdEffect = GetItemHoldEffect(gBattleMons[gBattlerTarget].item);
            param = GetItemHoldEffectParam(gBattleMons[gBattlerTarget].item);
        }

        gPotentialItemEffectBattler = gBattlerTarget;

        if (holdEffect == HOLD_EFFECT_EVASION_UP)
            calc = (calc * (100 - param)) / 100;

        // final calculation
        if ((Random() % 100 + 1) > calc)
        {
            gMoveResultFlags |= MOVE_RESULT_MISSED;
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE &&
                (gBattleMoves[move].target == MOVE_TARGET_BOTH || gBattleMoves[move].target == MOVE_TARGET_FOES_AND_ALLY))
                gBattleCommunication[MISS_TYPE] = B_MSG_AVOIDED_ATK;
            else
                gBattleCommunication[MISS_TYPE] = B_MSG_MISSED;
            CheckWonderGuardAndLevitate();
        }
        JumpIfMoveFailed(7, move);
    }
}

static void Cmd_attackstring(void)
{
    if (gBattleControllerExecFlags)
         return;
    if (!(gHitMarker & (HITMARKER_NO_ATTACKSTRING | HITMARKER_ATTACKSTRING_PRINTED)))
    {
        PrepareStringBattle(STRINGID_USEDMOVE, gBattlerAttacker);
        gHitMarker |= HITMARKER_ATTACKSTRING_PRINTED;
    }
    gBattlescriptCurrInstr++;
    gBattleCommunication[MSG_DISPLAY] = FALSE;
}

static void Cmd_ppreduce(void)
{
    s32 ppToDeduct = 1;

    if (gBattleControllerExecFlags)
        return;

    if (!gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure)
    {
        switch (gBattleMoves[gCurrentMove].target)
        {
        case MOVE_TARGET_FOES_AND_ALLY:
            ppToDeduct += AbilityBattleEffects(ABILITYEFFECT_COUNT_ON_FIELD, gBattlerAttacker, ABILITY_PRESSURE, 0, 0);
            break;
        case MOVE_TARGET_BOTH:
        case MOVE_TARGET_OPPONENTS_FIELD:
            ppToDeduct += AbilityBattleEffects(ABILITYEFFECT_COUNT_OTHER_SIDE, gBattlerAttacker, ABILITY_PRESSURE, 0, 0);
            break;
        default:
            if (gBattlerAttacker != gBattlerTarget && gBattleMons[gBattlerTarget].ability == ABILITY_PRESSURE)
                ppToDeduct++;
            break;
        }
    }

    if (!(gHitMarker & (HITMARKER_NO_PPDEDUCT | HITMARKER_NO_ATTACKSTRING)) && gBattleMons[gBattlerAttacker].pp[gCurrMovePos])
    {
        gProtectStructs[gBattlerAttacker].notFirstStrike = TRUE;

        if (gBattleMons[gBattlerAttacker].pp[gCurrMovePos] > ppToDeduct)
            gBattleMons[gBattlerAttacker].pp[gCurrMovePos] -= ppToDeduct;
        else
            gBattleMons[gBattlerAttacker].pp[gCurrMovePos] = 0;

        if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_TRANSFORMED)
            && !((gDisableStructs[gBattlerAttacker].mimickedMoves) & gBitTable[gCurrMovePos]))
        {
            gActiveBattler = gBattlerAttacker;
            BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_PPMOVE1_BATTLE + gCurrMovePos, 0, sizeof(gBattleMons[gBattlerAttacker].pp[gCurrMovePos]), &gBattleMons[gBattlerAttacker].pp[gCurrMovePos]);
            MarkBattlerForControllerExec(gBattlerAttacker);
        }
    }

    gHitMarker &= ~(HITMARKER_NO_PPDEDUCT);
    gBattlescriptCurrInstr++;
}

static void Cmd_critcalc(void)
{
    u8 holdEffect;
    u16 item, critChance;

    item = gBattleMons[gBattlerAttacker].item;

    if (item == ITEM_ENIGMA_BERRY)
        holdEffect = gEnigmaBerries[gBattlerAttacker].holdEffect;
    else
        holdEffect = GetItemHoldEffect(item);

    gPotentialItemEffectBattler = gBattlerAttacker;

    critChance  = 2 * ((gBattleMons[gBattlerAttacker].status2 & STATUS2_FOCUS_ENERGY) != 0)
                + (gBattleMoves[gCurrentMove].effect == EFFECT_HIGH_CRITICAL)
                + (gBattleMoves[gCurrentMove].effect == EFFECT_SKY_ATTACK)
                + (gBattleMoves[gCurrentMove].effect == EFFECT_BLAZE_KICK)
                + (gBattleMoves[gCurrentMove].effect == EFFECT_POISON_TAIL)
                + (holdEffect == HOLD_EFFECT_SCOPE_LENS)
                + 2 * (holdEffect == HOLD_EFFECT_LUCKY_PUNCH && gBattleMons[gBattlerAttacker].species == SPECIES_CHANSEY)
                + 2 * (holdEffect == HOLD_EFFECT_STICK && gBattleMons[gBattlerAttacker].species == SPECIES_FARFETCHD);

    if (critChance >= ARRAY_COUNT(sCriticalHitChance))
        critChance = ARRAY_COUNT(sCriticalHitChance) - 1;

    if ((gBattleMons[gBattlerTarget].ability != ABILITY_BATTLE_ARMOR && gBattleMons[gBattlerTarget].ability != ABILITY_SHELL_ARMOR)
     && !(gStatuses3[gBattlerAttacker] & STATUS3_CANT_SCORE_A_CRIT)
     && !(gBattleTypeFlags & (BATTLE_TYPE_WALLY_TUTORIAL | BATTLE_TYPE_FIRST_BATTLE))
     && !(Random() % sCriticalHitChance[critChance]))
        gCritMultiplier = 2;
    else
        gCritMultiplier = 1;

    gBattlescriptCurrInstr++;
}

static void Cmd_damagecalc(void)
{
    u16 sideStatus = gSideStatuses[GET_BATTLER_SIDE(gBattlerTarget)];
    gBattleMoveDamage = CalculateBaseDamage(&gBattleMons[gBattlerAttacker], &gBattleMons[gBattlerTarget], gCurrentMove,
                                            sideStatus, gDynamicBasePower,
                                            gBattleStruct->dynamicMoveType, gBattlerAttacker, gBattlerTarget);
    gBattleMoveDamage = gBattleMoveDamage * gCritMultiplier * gBattleStruct->dmgMultiplier;

    if (gStatuses3[gBattlerAttacker] & STATUS3_CHARGED_UP && gBattleMoves[gCurrentMove].type == TYPE_ELECTRIC)
        gBattleMoveDamage *= 2;
    if (gProtectStructs[gBattlerAttacker].helpingHand)
        gBattleMoveDamage = gBattleMoveDamage * 15 / 10;

    gBattlescriptCurrInstr++;
}

void AI_CalcDmg(u8 attacker, u8 defender)
{
    u16 sideStatus = gSideStatuses[GET_BATTLER_SIDE(defender)];
    gBattleMoveDamage = CalculateBaseDamage(&gBattleMons[attacker], &gBattleMons[defender], gCurrentMove,
                                            sideStatus, gDynamicBasePower,
                                            gBattleStruct->dynamicMoveType, attacker, defender);
    gDynamicBasePower = 0;
    gBattleMoveDamage = gBattleMoveDamage * gCritMultiplier * gBattleStruct->dmgMultiplier;

    if (gStatuses3[attacker] & STATUS3_CHARGED_UP && gBattleMoves[gCurrentMove].type == TYPE_ELECTRIC)
        gBattleMoveDamage *= 2;
    if (gProtectStructs[attacker].helpingHand)
        gBattleMoveDamage = gBattleMoveDamage * 15 / 10;
}

static void ModulateDmgByType(u8 multiplier)
{
    gBattleMoveDamage = gBattleMoveDamage * multiplier / 10;
    if (gBattleMoveDamage == 0 && multiplier != TYPE_MUL_NO_EFFECT)
        gBattleMoveDamage = 1;

    switch (multiplier)
    {
    case TYPE_MUL_NO_EFFECT:
        gMoveResultFlags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
        gMoveResultFlags &= ~MOVE_RESULT_NOT_VERY_EFFECTIVE;
        gMoveResultFlags &= ~MOVE_RESULT_SUPER_EFFECTIVE;
        break;
    case TYPE_MUL_NOT_EFFECTIVE:
        if (gBattleMoves[gCurrentMove].power && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
        {
            if (gMoveResultFlags & MOVE_RESULT_SUPER_EFFECTIVE)
                gMoveResultFlags &= ~MOVE_RESULT_SUPER_EFFECTIVE;
            else
                gMoveResultFlags |= MOVE_RESULT_NOT_VERY_EFFECTIVE;
        }
        break;
    case TYPE_MUL_SUPER_EFFECTIVE:
        if (gBattleMoves[gCurrentMove].power && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
        {
            if (gMoveResultFlags & MOVE_RESULT_NOT_VERY_EFFECTIVE)
                gMoveResultFlags &= ~MOVE_RESULT_NOT_VERY_EFFECTIVE;
            else
                gMoveResultFlags |= MOVE_RESULT_SUPER_EFFECTIVE;
        }
        break;
    }
}

static void Cmd_typecalc(void)
{
    s32 i = 0;
    u8 moveType;
    if (gCurrentMove != MOVE_STRUGGLE)
    {
        if (gBattleStruct->dynamicMoveType)
            moveType = gBattleStruct->dynamicMoveType & DYNAMIC_TYPE_MASK;
        else
            moveType = gBattleMoves[gCurrentMove].type;

        //check stab
        if (gBattleMons[gBattlerAttacker].type1 == moveType || gBattleMons[gBattlerAttacker].type2 == moveType)
        {
            gBattleMoveDamage = gBattleMoveDamage * 15;
            gBattleMoveDamage = gBattleMoveDamage / 10;
        }

        if (gBattleMons[gBattlerTarget].ability == ABILITY_LEVITATE && moveType == TYPE_GROUND)
        {
            gLastUsedAbility = gBattleMons[gBattlerTarget].ability;
            gMoveResultFlags |= (MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE);
            gLastLandedMoves[gBattlerTarget] = MOVE_NONE;
            gLastHitByType[gBattlerTarget] = 0;
            gBattleCommunication[MISS_TYPE] = moveType;
            RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
        }
        else
        {
            while (TYPE_EFFECT_ATK_TYPE(i)!= TYPE_ENDTABLE)
            {
                if (TYPE_EFFECT_ATK_TYPE(i) == TYPE_FORESIGHT)
                {
                    if (gBattleMons[gBattlerTarget].status2 & STATUS2_FORESIGHT)
                        break;
                    i += 3;
                    continue;
                }

                else if (TYPE_EFFECT_ATK_TYPE(i) == moveType)
                {
                    //check type1
                    if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type1)
                        ModulateDmgByType(TYPE_EFFECT_MULTIPLIER(i));
                    //check type2
                    if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type2 &&
                        gBattleMons[gBattlerTarget].type1 != gBattleMons[gBattlerTarget].type2)
                        ModulateDmgByType(TYPE_EFFECT_MULTIPLIER(i));
                }
                i += 3;
            }
        }

        if (gBattleMons[gBattlerTarget].ability == ABILITY_WONDER_GUARD && AttacksThisTurn(gBattlerAttacker, gCurrentMove) == 2
         && (!(gMoveResultFlags & MOVE_RESULT_SUPER_EFFECTIVE) || ((gMoveResultFlags & (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE)) == (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE)))
         && gBattleMoves[gCurrentMove].power)
        {
            gLastUsedAbility = ABILITY_WONDER_GUARD;
            gMoveResultFlags |= MOVE_RESULT_MISSED;
            gLastLandedMoves[gBattlerTarget] = MOVE_NONE;
            gLastHitByType[gBattlerTarget] = 0;
            gBattleCommunication[MISS_TYPE] = B_MSG_AVOIDED_DMG;
            RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
        }
        if (gMoveResultFlags & MOVE_RESULT_DOESNT_AFFECT_FOE)
            gProtectStructs[gBattlerAttacker].targetNotAffected = TRUE;
    }
    gBattlescriptCurrInstr++;
}
static void CheckWonderGuardAndLevitate(void)
{
    u8 flags = 0;
    s32 i = 0;
    u8 moveType;

    if (gCurrentMove == MOVE_STRUGGLE || !gBattleMoves[gCurrentMove].power)
        return;

    GET_MOVE_TYPE(gCurrentMove, moveType);

    if (gBattleMons[gBattlerTarget].ability == ABILITY_LEVITATE && moveType == TYPE_GROUND)
    {
        RecordAbilitySetField6(ABILITY_LEVITATE, moveType);
        return;
    }

    while (TYPE_EFFECT_ATK_TYPE(i) != TYPE_ENDTABLE)
    {
        if (TYPE_EFFECT_ATK_TYPE(i) == TYPE_FORESIGHT)
        {
            if (gBattleMons[gBattlerTarget].status2 & STATUS2_FORESIGHT)
                break;
            i += 3;
            continue;
        }

        if (TYPE_EFFECT_ATK_TYPE(i) == moveType)
        {
            // check no effect
            if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type1 && TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_NO_EFFECT)
            {
                gMoveResultFlags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
                gProtectStructs[gBattlerAttacker].targetNotAffected = TRUE;
            }
            if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type2 &&
                gBattleMons[gBattlerTarget].type1 != gBattleMons[gBattlerTarget].type2 &&
                TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_NO_EFFECT)
            {
                gMoveResultFlags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
                gProtectStructs[gBattlerAttacker].targetNotAffected = TRUE;
            }

            // check super effective
            if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type1 && TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_SUPER_EFFECTIVE)
                flags |= 1;
            if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type2
             && gBattleMons[gBattlerTarget].type1 != gBattleMons[gBattlerTarget].type2
             && TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_SUPER_EFFECTIVE)
                flags |= 1;

            // check not very effective
            if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type1 && TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_NOT_EFFECTIVE)
                flags |= 2;
            if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type2
             && gBattleMons[gBattlerTarget].type1 != gBattleMons[gBattlerTarget].type2
             && TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_NOT_EFFECTIVE)
                flags |= 2;
        }
        i += 3;
    }

    if (gBattleMons[gBattlerTarget].ability == ABILITY_WONDER_GUARD && AttacksThisTurn(gBattlerAttacker, gCurrentMove) == 2)
    {
        if (((flags & 2) || !(flags & 1)) && gBattleMoves[gCurrentMove].power)
        {
            RecordAbilitySetField6(ABILITY_WONDER_GUARD, 3);
        }
    }
}

static void ModulateDmgByType2(u8 multiplier, u16 move, u8* flags) // same as ModulateDmgByType except different arguments
{
    gBattleMoveDamage = gBattleMoveDamage * multiplier / 10;
    if (gBattleMoveDamage == 0 && multiplier != TYPE_MUL_NO_EFFECT)
        gBattleMoveDamage = 1;

    switch (multiplier)
    {
    case TYPE_MUL_NO_EFFECT:
        *flags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
        *flags &= ~MOVE_RESULT_NOT_VERY_EFFECTIVE;
        *flags &= ~MOVE_RESULT_SUPER_EFFECTIVE;
        break;
    case TYPE_MUL_NOT_EFFECTIVE:
        if (gBattleMoves[move].power && !(*flags & MOVE_RESULT_NO_EFFECT))
        {
            if (*flags & MOVE_RESULT_SUPER_EFFECTIVE)
                *flags &= ~MOVE_RESULT_SUPER_EFFECTIVE;
            else
                *flags |= MOVE_RESULT_NOT_VERY_EFFECTIVE;
        }
        break;
    case TYPE_MUL_SUPER_EFFECTIVE:
        if (gBattleMoves[move].power && !(*flags & MOVE_RESULT_NO_EFFECT))
        {
            if (*flags & MOVE_RESULT_NOT_VERY_EFFECTIVE)
                *flags &= ~MOVE_RESULT_NOT_VERY_EFFECTIVE;
            else
                *flags |= MOVE_RESULT_SUPER_EFFECTIVE;
        }
        break;
    }
}

u8 TypeCalc(u16 move, u8 attacker, u8 defender)
{
    s32 i = 0;
    u8 flags = 0;
    u8 moveType;

    if (move == MOVE_STRUGGLE)
        return 0;

    moveType = gBattleMoves[move].type;

    // check stab
    if (IS_BATTLER_OF_TYPE(attacker, moveType))
    {
        gBattleMoveDamage = gBattleMoveDamage * 15;
        gBattleMoveDamage = gBattleMoveDamage / 10;
    }

    if (gBattleMons[defender].ability == ABILITY_LEVITATE && moveType == TYPE_GROUND)
    {
        flags |= (MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE);
    }
    else
    {
        while (TYPE_EFFECT_ATK_TYPE(i)!= TYPE_ENDTABLE)
        {
            if (TYPE_EFFECT_ATK_TYPE(i) == TYPE_FORESIGHT)
            {
                if (gBattleMons[defender].status2 & STATUS2_FORESIGHT)
                    break;
                i += 3;
                continue;
            }

            else if (TYPE_EFFECT_ATK_TYPE(i) == moveType)
            {
                // check type1
                if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[defender].type1)
                    ModulateDmgByType2(TYPE_EFFECT_MULTIPLIER(i), move, &flags);
                // check type2
                if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[defender].type2 &&
                    gBattleMons[gBattlerTarget /* what the christ */].type1 != gBattleMons[defender].type2)
                    ModulateDmgByType2(TYPE_EFFECT_MULTIPLIER(i), move, &flags);
            }
            i += 3;
        }
    }

    if (gBattleMons[defender].ability == ABILITY_WONDER_GUARD && !(flags & MOVE_RESULT_MISSED) &&
        AttacksThisTurn(attacker, move) == 2 &&
        (!(flags & MOVE_RESULT_SUPER_EFFECTIVE) || ((flags & (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE)) == (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE))) &&
        gBattleMoves[move].power)
    {
        flags |= MOVE_RESULT_MISSED;
    }
    return flags;
}

u8 AI_TypeCalc(u16 move, u16 targetSpecies, u8 targetAbility)
{
    s32 i = 0;
    u8 flags = 0;
    u8 type1 = gSpeciesInfo[targetSpecies].type1, type2 = gSpeciesInfo[targetSpecies].type2, moveType;

    if (move == MOVE_STRUGGLE)
        return 0;

    moveType = gBattleMoves[move].type;

    if (targetAbility == ABILITY_LEVITATE && moveType == TYPE_GROUND)
    {
        flags = MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE;
    }
    else
    {
        while (TYPE_EFFECT_ATK_TYPE(i) != TYPE_ENDTABLE)
        {
            if (TYPE_EFFECT_ATK_TYPE(i) == TYPE_FORESIGHT)
            {
                i += 3;
                continue;
            }
            if (TYPE_EFFECT_ATK_TYPE(i) == moveType)
            {
                // check type1
                if (TYPE_EFFECT_DEF_TYPE(i) == type1)
                    ModulateDmgByType2(TYPE_EFFECT_MULTIPLIER(i), move, &flags);
                // check type2
                if (TYPE_EFFECT_DEF_TYPE(i) == type2 && gBattleMons[gBattlerTarget].type1 != type2) //gf you morons, you should check if (type1 != type2)...
                    ModulateDmgByType2(TYPE_EFFECT_MULTIPLIER(i), move, &flags);
            }
            i += 3;
        }
    }
    if (targetAbility == ABILITY_WONDER_GUARD
     && (!(flags & MOVE_RESULT_SUPER_EFFECTIVE) || ((flags & (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE)) == (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE)))
     && gBattleMoves[move].power)
        flags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
    return flags;
}

// Multiplies the damage by a random factor between 85% to 100% inclusive
static inline void ApplyRandomDmgMultiplier(void)
{
    u16 rand = Random();
    u16 randPercent = 100 - (rand % 16);

    if (gBattleMoveDamage != 0)
    {
        gBattleMoveDamage *= randPercent;
        gBattleMoveDamage /= 100;
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
    }
}

static void UNUSED Unused_ApplyRandomDmgMultiplier(void)
{
    ApplyRandomDmgMultiplier();
}

static void Cmd_adjustnormaldamage(void)
{
    u8 holdEffect, param;

    ApplyRandomDmgMultiplier();

    if (gBattleMons[gBattlerTarget].item == ITEM_ENIGMA_BERRY)
    {
        holdEffect = gEnigmaBerries[gBattlerTarget].holdEffect;
        param = gEnigmaBerries[gBattlerTarget].holdEffectParam;
    }
    else
    {
        holdEffect = GetItemHoldEffect(gBattleMons[gBattlerTarget].item);
        param = GetItemHoldEffectParam(gBattleMons[gBattlerTarget].item);
    }

    gPotentialItemEffectBattler = gBattlerTarget;

    if (holdEffect == HOLD_EFFECT_FOCUS_BAND && (Random() % 100) < param)
    {
        RecordItemEffectBattle(gBattlerTarget, holdEffect);
        gSpecialStatuses[gBattlerTarget].focusBanded = TRUE;
    }
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_SUBSTITUTE)
        goto END;
    if (gBattleMoves[gCurrentMove].effect != EFFECT_FALSE_SWIPE && !gProtectStructs[gBattlerTarget].endured
     && !gSpecialStatuses[gBattlerTarget].focusBanded)
        goto END;

    if (gBattleMons[gBattlerTarget].hp > gBattleMoveDamage)
        goto END;

    gBattleMoveDamage = gBattleMons[gBattlerTarget].hp - 1;

    if (gProtectStructs[gBattlerTarget].endured)
    {
        gMoveResultFlags |= MOVE_RESULT_FOE_ENDURED;
    }
    else if (gSpecialStatuses[gBattlerTarget].focusBanded)
    {
        gMoveResultFlags |= MOVE_RESULT_FOE_HUNG_ON;
        gLastUsedItem = gBattleMons[gBattlerTarget].item;
    }

    END:
        gBattlescriptCurrInstr++;
}

static void Cmd_adjustnormaldamage2(void) // The same as 0x7 except it doesn't check for false swipe move effect.
{
    u8 holdEffect, param;

    ApplyRandomDmgMultiplier();

    if (gBattleMons[gBattlerTarget].item == ITEM_ENIGMA_BERRY)
    {
        holdEffect = gEnigmaBerries[gBattlerTarget].holdEffect;
        param = gEnigmaBerries[gBattlerTarget].holdEffectParam;
    }
    else
    {
        holdEffect = GetItemHoldEffect(gBattleMons[gBattlerTarget].item);
        param = GetItemHoldEffectParam(gBattleMons[gBattlerTarget].item);
    }

    gPotentialItemEffectBattler = gBattlerTarget;

    if (holdEffect == HOLD_EFFECT_FOCUS_BAND && (Random() % 100) < param)
    {
        RecordItemEffectBattle(gBattlerTarget, holdEffect);
        gSpecialStatuses[gBattlerTarget].focusBanded = TRUE;
    }
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_SUBSTITUTE)
        goto END;
    if (!gProtectStructs[gBattlerTarget].endured && !gSpecialStatuses[gBattlerTarget].focusBanded)
        goto END;
    if (gBattleMons[gBattlerTarget].hp > gBattleMoveDamage)
        goto END;

    gBattleMoveDamage = gBattleMons[gBattlerTarget].hp - 1;

    if (gProtectStructs[gBattlerTarget].endured)
    {
        gMoveResultFlags |= MOVE_RESULT_FOE_ENDURED;
    }
    else if (gSpecialStatuses[gBattlerTarget].focusBanded)
    {
        gMoveResultFlags |= MOVE_RESULT_FOE_HUNG_ON;
        gLastUsedItem = gBattleMons[gBattlerTarget].item;
    }

    END:
        gBattlescriptCurrInstr++;
}

static void atk09_attackanimation(void)
{
    if (gBattleControllerExecFlags)
        return;

    if ((gHitMarker & HITMARKER_NO_ANIMATIONS) && (gCurrentMove != MOVE_TRANSFORM && gCurrentMove != MOVE_SUBSTITUTE))
    {
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_Pausex20;
        gBattleStruct->animTurn++;
        gBattleStruct->animTargetsHit++;
    }
    else
    {
        if ((gBattleMoves[gCurrentMove].target & MOVE_TARGET_BOTH || gBattleMoves[gCurrentMove].target & MOVE_TARGET_FOES_AND_ALLY || gBattleMoves[gCurrentMove].target & MOVE_TARGET_DEPENDS) && gBattleStruct->animTargetsHit)
        {
            gBattlescriptCurrInstr++;
            return;
        }
        if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
        {
            gActiveBattler = gBattlerAttacker;

            BtlController_EmitMoveAnimation(B_COMM_TO_CONTROLLER, gCurrentMove, gBattleStruct->animTurn, gBattleMovePower, gBattleMoveDamage, gBattleMons[gBattlerAttacker].friendship, &gDisableStructs[gBattlerAttacker]);
            gBattleStruct->animTurn += 1;
            gBattleStruct->animTargetsHit += 1;
            MarkBattlerForControllerExec(gBattlerAttacker);
            gBattlescriptCurrInstr++;
        }
        else
        {
            BattleScriptPush(gBattlescriptCurrInstr + 1);
            gBattlescriptCurrInstr = BattleScript_Pausex20;
        }
    }
}

static void Cmd_waitanimation(void)
{
    if (gBattleControllerExecFlags == 0)
        gBattlescriptCurrInstr++;
}

static void atk0B_healthbarupdate(void)
{
    if (gBattleControllerExecFlags)
        return;

    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
    {
        gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);

        if (gBattleMons[gActiveBattler].status2 & STATUS2_SUBSTITUTE && gDisableStructs[gActiveBattler].substituteHP && !(gHitMarker & HITMARKER_IGNORE_SUBSTITUTE))
        {
            PrepareStringBattle(STRINGID_SUBSTITUTEDAMAGED, gActiveBattler);
        }
        else
        {
            // Emerald
            /*
            s16 healthValue;

            s32 currDmg = gBattleMoveDamage;
            s32 maxPossibleDmgValue = 10000; // not present in R/S, ensures that huge damage values don't change sign

            if (currDmg <= maxPossibleDmgValue)
                healthValue = currDmg;
            else
                healthValue = maxPossibleDmgValue;

            BtlController_EmitHealthBarUpdate(0, healthValue);
            */

            BtlController_EmitHealthBarUpdate(B_COMM_TO_CONTROLLER, gBattleMoveDamage);
            MarkBattlerForControllerExec(gActiveBattler);

            if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER && gBattleMoveDamage > 0)
                gBattleResults.playerMonWasDamaged = TRUE;
        }
    }

    gBattlescriptCurrInstr += 2;
}

static void Cmd_datahpupdate(void)
{
    u32 moveType;

    if (gBattleControllerExecFlags)
        return;

    if (gBattleStruct->dynamicMoveType == 0)
        moveType = gBattleMoves[gCurrentMove].type;
    else if (!(gBattleStruct->dynamicMoveType & F_DYNAMIC_TYPE_IGNORE_PHYSICALITY))
        moveType = gBattleStruct->dynamicMoveType & DYNAMIC_TYPE_MASK;
    else
        moveType = gBattleMoves[gCurrentMove].type;

    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
    {
        gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
        if (gBattleMons[gActiveBattler].status2 & STATUS2_SUBSTITUTE && gDisableStructs[gActiveBattler].substituteHP && !(gHitMarker & HITMARKER_IGNORE_SUBSTITUTE))
        {
            if (gDisableStructs[gActiveBattler].substituteHP >= gBattleMoveDamage)
            {
                if (gSpecialStatuses[gActiveBattler].shellBellDmg == 0)
                    gSpecialStatuses[gActiveBattler].shellBellDmg = gBattleMoveDamage;
                gDisableStructs[gActiveBattler].substituteHP -= gBattleMoveDamage;
                gHpDealt = gBattleMoveDamage;
            }
            else
            {
                if (gSpecialStatuses[gActiveBattler].shellBellDmg == 0)
                    gSpecialStatuses[gActiveBattler].shellBellDmg = gDisableStructs[gActiveBattler].substituteHP;
                gHpDealt = gDisableStructs[gActiveBattler].substituteHP;
                gDisableStructs[gActiveBattler].substituteHP = 0;
            }
            // check substitute fading
            if (gDisableStructs[gActiveBattler].substituteHP == 0)
            {
                gBattlescriptCurrInstr += 2;
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_SubstituteFade;
                return;
            }
        }
        else
        {
            gHitMarker &= ~(HITMARKER_IGNORE_SUBSTITUTE);
            if (gBattleMoveDamage < 0) // hp goes up
            {
                gBattleMons[gActiveBattler].hp -= gBattleMoveDamage;
                if (gBattleMons[gActiveBattler].hp > gBattleMons[gActiveBattler].maxHP)
                    gBattleMons[gActiveBattler].hp = gBattleMons[gActiveBattler].maxHP;

            }
            else // hp goes down
            {
                if (gHitMarker & HITMARKER_IGNORE_BIDE)
                {
                    gHitMarker &= ~(HITMARKER_IGNORE_BIDE);
                }
                else
                {
                    gBideDmg[gActiveBattler] += gBattleMoveDamage;
                    if (gBattlescriptCurrInstr[1] == BS_TARGET)
                        gBideTarget[gActiveBattler] = gBattlerAttacker;
                    else
                        gBideTarget[gActiveBattler] = gBattlerTarget;
                }

                if (gBattleMons[gActiveBattler].hp > gBattleMoveDamage)
                {
                    gBattleMons[gActiveBattler].hp -= gBattleMoveDamage;
                    gHpDealt = gBattleMoveDamage;
                }
                else
                {
                    gHpDealt = gBattleMons[gActiveBattler].hp;
                    gBattleMons[gActiveBattler].hp = 0;
                }

                if (!gSpecialStatuses[gActiveBattler].shellBellDmg && !(gHitMarker & HITMARKER_PASSIVE_HP_UPDATE))
                    gSpecialStatuses[gActiveBattler].shellBellDmg = gHpDealt;

                if (IS_TYPE_PHYSICAL(moveType) && !(gHitMarker & HITMARKER_PASSIVE_HP_UPDATE) && gCurrentMove != MOVE_PAIN_SPLIT)
                {
                    gProtectStructs[gActiveBattler].physicalDmg = gHpDealt;
                    gSpecialStatuses[gActiveBattler].physicalDmg = gHpDealt;
                    if (gBattlescriptCurrInstr[1] == BS_TARGET)
                    {
                        gProtectStructs[gActiveBattler].physicalBattlerId = gBattlerAttacker;
                        gSpecialStatuses[gActiveBattler].physicalBattlerId = gBattlerAttacker;
                    }
                    else
                    {
                        gProtectStructs[gActiveBattler].physicalBattlerId = gBattlerTarget;
                        gSpecialStatuses[gActiveBattler].physicalBattlerId = gBattlerTarget;
                    }
                }
                else if (!IS_TYPE_PHYSICAL(moveType) && !(gHitMarker & HITMARKER_PASSIVE_HP_UPDATE))
                {
                    gProtectStructs[gActiveBattler].specialDmg = gHpDealt;
                    gSpecialStatuses[gActiveBattler].specialDmg = gHpDealt;
                    if (gBattlescriptCurrInstr[1] == BS_TARGET)
                    {
                        gProtectStructs[gActiveBattler].specialBattlerId = gBattlerAttacker;
                        gSpecialStatuses[gActiveBattler].specialBattlerId = gBattlerAttacker;
                    }
                    else
                    {
                        gProtectStructs[gActiveBattler].specialBattlerId = gBattlerTarget;
                        gSpecialStatuses[gActiveBattler].specialBattlerId = gBattlerTarget;
                    }
                }
            }
            gHitMarker &= ~(HITMARKER_PASSIVE_HP_UPDATE);
            BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_HP_BATTLE, 0, sizeof(gBattleMons[gActiveBattler].hp), &gBattleMons[gActiveBattler].hp);
            MarkBattlerForControllerExec(gActiveBattler);
        }
    }
    else
    {
        gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
        if (gSpecialStatuses[gActiveBattler].shellBellDmg == 0)
            gSpecialStatuses[gActiveBattler].shellBellDmg = IGNORE_SHELL_BELL;
    }
    gBattlescriptCurrInstr += 2;
}

static void Cmd_critmessage(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        if (gCritMultiplier == 2 && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
        {
            PrepareStringBattle(STRINGID_CRITICALHIT, gBattlerAttacker);
            gBattleCommunication[MSG_DISPLAY] = TRUE;
        }
        gBattlescriptCurrInstr++;
    }
}

static void Cmd_effectivenesssound(void)
{
    if (gBattleControllerExecFlags)
        return;

    gActiveBattler = gBattlerTarget;
    if (!(gMoveResultFlags & MOVE_RESULT_MISSED))
    {
        switch (gMoveResultFlags & (u8)(~(MOVE_RESULT_MISSED)))
        {
        case MOVE_RESULT_SUPER_EFFECTIVE:
            BtlController_EmitPlaySE(B_COMM_TO_CONTROLLER, SE_SUPER_EFFECTIVE);
            MarkBattlerForControllerExec(gActiveBattler);
            break;
        case MOVE_RESULT_NOT_VERY_EFFECTIVE:
            BtlController_EmitPlaySE(B_COMM_TO_CONTROLLER, SE_NOT_EFFECTIVE);
            MarkBattlerForControllerExec(gActiveBattler);
            break;
        case MOVE_RESULT_DOESNT_AFFECT_FOE:
        case MOVE_RESULT_FAILED:
            // no sound
            break;
        case MOVE_RESULT_FOE_ENDURED:
        case MOVE_RESULT_ONE_HIT_KO:
        case MOVE_RESULT_FOE_HUNG_ON:
        default:
            if (gMoveResultFlags & MOVE_RESULT_SUPER_EFFECTIVE)
            {
                BtlController_EmitPlaySE(B_COMM_TO_CONTROLLER, SE_SUPER_EFFECTIVE);
                MarkBattlerForControllerExec(gActiveBattler);
            }
            else if (gMoveResultFlags & MOVE_RESULT_NOT_VERY_EFFECTIVE)
            {
                BtlController_EmitPlaySE(B_COMM_TO_CONTROLLER, SE_NOT_EFFECTIVE);
                MarkBattlerForControllerExec(gActiveBattler);
            }
            else if (!(gMoveResultFlags & (MOVE_RESULT_DOESNT_AFFECT_FOE | MOVE_RESULT_FAILED)))
            {
                BtlController_EmitPlaySE(B_COMM_TO_CONTROLLER, SE_EFFECTIVE);
                MarkBattlerForControllerExec(gActiveBattler);
            }
            break;
        }
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_resultmessage(void)
{
    u32 stringId = 0;

    if (gBattleControllerExecFlags)
        return;

    if (gMoveResultFlags & MOVE_RESULT_MISSED && (!(gMoveResultFlags & MOVE_RESULT_DOESNT_AFFECT_FOE) || gBattleCommunication[MISS_TYPE] > B_MSG_AVOIDED_ATK))
    {
        stringId = gMissStringIds[gBattleCommunication[MISS_TYPE]];
        gBattleCommunication[MSG_DISPLAY] = TRUE;
    }
    else
    {
        gBattleCommunication[MSG_DISPLAY] = TRUE;
        switch (gMoveResultFlags & (u8)(~(MOVE_RESULT_MISSED)))
        {
        case MOVE_RESULT_SUPER_EFFECTIVE:
            stringId = STRINGID_SUPEREFFECTIVE;
            break;
        case MOVE_RESULT_NOT_VERY_EFFECTIVE:
            stringId = STRINGID_NOTVERYEFFECTIVE;
            break;
        case MOVE_RESULT_ONE_HIT_KO:
            stringId = STRINGID_ONEHITKO;
            break;
        case MOVE_RESULT_FOE_ENDURED:
            stringId = STRINGID_PKMNENDUREDHIT;
            break;
        case MOVE_RESULT_FAILED:
            stringId = STRINGID_BUTITFAILED;
            break;
        case MOVE_RESULT_DOESNT_AFFECT_FOE:
            stringId = STRINGID_ITDOESNTAFFECT;
            break;
        case MOVE_RESULT_FOE_HUNG_ON:
            gLastUsedItem = gBattleMons[gBattlerTarget].item;
            gPotentialItemEffectBattler = gBattlerTarget;
            gMoveResultFlags &= ~(MOVE_RESULT_FOE_ENDURED | MOVE_RESULT_FOE_HUNG_ON);
            BattleScriptPushCursor();
            gBattlescriptCurrInstr = BattleScript_FocusBandActivates;
            return;
        default:
            if (gMoveResultFlags & MOVE_RESULT_DOESNT_AFFECT_FOE)
            {
                stringId = STRINGID_ITDOESNTAFFECT;
            }
            else if (gMoveResultFlags & MOVE_RESULT_ONE_HIT_KO)
            {
                gMoveResultFlags &= ~(MOVE_RESULT_ONE_HIT_KO);
                gMoveResultFlags &= ~(MOVE_RESULT_SUPER_EFFECTIVE);
                gMoveResultFlags &= ~(MOVE_RESULT_NOT_VERY_EFFECTIVE);
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_OneHitKOMsg;
                return;
            }
            else if (gMoveResultFlags & MOVE_RESULT_FOE_ENDURED)
            {
                gMoveResultFlags &= ~(MOVE_RESULT_FOE_ENDURED | MOVE_RESULT_FOE_HUNG_ON);
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_EnduredMsg;
                return;
            }
            else if (gMoveResultFlags & MOVE_RESULT_FOE_HUNG_ON)
            {
                gLastUsedItem = gBattleMons[gBattlerTarget].item;
                gPotentialItemEffectBattler = gBattlerTarget;
                gMoveResultFlags &= ~(MOVE_RESULT_FOE_ENDURED | MOVE_RESULT_FOE_HUNG_ON);
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_FocusBandActivates;
                return;
            }
            else if (gMoveResultFlags & MOVE_RESULT_FAILED)
            {
                stringId = STRINGID_BUTITFAILED;
            }
            else
            {
                gBattleCommunication[MSG_DISPLAY] = FALSE;
            }
        }
    }

    if (stringId)
        PrepareStringBattle(stringId, gBattlerAttacker);

    gBattlescriptCurrInstr++;
}

static void Cmd_printstring(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        u16 var = T2_READ_16(gBattlescriptCurrInstr + 1);
        PrepareStringBattle(var, gBattlerAttacker);
        gBattlescriptCurrInstr += 3;
        gBattleCommunication[MSG_DISPLAY] = TRUE;
    }
}

static void Cmd_printselectionstring(void)
{
    gActiveBattler = gBattlerAttacker;

    BtlController_EmitPrintSelectionString(B_COMM_TO_CONTROLLER, T2_READ_16(gBattlescriptCurrInstr + 1));
    MarkBattlerForControllerExec(gActiveBattler);

    gBattlescriptCurrInstr += 3;
    gBattleCommunication[MSG_DISPLAY] = TRUE;
}

static void Cmd_waitmessage(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        if (!gBattleCommunication[MSG_DISPLAY])
        {
            gBattlescriptCurrInstr += 3;
        }
        else
        {
            u16 toWait = T2_READ_16(gBattlescriptCurrInstr + 1);
            if (++gPauseCounterBattle >= toWait)
            {
                gPauseCounterBattle = 0;
                gBattlescriptCurrInstr += 3;
                gBattleCommunication[MSG_DISPLAY] = FALSE;
            }
        }
    }
}

static void Cmd_printfromtable(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        u16 *ptr = (u16 *)T1_READ_PTR(gBattlescriptCurrInstr + 1);
        ptr += gBattleCommunication[MULTISTRING_CHOOSER];
        PrepareStringBattle(*(u16*)ptr, gBattlerAttacker);
        gBattlescriptCurrInstr += 5;
        gBattleCommunication[MSG_DISPLAY] = TRUE;
    }
}

static void Cmd_printselectionstringfromtable(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        u16 *ptr = (u16 *)T1_READ_PTR(gBattlescriptCurrInstr + 1); // FIXME
        ptr += gBattleCommunication[MULTISTRING_CHOOSER];
        gActiveBattler = gBattlerAttacker;
        BtlController_EmitPrintSelectionString(B_COMM_TO_CONTROLLER, *(u16*)ptr);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 5;
        gBattleCommunication[MSG_DISPLAY] = TRUE;
    }
}

u8 GetBattlerTurnOrderNum(u8 battler)
{
    s32 i;
    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattlerByTurnOrder[i] == battler)
            break;
    }
    return i;
}

void SetMoveEffect(bool8 primary, u8 certain)
{
    bool32 statusChanged = FALSE;
    u8 affectsUser = 0;
    bool32 noSunCanFreeze = TRUE;

    if (gBattleCommunication[MOVE_EFFECT_BYTE] & MOVE_EFFECT_AFFECTS_USER)
    {
        gEffectBattler = gBattlerAttacker; // battlerId that effects get applied on
        gBattleCommunication[MOVE_EFFECT_BYTE] &= ~(MOVE_EFFECT_AFFECTS_USER);
        affectsUser = MOVE_EFFECT_AFFECTS_USER;
        gBattleStruct->battler = gBattlerTarget; // theoretically the attacker
    }
    else
    {
        gEffectBattler = gBattlerTarget;
        gBattleStruct->battler = gBattlerAttacker;
    }

    if (gBattleMons[gEffectBattler].ability == ABILITY_SHIELD_DUST && !(gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
        && !primary && gBattleCommunication[MOVE_EFFECT_BYTE] <= MOVE_EFFECT_TRI_ATTACK)
    {
        gBattlescriptCurrInstr++;
        return;
    }

    if (gSideStatuses[GET_BATTLER_SIDE(gEffectBattler)] & SIDE_STATUS_SAFEGUARD && !(gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
    && !primary && gBattleCommunication[MOVE_EFFECT_BYTE] <= MOVE_EFFECT_CONFUSION)
    {
        gBattlescriptCurrInstr++;
        return;
    }

    if (gBattleMons[gEffectBattler].hp == 0
    && gBattleCommunication[MOVE_EFFECT_BYTE] != MOVE_EFFECT_PAYDAY
    && gBattleCommunication[MOVE_EFFECT_BYTE] != MOVE_EFFECT_STEAL_ITEM)
    {
        gBattlescriptCurrInstr++;
        return;
    }

    if (gBattleMons[gEffectBattler].status2 & STATUS2_SUBSTITUTE && affectsUser != MOVE_EFFECT_AFFECTS_USER)
    {
        gBattlescriptCurrInstr++;
        return;
    }

    if (gBattleCommunication[MOVE_EFFECT_BYTE] <= PRIMARY_STATUS_MOVE_EFFECT)
    {
        switch (sStatusFlagsForMoveEffects[gBattleCommunication[MOVE_EFFECT_BYTE]])
        {
        case STATUS1_SLEEP:
            // check active uproar
            if (gBattleMons[gEffectBattler].ability != ABILITY_SOUNDPROOF)
            {
                for (gActiveBattler = 0;
                     gActiveBattler < gBattlersCount && !(gBattleMons[gActiveBattler].status2 & STATUS2_UPROAR);
                     gActiveBattler++)
                {}
            }
            else
                gActiveBattler = gBattlersCount;

            if (gBattleMons[gEffectBattler].status1)
                break;
            if (gActiveBattler != gBattlersCount)
                break;
            if (gBattleMons[gEffectBattler].ability == ABILITY_VITAL_SPIRIT)
                break;
            if (gBattleMons[gEffectBattler].ability == ABILITY_INSOMNIA)
                break;

            CancelMultiTurnMoves(gEffectBattler);
            statusChanged = TRUE;
            break;
        case STATUS1_POISON:
            if (gBattleMons[gEffectBattler].ability == ABILITY_IMMUNITY
                && (primary == TRUE || certain == MOVE_EFFECT_CERTAIN))
            {
                gLastUsedAbility = ABILITY_IMMUNITY;
                RecordAbilityBattle(gEffectBattler, ABILITY_IMMUNITY);

                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_PSNPrevention;

                if (gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABILITY_PREVENTS_ABILITY_STATUS;
                    gHitMarker &= ~(HITMARKER_STATUS_ABILITY_EFFECT);
                }
                else
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABILITY_PREVENTS_MOVE_STATUS;
                }
                return;
            }
            if ((IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_POISON) || IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_STEEL))
                && (gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
                && (primary == TRUE || certain == MOVE_EFFECT_CERTAIN))
            {
                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_PSNPrevention;

                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STATUS_HAD_NO_EFFECT;
                return;
            }
            if (IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_POISON))
                break;
            if (IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_STEEL))
                break;
            if (gBattleMons[gEffectBattler].status1)
                break;
            if (gBattleMons[gEffectBattler].ability == ABILITY_IMMUNITY)
                break;

            statusChanged = TRUE;
            break;
        case STATUS1_BURN:
            if (gBattleMons[gEffectBattler].ability == ABILITY_WATER_VEIL
                && (primary == TRUE || certain == MOVE_EFFECT_CERTAIN))
            {
                gLastUsedAbility = ABILITY_WATER_VEIL;
                RecordAbilityBattle(gEffectBattler, ABILITY_WATER_VEIL);

                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_BRNPrevention;
                if (gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABILITY_PREVENTS_ABILITY_STATUS;
                    gHitMarker &= ~(HITMARKER_STATUS_ABILITY_EFFECT);
                }
                else
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABILITY_PREVENTS_MOVE_STATUS;
                }
                return;
            }
            if (IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_FIRE)
                && (gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
                && (primary == TRUE || certain == MOVE_EFFECT_CERTAIN))
            {
                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_BRNPrevention;

                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STATUS_HAD_NO_EFFECT;
                return;
            }
            if (IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_FIRE))
                break;
            if (gBattleMons[gEffectBattler].ability == ABILITY_WATER_VEIL)
                break;
            if (gBattleMons[gEffectBattler].status1)
                break;

            statusChanged = TRUE;
            break;
        case STATUS1_FREEZE:
            if (WEATHER_HAS_EFFECT && gBattleWeather & B_WEATHER_SUN)
                noSunCanFreeze = FALSE;
            if (IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_ICE))
                break;
            if (gBattleMons[gEffectBattler].status1)
                break;
            if (noSunCanFreeze == FALSE)
                break;
            if (gBattleMons[gEffectBattler].ability == ABILITY_MAGMA_ARMOR)
                break;

            CancelMultiTurnMoves(gEffectBattler);
            statusChanged = TRUE;
            break;
        case STATUS1_PARALYSIS:
            if (gBattleMons[gEffectBattler].ability == ABILITY_LIMBER)
            {
                if (primary == TRUE || certain == MOVE_EFFECT_CERTAIN)
                {
                    gLastUsedAbility = ABILITY_LIMBER;
                    RecordAbilityBattle(gEffectBattler, ABILITY_LIMBER);

                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = BattleScript_PRLZPrevention;

                    if (gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
                    {
                        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABILITY_PREVENTS_ABILITY_STATUS;
                        gHitMarker &= ~(HITMARKER_STATUS_ABILITY_EFFECT);
                    }
                    else
                    {
                        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABILITY_PREVENTS_MOVE_STATUS;
                    }
                    return;
                }
                else
                    break;
            }
            if (gBattleMons[gEffectBattler].status1)
                break;

            statusChanged = TRUE;
            break;
        case STATUS1_TOXIC_POISON:
            if (gBattleMons[gEffectBattler].ability == ABILITY_IMMUNITY && (primary == TRUE || certain == MOVE_EFFECT_CERTAIN))
            {
                gLastUsedAbility = ABILITY_IMMUNITY;
                RecordAbilityBattle(gEffectBattler, ABILITY_IMMUNITY);

                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_PSNPrevention;

                if (gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABILITY_PREVENTS_ABILITY_STATUS;
                    gHitMarker &= ~(HITMARKER_STATUS_ABILITY_EFFECT);
                }
                else
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABILITY_PREVENTS_MOVE_STATUS;
                }
                return;
            }
            if ((IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_POISON) || IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_STEEL))
                && (gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
                && (primary == TRUE || certain == MOVE_EFFECT_CERTAIN))
            {
                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_PSNPrevention;

                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STATUS_HAD_NO_EFFECT;
                return;
            }
            if (gBattleMons[gEffectBattler].status1)
                break;
            if (!IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_POISON) && !IS_BATTLER_OF_TYPE(gEffectBattler, TYPE_STEEL))
            {
                if (gBattleMons[gEffectBattler].ability == ABILITY_IMMUNITY)
                    break;

                // It's redundant, because at this point we know the status1 value is 0.
                gBattleMons[gEffectBattler].status1 &= ~(STATUS1_TOXIC_POISON);
                gBattleMons[gEffectBattler].status1 &= ~(STATUS1_POISON);
                statusChanged = TRUE;
                break;
            }
            else
            {
                gMoveResultFlags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
            }
            break;
        }
        if (statusChanged == TRUE)
        {
            BattleScriptPush(gBattlescriptCurrInstr + 1);

            if (sStatusFlagsForMoveEffects[gBattleCommunication[MOVE_EFFECT_BYTE]] == STATUS1_SLEEP)
                gBattleMons[gEffectBattler].status1 |= STATUS1_SLEEP_TURN((Random() & 3) + 2); // 2-5 turns
            else
                gBattleMons[gEffectBattler].status1 |= sStatusFlagsForMoveEffects[gBattleCommunication[MOVE_EFFECT_BYTE]];

            gBattlescriptCurrInstr = sMoveEffectBS_Ptrs[gBattleCommunication[MOVE_EFFECT_BYTE]];

            gActiveBattler = gEffectBattler;
            BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, 0, sizeof(gBattleMons[gEffectBattler].status1), &gBattleMons[gEffectBattler].status1);
            MarkBattlerForControllerExec(gActiveBattler);

            if (gHitMarker & HITMARKER_STATUS_ABILITY_EFFECT)
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STATUSED_BY_ABILITY;
                gHitMarker &= ~(HITMARKER_STATUS_ABILITY_EFFECT);
            }
            else
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STATUSED;
            }

            // for synchronize

            if (gBattleCommunication[MOVE_EFFECT_BYTE] == MOVE_EFFECT_POISON
                || gBattleCommunication[MOVE_EFFECT_BYTE] == MOVE_EFFECT_TOXIC
                || gBattleCommunication[MOVE_EFFECT_BYTE] == MOVE_EFFECT_PARALYSIS
                || gBattleCommunication[MOVE_EFFECT_BYTE] == MOVE_EFFECT_BURN)
            {
                gBattleStruct->synchronizeMoveEffect = gBattleCommunication[MOVE_EFFECT_BYTE];
                gHitMarker |= HITMARKER_SYNCHRONIZE_EFFECT;
            }
            return;
        }
        else if (statusChanged == FALSE)
        {
//            gBattleCommunication[MOVE_EFFECT_BYTE] = 0;
            gBattlescriptCurrInstr++;
            return;
        }
        return;
    }
    else
    {
        if (gBattleMons[gEffectBattler].status2 & sStatusFlagsForMoveEffects[gBattleCommunication[MOVE_EFFECT_BYTE]])
        {
            gBattlescriptCurrInstr++;
        }
        else
        {
            u8 side;
            switch (gBattleCommunication[MOVE_EFFECT_BYTE])
            {
            case MOVE_EFFECT_CONFUSION:
                if (gBattleMons[gEffectBattler].ability == ABILITY_OWN_TEMPO
                    || gBattleMons[gEffectBattler].status2 & STATUS2_CONFUSION)
                {
                    gBattlescriptCurrInstr++;
                }
                else
                {
                    gBattleMons[gEffectBattler].status2 |= STATUS2_CONFUSION_TURN(((Random()) % 4) + 2); // 2-5 turns

                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = sMoveEffectBS_Ptrs[gBattleCommunication[MOVE_EFFECT_BYTE]];
                }
                break;
            case MOVE_EFFECT_FLINCH:
                if (gBattleMons[gEffectBattler].ability == ABILITY_INNER_FOCUS)
                {
                    if (primary == TRUE || certain == MOVE_EFFECT_CERTAIN)
                    {
                        gLastUsedAbility = ABILITY_INNER_FOCUS;
                        RecordAbilityBattle(gEffectBattler, ABILITY_INNER_FOCUS);
                        gBattlescriptCurrInstr = BattleScript_FlinchPrevention;
                    }
                    else
                    {
                        gBattlescriptCurrInstr++;
                    }
                }
                else
                {
                    if (GetBattlerTurnOrderNum(gEffectBattler) > gCurrentTurnActionNumber)
                        gBattleMons[gEffectBattler].status2 |= sStatusFlagsForMoveEffects[gBattleCommunication[MOVE_EFFECT_BYTE]];
                    gBattlescriptCurrInstr++;
                }
                break;
            case MOVE_EFFECT_UPROAR:
                if (!(gBattleMons[gEffectBattler].status2 & STATUS2_UPROAR))
                {

                    gBattleMons[gEffectBattler].status2 |= STATUS2_MULTIPLETURNS;
                    gLockedMoves[gEffectBattler] = gCurrentMove;
                    gBattleMons[gEffectBattler].status2 |= STATUS2_UPROAR_TURN((Random() & 3) + 2); // 2-5 turns

                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = sMoveEffectBS_Ptrs[gBattleCommunication[MOVE_EFFECT_BYTE]];
                }
                else
                {
                    gBattlescriptCurrInstr++;
                }
                break;
            case MOVE_EFFECT_PAYDAY:
                if (GET_BATTLER_SIDE(gBattlerAttacker) == B_SIDE_PLAYER)
                {
                    u16 payday = gPaydayMoney;
                    gPaydayMoney += (gBattleMons[gBattlerAttacker].level * 5);
                    if (payday > gPaydayMoney)
                        gPaydayMoney = USHRT_MAX;
                }
                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = sMoveEffectBS_Ptrs[gBattleCommunication[MOVE_EFFECT_BYTE]];
                break;
            case MOVE_EFFECT_TRI_ATTACK:
                if (gBattleMons[gEffectBattler].status1)
                {
                    gBattlescriptCurrInstr++;
                }
                else
                {
                    gBattleCommunication[MOVE_EFFECT_BYTE] = Random() % (MOVE_EFFECT_PARALYSIS - MOVE_EFFECT_BURN + 1) + MOVE_EFFECT_BURN;
                    SetMoveEffect(FALSE, 0); // Recursive
                }
                break;
            case MOVE_EFFECT_CHARGING:
                gBattleMons[gEffectBattler].status2 |= STATUS2_MULTIPLETURNS;
                gLockedMoves[gEffectBattler] = gCurrentMove;
                gProtectStructs[gEffectBattler].chargingTurn = TRUE;
                gBattlescriptCurrInstr++;
                break;
            case MOVE_EFFECT_WRAP:
                if (gBattleMons[gEffectBattler].status2 & STATUS2_WRAPPED)
                {
                    gBattlescriptCurrInstr++;
                }
                else
                {
                    gBattleMons[gEffectBattler].status2 |= STATUS2_WRAPPED_TURN((Random() & 3) + 3); // 3-6 turns

                    gSharedMem[BSTRUCT_OFF(wrappedMove) + gEffectBattler * 2 + 0] = gCurrentMove;
                    gSharedMem[BSTRUCT_OFF(wrappedMove) + gEffectBattler * 2 + 1] = gCurrentMove >> 8;
                    gSharedMem[BSTRUCT_OFF(wrappedBy) + gEffectBattler] = gBattlerAttacker;

                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = sMoveEffectBS_Ptrs[gBattleCommunication[MOVE_EFFECT_BYTE]];

                    for (gBattleCommunication[MULTISTRING_CHOOSER] = 0; ; gBattleCommunication[MULTISTRING_CHOOSER]++)
                    {
                        if (gBattleCommunication[MULTISTRING_CHOOSER] >= NUM_TRAPPING_MOVES - 1)
                            break;
                        if (gTrappingMoves[gBattleCommunication[MULTISTRING_CHOOSER]] == gCurrentMove)
                            break;
                    }
                }
                break;
            case MOVE_EFFECT_RECOIL_25: // 25% recoil
                gBattleMoveDamage = (gHpDealt) / 4;
                if (gBattleMoveDamage == 0)
                    gBattleMoveDamage = 1;

                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = sMoveEffectBS_Ptrs[gBattleCommunication[MOVE_EFFECT_BYTE]];
                break;
            case MOVE_EFFECT_ATK_PLUS_1:
            case MOVE_EFFECT_DEF_PLUS_1:
            case MOVE_EFFECT_SPD_PLUS_1:
            case MOVE_EFFECT_SP_ATK_PLUS_1:
            case MOVE_EFFECT_SP_DEF_PLUS_1:
            case MOVE_EFFECT_ACC_PLUS_1:
            case MOVE_EFFECT_EVS_PLUS_1:
                if (ChangeStatBuffs(SET_STAT_BUFF_VALUE(1),
                                    gBattleCommunication[MOVE_EFFECT_BYTE] - MOVE_EFFECT_ATK_PLUS_1 + STAT_ATK,
                                    affectsUser, 0))
                {
                    gBattlescriptCurrInstr++;
                }
                else
                {
                    gBattleStruct->animArg1 = gBattleCommunication[MOVE_EFFECT_BYTE] & ~(MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN);
                    gBattleStruct->animArg2 = 0;
                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = BattleScript_StatUp;
                }
                break;
            case MOVE_EFFECT_ATK_MINUS_1:
            case MOVE_EFFECT_DEF_MINUS_1:
            case MOVE_EFFECT_SPD_MINUS_1:
            case MOVE_EFFECT_SP_ATK_MINUS_1:
            case MOVE_EFFECT_SP_DEF_MINUS_1:
            case MOVE_EFFECT_ACC_MINUS_1:
            case MOVE_EFFECT_EVS_MINUS_1:
                if (ChangeStatBuffs(SET_STAT_BUFF_VALUE(1) | STAT_BUFF_NEGATIVE,
                                    gBattleCommunication[MOVE_EFFECT_BYTE] - MOVE_EFFECT_ATK_MINUS_1 + STAT_ATK,
                                    affectsUser, 0))
                {
                    gBattlescriptCurrInstr++;
                }
                else
                {
                    gBattleStruct->animArg1 = gBattleCommunication[MOVE_EFFECT_BYTE] & ~(MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN);
                    gBattleStruct->animArg2 = 0;
                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = BattleScript_StatDown;
                }
                break;
            case MOVE_EFFECT_ATK_PLUS_2:
            case MOVE_EFFECT_DEF_PLUS_2:
            case MOVE_EFFECT_SPD_PLUS_2:
            case MOVE_EFFECT_SP_ATK_PLUS_2:
            case MOVE_EFFECT_SP_DEF_PLUS_2:
            case MOVE_EFFECT_ACC_PLUS_2:
            case MOVE_EFFECT_EVS_PLUS_2:
                if (ChangeStatBuffs(SET_STAT_BUFF_VALUE(2),
                                    gBattleCommunication[MOVE_EFFECT_BYTE] - MOVE_EFFECT_ATK_PLUS_2 + STAT_ATK,
                                    affectsUser, 0))
                {
                    gBattlescriptCurrInstr++;
                }
                else
                {
                    gBattleStruct->animArg1 = gBattleCommunication[MOVE_EFFECT_BYTE] & ~(MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN);
                    gBattleStruct->animArg2 = 0;
                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = BattleScript_StatUp;
                }
                break;
            case MOVE_EFFECT_ATK_MINUS_2:
            case MOVE_EFFECT_DEF_MINUS_2:
            case MOVE_EFFECT_SPD_MINUS_2:
            case MOVE_EFFECT_SP_ATK_MINUS_2:
            case MOVE_EFFECT_SP_DEF_MINUS_2:
            case MOVE_EFFECT_ACC_MINUS_2:
            case MOVE_EFFECT_EVS_MINUS_2:
                if (ChangeStatBuffs(SET_STAT_BUFF_VALUE(2) | STAT_BUFF_NEGATIVE,
                                    gBattleCommunication[MOVE_EFFECT_BYTE] - MOVE_EFFECT_ATK_MINUS_2 + STAT_ATK,
                                    affectsUser, 0))
                {
                    gBattlescriptCurrInstr++;
                }
                else
                {
                    gBattleStruct->animArg1 = gBattleCommunication[MOVE_EFFECT_BYTE] & ~(MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN);
                    gBattleStruct->animArg2 = 0;
                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = BattleScript_StatDown;
                }
                break;
            case MOVE_EFFECT_RECHARGE:
                gBattleMons[gEffectBattler].status2 |= STATUS2_RECHARGE;
                gDisableStructs[gEffectBattler].rechargeTimer = 2;
                gLockedMoves[gEffectBattler] = gCurrentMove;
                gBattlescriptCurrInstr++;
                break;
            case MOVE_EFFECT_RAGE:
                gBattleMons[gBattlerAttacker].status2 |= STATUS2_RAGE;
                gBattlescriptCurrInstr++;
                break;
            case MOVE_EFFECT_STEAL_ITEM:
            {
                side = GetBattlerSide(gBattlerAttacker);
                if (GetBattlerSide(gBattlerAttacker) == B_SIDE_OPPONENT
                    && !(gBattleTypeFlags &
                         (BATTLE_TYPE_EREADER_TRAINER
                          | BATTLE_TYPE_BATTLE_TOWER
                          | BATTLE_TYPE_LINK)
                        || gTrainerBattleOpponent_A == TRAINER_SECRET_BASE))
                {
                    gBattlescriptCurrInstr++;
                }
                else if (!(gBattleTypeFlags &
                           (BATTLE_TYPE_EREADER_TRAINER
                            | BATTLE_TYPE_BATTLE_TOWER
                            | BATTLE_TYPE_LINK)
                           || gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
                         && (gWishFutureKnock.knockedOffMons[side] & gBitTable[gBattlerPartyIndexes[gBattlerAttacker]]))
                {
                    gBattlescriptCurrInstr++;
                }
                else if (gBattleMons[gBattlerTarget].item
                         && gBattleMons[gBattlerTarget].ability == ABILITY_STICKY_HOLD)
                {
                    gBattlescriptCurrInstr = BattleScript_StickyHoldActivates;

                    gLastUsedAbility = gBattleMons[gBattlerTarget].ability;
                    RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
                }
                else if (gBattleMons[gBattlerAttacker].item == ITEM_NONE
                         && gBattleMons[gBattlerTarget].item != ITEM_ENIGMA_BERRY
                         && gBattleMons[gBattlerTarget].item != ITEM_NONE)
                {
                    u16* changedItem = &gBattleStruct->changedItems[gBattlerAttacker];
                    gLastUsedItem = *changedItem = gBattleMons[gBattlerTarget].item;
                    gBattleMons[gBattlerTarget].item = ITEM_NONE;

                    gActiveBattler = gBattlerAttacker;
                    BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE, 0, sizeof(gLastUsedItem), &gLastUsedItem);
                    MarkBattlerForControllerExec(gBattlerAttacker);

                    gActiveBattler = gBattlerTarget;
                    BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE, 0, sizeof(gBattleMons[gBattlerTarget].item), &gBattleMons[gBattlerTarget].item);
                    MarkBattlerForControllerExec(gBattlerTarget);

                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = BattleScript_ItemSteal;

                    gSharedMem[BSTRUCT_OFF(choicedMove) + (2 * gBattlerTarget) + 0] = 0;
                    gSharedMem[BSTRUCT_OFF(choicedMove) + (2 * gBattlerTarget) + 1] = 0;
                }
                else
                {
                    gBattlescriptCurrInstr++;
                }

            }
                break;
            case MOVE_EFFECT_PREVENT_ESCAPE:
                gBattleMons[gBattlerTarget].status2 |= STATUS2_ESCAPE_PREVENTION;
                gDisableStructs[gBattlerTarget].battlerPreventingEscape = gBattlerAttacker;
                gBattlescriptCurrInstr++;
                break;
            case MOVE_EFFECT_NIGHTMARE:
                gBattleMons[gBattlerTarget].status2 |= STATUS2_NIGHTMARE;
                gBattlescriptCurrInstr++;
                break;
            case MOVE_EFFECT_ALL_STATS_UP:
                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_AllStatsUp;
                break;
            case MOVE_EFFECT_RAPIDSPIN:
                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_RapidSpinAway;
                break;
            case MOVE_EFFECT_REMOVE_PARALYSIS: // Smelling salts
                if ((gBattleMons[gBattlerTarget].status1 & STATUS1_PARALYSIS))
                {
                    gBattleMons[gBattlerTarget].status1 &= ~(STATUS1_PARALYSIS);

                    gActiveBattler = gBattlerTarget;
                    BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, 0, sizeof(gBattleMons[gActiveBattler].status1), &gBattleMons[gActiveBattler].status1);
                    MarkBattlerForControllerExec(gActiveBattler);

                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = BattleScript_TargetPRLZHeal;
                }
                else
                {
                    gBattlescriptCurrInstr++;
                }
                break;
            case MOVE_EFFECT_ATK_DEF_DOWN: // SuperPower
                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_AtkDefDown;
                break;
            case MOVE_EFFECT_RECOIL_33: // Double Edge
                gBattleMoveDamage = gHpDealt / 3;
                if (gBattleMoveDamage == 0)
                    gBattleMoveDamage = 1;

                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = sMoveEffectBS_Ptrs[gBattleCommunication[MOVE_EFFECT_BYTE]];
                break;
            case MOVE_EFFECT_THRASH:
                if (!(gBattleMons[gEffectBattler].status2 & STATUS2_LOCK_CONFUSE))
                {
                    gBattleMons[gEffectBattler].status2 |= STATUS2_MULTIPLETURNS;
                    gLockedMoves[gEffectBattler] = gCurrentMove;
                    gBattleMons[gEffectBattler].status2 |= STATUS2_LOCK_CONFUSE_TURN((Random() & 1) + 2); // thrash for 2-3 turns
                }
                else
                {
                    gBattlescriptCurrInstr++;
                }
                break;
            case MOVE_EFFECT_KNOCK_OFF:
                if (gBattleMons[gEffectBattler].ability == ABILITY_STICKY_HOLD)
                {
                    if (gBattleMons[gEffectBattler].item == ITEM_NONE)
                    {
                        gBattlescriptCurrInstr++;
                    }
                    else
                    {
                        gLastUsedAbility = ABILITY_STICKY_HOLD;
                        gBattlescriptCurrInstr = BattleScript_StickyHoldActivates;
                        RecordAbilityBattle(gEffectBattler, ABILITY_STICKY_HOLD);
                    }
                    break;
                }
                if (gBattleMons[gEffectBattler].item)
                {
                    side = GetBattlerSide(gEffectBattler);

                    gLastUsedItem = gBattleMons[gEffectBattler].item;
                    gBattleMons[gEffectBattler].item = ITEM_NONE;
                    gWishFutureKnock.knockedOffMons[side] |= gBitTable[gBattlerPartyIndexes[gEffectBattler]];

                    BattleScriptPush(gBattlescriptCurrInstr + 1);
                    gBattlescriptCurrInstr = BattleScript_KnockedOff;

                    gSharedMem[BSTRUCT_OFF(choicedMove) + (2 * gEffectBattler) + 0] = 0;
                    gSharedMem[BSTRUCT_OFF(choicedMove) + (2 * gEffectBattler) + 1] = 0;
                }
                else
                {
                    gBattlescriptCurrInstr++;
                }
                break;
            case MOVE_EFFECT_SP_ATK_TWO_DOWN: // Overheat
                BattleScriptPush(gBattlescriptCurrInstr + 1);
                gBattlescriptCurrInstr = BattleScript_SAtkDown2;
                break;
            }
        }
    }

//    gBattleCommunication[MOVE_EFFECT_BYTE] = 0;
}

static void atk15_seteffectwithchance(void)
{
    u32 percentChance;

    if (gBattleMons[gBattlerAttacker].ability == ABILITY_SERENE_GRACE)
        percentChance = gBattleMoves[gCurrentMove].secondaryEffectChance * 2;
    else
        percentChance = gBattleMoves[gCurrentMove].secondaryEffectChance;

    if (DEBUG && (gUnknown_02023A14_50 & 4)
        && !(gBattleCommunication[MOVE_EFFECT_BYTE] & MOVE_EFFECT_CERTAIN)
        && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
    {
        SetMoveEffect(FALSE, 0);
    }
    else if ((gBattleCommunication[MOVE_EFFECT_BYTE] & MOVE_EFFECT_CERTAIN)
        && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
    {
        gBattleCommunication[MOVE_EFFECT_BYTE] &= ~(MOVE_EFFECT_CERTAIN);
        SetMoveEffect(FALSE, MOVE_EFFECT_CERTAIN);
    }
    else if (Random() % 100 <= percentChance  // Random() % 100 < percentChance in Emerald
             && gBattleCommunication[MOVE_EFFECT_BYTE]
             && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
    {
        if (percentChance >= 100)
            SetMoveEffect(FALSE, MOVE_EFFECT_CERTAIN);
        else
            SetMoveEffect(FALSE, 0);
    }
    else
    {
        gBattlescriptCurrInstr++;
    }

    gBattleCommunication[MOVE_EFFECT_BYTE] = 0;
    gBattleStruct->multihitMoveEffect = 0;
}

static void Cmd_seteffectprimary(void)
{
    SetMoveEffect(TRUE, 0);
}

static void Cmd_seteffectsecondary(void)
{
    SetMoveEffect(FALSE, 0);
}

static void Cmd_clearstatusfromeffect(void)
{
    gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);

    if (gBattleCommunication[MOVE_EFFECT_BYTE] <= MOVE_EFFECT_TOXIC)
        gBattleMons[gActiveBattler].status1 &= (~sStatusFlagsForMoveEffects[gBattleCommunication[MOVE_EFFECT_BYTE]]);
    else
        gBattleMons[gActiveBattler].status2 &= (~sStatusFlagsForMoveEffects[gBattleCommunication[MOVE_EFFECT_BYTE]]);

    gBattleCommunication[MOVE_EFFECT_BYTE] = 0;
    gBattlescriptCurrInstr += 2;
    gBattleStruct->multihitMoveEffect = 0;
}

static void atk19_tryfaintmon(void)
{
    const u8 *BS_ptr;

    if (gBattlescriptCurrInstr[2] != 0)
    {
        gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
        if (gHitMarker & HITMARKER_FAINTED(gActiveBattler))
        {
            BS_ptr = T1_READ_PTR(gBattlescriptCurrInstr + 3);

            BattleScriptPop();
            gBattlescriptCurrInstr = BS_ptr;
            gSideStatuses[GetBattlerSide(gActiveBattler)] &= ~(SIDE_STATUS_SPIKES_DAMAGED);
        }
        else
        {
            gBattlescriptCurrInstr += 7;
        }
    }
    else
    {
        u8 battlerId;

        if (gBattlescriptCurrInstr[1] == BS_ATTACKER)
        {
            gActiveBattler = gBattlerAttacker;
            battlerId = gBattlerTarget;
            BS_ptr = BattleScript_FaintAttacker;
        }
        else
        {
            gActiveBattler = gBattlerTarget;
            battlerId = gBattlerAttacker;
            BS_ptr = BattleScript_FaintTarget;
        }
        if (!(gAbsentBattlerFlags & gBitTable[gActiveBattler])
         && gBattleMons[gActiveBattler].hp == 0)
        {
            gSharedMem[BSTRUCT_OFF(lastTakenMove) + 2 * battlerId + 0] = 0;
            gSharedMem[BSTRUCT_OFF(lastTakenMove) + 2 * battlerId + 1] = 0;
            gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * battlerId + 0] = 0;
            gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * battlerId + 1] = 0;
            gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * battlerId + 2] = 0;
            gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * battlerId + 3] = 0;

            gHitMarker |= HITMARKER_FAINTED(gActiveBattler);
            BattleScriptPush(gBattlescriptCurrInstr + 7);
            gBattlescriptCurrInstr = BS_ptr;
            if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
            {
                gHitMarker |= HITMARKER_PLAYER_FAINTED;
                if (gBattleResults.playerFaintCounter < UCHAR_MAX)
                    gBattleResults.playerFaintCounter++;
                if (gBattleMons[battlerId].level > gBattleMons[gActiveBattler].level)
                {
                    if (gBattleMons[battlerId].level - gBattleMons[gActiveBattler].level > 29)
                        AdjustFriendship(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], FRIENDSHIP_EVENT_FAINT_LARGE);
                    else
                        AdjustFriendship(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], FRIENDSHIP_EVENT_FAINT_SMALL);
                }
            }
            else
            {
                if (gBattleResults.opponentFaintCounter < UCHAR_MAX)
                    gBattleResults.opponentFaintCounter++;
                gBattleResults.lastOpponentSpecies = gBattleMons[gActiveBattler].species;
            }
            if ((gHitMarker & HITMARKER_DESTINYBOND) && gBattleMons[gBattlerAttacker].hp != 0)
            {
                BattleScriptPush(gBattlescriptCurrInstr);
                gBattleMoveDamage = gBattleMons[battlerId].hp;
                gBattlescriptCurrInstr = BattleScript_DestinyBondTakesLife;
            }
            if ((gStatuses3[gBattlerTarget] & STATUS3_GRUDGE)
             && !(gHitMarker & HITMARKER_GRUDGE)
             && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget)
             && gBattleMons[gBattlerAttacker].hp != 0
             && gCurrentMove != MOVE_STRUGGLE)
            {
                u8 moveIndex = gSharedMem[BSTRUCT_OFF(chosenMovePositions) + gBattlerAttacker];

                gBattleMons[gBattlerAttacker].pp[moveIndex] = 0;
                BattleScriptPush(gBattlescriptCurrInstr);
                gBattlescriptCurrInstr = BattleScript_GrudgeTakesPP;
                gActiveBattler = gBattlerAttacker;
                BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, moveIndex + REQUEST_PPMOVE1_BATTLE, 0, sizeof(gBattleMons[gActiveBattler].pp[moveIndex]), &gBattleMons[gActiveBattler].pp[moveIndex]);
                MarkBattlerForControllerExec(gActiveBattler);

                PREPARE_MOVE_BUFFER(gBattleTextBuff1, gBattleMons[gBattlerAttacker].moves[moveIndex]);
            }
        }
        else
        {
            gBattlescriptCurrInstr += 7;
        }
    }
}

static void Cmd_dofaintanimation(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
        BtlController_EmitFaintAnimation(B_COMM_TO_CONTROLLER);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 2;
    }
}

static void atk1B_cleareffectsonfaint(void)
{
    //Clears things like attraction or trapping to other banks
    if (gBattleControllerExecFlags == 0)
    {
        gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
        gBattleMons[gActiveBattler].status1 = STATUS1_NONE;
        BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, 0, sizeof(gBattleMons[gActiveBattler].status1), &gBattleMons[gActiveBattler].status1);
        MarkBattlerForControllerExec(gActiveBattler);
        UndoEffectsAfterFainting();
        gBattlescriptCurrInstr += 2;
    }
}

static void Cmd_jumpifstatus(void)
{
    u8 battler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    u32 flags = T2_READ_32(gBattlescriptCurrInstr + 2);
    void* jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 6);
    if (gBattleMons[battler].status1 & flags && gBattleMons[battler].hp)
        gBattlescriptCurrInstr = jumpPtr;
    else
        gBattlescriptCurrInstr += 10;
}

static void Cmd_jumpifstatus2(void)
{
    u8 battler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    u32 flags = T2_READ_32(gBattlescriptCurrInstr + 2);
    void* jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 6);
    if (gBattleMons[battler].status2 & flags && gBattleMons[battler].hp)
        gBattlescriptCurrInstr = jumpPtr;
    else
        gBattlescriptCurrInstr += 10;
}

static void Cmd_jumpifability(void)
{
    u8 battler;
    u8 ability = gBattlescriptCurrInstr[2];
    void* jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 3);
    if (T2_READ_8(gBattlescriptCurrInstr + 1) == BS_ATTACKER_SIDE)
    {
        battler = AbilityBattleEffects(ABILITYEFFECT_CHECK_BATTLER_SIDE, gBattlerAttacker, ability, 0, 0);
        if (battler)
        {
            gLastUsedAbility = ability;
            gBattlescriptCurrInstr = jumpPtr;
            RecordAbilityBattle(battler -1, gLastUsedAbility);
            gBattleStruct->battlerWithAbility = battler - 1;
        }
        else
            gBattlescriptCurrInstr += 7;
    }
    else if (T2_READ_8(gBattlescriptCurrInstr + 1) == BS_NOT_ATTACKER_SIDE)
    {
        battler = AbilityBattleEffects(ABILITYEFFECT_CHECK_OTHER_SIDE, gBattlerAttacker, ability, 0, 0);
        if (battler)
        {
            gLastUsedAbility = ability;
            gBattlescriptCurrInstr = jumpPtr;
            RecordAbilityBattle(battler - 1, gLastUsedAbility);
            gBattleStruct->battlerWithAbility = battler - 1;
        }
        else
            gBattlescriptCurrInstr += 7;
    }
    else
    {
        battler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
        if (gBattleMons[battler].ability == ability)
        {
            gLastUsedAbility = ability;
            gBattlescriptCurrInstr = jumpPtr;
            RecordAbilityBattle(battler, gLastUsedAbility);
            gBattleStruct->battlerWithAbility = battler;
        }
        else
            gBattlescriptCurrInstr += 7;
    }
}

static void Cmd_jumpifsideaffecting(void)
{
    u8 side;
    u16 flags;
    void* jumpPtr;
    if (T2_READ_8(gBattlescriptCurrInstr + 1) == BS_ATTACKER)
        side = GetBattlerPosition(gBattlerAttacker) & BIT_SIDE;
    else
        side = GetBattlerPosition(gBattlerTarget) & BIT_SIDE;

    flags = T2_READ_16(gBattlescriptCurrInstr + 2);
    jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 4);

    if (gSideStatuses[side] & flags)
        gBattlescriptCurrInstr = jumpPtr;
    else
        gBattlescriptCurrInstr += 8;
}

static void Cmd_jumpifstat(void)
{
    u8 ret = 0;
    u8 battler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    u8 value = gBattleMons[battler].statStages[T2_READ_8(gBattlescriptCurrInstr + 3)];
    switch (T2_READ_8(gBattlescriptCurrInstr + 2))
    {
    case CMP_EQUAL:
        if (value == T2_READ_8(gBattlescriptCurrInstr + 4))
            ret++;
        break;
    case CMP_NOT_EQUAL:
        if (value != T2_READ_8(gBattlescriptCurrInstr + 4))
            ret++;
        break;
    case CMP_GREATER_THAN:
        if (value > T2_READ_8(gBattlescriptCurrInstr + 4))
            ret++;
        break;
    case CMP_LESS_THAN:
        if (value < T2_READ_8(gBattlescriptCurrInstr + 4))
            ret++;
        break;
    case CMP_COMMON_BITS:
        if (value & T2_READ_8(gBattlescriptCurrInstr + 4))
            ret++;
        break;
    case CMP_NO_COMMON_BITS:
        if (!(value & T2_READ_8(gBattlescriptCurrInstr + 4)))
            ret++;
        break;
    }
    if (ret)
        gBattlescriptCurrInstr = T2_READ_PTR(gBattlescriptCurrInstr + 5);
    else
        gBattlescriptCurrInstr += 9;
}

static void Cmd_jumpifstatus3condition(void)
{
    u32 status;
    void* jumpPtr;

    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    status = T2_READ_32(gBattlescriptCurrInstr + 2);
    jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 7);
    if (T2_READ_8(gBattlescriptCurrInstr + 6))
    {
        if ((gStatuses3[gActiveBattler] & status) != 0)
            gBattlescriptCurrInstr += 11;
        else
            gBattlescriptCurrInstr = jumpPtr;
    }
    else
    {
        if ((gStatuses3[gActiveBattler] & status) != 0)
            gBattlescriptCurrInstr = jumpPtr;
        else
            gBattlescriptCurrInstr += 11;
    }
}

static void Cmd_jumpiftype(void) //u8 battler, u8 type, *ptr
{
    u8 battler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    u8 type = T2_READ_8(gBattlescriptCurrInstr + 2);
    void* jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 3);

    if (gBattleMons[battler].type1 == type || gBattleMons[battler].type2 == type)
        gBattlescriptCurrInstr = jumpPtr;
    else
        gBattlescriptCurrInstr += 7;
}

static void atk23_getexp(void)
{
    u16 item;
    s32 i; // also used as stringId
    u8 holdEffect;
    s32 sentIn;

    s32 viaExpShare = 0;
    u16* exp = &gBattleStruct->expValue;

    gBattlerFainted = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    sentIn = gSentPokesToOpponent[(gBattlerFainted & BIT_FLANK) >> 1];

    switch (gBattleStruct->getexpState)
    {
    case 0: // check if should receive exp at all
        if (GetBattlerSide(gBattlerFainted) != B_SIDE_OPPONENT || (gBattleTypeFlags &
             (BATTLE_TYPE_LINK
              | BATTLE_TYPE_SAFARI
              | BATTLE_TYPE_BATTLE_TOWER
              | BATTLE_TYPE_EREADER_TRAINER)))
        {
            gBattleStruct->getexpState = 6; // goto last case
        }
        else
        {
            gBattleStruct->getexpState++;
            gBattleStruct->givenExpMons |= gBitTable[gBattlerPartyIndexes[gBattlerFainted]];
        }
        break;
    case 1: // calculate experience points to redistribute
        {
            u16 calculatedExp;
            s32 viaSentIn;

            for (viaSentIn = 0, i = 0; i < PARTY_SIZE; i++)
            {
                if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == SPECIES_NONE || GetMonData(&gPlayerParty[i], MON_DATA_HP) == 0)
                    continue;
                if (gBitTable[i] & sentIn)
                    viaSentIn++;

                item = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);

                if (item == ITEM_ENIGMA_BERRY)
                    holdEffect = gSaveBlock1.enigmaBerry.holdEffect;
                else
                    holdEffect = GetItemHoldEffect(item);

                if (holdEffect == HOLD_EFFECT_EXP_SHARE)
                    viaExpShare++;
            }

            calculatedExp = gSpeciesInfo[gBattleMons[gBattlerFainted].species].expYield * gBattleMons[gBattlerFainted].level / 7;

            if (viaExpShare) // at least one mon is getting exp via exp share
            {
                *exp = calculatedExp / 2 / viaSentIn;
                if (*exp == 0)
                    *exp = 1;

                gExpShareExp = calculatedExp / 2 / viaExpShare;
                if (gExpShareExp == 0)
                    gExpShareExp = 1;
            }
            else
            {
                *exp = calculatedExp / viaSentIn;
                if (*exp == 0)
                    *exp = 1;
                gExpShareExp = 0;
            }

            gBattleStruct->getexpState++;
            gBattleStruct->expGetterMonId = 0;
            gBattleStruct->sentInPokes = sentIn;
        }
        // fall through
    case 2: // set exp value to the poke in expgetter_id and print message
        if (gBattleControllerExecFlags == 0)
        {
            item = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HELD_ITEM);

            if (item == ITEM_ENIGMA_BERRY)
                holdEffect = gSaveBlock1.enigmaBerry.holdEffect;
            else
                holdEffect = GetItemHoldEffect(item);

            if (holdEffect != HOLD_EFFECT_EXP_SHARE && !(gBattleStruct->sentInPokes & 1))
            {
                gBattleStruct->sentInPokes >>= 1;
                gBattleStruct->getexpState = 5;
                gBattleMoveDamage = 0; // used for exp
            }
            else if (GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL) == MAX_LEVEL)
            {
                gBattleStruct->sentInPokes >>= 1;
                gBattleStruct->getexpState = 5;
                gBattleMoveDamage = 0; // used for exp
            }
            else
            {
                // music change in wild battle after fainting a poke
                if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER) && gBattleMons[0].hp && !gBattleStruct->wildVictorySong)
                {
                    BattleStopLowHpSound();
                    PlayBGM(MUS_VICTORY_WILD);
                    gBattleStruct->wildVictorySong++;
                }

                if (GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HP))
                {
                    if (gBattleStruct->sentInPokes & 1)
                        gBattleMoveDamage = *exp;
                    else
                        gBattleMoveDamage = 0;

                    if (holdEffect == HOLD_EFFECT_EXP_SHARE)
                        gBattleMoveDamage += gExpShareExp;
                    if (holdEffect == HOLD_EFFECT_LUCKY_EGG)
                        gBattleMoveDamage = (gBattleMoveDamage * 150) / 100;
                    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                        gBattleMoveDamage = (gBattleMoveDamage * 150) / 100;

                    if (IsTradedMon(&gPlayerParty[gBattleStruct->expGetterMonId]))
                    {
                        gBattleMoveDamage = (gBattleMoveDamage * 150) / 100;
                        i = STRINGID_ABOOSTED;
                    }
                    else
                    {
                        i = STRINGID_EMPTYSTRING4;
                    }

                    // get exp getter bank
                    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                    {
                        if (!(gBattlerPartyIndexes[2] != gBattleStruct->expGetterMonId) && !(gAbsentBattlerFlags & gBitTable[2]))
                            gBattleStruct->expGetterBattlerId = 2;
                        else
                        {
                            if (!(gAbsentBattlerFlags & gBitTable[0]))
                                gBattleStruct->expGetterBattlerId = 0;
                            else
                                gBattleStruct->expGetterBattlerId = 2;
                        }
                    }
                    else
                        gBattleStruct->expGetterBattlerId = 0;

                    PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, gBattleStruct->expGetterBattlerId, gBattleStruct->expGetterMonId)

					// buffer 'gained' or 'gained a boosted'
					PREPARE_STRING_BUFFER(gBattleTextBuff2, i)

                    PREPARE_WORD_NUMBER_BUFFER(gBattleTextBuff3, 5, gBattleMoveDamage)

                    PrepareStringBattle(STRINGID_PKMNGAINEDEXP, gBattleStruct->expGetterBattlerId);
                    MonGainEVs(&gPlayerParty[gBattleStruct->expGetterMonId], gBattleMons[gBattlerFainted].species);
                }
                gBattleStruct->sentInPokes >>= 1;
                gBattleStruct->getexpState++;
            }
        }
        break;
    case 3: // Set stats and give exp
        if (gBattleControllerExecFlags == 0)
        {
            gBattleBufferB[gBattleStruct->expGetterBattlerId][0] = 0;
            if (GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HP) && GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL) != MAX_LEVEL)
            {
                gBattleResources_statsBeforeLvlUp->hp = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_MAX_HP);
                gBattleResources_statsBeforeLvlUp->atk = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_ATK);
                gBattleResources_statsBeforeLvlUp->def = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_DEF);
                gBattleResources_statsBeforeLvlUp->spd = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPEED);
                gBattleResources_statsBeforeLvlUp->spAtk = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPATK);
                gBattleResources_statsBeforeLvlUp->spDef = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPDEF);

                gActiveBattler = gBattleStruct->expGetterBattlerId;

                BtlController_EmitExpUpdate(B_COMM_TO_CONTROLLER, gBattleStruct->expGetterMonId, gBattleMoveDamage);
                MarkBattlerForControllerExec(gActiveBattler);
            }
            gBattleStruct->getexpState++;
        }
        break;
    case 4: // lvl up if necessary
        if (gBattleControllerExecFlags == 0)
        {
            gActiveBattler = gBattleStruct->expGetterBattlerId;
            if (gBattleBufferB[gActiveBattler][0] == CONTROLLER_TWORETURNVALUES
             && gBattleBufferB[gActiveBattler][1] == RET_VALUE_LEVELED_UP)
            {
                if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && gBattlerPartyIndexes[gActiveBattler] == gBattleStruct->expGetterMonId)
                    HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);

                PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, gActiveBattler, gBattleStruct->expGetterMonId)

                PREPARE_BYTE_NUMBER_BUFFER(gBattleTextBuff2, 3, GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL))

                BattleScriptPushCursor();
                gLeveledUpInBattle |= gBitTable[gBattleStruct->expGetterMonId];
                gBattlescriptCurrInstr = BattleScript_LevelUp;
                gBattleMoveDamage = (gBattleBufferB[gActiveBattler][2] | (gBattleBufferB[gActiveBattler][3] << 8));
                AdjustFriendship(&gPlayerParty[gBattleStruct->expGetterMonId], FRIENDSHIP_EVENT_GROW_LEVEL);

                // update battle mon structure after level up
                if (gBattlerPartyIndexes[0] == gBattleStruct->expGetterMonId && gBattleMons[0].hp)
                {
                    gBattleMons[0].level = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL);
                    gBattleMons[0].hp = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HP);
                    gBattleMons[0].maxHP = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_MAX_HP);
                    gBattleMons[0].attack = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_ATK);
                    gBattleMons[0].defense = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_DEF);
                    // Why is this duplicated?
                    gBattleMons[0].speed = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPEED);
                    gBattleMons[0].speed = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPEED);
                    gBattleMons[0].spAttack = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPATK);
                    gBattleMons[0].spDefense = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPDEF);
                }
                // What is else if?
                if (gBattlerPartyIndexes[2] == gBattleStruct->expGetterMonId && gBattleMons[2].hp && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
                {
                    gBattleMons[2].level = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_LEVEL);
                    gBattleMons[2].hp = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_HP);
                    gBattleMons[2].maxHP = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_MAX_HP);
                    gBattleMons[2].attack = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_ATK);
                    gBattleMons[2].defense = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_DEF);
                    // Duplicated again, but this time there's no Sp Defense
                    gBattleMons[2].speed = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPEED);
                    gBattleMons[2].speed = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPEED);
                    gBattleMons[2].spAttack = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_SPATK);
                }
            }
            else
            {
                gBattleMoveDamage = 0;
            }
            gBattleStruct->getexpState = 5;
        }
        break;
    case 5: // looper increment
        if (gBattleMoveDamage) // there is exp to give, goto case 3 that gives exp
            gBattleStruct->getexpState = 3;
        else
        {
            gBattleStruct->expGetterMonId++;
            if (gBattleStruct->expGetterMonId <= (PARTY_SIZE - 1))
                gBattleStruct->getexpState = 2; // loop again
            else
                gBattleStruct->getexpState = 6; // we're done
        }
        break;
    case 6: // increment instruction
        if (gBattleControllerExecFlags == 0)
        {
            // not sure why gf clears the item and ability here
            gBattleMons[gBattlerFainted].item = ITEM_NONE;
            gBattleMons[gBattlerFainted].ability = ABILITY_NONE;
            gBattlescriptCurrInstr += 2;
        }
        break;
    }
}

static void atk24(void)
{
    u16 HP_count = 0;
    s32 i;
    s32 emptyPlayerSpots;
    s32 emptyOpponentSpots;

    if (gBattleControllerExecFlags == 0)
    {
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) && !GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
                HP_count += GetMonData(&gPlayerParty[i], MON_DATA_HP);
        }

        if (HP_count == 0)
            gBattleOutcome |= B_OUTCOME_LOST;

        for (HP_count = 0, i = 0; i < PARTY_SIZE; i++)
        {
            if (GetMonData(&gEnemyParty[i], MON_DATA_SPECIES) && !GetMonData(&gEnemyParty[i], MON_DATA_IS_EGG))
                HP_count += GetMonData(&gEnemyParty[i], MON_DATA_HP);
        }

        if (HP_count == 0)
            gBattleOutcome |= B_OUTCOME_WON;

        if (!gBattleOutcome && (gBattleTypeFlags & BATTLE_TYPE_LINK))
        {
            for (emptyPlayerSpots = 0, i = 0; i < gBattlersCount; i += 2)
            {
                if ((gHitMarker & HITMARKER_FAINTED2(i)) && !gSpecialStatuses[i].faintedHasReplacement)
                    emptyPlayerSpots++;
            }

            for (emptyOpponentSpots = 0, i = 1; i < gBattlersCount; i += 2)
            {
                if ((gHitMarker & HITMARKER_FAINTED2(i)) && !gSpecialStatuses[i].faintedHasReplacement)
                    emptyOpponentSpots++;
            }

            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                if (emptyOpponentSpots + emptyPlayerSpots > 1)
                    gBattlescriptCurrInstr = T2_READ_PTR(gBattlescriptCurrInstr + 1);
                else
                    gBattlescriptCurrInstr += 5;
            }
            else
            {
                if (emptyOpponentSpots != 0 && emptyPlayerSpots != 0)
                    gBattlescriptCurrInstr = T2_READ_PTR(gBattlescriptCurrInstr + 1);
                else
                    gBattlescriptCurrInstr += 5;
            }
        }
        else
        {
            gBattlescriptCurrInstr += 5;
        }
    }
}

static void MoveValuesCleanUp(void)
{
    gMoveResultFlags = 0;
    gBattleStruct->dmgMultiplier = 1;
    gCritMultiplier = 1;
    gBattleCommunication[MOVE_EFFECT_BYTE] = 0;
    gBattleCommunication[MISS_TYPE] = B_MSG_MISSED;
    gHitMarker &= ~(HITMARKER_DESTINYBOND);
    gHitMarker &= ~(HITMARKER_SYNCHRONIZE_EFFECT);
}

static void Cmd_movevaluescleanup(void)
{
    MoveValuesCleanUp();
    gBattlescriptCurrInstr += 1;
}

static void Cmd_setmultihit(void)
{
    gMultiHitCounter = T2_READ_8(gBattlescriptCurrInstr + 1);
    gBattlescriptCurrInstr += 2;
}

static void Cmd_decrementmultihit(void)
{
    if (--gMultiHitCounter == 0)
        gBattlescriptCurrInstr += 5;
    else
        gBattlescriptCurrInstr = T2_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_goto(void)
{
    gBattlescriptCurrInstr = T2_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_jumpifbyte(void)
{
    u8 caseID = T2_READ_8(gBattlescriptCurrInstr + 1);
    u8* memByte = T2_READ_PTR(gBattlescriptCurrInstr + 2);
    u8 value = T2_READ_8(gBattlescriptCurrInstr + 6);
    u8* jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 7);
    gBattlescriptCurrInstr += 11;
    switch (caseID)
    {
    case CMP_EQUAL:
        if (*memByte == value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_NOT_EQUAL:
        if (*memByte != value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_GREATER_THAN:
        if (*memByte > value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_LESS_THAN:
        if (*memByte < value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_COMMON_BITS:
        if (*memByte & value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_NO_COMMON_BITS:
        if (!(*memByte & value))
            gBattlescriptCurrInstr = jumpPtr;
        break;
    }
}

static void Cmd_jumpifhalfword(void)
{
    u8 caseID = T2_READ_8(gBattlescriptCurrInstr + 1);
    u16* memHword = T2_READ_PTR(gBattlescriptCurrInstr + 2);
    u16 value = T2_READ_16(gBattlescriptCurrInstr + 6);
    u8* jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 8);
    gBattlescriptCurrInstr += 12;
    switch (caseID)
    {
    case CMP_EQUAL:
        if (*memHword == value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_NOT_EQUAL:
        if (*memHword != value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_GREATER_THAN:
        if (*memHword > value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_LESS_THAN:
        if (*memHword < value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_COMMON_BITS:
        if (*memHword & value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_NO_COMMON_BITS:
        if (!(*memHword & value))
            gBattlescriptCurrInstr = jumpPtr;
        break;
    }
}

// Strange that there's an instance of T1_READ_32 in what seems to be a T2 function. see global.h for the distinction.
static void Cmd_jumpifword(void)
{
    u8 caseID = T2_READ_8(gBattlescriptCurrInstr + 1);
    u32* memWord = T2_READ_PTR(gBattlescriptCurrInstr + 2);
    u32 value = T1_READ_32(gBattlescriptCurrInstr + 6);
    u8* jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 10);
    gBattlescriptCurrInstr += 14;
    switch (caseID)
    {
    case CMP_EQUAL:
        if (*memWord == value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_NOT_EQUAL:
        if (*memWord != value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_GREATER_THAN:
        if (*memWord > value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_LESS_THAN:
        if (*memWord < value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_COMMON_BITS:
        if (*memWord & value)
            gBattlescriptCurrInstr = jumpPtr;
        break;
    case CMP_NO_COMMON_BITS:
        if (!(*memWord & value))
            gBattlescriptCurrInstr = jumpPtr;
        break;
    }
}

static void Cmd_jumpifarrayequal(void)
{
    //Mem1, Mem2, Size, Jump Loc
    u8* mem1 = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    u8* mem2 = T2_READ_PTR(gBattlescriptCurrInstr + 5);
    u32 size = T2_READ_8(gBattlescriptCurrInstr + 9);
    u8* jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 10);

    u8 i;
    for (i = 0; i < size; i++)
    {
        if (*mem1 != *mem2)
        {
            gBattlescriptCurrInstr += 14;
            break;
        }
        mem1++, mem2++;
    }

    if (i == size)
        gBattlescriptCurrInstr = jumpPtr;
}

static void Cmd_jumpifarraynotequal(void)
{
    //Mem1, Mem2, Size, Jump Loc
    u8 equalBytes = 0;
    u8* mem1 = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    u8* mem2 = T2_READ_PTR(gBattlescriptCurrInstr + 5);
    u32 size = T2_READ_8(gBattlescriptCurrInstr + 9);
    u8* jumpPtr = T2_READ_PTR(gBattlescriptCurrInstr + 10);

    u8 i;
    for (i = 0; i < size; i++)
    {
        if (*mem1 == *mem2)
        {
            equalBytes++;
        }
        mem1++, mem2++;
    }

    if (equalBytes != size)
        gBattlescriptCurrInstr = jumpPtr;
    else
        gBattlescriptCurrInstr += 14;
}

static void Cmd_setbyte(void)
{
    u8* memByte = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    *memByte = T2_READ_8(gBattlescriptCurrInstr + 5);
    gBattlescriptCurrInstr += 6;
}

static void Cmd_addbyte(void)
{
    u8* memByte = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    *memByte += T2_READ_8(gBattlescriptCurrInstr + 5);
    gBattlescriptCurrInstr += 6;
}

static void Cmd_subbyte(void)
{
    u8* memByte = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    *memByte -= T2_READ_8(gBattlescriptCurrInstr + 5);
    gBattlescriptCurrInstr += 6;
}

static void Cmd_copyarray(void)
{
    u8* dest = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    u8* src = T2_READ_PTR(gBattlescriptCurrInstr + 5);
    s32 size = T2_READ_8(gBattlescriptCurrInstr + 9);

    s32 i;
    for (i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }

    gBattlescriptCurrInstr += 10;
}

static void Cmd_copyarraywithindex(void)
{
    u8* dest = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    u8* src = T2_READ_PTR(gBattlescriptCurrInstr + 5);
    u8* index = T2_READ_PTR(gBattlescriptCurrInstr + 9);
    s32 size = T2_READ_8(gBattlescriptCurrInstr + 13);

    s32 i;
    for (i = 0; i < size; i++)
    {
        dest[i] = src[i + *index];
    }

    gBattlescriptCurrInstr += 14;
}

static void Cmd_orbyte(void)
{
    u8* memByte = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    *memByte |= T2_READ_8(gBattlescriptCurrInstr + 5);
    gBattlescriptCurrInstr += 6;
}

static void Cmd_orhalfword(void)
{
    u16* memHword = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    u16 val = T2_READ_16(gBattlescriptCurrInstr + 5);

    *memHword |= val;
    gBattlescriptCurrInstr += 7;
}

static void Cmd_orword(void)
{
    u32* memWord = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    u32 val = T2_READ_32(gBattlescriptCurrInstr + 5);

    *memWord |= val;
    gBattlescriptCurrInstr += 9;
}

static void Cmd_bicbyte(void)
{
    u8* memByte = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    *memByte &= ~(T2_READ_8(gBattlescriptCurrInstr + 5));
    gBattlescriptCurrInstr += 6;
}

static void Cmd_bichalfword(void)
{
    u16* memHword = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    u16 val = T2_READ_16(gBattlescriptCurrInstr + 5);

    *memHword &= ~val;
    gBattlescriptCurrInstr += 7;
}

static void Cmd_bicword(void)
{
    u32* memWord = T2_READ_PTR(gBattlescriptCurrInstr + 1);
    u32 val = T2_READ_32(gBattlescriptCurrInstr + 5);

    *memWord &= ~val;
    gBattlescriptCurrInstr += 9;
}

static void Cmd_pause(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        u16 value = T2_READ_16(gBattlescriptCurrInstr + 1);
        if (++gPauseCounterBattle >= value)
        {
            gPauseCounterBattle = 0;
            gBattlescriptCurrInstr += 3;
        }
    }
}

static void Cmd_waitstate(void)
{
    if (gBattleControllerExecFlags == 0)
        gBattlescriptCurrInstr++;
}

static void Cmd_healthbar_update(void)
{
    if (!T2_READ_8(gBattlescriptCurrInstr + 1))
        gActiveBattler = gBattlerTarget;
    else
        gActiveBattler = gBattlerAttacker;

    BtlController_EmitHealthBarUpdate(B_COMM_TO_CONTROLLER, gBattleMoveDamage);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 2;
}

static void Cmd_return(void)
{
    BattleScriptPop();
}

static void atk3D_end(void)
{
    gMoveResultFlags = 0;
    gActiveBattler = 0;
    gCurrentActionFuncId = B_ACTION_TRY_FINISH;
}

static void Cmd_end2(void)
{
    //not much difference between this and 3D. It's more apparent in Emerald
    gActiveBattler = 0;
    gCurrentActionFuncId = B_ACTION_TRY_FINISH;
}

static void Cmd_end3(void) //pops the main function stack
{
    BattleScriptPop();
    if (B_FUNCTION_STACK->size)
        B_FUNCTION_STACK->size--;
    gBattleMainFunc = B_FUNCTION_STACK->function[B_FUNCTION_STACK->size];
}

static void Cmd_call(void)
{
    BattleScriptPush(gBattlescriptCurrInstr + 5);
    gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_jumpiftype2(void) //u8 battler, u8 type, *ptr
{
    u8 battler = GetBattlerForBattleScript(T1_READ_8(gBattlescriptCurrInstr + 1));

    if (T1_READ_8(gBattlescriptCurrInstr + 2) == gBattleMons[battler].type1 || T1_READ_8(gBattlescriptCurrInstr + 2) == gBattleMons[battler].type2)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 3);
    else
        gBattlescriptCurrInstr += 7;
}

static void Cmd_jumpifabilitypresent(void)
{
    if (AbilityBattleEffects(ABILITYEFFECT_CHECK_ON_FIELD, 0, T2_READ_8(gBattlescriptCurrInstr + 1), 0, 0))
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 2);
    else
        gBattlescriptCurrInstr += 6;
}

static void Cmd_endselectionscript(void)
{
    gSharedMem[BSTRUCT_OFF(selectionScriptFinished) + gBattlerAttacker] = 1;
}

static void Cmd_playanimation(void)
{
    const u16* argumentPtr;

    gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    argumentPtr = T2_READ_PTR(gBattlescriptCurrInstr + 3);

    if (gBattlescriptCurrInstr[2] == B_ANIM_STATS_CHANGE
        || gBattlescriptCurrInstr[2] == B_ANIM_SNATCH_MOVE
        || gBattlescriptCurrInstr[2] == B_ANIM_SUBSTITUTE_FADE)
    {
        BtlController_EmitBattleAnimation(B_COMM_TO_CONTROLLER, gBattlescriptCurrInstr[2], *argumentPtr);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 7;
    }
    else if (gHitMarker & HITMARKER_NO_ANIMATIONS)
    {
        BattleScriptPush(gBattlescriptCurrInstr + 7);
        gBattlescriptCurrInstr = BattleScript_Pausex20;
    }
    else if (gBattlescriptCurrInstr[2] == B_ANIM_RAIN_CONTINUES
             || gBattlescriptCurrInstr[2] == B_ANIM_SUN_CONTINUES
             || gBattlescriptCurrInstr[2] == B_ANIM_SANDSTORM_CONTINUES
             || gBattlescriptCurrInstr[2] == B_ANIM_HAIL_CONTINUES)
    {
        BtlController_EmitBattleAnimation(B_COMM_TO_CONTROLLER, gBattlescriptCurrInstr[2], *argumentPtr);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 7;
    }
    else if (gStatuses3[gActiveBattler] & STATUS3_SEMI_INVULNERABLE)
    {
        gBattlescriptCurrInstr += 7;
    }
    else
    {
        BtlController_EmitBattleAnimation(B_COMM_TO_CONTROLLER, gBattlescriptCurrInstr[2], *argumentPtr);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 7;
    }
}

static void Cmd_playanimation_var(void) // animation Id is stored in the first pointer
{
    const u16* argumentPtr;
    const u8* animationIdPtr;

    gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    animationIdPtr = T2_READ_PTR(gBattlescriptCurrInstr + 2);
    argumentPtr = T2_READ_PTR(gBattlescriptCurrInstr + 6);

    if (*animationIdPtr == B_ANIM_STATS_CHANGE
        || *animationIdPtr == B_ANIM_SNATCH_MOVE
        || *animationIdPtr == B_ANIM_SUBSTITUTE_FADE)
    {
        BtlController_EmitBattleAnimation(B_COMM_TO_CONTROLLER, *animationIdPtr, *argumentPtr);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 10;
    }
    else if (gHitMarker & HITMARKER_NO_ANIMATIONS)
    {
        gBattlescriptCurrInstr += 10;
    }
    else if (*animationIdPtr == B_ANIM_RAIN_CONTINUES
             || *animationIdPtr == B_ANIM_SUN_CONTINUES
             || *animationIdPtr == B_ANIM_SANDSTORM_CONTINUES
             || *animationIdPtr == B_ANIM_HAIL_CONTINUES)
    {
        BtlController_EmitBattleAnimation(B_COMM_TO_CONTROLLER, *animationIdPtr, *argumentPtr);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 10;
    }
    else if (gStatuses3[gActiveBattler] & STATUS3_SEMI_INVULNERABLE)
    {
        gBattlescriptCurrInstr += 10;
    }
    else
    {
        BtlController_EmitBattleAnimation(B_COMM_TO_CONTROLLER, *animationIdPtr, *argumentPtr);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 10;
    }
}

static void Cmd_setgraphicalstatchangevalues(void)
{
    u8 value = 0;
    switch (GET_STAT_BUFF_VALUE2(gBattleStruct->statChanger))
    {
    case SET_STAT_BUFF_VALUE(1): //+1
        value = STAT_ANIM_PLUS1;
        break;
    case SET_STAT_BUFF_VALUE(2): //+2
        value = STAT_ANIM_PLUS2;
        break;
    case SET_STAT_BUFF_VALUE(1) | STAT_BUFF_NEGATIVE: //-1
        value = STAT_ANIM_MINUS1;
        break;
    case SET_STAT_BUFF_VALUE(2) | STAT_BUFF_NEGATIVE: //-2
        value = STAT_ANIM_MINUS2;
        break;
    }
    gBattleStruct->animArg1 = GET_STAT_BUFF_ID(gBattleStruct->statChanger) + value - 1;
    gBattleStruct->animArg2 = 0;
    gBattlescriptCurrInstr++;
}

static void Cmd_playstatchangeanimation(void)
{
    u32 currStat = 0;
    u16 statAnimId = 0;
    s32 changeableStatsCount = 0;
    u8 statsToCheck = 0;

    gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    statsToCheck = gBattlescriptCurrInstr[2];

    if (gBattlescriptCurrInstr[3] & STAT_CHANGE_NEGATIVE) // goes down
    {
        s16 startingStatAnimId;
        if (gBattlescriptCurrInstr[3] & STAT_CHANGE_BY_TWO)
            startingStatAnimId = STAT_ANIM_MINUS2 - 1;
        else
            startingStatAnimId = STAT_ANIM_MINUS1 - 1;

        while (statsToCheck != 0)
        {
            if (statsToCheck & 1)
            {
                if (gBattlescriptCurrInstr[3] & STAT_CHANGE_CANT_PREVENT)
                {
                    if (gBattleMons[gActiveBattler].statStages[currStat] > MIN_STAT_STAGE)
                    {
                        statAnimId = startingStatAnimId + currStat;
                        changeableStatsCount++;
                    }
                }
                else if (!gSideTimers[GET_BATTLER_SIDE(gActiveBattler)].mistTimer
                         && gBattleMons[gActiveBattler].ability != ABILITY_CLEAR_BODY
                         && gBattleMons[gActiveBattler].ability != ABILITY_WHITE_SMOKE
                         && !(gBattleMons[gActiveBattler].ability == ABILITY_KEEN_EYE && currStat == STAT_ACC)
                         && !(gBattleMons[gActiveBattler].ability == ABILITY_HYPER_CUTTER && currStat == STAT_ATK))
                {
                    if (gBattleMons[gActiveBattler].statStages[currStat] > MIN_STAT_STAGE)
                    {
                        statAnimId = startingStatAnimId + currStat;
                        changeableStatsCount++;
                    }
                }
            }
            statsToCheck >>= 1, currStat++;
        }

        if (changeableStatsCount > 1) // more than one stat, so the color is gray
        {
            if (gBattlescriptCurrInstr[3] & STAT_CHANGE_BY_TWO)
                statAnimId = STAT_ANIM_MULTIPLE_MINUS2;
            else
                statAnimId = STAT_ANIM_MULTIPLE_MINUS1;
        }
    }
    else // goes up
    {
        s16 startingStatAnimId;
        if (gBattlescriptCurrInstr[3] & STAT_CHANGE_BY_TWO)
            startingStatAnimId = STAT_ANIM_PLUS2 - 1;
        else
            startingStatAnimId = STAT_ANIM_PLUS1 - 1;

        while (statsToCheck != 0)
        {
            if (statsToCheck & 1 && gBattleMons[gActiveBattler].statStages[currStat] < MAX_STAT_STAGE)
            {
                statAnimId = startingStatAnimId + currStat;
                changeableStatsCount++;
            }
            statsToCheck >>= 1, currStat++;
        }

        if (changeableStatsCount > 1) // more than one stat, so the color is gray
        {
            if (gBattlescriptCurrInstr[3] & STAT_CHANGE_BY_TWO)
                statAnimId = STAT_ANIM_MULTIPLE_PLUS2;
            else
                statAnimId = STAT_ANIM_MULTIPLE_PLUS1;
        }
    }

    if (gBattlescriptCurrInstr[3] & STAT_CHANGE_MULTIPLE_STATS && changeableStatsCount < 2)
    {
        gBattlescriptCurrInstr += 4;
    }
    else if (changeableStatsCount != 0 && !gBattleStruct->statAnimPlayed)
    {
        BtlController_EmitBattleAnimation(B_COMM_TO_CONTROLLER, B_ANIM_STATS_CHANGE, statAnimId);
        MarkBattlerForControllerExec(gActiveBattler);
        if (gBattlescriptCurrInstr[3] & STAT_CHANGE_MULTIPLE_STATS && changeableStatsCount > 1)
            gBattleStruct->statAnimPlayed = TRUE;
        gBattlescriptCurrInstr += 4;
    }
    else
    {
        gBattlescriptCurrInstr += 4;
    }
}




// atk49, moveend cases
void atk49_moveend(void)
{
    s32 i;
    bool32 effect = FALSE;
    u16 moveType;
    u8 holdEffectAtk;
    u16 *choicedMoveAtk;
    u8 endMode, endState;

    endMode = T2_READ_8(gBattlescriptCurrInstr + 1);
    endState = T2_READ_8(gBattlescriptCurrInstr + 2);

    if (gBattleMons[gBattlerAttacker].item == ITEM_ENIGMA_BERRY)
        holdEffectAtk = gEnigmaBerries[gBattlerAttacker].holdEffect;
    else
        holdEffectAtk = GetItemHoldEffect(gBattleMons[gBattlerAttacker].item);

    choicedMoveAtk = &((u16*)(gSharedMem + BSTRUCT_OFF(choicedMove)))[gBattlerAttacker];
    GET_MOVE_TYPE(gCurrentMove, moveType);

    do
    {
        switch (gBattleStruct->moveendState)
        {
        case MOVEEND_RAGE: // rage check
            if (gBattleMons[gBattlerTarget].status2 & STATUS2_RAGE
                && gBattleMons[gBattlerTarget].hp != 0 && gBattlerAttacker != gBattlerTarget
                && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget)
                && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && (gProtectStructs[gBattlerTarget].physicalDmg != 0 || gProtectStructs[gBattlerTarget].specialDmg != 0)
                && gBattleMoves[gCurrentMove].power && gBattleMons[gBattlerTarget].statStages[STAT_ATK] <= (MAX_STAT_STAGE - 1))
            {
                gBattleMons[gBattlerTarget].statStages[STAT_ATK]++;
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_RageIsBuilding;
                effect = TRUE;
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_DEFROST: // defrosting check
            if (gBattleMons[gBattlerTarget].status1 & STATUS1_FREEZE
                && gBattleMons[gBattlerTarget].hp && gBattlerAttacker != gBattlerTarget
                && gSpecialStatuses[gBattlerTarget].specialDmg
                && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT) && moveType == TYPE_FIRE)
            {
                gBattleMons[gBattlerTarget].status1 &= ~(STATUS1_FREEZE);
                gActiveBattler = gBattlerTarget;
                BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, 0, sizeof(gBattleMons[gBattlerTarget].status1), &gBattleMons[gBattlerTarget].status1);
                MarkBattlerForControllerExec(gActiveBattler);
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_DefrostedViaFireMove;
                effect = TRUE;
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_SYNCHRONIZE_TARGET: // target synchronize
            if (AbilityBattleEffects(ABILITYEFFECT_SYNCHRONIZE, gBattlerTarget, 0, 0, 0))
                effect = TRUE;
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_ON_DAMAGE_ABILITIES: // Such as abilities activating on contact(Poison Spore, Rough Skin, etc.).
            if (AbilityBattleEffects(ABILITYEFFECT_ON_DAMAGE, gBattlerTarget, 0, 0, 0))
                effect = TRUE;
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_IMMUNITY_ABILITIES: //status immunities
            if (AbilityBattleEffects(ABILITYEFFECT_IMMUNITY, 0, 0, 0, 0))
                effect = TRUE; //it loops through 4 banks, so we increment after its done with all banks
            else
                gBattleStruct->moveendState++;
            break;
        case MOVEEND_SYNCHRONIZE_ATTACKER: //attacker synchronize
            if (AbilityBattleEffects(ABILITYEFFECT_ATK_SYNCHRONIZE, gBattlerAttacker, 0, 0, 0))
                effect = TRUE;
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_CHOICE_MOVE: //update choice band move
            if (!(gHitMarker & HITMARKER_OBEYS) || holdEffectAtk != HOLD_EFFECT_CHOICE_BAND
                || gChosenMove == MOVE_STRUGGLE || (*choicedMoveAtk != MOVE_NONE && *choicedMoveAtk != MOVE_UNAVAILABLE))
                    goto LOOP;
            if (gChosenMove == MOVE_BATON_PASS && !(gMoveResultFlags & MOVE_RESULT_FAILED))
            {
                gBattleStruct->moveendState++;
                break;
            }
            *choicedMoveAtk = gChosenMove;
            LOOP:
            {
                for (i = 0; i < MAX_MON_MOVES; i++)
                {
                    if (gBattleMons[gBattlerAttacker].moves[i] == *choicedMoveAtk)
                        break;
                }
                if (i == MAX_MON_MOVES)
                    *choicedMoveAtk = MOVE_NONE;
                gBattleStruct->moveendState++;
            }
            break;
        case MOVEEND_CHANGED_ITEMS: // changed held items
            for (i = 0; i < gBattlersCount; i++)
            {
                #define CHANGED_ITEM ((u16 *)(gSharedMem + BSTRUCT_OFF(changedItems)))
                if (CHANGED_ITEM[i] != ITEM_NONE)
                {
                    gBattleMons[i].item = CHANGED_ITEM[i];
                    CHANGED_ITEM[i] = ITEM_NONE;
                }
                #undef CHANGED_ITEM
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_ITEM_EFFECTS_ALL: // item effects for all battlers
            if (ItemBattleEffects(ITEMEFFECT_MOVE_END, 0, FALSE))
                effect = TRUE;
            else
                gBattleStruct->moveendState++;
            break;
        case MOVEEND_KINGSROCK_SHELLBELL: // king's rock and shell bell
            if (ItemBattleEffects(ITEMEFFECT_KINGSROCK_SHELLBELL, 0, FALSE))
                effect = TRUE;
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_ATTACKER_INVISIBLE: // make attacker sprite invisible
            if ((gStatuses3[gBattlerAttacker] & STATUS3_SEMI_INVULNERABLE) && (gHitMarker & HITMARKER_NO_ANIMATIONS))
            {
                gActiveBattler = gBattlerAttacker;
                BtlController_EmitSpriteInvisibility(B_COMM_TO_CONTROLLER, TRUE);
                MarkBattlerForControllerExec(gActiveBattler);
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_ATTACKER_VISIBLE: // make attacker sprite visible
            if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT
                || !(gStatuses3[gBattlerAttacker] & (STATUS3_SEMI_INVULNERABLE))
                || WasUnableToUseMove(gBattlerAttacker))
            {
                gActiveBattler = gBattlerAttacker;
                BtlController_EmitSpriteInvisibility(B_COMM_TO_CONTROLLER, FALSE);
                MarkBattlerForControllerExec(gActiveBattler);
                gStatuses3[gBattlerAttacker] &= ~(STATUS3_SEMI_INVULNERABLE);
                gSpecialStatuses[gBattlerAttacker].restoredBattlerSprite = TRUE;
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_TARGET_VISIBLE: // make target sprite visible
            if (!gSpecialStatuses[gBattlerTarget].restoredBattlerSprite && gBattlerTarget < gBattlersCount
                && !(gStatuses3[gBattlerTarget] & STATUS3_SEMI_INVULNERABLE))
            {
                gActiveBattler = gBattlerTarget;
                BtlController_EmitSpriteInvisibility(B_COMM_TO_CONTROLLER, FALSE);
                MarkBattlerForControllerExec(gActiveBattler);
                gStatuses3[gBattlerTarget] &= ~(STATUS3_SEMI_INVULNERABLE);
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_SUBSTITUTE: // update substitute
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gDisableStructs[i].substituteHP == 0)
                    gBattleMons[i].status2 &= ~(STATUS2_SUBSTITUTE);
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_UPDATE_LAST_MOVES:
            if (gHitMarker & HITMARKER_SWAP_ATTACKER_TARGET)
            {
                gActiveBattler = gBattlerAttacker;
                gBattlerAttacker = gBattlerTarget;
                gBattlerTarget = gActiveBattler;
                gHitMarker &= ~(HITMARKER_SWAP_ATTACKER_TARGET);
            }

            if (gBattleMoves[gChosenMove].effect != EFFECT_BATON_PASS || (gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
                gLastPrintedMoves[gBattlerAttacker] = gChosenMove;

            if (!(gAbsentBattlerFlags & gBitTable[gBattlerAttacker])
                && !(gSharedMem[BSTRUCT_OFF(absentBattlerFlags)] & gBitTable[gBattlerAttacker])
                && gBattleMoves[gChosenMove].effect != EFFECT_BATON_PASS)
            {
                if (gHitMarker & HITMARKER_OBEYS)
                {
                    gLastMoves[gBattlerAttacker] = gChosenMove;
                    gLastResultingMoves[gBattlerAttacker] = gCurrentMove;
                }
                else
                {
                    gLastMoves[gBattlerAttacker] = MOVE_UNAVAILABLE;
                    gLastResultingMoves[gBattlerAttacker] = MOVE_UNAVAILABLE;
                }

                if (!(gHitMarker & HITMARKER_FAINTED(gBattlerTarget)))
                    gLastHitBy[gBattlerTarget] = gBattlerAttacker;

                if (gHitMarker & HITMARKER_OBEYS && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
                {
                    if (gChosenMove == MOVE_UNAVAILABLE)
                    {
                        gLastLandedMoves[gBattlerTarget] = gChosenMove;
                    }
                    else
                    {
                        gLastLandedMoves[gBattlerTarget] = gCurrentMove;
                        GET_MOVE_TYPE(gCurrentMove, gLastHitByType[gBattlerTarget]);
                    }
                }
                else
                {
                    gLastLandedMoves[gBattlerTarget] = MOVE_UNAVAILABLE;
                }
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_MIRROR_MOVE:
            if (!(gAbsentBattlerFlags & gBitTable[gBattlerAttacker]) && !(gSharedMem[BSTRUCT_OFF(absentBattlerFlags)] & gBitTable[gBattlerAttacker]))
            {
                if (gBattleMoves[gChosenMove].flags & FLAG_MIRROR_MOVE_AFFECTED && gHitMarker & HITMARKER_OBEYS
                 && gBattlerAttacker != gBattlerTarget && !(gHitMarker & HITMARKER_FAINTED(gBattlerTarget))
                 && !(gMoveResultFlags & MOVE_RESULT_NO_EFFECT))
                {
                    gSharedMem[BSTRUCT_OFF(lastTakenMove) + gBattlerTarget * 2 + 0] = gChosenMove;
                    gSharedMem[BSTRUCT_OFF(lastTakenMove) + gBattlerTarget * 2 + 1] = gChosenMove >> 8;

                    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + gBattlerTarget * 4 + 0 + (gBattlerAttacker / 2) * 2] = gChosenMove;
                    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + gBattlerTarget * 4 + 1 + (gBattlerAttacker / 2) * 2] = gChosenMove >> 8;
                }
                else if (gBattlerAttacker != gBattlerTarget)
                {
                    gSharedMem[BSTRUCT_OFF(lastTakenMove) + gBattlerTarget * 2 + 0] = 0;
                    gSharedMem[BSTRUCT_OFF(lastTakenMove) + gBattlerTarget * 2 + 1] = 0;

                    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + gBattlerTarget * 4 + 0 + (gBattlerAttacker / 2) * 2] = 0;
                    gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + gBattlerTarget * 4 + 1 + (gBattlerAttacker / 2) * 2] = 0;
                }
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_NEXT_TARGET:
            if (!(gHitMarker & HITMARKER_UNABLE_TO_USE_MOVE) && gBattleTypeFlags & BATTLE_TYPE_DOUBLE
                && !gProtectStructs[gBattlerAttacker].chargingTurn && gBattleMoves[gCurrentMove].target == MOVE_TARGET_BOTH
                && !(gHitMarker & HITMARKER_NO_ATTACKSTRING))
            {
                u8 battlerId = GetBattlerAtPosition(BATTLE_PARTNER(GetBattlerPosition(gBattlerTarget)));
                if (gBattleMons[battlerId].hp != 0)
                {
                    gBattlerTarget = battlerId;
                    gHitMarker |= HITMARKER_NO_ATTACKSTRING;
                    gBattleStruct->moveendState = MOVEEND_RAGE;
                    MoveValuesCleanUp();
                    BattleScriptPush(gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect]);
                    gBattlescriptCurrInstr = BattleScript_FlushMessageBox;
                    return;
                }
                else
                {
                    gHitMarker |= HITMARKER_NO_ATTACKSTRING;
                }
            }
            gBattleStruct->moveendState++;
            break;
        case MOVEEND_COUNT:
            break;
        }

        if (endMode == 1 && effect == FALSE)
            gBattleStruct->moveendState = MOVEEND_COUNT;
        if (endMode == 2 && endState == gBattleStruct->moveendState)
            gBattleStruct->moveendState = MOVEEND_COUNT;

    } while (gBattleStruct->moveendState != MOVEEND_COUNT && effect == FALSE);

    if (gBattleStruct->moveendState == MOVEEND_COUNT && effect == FALSE)
        gBattlescriptCurrInstr += 3;
}

static void Cmd_typecalc2(void)
{
    u8 flags = 0;
    s32 i = 0;
    u8 moveType = gBattleMoves[gCurrentMove].type;

    if (gBattleMons[gBattlerTarget].ability == ABILITY_LEVITATE && moveType == TYPE_GROUND)
    {
        gLastUsedAbility = gBattleMons[gBattlerTarget].ability;
        gMoveResultFlags |= (MOVE_RESULT_MISSED | MOVE_RESULT_DOESNT_AFFECT_FOE);
        gLastLandedMoves[gBattlerTarget] = MOVE_NONE;
        gBattleCommunication[MISS_TYPE] = moveType;
        RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
    }
    else
    {
        while (TYPE_EFFECT_ATK_TYPE(i)!= TYPE_ENDTABLE)
        {
            if (TYPE_EFFECT_ATK_TYPE(i) == TYPE_FORESIGHT)
            {
                if (gBattleMons[gBattlerTarget].status2 & STATUS2_FORESIGHT) {break;}
                else {i += 3; continue;}
            }

            if (TYPE_EFFECT_ATK_TYPE(i) == moveType)
            {
                //check type1
                if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type1)
                {
                    if (TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_NO_EFFECT)
                    {
                        gMoveResultFlags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
                        break;
                    }
                    if (TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_NOT_EFFECTIVE)
                        flags |= MOVE_RESULT_NOT_VERY_EFFECTIVE;
                    if (TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_SUPER_EFFECTIVE)
                        flags |= MOVE_RESULT_SUPER_EFFECTIVE;
                }
                //check type2
                if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type2)
                {
                    if (gBattleMons[gBattlerTarget].type1 != gBattleMons[gBattlerTarget].type2
                        && TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_NO_EFFECT)
                    {
                        gMoveResultFlags |= MOVE_RESULT_DOESNT_AFFECT_FOE;
                        break;
                    }
                    if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type2 && gBattleMons[gBattlerTarget].type1 != gBattleMons[gBattlerTarget].type2 && TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_NOT_EFFECTIVE)
                        flags |= MOVE_RESULT_NOT_VERY_EFFECTIVE;
                    if (TYPE_EFFECT_DEF_TYPE(i) == gBattleMons[gBattlerTarget].type2
                        && gBattleMons[gBattlerTarget].type1 != gBattleMons[gBattlerTarget].type2 && TYPE_EFFECT_MULTIPLIER(i) == TYPE_MUL_SUPER_EFFECTIVE)
                            flags |= MOVE_RESULT_SUPER_EFFECTIVE;
                }
            }
            i += 3;
        }
    }

    if (gBattleMons[gBattlerTarget].ability == ABILITY_WONDER_GUARD && !(flags & MOVE_RESULT_NO_EFFECT) && AttacksThisTurn(gBattlerAttacker, gCurrentMove) == 2 &&
        (!(flags & MOVE_RESULT_SUPER_EFFECTIVE) || ((flags & (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE)) == (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE))) &&
        gBattleMoves[gCurrentMove].power)
    {
        gLastUsedAbility = ABILITY_WONDER_GUARD;
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gLastLandedMoves[gBattlerTarget] = MOVE_NONE;
        gBattleCommunication[MISS_TYPE] = B_MSG_AVOIDED_DMG;
        RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
    }
    if (gMoveResultFlags & MOVE_RESULT_DOESNT_AFFECT_FOE)
        gProtectStructs[gBattlerAttacker].targetNotAffected = TRUE;

    gBattlescriptCurrInstr++;
}

static void Cmd_returnatktoball(void)
{
    gActiveBattler = gBattlerAttacker;
    if (!(gHitMarker & HITMARKER_FAINTED(gActiveBattler)))
    {
        BtlController_EmitReturnMonToBall(B_COMM_TO_CONTROLLER, FALSE);
        MarkBattlerForControllerExec(gActiveBattler);
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_getswitchedmondata(void)
{
    if (gBattleControllerExecFlags)
        return;

    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));

    gBattlerPartyIndexes[gActiveBattler] = gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + gActiveBattler];

    BtlController_EmitGetMonData(B_COMM_TO_CONTROLLER, REQUEST_ALL_BATTLE, gBitTable[gBattlerPartyIndexes[gActiveBattler]]);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 2;
}

static void atk4D_switchindataupdate(void)
{
    struct BattlePokemon oldData;
    s32 i;
    u8 *monData;

    if (gBattleControllerExecFlags)
        return;

    gActiveBattler = GetBattlerForBattleScript(gBattlescriptCurrInstr[1]);
    oldData = gBattleMons[gActiveBattler];
    monData = (u8*)(&gBattleMons[gActiveBattler]);

    for (i = 0; i < sizeof(struct BattlePokemon); i++)
    {
        monData[i] = gBattleBufferB[gActiveBattler][4 + i];
    }

    gBattleMons[gActiveBattler].type1 = gSpeciesInfo[gBattleMons[gActiveBattler].species].type1;
    gBattleMons[gActiveBattler].type2 = gSpeciesInfo[gBattleMons[gActiveBattler].species].type2;
    gBattleMons[gActiveBattler].ability = GetAbilityBySpecies(gBattleMons[gActiveBattler].species, gBattleMons[gActiveBattler].abilityNum);

    // check knocked off item
    i = GetBattlerSide(gActiveBattler);
    if (gWishFutureKnock.knockedOffMons[i] & gBitTable[gBattlerPartyIndexes[gActiveBattler]])
    {
        gBattleMons[gActiveBattler].item = ITEM_NONE;
    }

    if (gBattleMoves[gCurrentMove].effect == EFFECT_BATON_PASS)
    {
        for (i = 0; i < NUM_BATTLE_STATS; i++)
        {
            gBattleMons[gActiveBattler].statStages[i] = oldData.statStages[i];
        }
        gBattleMons[gActiveBattler].status2 = oldData.status2;
    }

    SwitchInClearSetData();

    gBattleStruct->battler = gActiveBattler;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gActiveBattler, gBattlerPartyIndexes[gActiveBattler]);

    gBattlescriptCurrInstr += 2;
}

static void atk4E_switchinanim(void)
{
    if (gBattleControllerExecFlags)
        return;

    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    if (GetBattlerSide(gActiveBattler) == B_SIDE_OPPONENT && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_BATTLE_TOWER)))
    {
        GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[gActiveBattler].species), FLAG_SET_SEEN);
    }
    gAbsentBattlerFlags &= ~(gBitTable[gActiveBattler]);
    BtlController_EmitSwitchInAnim(B_COMM_TO_CONTROLLER, gBattlerPartyIndexes[gActiveBattler], T2_READ_8(gBattlescriptCurrInstr + 2));
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 3;
}

static void atk4F_jumpifcantswitch(void)
{
    s32 i;
    int to_cmp;
    register struct Pokemon *party;
    u8 battlerIn1;
    //0x80 byte is used as a way of telling the function whether to not check status2/status3
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1) & (u8)~SWITCH_IGNORE_ESCAPE_PREVENTION);
    if (!(T2_READ_8(gBattlescriptCurrInstr + 1) & SWITCH_IGNORE_ESCAPE_PREVENTION)
        && ((gBattleMons[gActiveBattler].status2 & (STATUS2_WRAPPED | STATUS2_ESCAPE_PREVENTION))
            || (gStatuses3[gActiveBattler] & STATUS3_ROOTED)))
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 2);
        return;
    }
    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        if (GetBattlerSide(gActiveBattler) == B_SIDE_OPPONENT)
            party = gEnemyParty;
        else
            party = gPlayerParty;
        i = 0;
        if (GetLinkTrainerFlankId(GetBattlerMultiplayerId(gActiveBattler)) == 1)
            i = MULTI_PARTY_SIZE;
        for (to_cmp = i + MULTI_PARTY_SIZE; i < to_cmp; i++)
        {
            if (GetMonData(&party[i], MON_DATA_SPECIES) != SPECIES_NONE
             && !GetMonData(&party[i], MON_DATA_IS_EGG)
             && GetMonData(&party[i], MON_DATA_HP) != 0
             && gBattlerPartyIndexes[gActiveBattler] != i)
                break;
        }
        if (i == to_cmp)
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 2);
        else
            gBattlescriptCurrInstr += 6;
    }
    else
    {
        if (GetBattlerSide(gActiveBattler) == B_SIDE_OPPONENT)
        {
            battlerIn1 = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                to_cmp = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
            else
                to_cmp = battlerIn1;
            party = gEnemyParty;
        }
        else
        {
            battlerIn1 = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                to_cmp = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
            else
                to_cmp = battlerIn1;
            party = gPlayerParty;
        }
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (GetMonData(&party[i], MON_DATA_HP) != 0
             && GetMonData(&party[i], MON_DATA_SPECIES) != SPECIES_NONE
             && !GetMonData(&party[i], MON_DATA_IS_EGG)
             && i != gBattlerPartyIndexes[battlerIn1] && i != gBattlerPartyIndexes[to_cmp])
                break;
        }
        if (i == PARTY_SIZE)
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 2);
        else
            gBattlescriptCurrInstr += 6;
    }
}

void sub_8022A3C(u8 unkown)
{
    gSharedMem[BSTRUCT_OFF(battlerPartyIndexes) + gActiveBattler] = gBattlerPartyIndexes[gActiveBattler];
    BtlController_EmitChoosePokemon(B_COMM_TO_CONTROLLER, PARTY_ACTION_SEND_OUT, unkown, ABILITY_NONE, gBattleStruct->battlerPartyOrders[gActiveBattler]);
    MarkBattlerForControllerExec(gActiveBattler);
}


static void atk50_openpartyscreen(void)
{
    u32 flags;
    u8 hitmarkerFaintBits;
    u8 battlerId;
    const u8 *jumpPtr;

    battlerId = 0;
    flags = 0;
    jumpPtr = T1_READ_PTR(gBattlescriptCurrInstr + 2);

    if (gBattlescriptCurrInstr[1] == BS_FAINTED_LINK_MULTIPLE_1)
    {
        if ((gBattleTypeFlags & (BATTLE_TYPE_DOUBLE | BATTLE_TYPE_MULTI)) != BATTLE_TYPE_DOUBLE)
        {
            for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
            {
                if (gHitMarker & HITMARKER_FAINTED(gActiveBattler))
                {
                    if (HasNoMonsToSwitch(gActiveBattler, PARTY_SIZE, PARTY_SIZE))
                    {
                        gAbsentBattlerFlags |= gBitTable[gActiveBattler];
                        gHitMarker &= ~(HITMARKER_FAINTED(gActiveBattler));
                        BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                        MarkBattlerForControllerExec(gActiveBattler);
                    }
                    else if (!gSpecialStatuses[gActiveBattler].faintedHasReplacement)
                    {
                        sub_8022A3C(PARTY_SIZE);
                        gSpecialStatuses[gActiveBattler].faintedHasReplacement = TRUE;
                    }
                }
                else
                {
                    BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
            }
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
        {
            bool8 hasReplacement_0, hasReplacement_1, hasReplacement_2, hasReplacement_3;

            hitmarkerFaintBits = gHitMarker >> 28;

            if (gBitTable[0] & hitmarkerFaintBits)
            {
                gActiveBattler = 0;
                if (HasNoMonsToSwitch(0, PARTY_SIZE, PARTY_SIZE))
                {
                    gAbsentBattlerFlags |= gBitTable[gActiveBattler];
                    gHitMarker &= ~(HITMARKER_FAINTED(gActiveBattler));
                    BtlController_EmitCantSwitch(B_COMM_TO_CONTROLLER);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
                else if (!gSpecialStatuses[gActiveBattler].faintedHasReplacement)
                {
                    sub_8022A3C(gBattleStruct->monToSwitchIntoId[2]);
                    gSpecialStatuses[gActiveBattler].faintedHasReplacement = TRUE;
                }
                else
                {
                    BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                    MarkBattlerForControllerExec(gActiveBattler);
                    flags |= 1;
                }
            }
            if (gBitTable[2] & hitmarkerFaintBits && !(gBitTable[0] & hitmarkerFaintBits))
            {
                gActiveBattler = 2;
                if (HasNoMonsToSwitch(2, PARTY_SIZE, PARTY_SIZE))
                {
                    gAbsentBattlerFlags |= gBitTable[gActiveBattler];
                    gHitMarker &= ~(HITMARKER_FAINTED(gActiveBattler));
                    BtlController_EmitCantSwitch(B_COMM_TO_CONTROLLER);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
                else if (!gSpecialStatuses[gActiveBattler].faintedHasReplacement)
                {
                    sub_8022A3C(gBattleStruct->monToSwitchIntoId[0]);
                    gSpecialStatuses[gActiveBattler].faintedHasReplacement = TRUE;
                }
                else if (!(flags & 1))
                {
                    BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
            }
            if (gBitTable[1] & hitmarkerFaintBits)
            {
                gActiveBattler = 1;
                if (HasNoMonsToSwitch(1, PARTY_SIZE, PARTY_SIZE))
                {
                    gAbsentBattlerFlags |= gBitTable[gActiveBattler];
                    gHitMarker &= ~(HITMARKER_FAINTED(gActiveBattler));
                    BtlController_EmitCantSwitch(B_COMM_TO_CONTROLLER);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
                else if (!gSpecialStatuses[gActiveBattler].faintedHasReplacement)
                {
                    sub_8022A3C(gBattleStruct->monToSwitchIntoId[3]);
                    gSpecialStatuses[gActiveBattler].faintedHasReplacement = TRUE;
                }
                else
                {
                    BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                    MarkBattlerForControllerExec(gActiveBattler);
                    flags |= 2;
                }
            }
            if (gBitTable[3] & hitmarkerFaintBits && !(gBitTable[1] & hitmarkerFaintBits))
            {
                gActiveBattler = 3;
                if (HasNoMonsToSwitch(3, PARTY_SIZE, PARTY_SIZE))
                {
                    gAbsentBattlerFlags |= gBitTable[gActiveBattler];
                    gHitMarker &= ~(HITMARKER_FAINTED(gActiveBattler));
                    BtlController_EmitCantSwitch(B_COMM_TO_CONTROLLER);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
                else if (!gSpecialStatuses[gActiveBattler].faintedHasReplacement)
                {
                    sub_8022A3C(gBattleStruct->monToSwitchIntoId[1]);
                    gSpecialStatuses[gActiveBattler].faintedHasReplacement = TRUE;
                }
                else if (!(flags & 2))
                {
                    BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
            }

            hasReplacement_0 = gSpecialStatuses[0].faintedHasReplacement;
            if (!hasReplacement_0)
            {
                hasReplacement_2 = gSpecialStatuses[2].faintedHasReplacement;
                if (!hasReplacement_2 && hitmarkerFaintBits != 0)
                {
                    if (gAbsentBattlerFlags & gBitTable[0])
                        gActiveBattler = 2;
                    else
                        gActiveBattler = 0;

                    BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                    MarkBattlerForControllerExec(gActiveBattler);
                }

            }
            hasReplacement_1 = gSpecialStatuses[1].faintedHasReplacement;
            if (!hasReplacement_1)
            {
                hasReplacement_3 = gSpecialStatuses[3].faintedHasReplacement;
                if (!hasReplacement_3 && hitmarkerFaintBits != 0)
                {
                    if (gAbsentBattlerFlags & gBitTable[1])
                        gActiveBattler = 3;
                    else
                        gActiveBattler = 1;

                    BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                    MarkBattlerForControllerExec(gActiveBattler);
                }
            }
        }
        gBattlescriptCurrInstr += 6;
    }
    else if (gBattlescriptCurrInstr[1] == BS_FAINTED_LINK_MULTIPLE_2)
    {
        if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
            {
                hitmarkerFaintBits = gHitMarker >> 28;
                if (gBitTable[2] & hitmarkerFaintBits && gBitTable[0] & hitmarkerFaintBits)
                {
                    gActiveBattler = 2;
                    if (HasNoMonsToSwitch(2, gBattleBufferB[0][1], PARTY_SIZE))
                    {
                        gAbsentBattlerFlags |= gBitTable[gActiveBattler];
                        gHitMarker &= ~(HITMARKER_FAINTED(gActiveBattler));
                        BtlController_EmitCantSwitch(B_COMM_TO_CONTROLLER);
                        MarkBattlerForControllerExec(gActiveBattler);
                    }
                    else if (!gSpecialStatuses[gActiveBattler].faintedHasReplacement)
                    {
                        sub_8022A3C(gBattleStruct->monToSwitchIntoId[0]);
                        gSpecialStatuses[gActiveBattler].faintedHasReplacement = TRUE;
                    }
                }
                if (gBitTable[3] & hitmarkerFaintBits && hitmarkerFaintBits & gBitTable[1])
                {
                    gActiveBattler = 3;
                    if (HasNoMonsToSwitch(3, gBattleBufferB[1][1], PARTY_SIZE))
                    {
                        gAbsentBattlerFlags |= gBitTable[gActiveBattler];
                        gHitMarker &= ~(HITMARKER_FAINTED(gActiveBattler));
                        BtlController_EmitCantSwitch(B_COMM_TO_CONTROLLER);
                        MarkBattlerForControllerExec(gActiveBattler);
                    }
                    else if (!gSpecialStatuses[gActiveBattler].faintedHasReplacement)
                    {
                        sub_8022A3C(gBattleStruct->monToSwitchIntoId[1]);
                        gSpecialStatuses[gActiveBattler].faintedHasReplacement = TRUE;
                    }
                }
                gBattlescriptCurrInstr += 6;
            }
            else
            {
                gBattlescriptCurrInstr += 6;
            }
        }
        else
        {
            gBattlescriptCurrInstr += 6;
        }

        hitmarkerFaintBits = gHitMarker >> 28;

        gBattlerFainted = 0;
        while (!(gBitTable[gBattlerFainted] & hitmarkerFaintBits)
               && gBattlerFainted < gBattlersCount)
            gBattlerFainted++;

        if (gBattlerFainted == gBattlersCount)
            gBattlescriptCurrInstr = jumpPtr;
    }
    else
    {
        if (gBattlescriptCurrInstr[1] & PARTY_SCREEN_OPTIONAL)
            hitmarkerFaintBits = PARTY_ACTION_CHOOSE_MON; // Used here as the caseId for the EmitChoose function.
        else
            hitmarkerFaintBits = PARTY_ACTION_SEND_OUT;

        battlerId = GetBattlerForBattleScript(gBattlescriptCurrInstr[1] & ~(PARTY_SCREEN_OPTIONAL));
        if (gSpecialStatuses[battlerId].faintedHasReplacement)
        {
            gBattlescriptCurrInstr += 6;
        }
        else if (HasNoMonsToSwitch(battlerId, PARTY_SIZE, PARTY_SIZE))
        {
            gActiveBattler = battlerId;
            gAbsentBattlerFlags |= gBitTable[gActiveBattler];
            gHitMarker &= ~(HITMARKER_FAINTED(gActiveBattler));
            gBattlescriptCurrInstr = jumpPtr;
        }
        else
        {
            gActiveBattler = battlerId;
            gSharedMem[BSTRUCT_OFF(battlerPartyIndexes) + gActiveBattler] = gBattlerPartyIndexes[gActiveBattler];
            BtlController_EmitChoosePokemon(B_COMM_TO_CONTROLLER, hitmarkerFaintBits, gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + (gActiveBattler ^ BIT_FLANK)], ABILITY_NONE, gBattleStruct->battlerPartyOrders[gActiveBattler]);
            MarkBattlerForControllerExec(gActiveBattler);

            gBattlescriptCurrInstr += 6;

            if (GetBattlerPosition(gActiveBattler) == B_POSITION_PLAYER_LEFT && gBattleResults.totalMonSwitchCounter < UCHAR_MAX)
                gBattleResults.totalMonSwitchCounter++;

            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
                {
                    if (gActiveBattler != battlerId)
                    {
                        BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                        MarkBattlerForControllerExec(gActiveBattler);
                    }
                }
            }
            else
            {
                gActiveBattler = GetBattlerAtPosition(GetBattlerPosition(battlerId) ^ BIT_SIDE);
                if (gAbsentBattlerFlags & gBitTable[gActiveBattler])
                    gActiveBattler ^= BIT_FLANK;

                BtlController_EmitLinkStandbyMsg(B_COMM_TO_CONTROLLER, LINK_STANDBY_MSG_ONLY);
                MarkBattlerForControllerExec(gActiveBattler);
            }
        }
    }
}

static void atk51_switchhandleorder(void)
{
    s32 i;
    if (gBattleControllerExecFlags)
        return;

    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    switch (T2_READ_8(gBattlescriptCurrInstr + 2))
    {
    case 0:
        for (i = 0; i < gBattlersCount; i++)
        {
            if (gBattleBufferB[i][0] == CONTROLLER_CHOSENMONRETURNVALUE)
                gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + i] = gBattleBufferB[i][1];
        }
        break;
    case 1:
        if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI))
            SwitchPartyOrder(gActiveBattler);
        break;
    case 2:
        gBattleCommunication[MULTIUSE_STATE] = gBattleBufferB[gActiveBattler][1];
        gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + gActiveBattler] = gBattleBufferB[gActiveBattler][1];
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        {
            gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0] &= 0xF;
            gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 0] |= (gBattleBufferB[gActiveBattler][2] & 0xF0);
            gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * gActiveBattler + 1] = gBattleBufferB[gActiveBattler][3];
            gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * (gActiveBattler ^ BIT_FLANK) + 0] &= (0xF0);
            gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * (gActiveBattler ^ BIT_FLANK) + 0] |= (gBattleBufferB[gActiveBattler][2] & 0xF0) >> 4;
            gSharedMem[BSTRUCT_OFF(battlerPartyOrders) + 3 * (gActiveBattler ^ BIT_FLANK) + 2] = gBattleBufferB[gActiveBattler][3];
        }
        else
            SwitchPartyOrder(gActiveBattler);

        PREPARE_SPECIES_BUFFER(gBattleTextBuff1, gBattleMons[gBattlerAttacker].species);

        PREPARE_MON_NICK_BUFFER(gBattleTextBuff2, gActiveBattler, gBattleBufferB[gActiveBattler][1]);
        break;
    }
    gBattlescriptCurrInstr += 3;
}

static void atk52_switchineffects(void)
{
    s32 i;

    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    UpdateSentPokesToOpponentValue(gActiveBattler);
    gHitMarker &= ~(HITMARKER_FAINTED(gActiveBattler));
    gSpecialStatuses[gActiveBattler].faintedHasReplacement = FALSE;

    if (!(gSideStatuses[GetBattlerSide(gActiveBattler)] & SIDE_STATUS_SPIKES_DAMAGED) && (gSideStatuses[GetBattlerSide(gActiveBattler)] & SIDE_STATUS_SPIKES)
        && gBattleMons[gActiveBattler].type1 != TYPE_FLYING && gBattleMons[gActiveBattler].type2 != TYPE_FLYING && gBattleMons[gActiveBattler].ability != ABILITY_LEVITATE)
    {
        u8 spikesDmg;

        gSideStatuses[GetBattlerSide(gActiveBattler)] |= SIDE_STATUS_SPIKES_DAMAGED;

        spikesDmg = (5 - gSideTimers[GetBattlerSide(gActiveBattler)].spikesAmount) * 2;
        gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / (spikesDmg);
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;

        gBattleStruct->battler = gActiveBattler;
        BattleScriptPushCursor();

        if (T2_READ_8(gBattlescriptCurrInstr + 1) == BS_TARGET)
            gBattlescriptCurrInstr = BattleScript_SpikesOnTarget;
        else if (T2_READ_8(gBattlescriptCurrInstr + 1) == BS_ATTACKER)
            gBattlescriptCurrInstr = BattleScript_SpikesOnAttacker;
        else
            gBattlescriptCurrInstr = BattleScript_SpikesOnFaintedBattler;
    }
    else
    {
        if (gBattleMons[gActiveBattler].ability == ABILITY_TRUANT)
        {
            gDisableStructs[gActiveBattler].truantCounter = TRUE;
        }

        if (AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, gActiveBattler, 0, 0, 0) == 0 && ItemBattleEffects(ITEMEFFECT_ON_SWITCH_IN, gActiveBattler, FALSE) == 0)
        {
            gSideStatuses[GetBattlerSide(gActiveBattler)] &= ~(SIDE_STATUS_SPIKES_DAMAGED);

            for (i = 0; i < gBattlersCount; i++)
            {
                if (gBattlerByTurnOrder[i] == gActiveBattler)
                    gActionsByTurnOrder[i] = B_ACTION_FINISHED;
            }

            for (i = 0; i < gBattlersCount; i++)
            {
                *((u16 *)&gSharedMem[BSTRUCT_OFF(hpOnSwitchout) + 2 * GetBattlerSide(i)]) = gBattleMons[i].hp;
            }

            if (T2_READ_8(gBattlescriptCurrInstr + 1) == BS_FAINTED_LINK_MULTIPLE_1)
            {
                u32 hitmarkerFaintBits = gHitMarker >> 0x1C;
                gBattlerFainted++;
                while (1)
                {
                    if (hitmarkerFaintBits & gBitTable[gBattlerFainted] && !(gAbsentBattlerFlags & gBitTable[gBattlerFainted]))
                        break;
                    if (gBattlerFainted >= gBattlersCount)
                        break;
                    gBattlerFainted++;
                }
            }
            gBattlescriptCurrInstr += 2;
        }
    }
}

static void atk53_trainerslidein(void)
{
    if (!T2_READ_8(gBattlescriptCurrInstr + 1))
        gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
    else
        gActiveBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

    BtlController_EmitTrainerSlide(B_COMM_TO_CONTROLLER);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 2;
}

static void Cmd_playse(void)
{
    gActiveBattler = gBattlerAttacker;
    BtlController_EmitPlaySE(B_COMM_TO_CONTROLLER, T2_READ_16(gBattlescriptCurrInstr + 1));
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 3;
}

static void atk55_fanfare(void)
{
    gActiveBattler = gBattlerAttacker;
    BtlController_EmitPlayFanfareOrBGM(B_COMM_TO_CONTROLLER, T2_READ_16(gBattlescriptCurrInstr + 1));
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 3;
}

static void Cmd_playfaintcry(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    BtlController_EmitFaintingCry(B_COMM_TO_CONTROLLER);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 2;
}

static void atk57(void)
{
    gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
    BtlController_EmitCmd55(B_COMM_TO_CONTROLLER, gBattleOutcome);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 1;
}

static void Cmd_returntoball(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    BtlController_EmitReturnMonToBall(B_COMM_TO_CONTROLLER, TRUE);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 2;
}

static void Cmd_handlelearnnewmove(void)
{
    u8* learnedMovePtr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    u8* nothingToLearnPtr = T1_READ_PTR(gBattlescriptCurrInstr + 5);

    u16 learnMove = MonTryLearningNewMove(&gPlayerParty[gBattleStruct->expGetterMonId], T2_READ_8(gBattlescriptCurrInstr + 9));
    while (learnMove == MON_ALREADY_KNOWS_MOVE)
        learnMove = MonTryLearningNewMove(&gPlayerParty[gBattleStruct->expGetterMonId], 0);

    if (learnMove == 0)
    {
        gBattlescriptCurrInstr = nothingToLearnPtr;
    }
    else if (learnMove == MON_HAS_MAX_MOVES)
    {
        gBattlescriptCurrInstr += 10;
    }
    else
    {
        gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        if (gBattlerPartyIndexes[gActiveBattler] == gBattleStruct->expGetterMonId && !(gBattleMons[gActiveBattler].status2 & STATUS2_TRANSFORMED))
            GiveMoveToBattleMon(&gBattleMons[gActiveBattler], learnMove);
        if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) //what is else if
        {
            gActiveBattler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
            if (gBattlerPartyIndexes[gActiveBattler] == gBattleStruct->expGetterMonId && !(gBattleMons[gActiveBattler].status2 & STATUS2_TRANSFORMED))
                GiveMoveToBattleMon(&gBattleMons[gActiveBattler], learnMove);
        }
        gBattlescriptCurrInstr = learnedMovePtr;
    }
}

void sub_8023A80(void)
{
    sub_802BBD4(0x18, 8, 0x1D, 0xD, FALSE);
    Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gText_BattleYesNoChoice, 0x100, 0x19, 0x9);
    Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    MenuCursor_Create814A5C0(0, 0xFFFF, 0xC, 0x2D9F, 0x20);
}

void sub_8023AD8(void)
{
    sub_802BBD4(0x18, 8, 0x1D, 0xD, TRUE);
    DestroyMenuCursor();
}

static void atk5A_yesnoboxlearnmove(void)
{
    gActiveBattler = 0;
    switch (gBattleStruct->learnMoveState)
    {
    case 0:
        sub_8023A80();
        gBattleStruct->learnMoveState++;
        gBattleCommunication[CURSOR_POSITION] = 0;
        sub_802BC6C();
        break;
    case 1:
        if (JOY_NEW(DPAD_UP) && gBattleCommunication[CURSOR_POSITION] != 0)
        {
            PlaySE(SE_SELECT);
            nullsub_6();
            gBattleCommunication[CURSOR_POSITION] = 0;
            sub_802BC6C();
        }
        if (JOY_NEW(DPAD_DOWN) && gBattleCommunication[CURSOR_POSITION] == 0)
        {
            PlaySE(SE_SELECT);
            nullsub_6();
            gBattleCommunication[CURSOR_POSITION] = 1;
            sub_802BC6C();
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (gBattleCommunication[CURSOR_POSITION] == 0)
            {
                sub_8023AD8();
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gBattleStruct->learnMoveState++;
                return;
            }
            goto state_tracker_4;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
          state_tracker_4:
            gBattleStruct->learnMoveState = 4;
        }
        break;
    case 2:
        if (!gPaletteFade.active)
        {
            ShowSelectMovePokemonSummaryScreen(gPlayerParty, gBattleStruct->expGetterMonId, gPlayerPartyCount - 1, ReshowBattleScreenAfterMenu, gMoveToLearn);
            gBattleStruct->learnMoveState++;
        }
        break;
    case 3:
        if (!gPaletteFade.active && gMain.callback2 == BattleMainCB2)
        {
            u8 movePosition = GetMoveSlotToReplace();
            if (movePosition == MAX_MON_MOVES)
            {
                gBattleStruct->learnMoveState = 4;
            }
            else
            {
                u16 move = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], MON_DATA_MOVE1 + movePosition);
                if (IsHMMove2(move))
                {
                    PrepareStringBattle(STRINGID_HMMOVESCANTBEFORGOTTEN, gActiveBattler);
                    gBattleStruct->learnMoveState = 5;
                }
                else
                {
                    gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
                    PREPARE_MOVE_BUFFER(gBattleTextBuff2, move);
                    RemoveMonPPBonus(&gPlayerParty[gBattleStruct->expGetterMonId], movePosition);
                    SetMonMoveSlot(&gPlayerParty[gBattleStruct->expGetterMonId], gMoveToLearn, movePosition);
                    if (gBattlerPartyIndexes[0] == gBattleStruct->expGetterMonId && !(gBattleMons[0].status2 & STATUS2_TRANSFORMED)
                        && !(gDisableStructs[0].mimickedMoves & gBitTable[movePosition]))
                    {
                        RemoveBattleMonPPBonus(&gBattleMons[0], movePosition);
                        SetBattleMonMoveSlot(&gBattleMons[0], gMoveToLearn, movePosition);
                    }
                    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && gBattlerPartyIndexes[2] == gBattleStruct->expGetterMonId && !(gBattleMons[2].status2 & STATUS2_TRANSFORMED)
                        && !(gDisableStructs[2].mimickedMoves & gBitTable[movePosition]))
                    {
                        RemoveBattleMonPPBonus(&gBattleMons[2], movePosition);
                        SetBattleMonMoveSlot(&gBattleMons[2], gMoveToLearn, movePosition);
                    }
                }
            }
        }
        break;
    case 4:
        sub_8023AD8();
        gBattlescriptCurrInstr += 5;
        break;
    case 5:
        if (gBattleControllerExecFlags == 0)
        {
            gBattleStruct->learnMoveState = 2;
        }
        break;
    }
}

static void atk5B_yesnoboxstoplearningmove(void)
{
    switch (gBattleStruct->learnMoveState)
    {
    case 0:
        sub_8023A80();
        gBattleStruct->learnMoveState++;
        gBattleCommunication[CURSOR_POSITION] = 0;
        sub_802BC6C();
        break;
    case 1:
        if (JOY_NEW(DPAD_UP) && gBattleCommunication[CURSOR_POSITION] != 0)
        {
            PlaySE(SE_SELECT);
            nullsub_6();
            gBattleCommunication[CURSOR_POSITION] = 0;
            sub_802BC6C();
        }
        if (JOY_NEW(DPAD_DOWN) && gBattleCommunication[CURSOR_POSITION] == 0)
        {
            PlaySE(SE_SELECT);
            nullsub_6();
            gBattleCommunication[CURSOR_POSITION] = 1;
            sub_802BC6C();
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (gBattleCommunication[CURSOR_POSITION] != 0)
                gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
            else
                gBattlescriptCurrInstr += 5;
            sub_8023AD8();
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
            sub_8023AD8();
        }
    }
}

static void Cmd_hitanimation(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
        gBattlescriptCurrInstr += 2;
    else if (!(gHitMarker & HITMARKER_IGNORE_SUBSTITUTE) || !(gBattleMons[gActiveBattler].status2 & STATUS2_SUBSTITUTE) || gDisableStructs[gActiveBattler].substituteHP == 0)
    {
        BtlController_EmitHitAnimation(B_COMM_TO_CONTROLLER);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 2;
    }
    else
        gBattlescriptCurrInstr += 2;
}

static void atk5D_getmoneyreward(void)
{
    u32 i = 0;
    u8 lastMonLevel = 0;
    u32 moneyReward;

    const struct TrainerMonNoItemDefaultMoves * party1;
    const struct TrainerMonNoItemCustomMoves * party2;
    const struct TrainerMonItemDefaultMoves * party3;
    const struct TrainerMonItemCustomMoves * party4;

    if (gTrainerBattleOpponent_A == TRAINER_SECRET_BASE)
    {
        moneyReward = 20 * eSecretBaseRecord->party.levels[0] * gBattleStruct->moneyMultiplier;
    }
    else
    {
        switch(gTrainers[gTrainerBattleOpponent_A].partyFlags)
        {
        case 0:
        {
            party1 = gTrainers[gTrainerBattleOpponent_A].party.NoItemDefaultMoves;
            lastMonLevel = party1[gTrainers[gTrainerBattleOpponent_A].partySize - 1].level;
        }
            break;
        case F_TRAINER_PARTY_CUSTOM_MOVESET:
        {
            party2 = gTrainers[gTrainerBattleOpponent_A].party.NoItemCustomMoves;
            lastMonLevel = party2[gTrainers[gTrainerBattleOpponent_A].partySize - 1].level;
        }
            break;
        case F_TRAINER_PARTY_HELD_ITEM:
        {
            party3 = gTrainers[gTrainerBattleOpponent_A].party.ItemDefaultMoves;
            lastMonLevel = party3[gTrainers[gTrainerBattleOpponent_A].partySize - 1].level;
        }
            break;
        case F_TRAINER_PARTY_CUSTOM_MOVESET | F_TRAINER_PARTY_HELD_ITEM:
        {
            party4 = gTrainers[gTrainerBattleOpponent_A].party.ItemCustomMoves;
            lastMonLevel = party4[gTrainers[gTrainerBattleOpponent_A].partySize - 1].level;
        }
            break;
        }
        for (; gTrainerMoneyTable[i].trainerClass != 0xFF; i++) {
            if (gTrainerMoneyTable[i].trainerClass == gTrainers[gTrainerBattleOpponent_A].trainerClass)
                break;
        }
        party4 = gTrainers[gTrainerBattleOpponent_A].party.ItemCustomMoves; // Needed to match. Has no effect.
        moneyReward = 4 * lastMonLevel * gBattleStruct->moneyMultiplier * (gBattleTypeFlags & BATTLE_TYPE_DOUBLE ? 2 : 1) * gTrainerMoneyTable[i].baseMoney;
    }

    AddMoney(&gSaveBlock1.money, moneyReward);
    PREPARE_WORD_NUMBER_BUFFER(gBattleTextBuff1, 5, moneyReward);
    gBattlescriptCurrInstr += 1;
}

static void Cmd_updatebattlermoves(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        BtlController_EmitGetMonData(B_COMM_TO_CONTROLLER, REQUEST_ALL_BATTLE, 0);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 1:
         if (gBattleControllerExecFlags == 0)
         {
            s32 i;
            struct BattlePokemon* bufferPoke = (struct BattlePokemon*) &gBattleBufferB[gActiveBattler][4];
            for (i = 0; i < MAX_MON_MOVES; i++)
            {
                gBattleMons[gActiveBattler].moves[i] = bufferPoke->moves[i];
                gBattleMons[gActiveBattler].pp[i] = bufferPoke->pp[i];
            }
            gBattlescriptCurrInstr += 2;
         }
         break;
    }
}

static void Cmd_swapattackerwithtarget(void)
{
    gActiveBattler = gBattlerAttacker;
    gBattlerAttacker = gBattlerTarget;
    gBattlerTarget = gActiveBattler;
    //what is xor...
    if (gHitMarker & HITMARKER_SWAP_ATTACKER_TARGET)
        gHitMarker &= ~(HITMARKER_SWAP_ATTACKER_TARGET);
    else
        gHitMarker |= HITMARKER_SWAP_ATTACKER_TARGET;
    gBattlescriptCurrInstr++;
}

static void Cmd_incrementgamestat(void)
{
    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
    {
        IncrementGameStat(T2_READ_8(gBattlescriptCurrInstr + 1));
    }
    gBattlescriptCurrInstr += 2;
}

static void Cmd_drawpartystatussummary(void)
{
    s32 i;
    struct Pokemon* party;
    struct HpAndStatus hpStatuses[6];
    if (gBattleControllerExecFlags)
        return;

    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&party[i], MON_DATA_SPECIES_OR_EGG) == SPECIES_NONE || GetMonData(&party[i], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG)
        {
            hpStatuses[i].hp = HP_EMPTY_SLOT;
            hpStatuses[i].status = 0;
        }
        else
        {
            hpStatuses[i].hp = GetMonData(&party[i], MON_DATA_HP);
            hpStatuses[i].status = GetMonData(&party[i], MON_DATA_STATUS);
        }
    }
    BtlController_EmitDrawPartyStatusSummary(B_COMM_TO_CONTROLLER, hpStatuses, 1);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 2;
}

static void Cmd_hidepartystatussummary(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    BtlController_EmitHidePartyStatusSummary(B_COMM_TO_CONTROLLER);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 2;
}

static void Cmd_jumptocalledmove(void)
{
    if (T2_READ_8(gBattlescriptCurrInstr + 1))
        gCurrentMove = gCalledMove;
    else
        gChosenMove = gCurrentMove = gCalledMove;
    gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect];
}

static void Cmd_statusanimation(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
        if (!(gStatuses3[gActiveBattler] & STATUS3_SEMI_INVULNERABLE) && gDisableStructs[gActiveBattler].substituteHP == 0 && !(gHitMarker & HITMARKER_NO_ANIMATIONS))
        {
            BtlController_EmitStatusAnimation(B_COMM_TO_CONTROLLER, FALSE, gBattleMons[gActiveBattler].status1);
            MarkBattlerForControllerExec(gActiveBattler);
        }
        gBattlescriptCurrInstr += 2;
    }
}

static void Cmd_status2animation(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        u32 wantedToAnimate;
        gActiveBattler = GetBattlerForBattleScript(T1_READ_8(gBattlescriptCurrInstr + 1));
        wantedToAnimate = T1_READ_32(gBattlescriptCurrInstr + 2);
        if (!(gStatuses3[gActiveBattler] & STATUS3_SEMI_INVULNERABLE) && gDisableStructs[gActiveBattler].substituteHP == 0 && !(gHitMarker & HITMARKER_NO_ANIMATIONS))
        {
            BtlController_EmitStatusAnimation(B_COMM_TO_CONTROLLER, TRUE, gBattleMons[gActiveBattler].status2 & wantedToAnimate);
            MarkBattlerForControllerExec(gActiveBattler);
        }
        gBattlescriptCurrInstr += 6;
    }
}

static void Cmd_chosenstatusanimation(void)
{
    if (gBattleControllerExecFlags == 0)
    {
        u32 wantedStatus;
        gActiveBattler = GetBattlerForBattleScript(T1_READ_8(gBattlescriptCurrInstr + 1));
        wantedStatus = T1_READ_32(gBattlescriptCurrInstr + 3);
        if (!(gStatuses3[gActiveBattler] & STATUS3_SEMI_INVULNERABLE) && gDisableStructs[gActiveBattler].substituteHP == 0 && !(gHitMarker & HITMARKER_NO_ANIMATIONS))
        {
            BtlController_EmitStatusAnimation(B_COMM_TO_CONTROLLER, T1_READ_8(gBattlescriptCurrInstr + 2), wantedStatus);
            MarkBattlerForControllerExec(gActiveBattler);
        }
        gBattlescriptCurrInstr += 7;
    }
}

static void atk67_yesnobox(void)
{
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        sub_8023A80();
        gBattleCommunication[MULTIUSE_STATE]++;
        gBattleCommunication[CURSOR_POSITION] = 0;
        sub_802BC6C();
        break;
    case 1:
        if (JOY_NEW(DPAD_UP) && gBattleCommunication[CURSOR_POSITION] != 0)
        {
            PlaySE(SE_SELECT);
            nullsub_6();
            gBattleCommunication[CURSOR_POSITION] = 0;
            sub_802BC6C();
        }
        if (JOY_NEW(DPAD_DOWN) && gBattleCommunication[CURSOR_POSITION] == 0)
        {
            PlaySE(SE_SELECT);
            nullsub_6();
            gBattleCommunication[CURSOR_POSITION] = 1;
            sub_802BC6C();
        }
        if (JOY_NEW(B_BUTTON))
        {
            gBattleCommunication[CURSOR_POSITION] = 1;
            PlaySE(SE_SELECT);
            sub_8023AD8();
            gBattlescriptCurrInstr++;
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sub_8023AD8();
            gBattlescriptCurrInstr++;
        }
        break;
    }
}

static void Cmd_cancelallactions(void)
{
    s32 i;
    for (i = 0; i < gBattlersCount; i++)
    {
        gActionsByTurnOrder[i] = B_ACTION_FINISHED;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_adjustsetdamage(void) //literally a copy of atk07 except theres no rand dmg modifier...
{
    u8 holdEffect, param;
    if (gBattleMons[gBattlerTarget].item == ITEM_ENIGMA_BERRY)
        holdEffect = gEnigmaBerries[gBattlerTarget].holdEffect, param = gEnigmaBerries[gBattlerTarget].holdEffectParam;
    else
    {
        holdEffect = GetItemHoldEffect(gBattleMons[gBattlerTarget].item);
        param = GetItemHoldEffectParam(gBattleMons[gBattlerTarget].item);
    }

    gPotentialItemEffectBattler = gBattlerTarget;

    if (holdEffect == HOLD_EFFECT_FOCUS_BAND && (Random() % 100) < param)
    {
        RecordItemEffectBattle(gBattlerTarget, holdEffect);
        gSpecialStatuses[gBattlerTarget].focusBanded = TRUE;
    }
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_SUBSTITUTE)
        goto END;
    if (gBattleMoves[gCurrentMove].effect != EFFECT_FALSE_SWIPE && !gProtectStructs[gBattlerTarget].endured
     && !gSpecialStatuses[gBattlerTarget].focusBanded)
        goto END;
    if (gBattleMons[gBattlerTarget].hp > gBattleMoveDamage)
        goto END;

    gBattleMoveDamage = gBattleMons[gBattlerTarget].hp - 1;

    if (gProtectStructs[gBattlerTarget].endured)
    {
        gMoveResultFlags |= MOVE_RESULT_FOE_ENDURED;
        goto END;
    }
    if (gSpecialStatuses[gBattlerTarget].focusBanded)
    {
        gMoveResultFlags |= MOVE_RESULT_FOE_HUNG_ON;
        gLastUsedItem = gBattleMons[gBattlerTarget].item;
    }

    END:
        gBattlescriptCurrInstr++;
}

static void Cmd_removeitem(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    *((u16 *)&gSharedMem[BSTRUCT_OFF(usedHeldItems) + gActiveBattler * 2]) = gBattleMons[gActiveBattler].item;

    gBattleMons[gActiveBattler].item = ITEM_NONE;
    BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE, 0, sizeof(gBattleMons[gActiveBattler].item), &gBattleMons[gActiveBattler].item);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 2;
}

static void Cmd_atknameinbuff1(void)
{
    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker]);
    gBattlescriptCurrInstr++;
}

extern const u8 *const gStatNamesTable2[];

static void atk6C_drawlvlupbox(void)
{
    s16 r1 = 0;
    s16 r7 = 0;
    int i;
    u8 * r5;
    bool32 sign;
    switch (gBattleStruct->drawlvlupboxState)
    {
    case 0:
        sub_802BBD4(0xB, 0, 0x1D, 0x7, FALSE);
        r5 = StringCopy(gStringVar4, gText_BattleSwitchWhich2);
        for (i = 0; i < NUM_STATS; i++)
        {
            sign = FALSE;
            r5 = StringAppend(r5, gStatNamesTable2[i]);
            r1 = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], sRubyLevelUpStatBoxStats[i]);
            switch (i)
            {
            case 0:
                r7 = r1 - gBattleResources_statsBeforeLvlUp->hp;
                break;
            case 1:
                r7 = r1 - gBattleResources_statsBeforeLvlUp->spAtk;
                break;
            case 2:
                r7 = r1 - gBattleResources_statsBeforeLvlUp->atk;
                break;
            case 3:
                r7 = r1 - gBattleResources_statsBeforeLvlUp->spDef;
                break;
            case 4:
                r7 = r1 - gBattleResources_statsBeforeLvlUp->def;
                break;
            case 5:
                r7 = r1 - gBattleResources_statsBeforeLvlUp->spd;
                break;
            }
            if (r7 < 0)
            {
                r7 = -r7;
                sign++;
            }
            r5[0] = EXT_CTRL_CODE_BEGIN;
            r5[1] = EXT_CTRL_CODE_CLEAR_TO;
            r5[2] = ((i & 1) * 9 + 5) * 8 + 6;
            r5 += 3;
            if (sign)
                r5 = StringCopy(r5, gText_Dash);
            else
                r5 = StringCopy(r5, gText_Plus);
            r5[0] = EXT_CTRL_CODE_BEGIN;
            r5[1] = EXT_CTRL_CODE_MIN_LETTER_SPACING;
            r5[2] = 6;
            r5 += 3;
            r5 = ConvertIntToDecimalStringN(r5, r7, STR_CONV_MODE_RIGHT_ALIGN, 2);
            r5[0] = EXT_CTRL_CODE_BEGIN;
            r5[1] = EXT_CTRL_CODE_MIN_LETTER_SPACING;
            r5[2] = 0;
            r5 += 3;
            if (i & 1)
            {
                r5[0] = CHAR_NEWLINE;
                r5[1] = EOS;
                r5 += 1;
            }
            else
            {
                r5[0] = EXT_CTRL_CODE_BEGIN;
                r5[1] = EXT_CTRL_CODE_CLEAR;
                r5[2] = 8;
                r5 += 3;
                *r5 = EOS;
            }
        }
        Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gStringVar4, 0x100, 12, 1);
        Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
        gBattleStruct->drawlvlupboxState++;
        break;
    case 1:
        if (gMain.newKeys != 0)
        {
            PlaySE(SE_SELECT);
            r5 = StringCopy(gStringVar4, gText_BattleSwitchWhich2);
            for (i = 0; i < NUM_STATS; i++)
            {
                sign = FALSE;
                r5 = StringAppend(r5, gStatNamesTable2[i]);
                r1 = GetMonData(&gPlayerParty[gBattleStruct->expGetterMonId], sRubyLevelUpStatBoxStats[i]);
                r5[0] = EXT_CTRL_CODE_BEGIN;
                r5[1] = EXT_CTRL_CODE_CLEAR_TO;
                r5[2] = ((i & 1) * 9 + 5) * 8 + 6;
                r5 += 3;
                r5[0] = EXT_CTRL_CODE_BEGIN;
                r5[1] = EXT_CTRL_CODE_MIN_LETTER_SPACING;
                r5[2] = 6;
                r5 += 3;
                r5 = ConvertIntToDecimalStringN(r5, r1, STR_CONV_MODE_RIGHT_ALIGN, 3);
                r5[0] = EXT_CTRL_CODE_BEGIN;
                r5[1] = EXT_CTRL_CODE_MIN_LETTER_SPACING;
                r5[2] = 0;
                r5 += 3;
                if (i & 1)
                {
                    r5[0] = CHAR_NEWLINE;
                    r5[1] = EOS;
                    r5 += 1;
                }
                else
                {
                    r5[0] = EXT_CTRL_CODE_BEGIN;
                    r5[1] = EXT_CTRL_CODE_CLEAR;
                    r5[2] = 8;
                    r5 += 3;
                    *r5 = EOS;
                }
            }
            Text_InitWindow(&gWindowTemplate_Contest_MoveDescription, gStringVar4, 0x100, 12, 1);
            Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
            gBattleStruct->drawlvlupboxState++;
        }
        break;
    case 2:
        if (gMain.newKeys != 0)
        {
            PlaySE(SE_SELECT);
            sub_802BBD4(11, 0, 29, 7, TRUE);
            gBattleStruct->drawlvlupboxState++;
        }
        break;
    case 3:
        gBattlescriptCurrInstr++;
        break;
    }
}

static void Cmd_resetsentmonsvalue(void)
{
    ResetSentPokesToOpponentValue();
    gBattlescriptCurrInstr++;
}

static void Cmd_setatktoplayer0(void)
{
    gBattlerAttacker = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
    gBattlescriptCurrInstr++;
}

static void Cmd_makevisible(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    BtlController_EmitSpriteInvisibility(B_COMM_TO_CONTROLLER, FALSE);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr += 2;
}

static void Cmd_recordlastability(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    RecordAbilityBattle(gActiveBattler, gLastUsedAbility);
    gBattlescriptCurrInstr += 1; //buggy, should be += 2, one byte for command, one byte for argument...
}

void BufferMoveToLearnIntoBattleTextBuff2(void)
{
    PREPARE_MOVE_BUFFER(gBattleTextBuff2, gMoveToLearn);
}

static void Cmd_buffermovetolearn(void)
{
    BufferMoveToLearnIntoBattleTextBuff2();
    gBattlescriptCurrInstr++;
}

static void Cmd_jumpifplayerran(void)
{
    if (TryRunFromBattle(gBattlerFainted))
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
        gBattlescriptCurrInstr += 5;
}

static void Cmd_hpthresholds(void)
{
    u8 opposingBattler;
    s32 result;
    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
        opposingBattler = gActiveBattler ^ BIT_SIDE;

        result = gBattleMons[opposingBattler].hp * 100 / gBattleMons[opposingBattler].maxHP;
        if (result == 0)
            result = 1;

        if (result >= 70 || !gBattleMons[opposingBattler].hp)
            gBattleStruct->hpScale = 0;
        else if (result >= 40)
            gBattleStruct->hpScale = 1;
        else if (result >= 10)
            gBattleStruct->hpScale = 2;
        else
            gBattleStruct->hpScale = 3;
    }

    gBattlescriptCurrInstr += 2;
}

static void Cmd_hpthresholds2(void)
{
    u8 opposingBattler;
    u8 hpSwitchout;
    s32 result;
    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
    {
        gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
        opposingBattler = gActiveBattler ^ BIT_SIDE;
        hpSwitchout = gSharedMem[BSTRUCT_OFF(hpOnSwitchout) + GetBattlerSide(opposingBattler) * 2];
        result = (hpSwitchout - gBattleMons[opposingBattler].hp) * 100 / hpSwitchout;

        if (gBattleMons[opposingBattler].hp >= hpSwitchout)
            gBattleStruct->hpScale = 0;
        else if (result <= 29)
            gBattleStruct->hpScale = 1;
        else if (result <= 69)
            gBattleStruct->hpScale = 2;
        else
            gBattleStruct->hpScale = 3;
    }

    gBattlescriptCurrInstr += 2;
}

static void Cmd_useitemonopponent(void)
{
    gBattlerInMenuId = gBattlerAttacker;
    PokemonUseItemEffects(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker]], gLastUsedItem, gBattlerPartyIndexes[gBattlerAttacker], 0, 1);
    gBattlescriptCurrInstr += 1;
}

static void atk76_various(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    switch (T2_READ_8(gBattlescriptCurrInstr + 2))
    {
    case VARIOUS_CANCEL_MULTI_TURN_MOVES:
        CancelMultiTurnMoves(gActiveBattler);
        break;
    case VARIOUS_SET_MAGIC_COAT_TARGET:
        {
            u8 side;
            gBattlerAttacker = gBattlerTarget;
            side = GetBattlerSide(gBattlerAttacker) ^ BIT_SIDE;
            if (gSideTimers[side].followmeTimer && gBattleMons[gSideTimers[side].followmeTarget].hp)
                gBattlerTarget = gSideTimers[side].followmeTarget;
            else
                gBattlerTarget = gActiveBattler;
        }
        break;
    case VARIOUS_IS_RUNNING_IMPOSSIBLE:
        gBattleCommunication[MULTIUSE_STATE] = IsRunningFromBattleImpossible();
        break;
    case VARIOUS_GET_MOVE_TARGET:
        gBattlerTarget = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
        break;
    case VARIOUS_GET_BATTLER_FAINTED:
        if (gHitMarker & HITMARKER_FAINTED(gActiveBattler))
            gBattleCommunication[MULTIUSE_STATE] = TRUE;
        else
            gBattleCommunication[MULTIUSE_STATE] = FALSE;
        break;
    case VARIOUS_RESET_INTIMIDATE_TRACE_BITS:
        gSpecialStatuses[gActiveBattler].intimidatedMon = FALSE;
        gSpecialStatuses[gActiveBattler].traced = FALSE;
        break;
    case VARIOUS_UPDATE_CHOICE_MOVE_ON_LVL_UP:
        {
            s32 i;
            u16* choicedMove;
            if (gBattlerPartyIndexes[0] == gBattleStruct->expGetterMonId)
                goto ACTIVE_0;
            if (gBattlerPartyIndexes[2] != gBattleStruct->expGetterMonId)
                break;
            if (gBattlerPartyIndexes[0] == gBattlerPartyIndexes[2])
            {
                ACTIVE_0:
                    gActiveBattler = 0;
            }
            else
                gActiveBattler = 2;

            choicedMove = &gBattleStruct->choicedMove[gActiveBattler];
            for (i = 0; i < MAX_MON_MOVES; i++)
            {
                if (gBattleMons[gActiveBattler].moves[i] == *choicedMove)
                    break;
            }
            if (i == MAX_MON_MOVES)
                *choicedMove = MOVE_NONE;
        }
        break;
    }

    gBattlescriptCurrInstr += 3;
}

static void atk77_setprotectlike(void) //protect and endure
{
    bool8 notLastTurn = 1;
    u16 lastMove = gLastResultingMoves[gBattlerAttacker];

    if (lastMove != MOVE_PROTECT && lastMove != MOVE_DETECT && lastMove != MOVE_ENDURE)
        gDisableStructs[gBattlerAttacker].protectUses = 0;
    if (gCurrentTurnActionNumber == (gBattlersCount - 1))
        notLastTurn = 0;

    if (sProtectSuccessRates[gDisableStructs[gBattlerAttacker].protectUses] > Random() && notLastTurn)
    {
        if (gBattleMoves[gCurrentMove].effect == EFFECT_PROTECT)
        {
            gProtectStructs[gBattlerAttacker].protected = TRUE;
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_PROTECTED_ITSELF;
        }
        if (gBattleMoves[gCurrentMove].effect == EFFECT_ENDURE) //what is else if
        {
            gProtectStructs[gBattlerAttacker].endured = TRUE;
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_BRACED_ITSELF;
        }
        gDisableStructs[gBattlerAttacker].protectUses++;
    }
    else
    {
        gDisableStructs[gBattlerAttacker].protectUses = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_PROTECT_FAILED;
        gMoveResultFlags |= MOVE_RESULT_MISSED;
    }

    gBattlescriptCurrInstr++;
}

static void Cmd_tryexplosion(void)
{
    if (gBattleControllerExecFlags)
        return;

    for (gBattlerTarget = 0; gBattlerTarget < gBattlersCount; gBattlerTarget++)
    {
        if (gBattleMons[gBattlerTarget].ability == ABILITY_DAMP)
            break;
    }

    if (gBattlerTarget == gBattlersCount)
    {
        gActiveBattler = gBattlerAttacker;
        gBattleMoveDamage = gBattleMons[gActiveBattler].hp;
        BtlController_EmitHealthBarUpdate(B_COMM_TO_CONTROLLER, INSTANT_HP_BAR_DROP);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr++;

        for (gBattlerTarget = 0; gBattlerTarget < gBattlersCount; gBattlerTarget++)
        {
            if (gBattlerTarget == gBattlerAttacker)
                continue;
            if (!(gAbsentBattlerFlags & gBitTable[gBattlerTarget]))
                break;
        }
    }
    else
    {
        gLastUsedAbility = ABILITY_DAMP;
        RecordAbilityBattle(gBattlerTarget, gBattleMons[gBattlerTarget].ability);
        gBattlescriptCurrInstr = BattleScript_DampStopsExplosion;
    }
}

static void Cmd_setatkhptozero(void)
{
    if (gBattleControllerExecFlags)
        return;

    gActiveBattler = gBattlerAttacker;
    gBattleMons[gActiveBattler].hp = 0;
    BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_HP_BATTLE, 0, sizeof(gBattleMons[gActiveBattler].hp), &gBattleMons[gActiveBattler].hp);
    MarkBattlerForControllerExec(gActiveBattler);

    gBattlescriptCurrInstr++;
}

static void Cmd_jumpifnexttargetvalid(void) //used by intimidate to loop through all targets
{
    u8* jumpPtr = T1_READ_PTR(gBattlescriptCurrInstr + 1);

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        for (gBattlerTarget++; ; gBattlerTarget++)
        {
            if (gBattlerTarget == gBattlerAttacker)
                continue;
            if (!(gAbsentBattlerFlags & gBitTable[gBattlerTarget]))
                break;
        }

        if (gBattlerTarget >= gBattlersCount)
            gBattlescriptCurrInstr += 5;
        else
            gBattlescriptCurrInstr = jumpPtr;
    }
    else
        gBattlescriptCurrInstr += 5;
}

static void Cmd_tryhealhalfhealth(void)
{
    u8* failPtr = T1_READ_PTR(gBattlescriptCurrInstr + 1);

    if (T2_READ_8(gBattlescriptCurrInstr + 5) == BS_ATTACKER)
        gBattlerTarget = gBattlerAttacker;

    gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 2;
    if (gBattleMoveDamage == 0)
        gBattleMoveDamage = 1;
    gBattleMoveDamage *= -1;

    if (gBattleMons[gBattlerTarget].hp == gBattleMons[gBattlerTarget].maxHP)
        gBattlescriptCurrInstr = failPtr;
    else
        gBattlescriptCurrInstr += 6;
}

static void atk7C_trymirrormove(void)
{
    u16 r7 = gSharedMem[BSTRUCT_OFF(lastTakenMove) + 2 * gBattlerAttacker + 0] | (gSharedMem[BSTRUCT_OFF(lastTakenMove) + 2 * gBattlerAttacker + 1] << 8);
    u16 r6 = gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gBattlerAttacker + 0] | (gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gBattlerAttacker + 1] << 8);
    u16 r5 = gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gBattlerAttacker + 2] | (gSharedMem[BSTRUCT_OFF(lastTakenMoveFrom) + 4 * gBattlerAttacker + 3] << 8);

    if (r7 != MOVE_NONE && r7 != MOVE_UNAVAILABLE)
    {
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
        gCurrentMove = r7;
        gBattlerTarget = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
        gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect];
    }
    else if (r6 != MOVE_NONE && r5 != MOVE_NONE && r6 != MOVE_UNAVAILABLE && r5 != MOVE_UNAVAILABLE)
    {
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
        if (Random() & 1)
            gCurrentMove = r6;
        else
            gCurrentMove = r5;
        gBattlerTarget = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
        gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect];
    }
    else if (r6 != MOVE_NONE && r6 != MOVE_UNAVAILABLE)
    {
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
        gCurrentMove = r6;
        gBattlerTarget = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
        gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect];
    }
    else if (r5 != MOVE_NONE && r5 != MOVE_UNAVAILABLE)
    {
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
        gCurrentMove = r5;
        gBattlerTarget = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
        gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect];
    }
    else
    {
        gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
        gBattlescriptCurrInstr++;
    }
}

static void Cmd_setrain(void)
{
    if (gBattleWeather & B_WEATHER_RAIN)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEATHER_FAILED;
    }
    else
    {
        gBattleWeather = B_WEATHER_RAIN_TEMPORARY;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STARTED_RAIN;
        gWishFutureKnock.weatherDuration = 5;
    }
    gBattlescriptCurrInstr++;
}

static void atk7E_setreflect(void)
{
    if (gSideStatuses[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE] & SIDE_STATUS_REFLECT)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SIDE_STATUS_FAILED;
    }
    else
    {
        gSideStatuses[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE] |= SIDE_STATUS_REFLECT;
        gSideTimers[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE].reflectTimer = 5;
        if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && CountAliveMonsInBattle(1) == 2)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SET_REFLECT_DOUBLE;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SET_REFLECT_SINGLE;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_setseeded(void)
{
    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT || gStatuses3[gBattlerTarget] & STATUS3_LEECHSEED)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_LEECH_SEED_MISS;
    }
    else if (gBattleMons[gBattlerTarget].type1 == TYPE_GRASS || gBattleMons[gBattlerTarget].type2 == TYPE_GRASS)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_LEECH_SEED_FAIL;
    }
    else
    {
        gStatuses3[gBattlerTarget] |= gBattlerAttacker;
        gStatuses3[gBattlerTarget] |= STATUS3_LEECHSEED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_LEECH_SEED_SET;
    }

    gBattlescriptCurrInstr++;
}

static void Cmd_manipulatedamage(void)
{
    switch (T2_READ_8(gBattlescriptCurrInstr + 1))
    {
    case DMG_CHANGE_SIGN:
        gBattleMoveDamage *= -1;
        break;
    case DMG_RECOIL_FROM_MISS:
        gBattleMoveDamage /= 2;
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        if ((gBattleMons[gBattlerTarget].maxHP / 2) < gBattleMoveDamage)
            gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 2;
        break;
    case DMG_DOUBLED:
        gBattleMoveDamage *= 2;
        break;
    }

    gBattlescriptCurrInstr += 2;
}

static void Cmd_trysetrest(void)
{
    u8* failJump = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    gActiveBattler = gBattlerTarget = gBattlerAttacker;
    gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP * (-1);
    if (gBattleMons[gBattlerTarget].hp == gBattleMons[gBattlerTarget].maxHP)
        gBattlescriptCurrInstr = failJump;
    else
    {
        if (gBattleMons[gBattlerTarget].status1 & ((u8)(~STATUS1_SLEEP)))
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_REST_STATUSED;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_REST;

        gBattleMons[gBattlerTarget].status1 = STATUS1_SLEEP_TURN(3);
        BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, 0, sizeof(gBattleMons[gActiveBattler].status1), &gBattleMons[gActiveBattler].status1);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_jumpifnotfirstturn(void)
{
    u8* failJump = T1_READ_PTR(gBattlescriptCurrInstr + 1);

    if (gDisableStructs[gBattlerAttacker].isFirstTurn)
        gBattlescriptCurrInstr += 5;
    else
        gBattlescriptCurrInstr = failJump;
}

static void Cmd_nop(void)
{
    gBattlescriptCurrInstr++;
}

bool8 UproarWakeUpCheck(u8 battler)
{
    s32 i;
    for (i = 0; i < gBattlersCount; i++)
    {
        if (!(gBattleMons[i].status2 & STATUS2_UPROAR) || gBattleMons[battler].ability == ABILITY_SOUNDPROOF) //wtf gamefreak, you should check this only once, not every time in a loop...
            continue;
        gBattleStruct->battler = i;
        if (gBattlerTarget == 0xFF)
            gBattlerTarget = i;
        else if (gBattlerTarget == i)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CANT_SLEEP_UPROAR;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_UPROAR_KEPT_AWAKE;
        break;
    }
    if (i == gBattlersCount)
        return FALSE;
    else
        return TRUE;
}

static void Cmd_jumpifcantmakeasleep(void)
{
    u8* jumpPtr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    if (UproarWakeUpCheck(gBattlerTarget))
        gBattlescriptCurrInstr = jumpPtr;
    else if (gBattleMons[gBattlerTarget].ability == ABILITY_INSOMNIA || gBattleMons[gBattlerTarget].ability == ABILITY_VITAL_SPIRIT)
    {
        gLastUsedAbility = gBattleMons[gBattlerTarget].ability;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STAYED_AWAKE_USING;
        gBattlescriptCurrInstr = jumpPtr;
        RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
    }
    else
    {
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_stockpile(void)
{
    if (gDisableStructs[gBattlerAttacker].stockpileCounter == 3)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CANT_STOCKPILE;
    }
    else
    {
        gDisableStructs[gBattlerAttacker].stockpileCounter++;
        PREPARE_BYTE_NUMBER_BUFFER(gBattleTextBuff1, 1, gDisableStructs[gBattlerAttacker].stockpileCounter);
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STOCKPILED;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_stockpiletobasedamage(void)
{
    u8* jumpPtr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    if (gDisableStructs[gBattlerAttacker].stockpileCounter == 0)
    {
        gBattlescriptCurrInstr = jumpPtr;
    }
    else
    {
        if (gBattleCommunication[MISS_TYPE] != 1)
        {
            gBattleMoveDamage = CalculateBaseDamage(&gBattleMons[gBattlerAttacker], &gBattleMons[gBattlerTarget], gCurrentMove,
                                                    gSideStatuses[GetBattlerPosition(gBattlerTarget) & BIT_SIDE], 0,
                                                    0, gBattlerAttacker, gBattlerTarget)
                                * gDisableStructs[gBattlerAttacker].stockpileCounter;
            gBattleStruct->animTurn = gDisableStructs[gBattlerAttacker].stockpileCounter;

            if (gProtectStructs[gBattlerAttacker].helpingHand)
                gBattleMoveDamage = gBattleMoveDamage * 15 / 10;
        }
        gDisableStructs[gBattlerAttacker].stockpileCounter = 0;
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_stockpiletohpheal(void)
{
    u8* jumpPtr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    if (gDisableStructs[gBattlerAttacker].stockpileCounter == 0)
    {
        gBattlescriptCurrInstr = jumpPtr;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SWALLOW_FAILED;
    }
    else if (gBattleMons[gBattlerAttacker].maxHP == gBattleMons[gBattlerAttacker].hp)
    {
        gDisableStructs[gBattlerAttacker].stockpileCounter = 0;
        gBattlescriptCurrInstr = jumpPtr;
        gBattlerTarget = gBattlerAttacker;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SWALLOW_FULL_HP;
    }
    else
    {
        gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / (1 << (3 - gDisableStructs[gBattlerAttacker].stockpileCounter));
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattleMoveDamage *= -1;
        gBattleStruct->animTurn = gDisableStructs[gBattlerAttacker].stockpileCounter;
        gDisableStructs[gBattlerAttacker].stockpileCounter = 0;
        gBattlescriptCurrInstr += 5;
        gBattlerTarget = gBattlerAttacker;
    }
}

static void Cmd_negativedamage(void)
{
    gBattleMoveDamage = -(gHpDealt / 2);
    if (gBattleMoveDamage == 0)
        gBattleMoveDamage = -1;
    gBattlescriptCurrInstr++;
}

#define STAT_CHANGE_WORKED      0
#define STAT_CHANGE_DIDNT_WORK  1

static u8 ChangeStatBuffs(s8 statValue, u8 statId, u8 flags, const u8 *BS_ptr)
{
    bool8 certain = FALSE;
    bool8 notProtectAffected = FALSE;
    u32 index;

    if (flags & MOVE_EFFECT_AFFECTS_USER)
        gActiveBattler = gBattlerAttacker;
    else
        gActiveBattler = gBattlerTarget;

    flags &= ~(MOVE_EFFECT_AFFECTS_USER);

    if (flags & MOVE_EFFECT_CERTAIN)
        certain++;
    flags &= ~(MOVE_EFFECT_CERTAIN);

    if (flags & STAT_CHANGE_NOT_PROTECT_AFFECTED)
        notProtectAffected++;
    flags &= ~(STAT_CHANGE_NOT_PROTECT_AFFECTED);

    PREPARE_STAT_BUFFER(gBattleTextBuff1, statId)

    if ((statValue << 0x18) < 0) // stat decrease
    {
        if (gSideTimers[GET_BATTLER_SIDE(gActiveBattler)].mistTimer
            && !certain && gCurrentMove != MOVE_CURSE)
        {
            if (flags == STAT_CHANGE_ALLOW_PTR)
            {
                if (gSpecialStatuses[gActiveBattler].statLowered)
                {
                    gBattlescriptCurrInstr = BS_ptr;
                }
                else
                {
                    BattleScriptPush(BS_ptr);
                    gBattleStruct->battler = gActiveBattler;
                    gBattlescriptCurrInstr = BattleScript_MistProtected;
                    gSpecialStatuses[gActiveBattler].statLowered = TRUE;
                }
            }
            return STAT_CHANGE_DIDNT_WORK;
        }
        else if (gCurrentMove != MOVE_CURSE
                 && notProtectAffected != TRUE && JumpIfMoveAffectedByProtect(ACC_CURR_MOVE))
        {
            gBattlescriptCurrInstr = BattleScript_ButItFailed;
            return STAT_CHANGE_DIDNT_WORK;
        }
        else if ((gBattleMons[gActiveBattler].ability == ABILITY_CLEAR_BODY
                  || gBattleMons[gActiveBattler].ability == ABILITY_WHITE_SMOKE)
                 && !certain && gCurrentMove != MOVE_CURSE)
        {
            if (flags == STAT_CHANGE_ALLOW_PTR)
            {
                if (gSpecialStatuses[gActiveBattler].statLowered)
                {
                    gBattlescriptCurrInstr = BS_ptr;
                }
                else
                {
                    BattleScriptPush(BS_ptr);
                    gBattleStruct->battler = gActiveBattler;
                    gBattlescriptCurrInstr = BattleScript_AbilityNoStatLoss;
                    gLastUsedAbility = gBattleMons[gActiveBattler].ability;
                    RecordAbilityBattle(gActiveBattler, gLastUsedAbility);
                    gSpecialStatuses[gActiveBattler].statLowered = TRUE;
                }
            }
            return STAT_CHANGE_DIDNT_WORK;
        }
        else if (gBattleMons[gActiveBattler].ability == ABILITY_KEEN_EYE
                 && !certain && statId == STAT_ACC)
        {
            if (flags == STAT_CHANGE_ALLOW_PTR)
            {
                BattleScriptPush(BS_ptr);
                gBattleStruct->battler = gActiveBattler;
                gBattlescriptCurrInstr = BattleScript_AbilityNoSpecificStatLoss;
                gLastUsedAbility = gBattleMons[gActiveBattler].ability;
                RecordAbilityBattle(gActiveBattler, gLastUsedAbility);
            }
            return STAT_CHANGE_DIDNT_WORK;
        }
        else if (gBattleMons[gActiveBattler].ability == ABILITY_HYPER_CUTTER
                 && !certain && statId == STAT_ATK)
        {
            if (flags == STAT_CHANGE_ALLOW_PTR)
            {
                BattleScriptPush(BS_ptr);
                gBattleStruct->battler = gActiveBattler;
                gBattlescriptCurrInstr = BattleScript_AbilityNoSpecificStatLoss;
                gLastUsedAbility = gBattleMons[gActiveBattler].ability;
                RecordAbilityBattle(gActiveBattler, gLastUsedAbility);
            }
            return STAT_CHANGE_DIDNT_WORK;
        }
        else if (gBattleMons[gActiveBattler].ability == ABILITY_SHIELD_DUST && flags == 0)
        {
            return STAT_CHANGE_DIDNT_WORK;
        }
        else // try to decrease
        {
            statValue = -GET_STAT_BUFF_VALUE(statValue);
            gBattleTextBuff2[0] = B_BUFF_PLACEHOLDER_BEGIN;
            index = 1;
            if (statValue == -2)
            {
                gBattleTextBuff2[1] = B_BUFF_STRING;
                gBattleTextBuff2[2] = STRINGID_STATHARSHLY;
                gBattleTextBuff2[3] = STRINGID_STATHARSHLY >> 8;
                index = 4;
            }
            gBattleTextBuff2[index] = B_BUFF_STRING;
            index++;
            gBattleTextBuff2[index] = STRINGID_STATFELL;
            index++;
            gBattleTextBuff2[index] = STRINGID_STATFELL >> 8;
            index++;
            gBattleTextBuff2[index] = B_BUFF_EOS;

            if (gBattleMons[gActiveBattler].statStages[statId] == MIN_STAT_STAGE)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STAT_WONT_DECREASE;
            else
                gBattleCommunication[MULTISTRING_CHOOSER] = (gBattlerTarget == gActiveBattler);

        }
    }
    else // stat increase
    {
        statValue = GET_STAT_BUFF_VALUE(statValue);
        gBattleTextBuff2[0] = B_BUFF_PLACEHOLDER_BEGIN;
        index = 1;
        if (statValue == 2)
        {
            gBattleTextBuff2[1] = B_BUFF_STRING;
            gBattleTextBuff2[2] = STRINGID_STATSHARPLY;
            gBattleTextBuff2[3] = STRINGID_STATSHARPLY >> 8;
            index = 4;
        }
        gBattleTextBuff2[index] = B_BUFF_STRING;
        index++;
        gBattleTextBuff2[index] = STRINGID_STATROSE;
        index++;
        gBattleTextBuff2[index] = STRINGID_STATROSE >> 8;
        index++;
        gBattleTextBuff2[index] = B_BUFF_EOS;

        if (gBattleMons[gActiveBattler].statStages[statId] == MAX_STAT_STAGE)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STAT_WONT_INCREASE;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = (gBattlerTarget == gActiveBattler);
    }

    gBattleMons[gActiveBattler].statStages[statId] += statValue;
    if (gBattleMons[gActiveBattler].statStages[statId] < MIN_STAT_STAGE)
        gBattleMons[gActiveBattler].statStages[statId] = MIN_STAT_STAGE;
    if (gBattleMons[gActiveBattler].statStages[statId] > MAX_STAT_STAGE)
        gBattleMons[gActiveBattler].statStages[statId] = MAX_STAT_STAGE;

    if (gBattleCommunication[MULTISTRING_CHOOSER] == 2 && flags & STAT_CHANGE_ALLOW_PTR)
        gMoveResultFlags |= MOVE_RESULT_MISSED;

    if (gBattleCommunication[MULTISTRING_CHOOSER] == 2 && !(flags & STAT_CHANGE_ALLOW_PTR))
        return STAT_CHANGE_DIDNT_WORK;

    return STAT_CHANGE_WORKED;
}

static void Cmd_statbuffchange(void)
{
    u8* jumpPtr = T1_READ_PTR(gBattlescriptCurrInstr + 2);
    if (ChangeStatBuffs(GET_STAT_BUFF_VALUE2(gBattleStruct->statChanger), GET_STAT_BUFF_ID(gBattleStruct->statChanger), T2_READ_8(gBattlescriptCurrInstr + 1), jumpPtr) == STAT_CHANGE_WORKED)
        gBattlescriptCurrInstr += 6;
}

static void Cmd_normalisebuffs(void) //haze
{
    s32 i, j;
    for (i = 0; i < gBattlersCount; i++)
    {
        for (j = 0; j < NUM_BATTLE_STATS; j++)
        {
            gBattleMons[i].statStages[j] = DEFAULT_STAT_STAGE;
        }
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_setbide(void)
{
    gBattleMons[gBattlerAttacker].status2 |= STATUS2_MULTIPLETURNS;
    gLockedMoves[gBattlerAttacker] = gCurrentMove;
    gBideDmg[gBattlerAttacker] = 0;
    gBattleMons[gBattlerAttacker].status2 |= STATUS2_BIDE_TURN(2); //2 turns
    gBattlescriptCurrInstr++;
}

static void Cmd_confuseifrepeatingattackends(void)
{
    if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_LOCK_CONFUSE))
        gBattleCommunication[MOVE_EFFECT_BYTE] = (MOVE_EFFECT_THRASH | MOVE_EFFECT_AFFECTS_USER);
    gBattlescriptCurrInstr++;
}

static void Cmd_setmultihitcounter(void)
{
    if (T2_READ_8(gBattlescriptCurrInstr + 1))
        gMultiHitCounter = T2_READ_8(gBattlescriptCurrInstr + 1);
    else
    {
        gMultiHitCounter = Random() & 3;
        if (gMultiHitCounter > 1)
            gMultiHitCounter = (Random() & 3) + 2;
        else
            gMultiHitCounter += 2;
    }
    gBattlescriptCurrInstr += 2;
}

static void Cmd_initmultihitstring(void)
{
    PREPARE_BYTE_NUMBER_BUFFER(gBattleStruct->multihitString, 1, 0);
    gBattlescriptCurrInstr++;
}

static bool8 sub_80264C0(void)
{
    if (gBattleMons[gBattlerAttacker].level >= gBattleMons[gBattlerTarget].level)
    {
        gSharedMem[BSTRUCT_OFF(battlerPartyIndexes) + gBattlerTarget] = gBattlerPartyIndexes[gBattlerTarget];
    }
    else
    {
        u16 random = Random() & 0xFF;
        if ((u32)((random * (gBattleMons[gBattlerAttacker].level + gBattleMons[gBattlerTarget].level) >> 8) + 1) <= (gBattleMons[gBattlerTarget].level / 4))
        {
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
            return FALSE;
        }
        gSharedMem[BSTRUCT_OFF(battlerPartyIndexes) + gBattlerTarget] = gBattlerPartyIndexes[gBattlerTarget];
    }
    gBattlescriptCurrInstr = BattleScript_SuccessForceOut;
    return TRUE;
}

static void atk8F_forcerandomswitch(void)
{
    if ((gBattleTypeFlags & BATTLE_TYPE_TRAINER))
    {
        u8 i;
        struct Pokemon* party;
        u8 validMons;
        u8 val;
        if (!GetBattlerSide(gBattlerTarget))
            party = gPlayerParty;
        else
            party = gEnemyParty;

        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        {
            validMons = 0;
            val = 0;
            if (GetLinkTrainerFlankId(GetBattlerMultiplayerId(gBattlerTarget)) == 1)
                val = MULTI_PARTY_SIZE;
            for (i = val; i < val + MULTI_PARTY_SIZE; i++)
            {
                if (GetMonData(&party[i], MON_DATA_SPECIES) != SPECIES_NONE
                 && !GetMonData(&party[i], MON_DATA_IS_EGG)
                 && GetMonData(&party[i], MON_DATA_HP) != 0)
                    validMons++;
            }
        }
        else
        {
            validMons = 0;
            for (i = 0; i < PARTY_SIZE; i++)
            {
                if (GetMonData(&party[i], MON_DATA_SPECIES) != SPECIES_NONE
                 && !GetMonData(&party[i], MON_DATA_IS_EGG)
                 && GetMonData(&party[i], MON_DATA_HP) != 0)
                    validMons++;
            }
        }

        if ((validMons < 2 && (gBattleTypeFlags & (BATTLE_TYPE_DOUBLE | BATTLE_TYPE_MULTI)) != BATTLE_TYPE_DOUBLE)
         || (validMons < 3 && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) && !(gBattleTypeFlags & BATTLE_TYPE_MULTI)))
        {
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        }
        else if (sub_80264C0())
        {
#define MON_CAN_BATTLE(mon) (((GetMonData(mon, MON_DATA_SPECIES) && GetMonData(mon, MON_DATA_IS_EGG) != TRUE && GetMonData(mon, MON_DATA_HP))))
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                do
                {
                    val = Random() % MULTI_PARTY_SIZE;
                    if (GetLinkTrainerFlankId(GetBattlerMultiplayerId(gBattlerTarget)) == 1)
                        i = val + MULTI_PARTY_SIZE;
                    else
                        i = val;
                } while (i == gBattlerPartyIndexes[gBattlerTarget] || i == gBattlerPartyIndexes[gBattlerTarget ^ BIT_FLANK] || !MON_CAN_BATTLE(&party[i]));
            }
            else
            {
                if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                {
                    do
                    {
                        i = Random() % PARTY_SIZE;
                    } while (i == gBattlerPartyIndexes[gBattlerTarget] || i == gBattlerPartyIndexes[gBattlerTarget ^ BIT_FLANK] || !MON_CAN_BATTLE(&party[i]));
                }
                else
                {
                    do
                    {
                        i = Random() % PARTY_SIZE;
                    } while (i == gBattlerPartyIndexes[gBattlerTarget] || !MON_CAN_BATTLE(&party[i]));
                }
            }
            gSharedMem[BSTRUCT_OFF(monToSwitchIntoId) + gBattlerTarget] = i;
            if (!IsLinkDoubleBattle())
                SwitchPartyOrder(gBattlerTarget);
            SwitchPartyOrderLinkMulti(gBattlerTarget, i, 0);
            SwitchPartyOrderLinkMulti(gBattlerTarget ^ BIT_FLANK, i, 1);
#undef MON_CAN_BATTLE
        }
    }
    else
    {
        sub_80264C0();
    }
}

static void Cmd_tryconversiontypechange(void)
{
    //randomly changes user's type to one of its moves' type
    u8 validMoves = 0;
    u8 moveChecked;
    u8 moveType;
    while (validMoves < MAX_MON_MOVES)
    {
        if (gBattleMons[gBattlerAttacker].moves[validMoves] == MOVE_NONE)
            break;
        validMoves++;
    }

    for (moveChecked = 0; moveChecked < validMoves; moveChecked++)
    {
        moveType = gBattleMoves[gBattleMons[gBattlerAttacker].moves[moveChecked]].type;
        if (moveType == TYPE_MYSTERY)
        {
            if (gBattleMons[gBattlerAttacker].type1 == TYPE_GHOST || gBattleMons[gBattlerAttacker].type2 == TYPE_GHOST)
                moveType = TYPE_GHOST;
            else
                moveType = TYPE_NORMAL;
        }
        if (moveType != gBattleMons[gBattlerAttacker].type1 && moveType != gBattleMons[gBattlerAttacker].type2)
            break;
    }

    if (moveChecked == validMoves)
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        do
        {

            while ((moveChecked = Random() & (MAX_MON_MOVES - 1)) >= validMoves);

            moveType = gBattleMoves[gBattleMons[gBattlerAttacker].moves[moveChecked]].type;
            if (moveType == TYPE_MYSTERY)
            {
                if (gBattleMons[gBattlerAttacker].type1 == TYPE_GHOST || gBattleMons[gBattlerAttacker].type2 == TYPE_GHOST)
                    moveType = TYPE_GHOST;
                else
                    moveType = TYPE_NORMAL;
            }
        } while (moveType == gBattleMons[gBattlerAttacker].type1 || moveType == gBattleMons[gBattlerAttacker].type2);

        gBattleMons[gBattlerAttacker].type1 = moveType;
        gBattleMons[gBattlerAttacker].type2 = moveType;

        PREPARE_TYPE_BUFFER(gBattleTextBuff1, moveType);

        gBattlescriptCurrInstr += 5;
    }
}

static void atk91_givepaydaymoney(void)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_LINK) && gPaydayMoney)
    {
        AddMoney(&gSaveBlock1.money, gPaydayMoney * gBattleStruct->moneyMultiplier);
        PREPARE_HWORD_NUMBER_BUFFER(gBattleTextBuff1, 5, gPaydayMoney);
        BattleScriptPush(gBattlescriptCurrInstr + 1);
        gBattlescriptCurrInstr = BattleScript_PrintPayDayMoneyString;
    }
    else
        gBattlescriptCurrInstr++;
}

static void atk92_setlightscreen(void)
{
    if (gSideStatuses[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE] & SIDE_STATUS_LIGHTSCREEN)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SIDE_STATUS_FAILED;
    }
    else
    {
        gSideStatuses[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE] |= SIDE_STATUS_LIGHTSCREEN;
        gSideTimers[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE].lightscreenTimer = 5;
        if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && CountAliveMonsInBattle(1) == 2)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SET_LIGHTSCREEN_DOUBLE;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SET_LIGHTSCREEN_SINGLE;
    }
    gBattlescriptCurrInstr++;
}

#ifdef NOTMATCHING
static void atk93_tryKO(void)
{
   if (gBattleMons[gBattlerTarget].item == ITEM_ENIGMA_BERRY)
        holdEffect = gEnigmaBerries[gBattlerTarget].holdEffect, param = gEnigmaBerries[gBattlerTarget].holdEffectParam;
    else
    {
        holdEffect = GetItemHoldEffect(gBattleMons[gBattlerTarget].item);
        param = GetItemHoldEffectParam(gBattleMons[gBattlerTarget].item);
    }

    gPotentialItemEffectBattler = gBattlerTarget;

    if (holdEffect == HOLD_EFFECT_FOCUS_BAND && (Random() % 100) < param)
    {
        RecordItemEffectBattle(gBattlerTarget, holdEffect);
        gSpecialStatuses[gBattlerTarget].focusBanded = 1;
    }

    if (gBattleMons[gBattlerTarget].ability == ABILITY_STURDY)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gLastUsedAbility = ABILITY_STURDY;
        gBattlescriptCurrInstr = x;
        RecordAbilityBattle(gBattlerTarget, ABILITY_STURDY);
        return;
    }

    if (!(gStatuses3[gBattlerTarget] & STATUS3_ALWAYS_HITS))
    {
        u16 to_cmp = gBattleMons[gBattlerAttacker].level - gBattleMons[gBattlerTarget].level + gBattleMoves[gCurrentMove].accuracy;
        if (Random() % 0x64 + 1 < to_cmp || gBattleMons[gBattlerAttacker].level < gBattleMons[gBattlerTarget].level)
        {
            goto MOVE_RESULT_MISSED_LABEL;
        }
    }
    else
    {
        if (gDisableStructs[gBattlerTarget].battlerWithSureHit != gBattlerAttacker || gBattleMons[gBattlerAttacker].level < gBattleMons[gBattlerTarget].level)
        {

        }
    }

MOVE_RESULT_MISSED_LABEL:
    gBattleTypeFlags |= MOVE_RESULT_MISSED;
    if (gBattleMons[gBattlerAttacker].level < gBattleMons[gBattlerTarget].level)
        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
    else
        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
    gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

#else
// Expand shared constants inside the native inline assembly.
#define ASM_CONSTANT_(value) #value
#define ASM_CONSTANT(value) ASM_CONSTANT_(value)

__attribute((naked))
static void atk93_tryKO(void)
{
    asm(".syntax unified\n\
        push {r4-r7,lr}\n\
    mov r7, r10\n\
    mov r6, r9\n\
    mov r5, r8\n\
    push {r5-r7}\n\
    ldr r7, _08026BA8 @ =gBattleMons\n\
    ldr r6, _08026BAC @ =gBattlerTarget\n\
    ldrb r2, [r6]\n\
    movs r5, 0x58\n\
    adds r0, r2, 0\n\
    muls r0, r5\n\
    adds r1, r0, r7\n\
    ldrh r0, [r1, 0x2E]\n\
    cmp r0, " ASM_CONSTANT(ITEM_ENIGMA_BERRY) "\n\
    bne _08026BB4\n\
    ldr r1, _08026BB0 @ =gEnigmaBerries\n\
    lsls r0, r2, 3\n\
    subs r0, r2\n\
    lsls r0, 2\n\
    adds r0, r1\n\
    ldrb r4, [r0, 0x7]\n\
    ldrb r6, [r0, 0x1A]\n\
    b _08026BCE\n\
    .align 2, 0\n\
_08026BA8: .4byte gBattleMons\n\
_08026BAC: .4byte gBattlerTarget\n\
_08026BB0: .4byte gEnigmaBerries\n\
_08026BB4:\n\
    ldrh r0, [r1, 0x2E]\n\
    bl GetItemHoldEffect\n\
    lsls r0, 24\n\
    lsrs r4, r0, 24\n\
    ldrb r0, [r6]\n\
    muls r0, r5\n\
    adds r0, r7\n\
    ldrh r0, [r0, 0x2E]\n\
    bl GetItemHoldEffectParam\n\
    lsls r0, 24\n\
    lsrs r6, r0, 24\n\
_08026BCE:\n\
    ldr r1, _08026C4C @ =gPotentialItemEffectBattler\n\
    ldr r5, _08026C50 @ =gBattlerTarget\n\
    ldrb r0, [r5]\n\
    strb r0, [r1]\n\
    cmp r4, " ASM_CONSTANT(HOLD_EFFECT_FOCUS_BAND) "\n\
    bne _08026C0C\n\
    bl Random\n\
    lsls r0, 16\n\
    lsrs r0, 16\n\
    movs r1, 0x64\n\
    bl __umodsi3\n\
    lsls r0, 16\n\
    lsrs r0, 16\n\
    cmp r0, r6\n\
    bcs _08026C0C\n\
    ldrb r0, [r5]\n\
    movs r1, " ASM_CONSTANT(HOLD_EFFECT_FOCUS_BAND) "\n\
    bl RecordItemEffectBattle\n\
    ldr r2, _08026C54 @ =gSpecialStatuses\n\
    ldrb r1, [r5]\n\
    lsls r0, r1, 2\n\
    adds r0, r1\n\
    lsls r0, 2\n\
    adds r0, r2\n\
    ldrb r1, [r0]\n\
    movs r2, 0x80\n\
    orrs r1, r2\n\
    strb r1, [r0]\n\
_08026C0C:\n\
    ldr r0, _08026C58 @ =gBattleMons\n\
    mov r8, r0\n\
    ldr r1, _08026C50 @ =gBattlerTarget\n\
    ldrb r2, [r1]\n\
    movs r6, 0x58\n\
    adds r0, r2, 0\n\
    muls r0, r6\n\
    mov r3, r8\n\
    adds r5, r0, r3\n\
    adds r0, r5, 0\n\
    adds r0, 0x20\n\
    ldrb r3, [r0]\n\
    mov r10, r8\n\
    cmp r3, " ASM_CONSTANT(ABILITY_STURDY) "\n\
    bne _08026C6C\n\
    ldr r2, _08026C5C @ =gMoveResultFlags\n\
    ldrb r0, [r2]\n\
    movs r1, " ASM_CONSTANT(MOVE_RESULT_MISSED) "\n\
    orrs r0, r1\n\
    strb r0, [r2]\n\
    ldr r0, _08026C60 @ =gLastUsedAbility\n\
    strb r3, [r0]\n\
    ldr r1, _08026C64 @ =gBattlescriptCurrInstr\n\
    ldr r0, _08026C68 @ =BattleScript_SturdyPreventsOHKO\n\
    str r0, [r1]\n\
    ldr r1, _08026C50 @ =gBattlerTarget\n\
    ldrb r0, [r1]\n\
    movs r1, " ASM_CONSTANT(ABILITY_STURDY) "\n\
    bl RecordAbilityBattle\n\
    b _08026E40\n\
    .align 2, 0\n\
_08026C4C: .4byte gPotentialItemEffectBattler\n\
_08026C50: .4byte gBattlerTarget\n\
_08026C54: .4byte gSpecialStatuses\n\
_08026C58: .4byte gBattleMons\n\
_08026C5C: .4byte gMoveResultFlags\n\
_08026C60: .4byte gLastUsedAbility\n\
_08026C64: .4byte gBattlescriptCurrInstr\n\
_08026C68: .4byte BattleScript_SturdyPreventsOHKO\n\
_08026C6C:\n\
    ldr r1, _08026CE0 @ =gStatuses3\n\
    lsls r0, r2, 2\n\
    adds r0, r1\n\
    ldr r0, [r0]\n\
    movs r1, " ASM_CONSTANT(STATUS3_ALWAYS_HITS) "\n\
    ands r0, r1\n\
    cmp r0, 0\n\
    bne _08026CF4\n\
    ldr r1, _08026CE4 @ =gBattleMoves\n\
    ldr r0, _08026CE8 @ =gCurrentMove\n\
    ldrh r0, [r0]\n\
    lsls r2, r0, 1\n\
    adds r2, r0\n\
    lsls r2, 2\n\
    adds r2, r1\n\
    ldr r7, _08026CEC @ =gBattlerAttacker\n\
    ldrb r0, [r7]\n\
    muls r0, r6\n\
    add r0, r10\n\
    adds r0, 0x2A\n\
    ldrb r0, [r0]\n\
    adds r1, r5, 0\n\
    adds r1, 0x2A\n\
    ldrb r1, [r1]\n\
    subs r0, r1\n\
    ldrb r2, [r2, 0x3]\n\
    adds r0, r2\n\
    lsls r0, 16\n\
    lsrs r4, r0, 16\n\
    bl Random\n\
    lsls r0, 16\n\
    lsrs r0, 16\n\
    movs r1, 0x64\n\
    bl __umodsi3\n\
    lsls r0, 16\n\
    lsrs r0, 16\n\
    adds r0, 0x1\n\
    cmp r0, r4\n\
    bge _08026CDC\n\
    ldrb r0, [r7]\n\
    adds r1, r0, 0\n\
    muls r1, r6\n\
    add r1, r10\n\
    adds r1, 0x2A\n\
    ldr r2, _08026CF0 @ =gBattlerTarget\n\
    ldrb r0, [r2]\n\
    muls r0, r6\n\
    add r0, r10\n\
    adds r0, 0x2A\n\
    ldrb r1, [r1]\n\
    movs r4, " ASM_CONSTANT(TRUE) "\n\
    ldrb r0, [r0]\n\
    cmp r1, r0\n\
    bcs _08026D20\n\
_08026CDC:\n\
    mov r10, r8\n\
    b _08026DE0\n\
    .align 2, 0\n\
_08026CE0: .4byte gStatuses3\n\
_08026CE4: .4byte gBattleMoves\n\
_08026CE8: .4byte gCurrentMove\n\
_08026CEC: .4byte gBattlerAttacker\n\
_08026CF0: .4byte gBattlerTarget\n\
_08026CF4:\n\
    ldr r0, _08026D4C @ =gDisableStructs\n\
    lsls r1, r2, 3\n\
    subs r1, r2\n\
    lsls r1, 2\n\
    adds r1, r0\n\
    ldr r2, _08026D50 @ =gBattlerAttacker\n\
    ldrb r0, [r1, 0x15]\n\
    movs r4, " ASM_CONSTANT(FALSE) "\n\
    ldrb r3, [r2]\n\
    cmp r0, r3\n\
    bne _08026D20\n\
    ldrb r0, [r2]\n\
    muls r0, r6\n\
    add r0, r10\n\
    adds r0, 0x2A\n\
    adds r1, r5, 0\n\
    adds r1, 0x2A\n\
    ldrb r0, [r0]\n\
    ldrb r1, [r1]\n\
    cmp r0, r1\n\
    bcc _08026D20\n\
    movs r4, " ASM_CONSTANT(TRUE) "\n\
_08026D20:\n\
    cmp r4, 0\n\
    beq _08026DE0\n\
    ldr r0, _08026D54 @ =gProtectStructs\n\
    ldr r1, _08026D58 @ =gBattlerTarget\n\
    ldrb r2, [r1]\n\
    lsls r1, r2, 4\n\
    adds r1, r0\n\
    ldrb r0, [r1]\n\
    lsls r0, 30\n\
    cmp r0, 0\n\
    bge _08026D64\n\
    ldr r1, _08026D5C @ =gBattleMoveDamage\n\
    movs r0, 0x58\n\
    muls r0, r2\n\
    add r0, r10\n\
    ldrh r0, [r0, 0x28]\n\
    subs r0, 0x1\n\
    str r0, [r1]\n\
    ldr r2, _08026D60 @ =gMoveResultFlags\n\
    ldrb r0, [r2]\n\
    movs r1, " ASM_CONSTANT(MOVE_RESULT_FOE_ENDURED) "\n\
    b _08026DC6\n\
    .align 2, 0\n\
_08026D4C: .4byte gDisableStructs\n\
_08026D50: .4byte gBattlerAttacker\n\
_08026D54: .4byte gProtectStructs\n\
_08026D58: .4byte gBattlerTarget\n\
_08026D5C: .4byte gBattleMoveDamage\n\
_08026D60: .4byte gMoveResultFlags\n\
_08026D64:\n\
    ldr r0, _08026DA0 @ =gSpecialStatuses\n\
    lsls r1, r2, 2\n\
    adds r1, r2\n\
    lsls r1, 2\n\
    adds r1, r0\n\
    ldrb r0, [r1]\n\
    lsrs r0, 7\n\
    cmp r0, 0\n\
    beq _08026DB4\n\
    ldr r1, _08026DA4 @ =gBattleMoveDamage\n\
    movs r3, 0x58\n\
    adds r0, r2, 0\n\
    muls r0, r3\n\
    add r0, r10\n\
    ldrh r0, [r0, 0x28]\n\
    subs r0, 0x1\n\
    str r0, [r1]\n\
    ldr r2, _08026DA8 @ =gMoveResultFlags\n\
    ldrb r0, [r2]\n\
    movs r1, " ASM_CONSTANT(MOVE_RESULT_FOE_HUNG_ON) "\n\
    orrs r0, r1\n\
    strb r0, [r2]\n\
    ldr r1, _08026DAC @ =gLastUsedItem\n\
    ldr r2, _08026DB0 @ =gBattlerTarget\n\
    ldrb r0, [r2]\n\
    muls r0, r3\n\
    add r0, r10\n\
    ldrh r0, [r0, 0x2E]\n\
    strh r0, [r1]\n\
    b _08026DCA\n\
    .align 2, 0\n\
_08026DA0: .4byte gSpecialStatuses\n\
_08026DA4: .4byte gBattleMoveDamage\n\
_08026DA8: .4byte gMoveResultFlags\n\
_08026DAC: .4byte gLastUsedItem\n\
_08026DB0: .4byte gBattlerTarget\n\
_08026DB4:\n\
    ldr r1, _08026DD4 @ =gBattleMoveDamage\n\
    movs r0, 0x58\n\
    muls r0, r2\n\
    add r0, r10\n\
    ldrh r0, [r0, 0x28]\n\
    str r0, [r1]\n\
    ldr r2, _08026DD8 @ =gMoveResultFlags\n\
    ldrb r0, [r2]\n\
    movs r1, " ASM_CONSTANT(MOVE_RESULT_ONE_HIT_KO) "\n\
_08026DC6:\n\
    orrs r0, r1\n\
    strb r0, [r2]\n\
_08026DCA:\n\
    ldr r1, _08026DDC @ =gBattlescriptCurrInstr\n\
    ldr r0, [r1]\n\
    adds r0, 0x5\n\
    str r0, [r1]\n\
    b _08026E40\n\
    .align 2, 0\n\
_08026DD4: .4byte gBattleMoveDamage\n\
_08026DD8: .4byte gMoveResultFlags\n\
_08026DDC: .4byte gBattlescriptCurrInstr\n\
_08026DE0:\n\
    ldr r2, _08026E10 @ =gMoveResultFlags\n\
    ldrb r0, [r2]\n\
    movs r1, " ASM_CONSTANT(MOVE_RESULT_MISSED) "\n\
    orrs r0, r1\n\
    strb r0, [r2]\n\
    ldr r0, _08026E14 @ =gBattlerAttacker\n\
    ldrb r0, [r0]\n\
    movs r2, 0x58\n\
    adds r1, r0, 0\n\
    muls r1, r2\n\
    add r1, r10\n\
    adds r1, 0x2A\n\
    ldr r3, _08026E18 @ =gBattlerTarget\n\
    ldrb r0, [r3]\n\
    muls r0, r2\n\
    add r0, r10\n\
    adds r0, 0x2A\n\
    ldrb r1, [r1]\n\
    ldrb r0, [r0]\n\
    cmp r1, r0\n\
    bcc _08026E20\n\
    ldr r1, _08026E1C @ =gBattleCommunication\n\
    movs r0, " ASM_CONSTANT(B_MSG_KO_MISS) "\n\
    b _08026E24\n\
    .align 2, 0\n\
_08026E10: .4byte gMoveResultFlags\n\
_08026E14: .4byte gBattlerAttacker\n\
_08026E18: .4byte gBattlerTarget\n\
_08026E1C: .4byte gBattleCommunication\n\
_08026E20:\n\
    ldr r1, _08026E50 @ =gBattleCommunication\n\
    movs r0, " ASM_CONSTANT(B_MSG_KO_UNAFFECTED) "\n\
_08026E24:\n\
    strb r0, [r1, " ASM_CONSTANT(MULTISTRING_CHOOSER) "]\n\
    ldr r3, _08026E54 @ =gBattlescriptCurrInstr\n\
    ldr r2, [r3]\n\
    ldrb r1, [r2, 0x1]\n\
    ldrb r0, [r2, 0x2]\n\
    lsls r0, 8\n\
    orrs r1, r0\n\
    ldrb r0, [r2, 0x3]\n\
    lsls r0, 16\n\
    orrs r1, r0\n\
    ldrb r0, [r2, 0x4]\n\
    lsls r0, 24\n\
    orrs r1, r0\n\
    str r1, [r3]\n\
_08026E40:\n\
    pop {r3-r5}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    mov r10, r5\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_08026E50: .4byte gBattleCommunication\n\
_08026E54: .4byte gBattlescriptCurrInstr\n\
        .syntax divided");
}
#endif // NOTMATCHING

static void Cmd_damagetohalftargethp(void) //super fang
{
    gBattleMoveDamage = gBattleMons[gBattlerTarget].hp / 2;
    if (gBattleMoveDamage == 0)
        gBattleMoveDamage = 1;
    gBattlescriptCurrInstr++;
}

static void Cmd_setsandstorm(void)
{
    if (gBattleWeather & B_WEATHER_SANDSTORM)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEATHER_FAILED;
    }
    else
    {
        gBattleWeather = B_WEATHER_SANDSTORM_TEMPORARY;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STARTED_SANDSTORM;
        gWishFutureKnock.weatherDuration = 5;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_weatherdamage(void)
{
    if (WEATHER_HAS_EFFECT)
    {
        if (gBattleWeather & B_WEATHER_SANDSTORM)
        {
            if (gBattleMons[gBattlerAttacker].type1 != TYPE_ROCK && gBattleMons[gBattlerAttacker].type1 != TYPE_STEEL && gBattleMons[gBattlerAttacker].type1 != TYPE_GROUND
             && gBattleMons[gBattlerAttacker].type2 != TYPE_ROCK && gBattleMons[gBattlerAttacker].type2 != TYPE_STEEL && gBattleMons[gBattlerAttacker].type2 != TYPE_GROUND
             && gBattleMons[gBattlerAttacker].ability != ABILITY_SAND_VEIL && !(gStatuses3[gBattlerAttacker] & STATUS3_UNDERGROUND) && !(gStatuses3[gBattlerAttacker] & STATUS3_UNDERWATER))
            {
                gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 16;
                if (gBattleMoveDamage == 0)
                    gBattleMoveDamage = 1;
            }
            else
            {
                gBattleMoveDamage = 0;
            }
        }
        if (gBattleWeather & B_WEATHER_HAIL_TEMPORARY)
        {
            if (gBattleMons[gBattlerAttacker].type1 != TYPE_ICE && gBattleMons[gBattlerAttacker].type2 != TYPE_ICE && !(gStatuses3[gBattlerAttacker] & STATUS3_UNDERGROUND) && !(gStatuses3[gBattlerAttacker] & STATUS3_UNDERWATER))
            {
                gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 16;
                if (gBattleMoveDamage == 0)
                    gBattleMoveDamage = 1;
            }
            else
            {
                gBattleMoveDamage = 0;
            }
        }
    }
    else
        gBattleMoveDamage = 0;

    if (gAbsentBattlerFlags & gBitTable[gBattlerAttacker])
        gBattleMoveDamage = 0;

    gBattlescriptCurrInstr++;
}

static void atk97_tryinfatuating(void)
{
    struct Pokemon *monAttacker, *monTarget;
    u16 speciesAttacker, speciesTarget;
    u32 personalityAttacker, personalityTarget;
    if (!GetBattlerSide(gBattlerAttacker))
        monAttacker = &gPlayerParty[gBattlerPartyIndexes[gBattlerAttacker]];
    else
        monAttacker = &gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker]];

    if (!GetBattlerSide(gBattlerTarget))
        monTarget = &gPlayerParty[gBattlerPartyIndexes[gBattlerTarget]];
    else
        monTarget = &gEnemyParty[gBattlerPartyIndexes[gBattlerTarget]];

    speciesAttacker = GetMonData(monAttacker, MON_DATA_SPECIES);
    personalityAttacker = GetMonData(monAttacker, MON_DATA_PERSONALITY);

    speciesTarget = GetMonData(monTarget, MON_DATA_SPECIES);
    personalityTarget = GetMonData(monTarget, MON_DATA_PERSONALITY);

    if (gBattleMons[gBattlerTarget].ability == ABILITY_OBLIVIOUS)
    {
        gBattlescriptCurrInstr = BattleScript_ObliviousPreventsAttraction;
        gLastUsedAbility = ABILITY_OBLIVIOUS;
        RecordAbilityBattle(gBattlerTarget, ABILITY_OBLIVIOUS);
    }
    else
    {
        if (GetGenderFromSpeciesAndPersonality(speciesAttacker, personalityAttacker) == GetGenderFromSpeciesAndPersonality(speciesTarget, personalityTarget)
            || gStatuses3[gBattlerTarget] & STATUS3_SEMI_INVULNERABLE || gBattleMons[gBattlerTarget].status2 & STATUS2_INFATUATION || GetGenderFromSpeciesAndPersonality(speciesAttacker, personalityAttacker) == MON_GENDERLESS
            || GetGenderFromSpeciesAndPersonality(speciesTarget, personalityTarget) == MON_GENDERLESS)
            {
                gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
            }
        else
        {
            gBattleMons[gBattlerTarget].status2 |= STATUS2_INFATUATED_WITH(gBattlerAttacker);
            gBattlescriptCurrInstr += 5;
        }
    }
}

static void Cmd_updatestatusicon(void)
{
    if (gBattleControllerExecFlags)
        return;

    if (T2_READ_8(gBattlescriptCurrInstr + 1) != BS_ATTACKER_WITH_PARTNER)
    {
        gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
        BtlController_EmitStatusIconUpdate(B_COMM_TO_CONTROLLER, gBattleMons[gActiveBattler].status1, gBattleMons[gActiveBattler].status2);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 2;
    }
    else
    {
        gActiveBattler = gBattlerAttacker;
        if (!(gAbsentBattlerFlags & gBitTable[gActiveBattler]))
        {
            BtlController_EmitStatusIconUpdate(B_COMM_TO_CONTROLLER, gBattleMons[gActiveBattler].status1, gBattleMons[gActiveBattler].status2);
            MarkBattlerForControllerExec(gActiveBattler);
        }
        if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
        {
            gActiveBattler = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_FLANK);
            if (!(gAbsentBattlerFlags & gBitTable[gActiveBattler]))
            {
                BtlController_EmitStatusIconUpdate(B_COMM_TO_CONTROLLER, gBattleMons[gActiveBattler].status1, gBattleMons[gActiveBattler].status2);
                MarkBattlerForControllerExec(gActiveBattler);
            }
        }
        gBattlescriptCurrInstr += 2;
    }
}

static void atk99_setmist(void)
{
    if (gSideTimers[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE].mistTimer)
    {
        gMoveResultFlags |= MOVE_RESULT_FAILED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MIST_FAILED;
    }
    else
    {
        gSideTimers[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE].mistTimer = 5;
        gSideStatuses[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE] |= SIDE_STATUS_MIST;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SET_MIST;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_setfocusenergy(void)
{
    if (gBattleMons[gBattlerAttacker].status2 & STATUS2_FOCUS_ENERGY)
    {
        gMoveResultFlags |= MOVE_RESULT_FAILED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_FOCUS_ENERGY_FAILED;
    }
    else
    {
        gBattleMons[gBattlerAttacker].status2 |= STATUS2_FOCUS_ENERGY;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_GETTING_PUMPED;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_transformdataexecution(void)
{
    gChosenMove = MOVE_UNAVAILABLE;
    gBattlescriptCurrInstr++;
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_TRANSFORMED || gStatuses3[gBattlerTarget] & STATUS3_SEMI_INVULNERABLE)
    {
        gMoveResultFlags |= MOVE_RESULT_FAILED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_TRANSFORM_FAILED;
    }
    else
    {
        u32 i; s32 j;
        u8 *battleMonAttacker, *battleMonTarget;
        gBattleMons[gBattlerAttacker].status2 |= STATUS2_TRANSFORMED;
        gDisableStructs[gBattlerAttacker].disabledMove = MOVE_NONE;
        gDisableStructs[gBattlerAttacker].disableTimer = 0;
        gDisableStructs[gBattlerAttacker].transformedMonPersonality = gBattleMons[gBattlerTarget].personality;
        gDisableStructs[gBattlerAttacker].mimickedMoves = 0;

        PREPARE_SPECIES_BUFFER(gBattleTextBuff1, gBattleMons[gBattlerTarget].species);

        battleMonAttacker = (u8*)(&gBattleMons[gBattlerAttacker]);
        battleMonTarget = (u8*)(&gBattleMons[gBattlerTarget]);

        for (i = 0; i < offsetof(struct BattlePokemon, pp); i++)
            battleMonAttacker[i] = battleMonTarget[i];

        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            if (gBattleMoves[gBattleMons[gBattlerAttacker].moves[j]].pp < 5)
                gBattleMons[gBattlerAttacker].pp[j] = gBattleMoves[gBattleMons[gBattlerAttacker].moves[j]].pp;
            else
                gBattleMons[gBattlerAttacker].pp[j] = 5;
        }

        gActiveBattler = gBattlerAttacker;
        BtlController_EmitResetActionMoveSelection(B_COMM_TO_CONTROLLER, RESET_MOVE_SELECTION);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_TRANSFORMED;
    }
}

static void Cmd_setsubstitute(void)
{
    u32 hp = gBattleMons[gBattlerAttacker].maxHP / 4;
    if (gBattleMons[gBattlerAttacker].maxHP / 4 == 0)
        hp = 1;
    if (gBattleMons[gBattlerAttacker].hp <= hp)
    {
        gBattleMoveDamage = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SUBSTITUTE_FAILED;
    }
    else
    {
        gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 4;
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattleMons[gBattlerAttacker].status2 |= STATUS2_SUBSTITUTE;
        gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_WRAPPED);
        gDisableStructs[gBattlerAttacker].substituteHP = gBattleMoveDamage;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SET_SUBSTITUTE;
        gHitMarker |= HITMARKER_IGNORE_SUBSTITUTE;
    }
    gBattlescriptCurrInstr++;
}

static bool8 IsMoveUncopyableByMimic(u16 move)
{
    s32 i;
    for (i = 0; sMovesForbiddenToCopy[i] != MIMIC_FORBIDDEN_END && sMovesForbiddenToCopy[i] != move; i++) {}
    return (sMovesForbiddenToCopy[i] != MIMIC_FORBIDDEN_END);
}

static void Cmd_mimicattackcopy(void)
{
    gChosenMove = MOVE_UNAVAILABLE;
    if (IsMoveUncopyableByMimic(gLastMoves[gBattlerTarget]) || gBattleMons[gBattlerAttacker].status2 & STATUS2_TRANSFORMED
        || gLastMoves[gBattlerTarget] == MOVE_NONE || gLastMoves[gBattlerTarget] == MOVE_UNAVAILABLE)
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        int i;
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if (gBattleMons[gBattlerAttacker].moves[i] == gLastMoves[gBattlerTarget])
                break;
        }
        if (i == MAX_MON_MOVES)
        {
            gBattleMons[gBattlerAttacker].moves[gCurrMovePos] = gLastMoves[gBattlerTarget];
            if (gBattleMoves[gLastMoves[gBattlerTarget]].pp < 5)
                gBattleMons[gBattlerAttacker].pp[gCurrMovePos] = gBattleMoves[gLastMoves[gBattlerTarget]].pp;
            else
                gBattleMons[gBattlerAttacker].pp[gCurrMovePos] = 5;

            PREPARE_MOVE_BUFFER(gBattleTextBuff1, gLastMoves[gBattlerTarget]);

            gDisableStructs[gBattlerAttacker].mimickedMoves |= gBitTable[gCurrMovePos];
            gBattlescriptCurrInstr += 5;
        }
        else
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

void atk9E_metronome(void)
{
#if DEBUG
    u16 * r3 = (u16 *)&gSharedMem[BSTRUCT_OFF(lastTakenMove) + 8 + (2 * gBattlerAttacker)];
    if (
        gBattleMons[gBattlerAttacker].moves[1] == MOVE_NONE
     && gBattleMons[gBattlerAttacker].moves[2] != MOVE_NONE
     && gBattleMons[gBattlerAttacker].moves[3] != MOVE_NONE
    )
    {
        if (*r3 == MOVE_NONE)
            *r3 = gBattleMons[gBattlerAttacker].moves[2];
        gCurrentMove = *r3;
        if (gBattleMons[gBattlerAttacker].moves[2] < gBattleMons[gBattlerAttacker].moves[3])
        {
            if (*r3 == gBattleMons[gBattlerAttacker].moves[3])
                *r3 = gBattleMons[gBattlerAttacker].moves[2];
            else
                (*r3)++;
        }
        else
        {
            asm("":::"r4"); // Force correct register use
            if (*r3 == MOVES_COUNT - 1)
                *r3 = 1;
            else if (*r3 == gBattleMons[gBattlerAttacker].moves[3])
                *r3 = gBattleMons[gBattlerAttacker].moves[2];
            else
                (*r3)++;
        }
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
        asm("":::"r5"); // Force correct register use
        gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect];
        gHitMarker |= HITMARKER_NO_PPDEDUCT;
        gBattlerTarget = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
        return;
    }
#endif //DEBUG
    while (1)
    {
        s32 i;

        gCurrentMove = (Random() & 0x1FF) + 1;
        if (gCurrentMove >= MOVES_COUNT)
            continue;
        for (i = 0; i < MAX_MON_MOVES; ++i); // redundant
        i = -1;
        while (1)
        {
            ++i;
            if (sMovesForbiddenToCopy[i] == gCurrentMove)
                break;
            if (sMovesForbiddenToCopy[i] == METRONOME_FORBIDDEN_END)
                break;
        }
        if (sMovesForbiddenToCopy[i] == METRONOME_FORBIDDEN_END)
        {
            gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
            gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect];
            gBattlerTarget = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
            return;
        }
    }
}

static void Cmd_dmgtolevel(void)
{
    gBattleMoveDamage = gBattleMons[gBattlerAttacker].level;
    gBattlescriptCurrInstr++;
}

static void Cmd_psywavedamageeffect(void)
{
    s32 randDamage;
    while ((randDamage = (Random() & 0xF)) > 0xA);
    randDamage *= 10;
    gBattleMoveDamage = gBattleMons[gBattlerAttacker].level * (randDamage + 50) / 100;
    gBattlescriptCurrInstr++;
}

static void Cmd_counterdamagecalculator(void)
{
    u8 sideAttacker = GetBattlerSide(gBattlerAttacker);
    u8 sideTarget = GetBattlerSide(gProtectStructs[gBattlerAttacker].physicalBattlerId);
    if (gProtectStructs[gBattlerAttacker].physicalDmg && sideAttacker != sideTarget && gBattleMons[gProtectStructs[gBattlerAttacker].physicalBattlerId].hp)
    {
        gBattleMoveDamage = gProtectStructs[gBattlerAttacker].physicalDmg * 2;
        if (gSideTimers[sideTarget].followmeTimer && gBattleMons[gSideTimers[sideTarget].followmeTarget].hp)
            gBattlerTarget = gSideTimers[sideTarget].followmeTarget;
        else
            gBattlerTarget = gProtectStructs[gBattlerAttacker].physicalBattlerId;
        gBattlescriptCurrInstr += 5;
    }
    else
    {
        gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_mirrorcoatdamagecalculator(void) //a copy of atkA1 with the physical -> special field changes
{
    u8 sideAttacker = GetBattlerSide(gBattlerAttacker);
    u8 sideTarget = GetBattlerSide(gProtectStructs[gBattlerAttacker].specialBattlerId);
    if (gProtectStructs[gBattlerAttacker].specialDmg && sideAttacker != sideTarget && gBattleMons[gProtectStructs[gBattlerAttacker].specialBattlerId].hp)
    {
        gBattleMoveDamage = gProtectStructs[gBattlerAttacker].specialDmg * 2;
        if (gSideTimers[sideTarget].followmeTimer && gBattleMons[gSideTimers[sideTarget].followmeTarget].hp)
            gBattlerTarget = gSideTimers[sideTarget].followmeTarget;
        else
            gBattlerTarget = gProtectStructs[gBattlerAttacker].specialBattlerId;
        gBattlescriptCurrInstr += 5;
    }
    else
    {
        gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_disablelastusedattack(void)
{
    s32 i;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (gBattleMons[gBattlerTarget].moves[i] == gLastMoves[gBattlerTarget])
            break;
    }
    if (gDisableStructs[gBattlerTarget].disabledMove == MOVE_NONE && i != MAX_MON_MOVES && gBattleMons[gBattlerTarget].pp[i] != 0)
    {
        PREPARE_MOVE_BUFFER(gBattleTextBuff1, gBattleMons[gBattlerTarget].moves[i]);

        gDisableStructs[gBattlerTarget].disabledMove = gBattleMons[gBattlerTarget].moves[i];
        gDisableStructs[gBattlerTarget].disableTimer = (Random() & 3) + 2;
        gDisableStructs[gBattlerTarget].disableTimerStartValue = gDisableStructs[gBattlerTarget].disableTimer; //that's interesting
        gBattlescriptCurrInstr += 5;
    }
    else
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_trysetencore(void)
{
    s32 i;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (gBattleMons[gBattlerTarget].moves[i] == gLastMoves[gBattlerTarget])
            break;
    }
    if (gLastMoves[gBattlerTarget] == MOVE_STRUGGLE || gLastMoves[gBattlerTarget] == MOVE_ENCORE || gLastMoves[gBattlerTarget] == MOVE_MIRROR_MOVE)
        i = 4;
    if (gDisableStructs[gBattlerTarget].encoredMove == MOVE_NONE && i != MAX_MON_MOVES && gBattleMons[gBattlerTarget].pp[i] != 0)
    {
        gDisableStructs[gBattlerTarget].encoredMove = gBattleMons[gBattlerTarget].moves[i];
        gDisableStructs[gBattlerTarget].encoredMovePos = i;
        gDisableStructs[gBattlerTarget].encoreTimer = (Random() & 3) + 3;
        gDisableStructs[gBattlerTarget].encoreTimerStartValue = gDisableStructs[gBattlerTarget].encoreTimer;
        gBattlescriptCurrInstr += 5;
    }
    else
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_painsplitdmgcalc(void)
{
    if (!(gBattleMons[gBattlerTarget].status2 & STATUS2_SUBSTITUTE))
    {
        s32 hpDiff = (gBattleMons[gBattlerAttacker].hp + gBattleMons[gBattlerTarget].hp) / 2;
        s32 painSplitHp = gBattleMoveDamage = gBattleMons[gBattlerTarget].hp - hpDiff;

        gSharedMem[BSTRUCT_OFF(painSplitHp) + 0] = sBYTE0_32(painSplitHp);
        gSharedMem[BSTRUCT_OFF(painSplitHp) + 1] = sBYTE1_32(painSplitHp);
        gSharedMem[BSTRUCT_OFF(painSplitHp) + 2] = sBYTE2_32(painSplitHp);
        gSharedMem[BSTRUCT_OFF(painSplitHp) + 3] = sBYTE3_32(painSplitHp);

        gBattleMoveDamage = gBattleMons[gBattlerAttacker].hp - hpDiff;
        gSpecialStatuses[gBattlerTarget].shellBellDmg = IGNORE_SHELL_BELL;

        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkA6_settypetorandomresistance(void)
{
    if (gLastLandedMoves[gBattlerAttacker] == MOVE_NONE
        || gLastLandedMoves[gBattlerAttacker] == MOVE_UNAVAILABLE)
    {
        // The attacker has not yet been hit with a move
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else if (IsTwoTurnsMove(gLastLandedMoves[gBattlerAttacker])
             && !(gProtectStructs[gBattlerAttacker].physicalDmg != 0 || gProtectStructs[gBattlerAttacker].specialDmg != 0))
    {
        // The attacker is about to be hit with the effects of a two-turn move
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        s32 i, j, rands;

        // Try up to 1000 times
        for (rands = 0; rands < 1000; rands++)
        {
            while (((i = (Random() & 0x7F)) > sizeof(gTypeEffectiveness) / 3));

            i *= 3;

            if (TYPE_EFFECT_ATK_TYPE(i) == gLastHitByType[gBattlerAttacker]
                && TYPE_EFFECT_MULTIPLIER(i) <= TYPE_MUL_NOT_EFFECTIVE
                && !IS_BATTLER_OF_TYPE(gBattlerAttacker, TYPE_EFFECT_DEF_TYPE(i)))
            {
                // Success
                SET_BATTLER_TYPE(gBattlerAttacker, TYPE_EFFECT_DEF_TYPE(i));
                PREPARE_TYPE_BUFFER(gBattleTextBuff1, TYPE_EFFECT_DEF_TYPE(i));

                gBattlescriptCurrInstr += 5;
                return;
            }
        }

        // Random selection failed, so go through linearly
        for (j = 0, rands = 0; rands < sizeof(gTypeEffectiveness); j += 3, rands += 3)
        {
            switch (TYPE_EFFECT_ATK_TYPE(j))
            {
            case TYPE_ENDTABLE:
            case TYPE_FORESIGHT:
                break;
            default:
                if (TYPE_EFFECT_ATK_TYPE(j) == gLastHitByType[gBattlerAttacker]
                    && TYPE_EFFECT_MULTIPLIER(j) <= TYPE_MUL_NOT_EFFECTIVE
                    && !IS_BATTLER_OF_TYPE(gBattlerAttacker, TYPE_EFFECT_DEF_TYPE(i)))
                {
                    SET_BATTLER_TYPE(gBattlerAttacker, TYPE_EFFECT_DEF_TYPE(rands));
                    PREPARE_TYPE_BUFFER(gBattleTextBuff1, TYPE_EFFECT_DEF_TYPE(rands))

                    gBattlescriptCurrInstr += 5;
                    return;
                }
                break;
            }
        }

        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_setalwayshitflag(void)
{
    gStatuses3[gBattlerTarget] &= ~(STATUS3_ALWAYS_HITS);
    gStatuses3[gBattlerTarget] |= STATUS3_ALWAYS_HITS_TURN(2);
    gDisableStructs[gBattlerTarget].battlerWithSureHit = gBattlerAttacker;
    gBattlescriptCurrInstr++;
}

static void Cmd_copymovepermanently(void)
{
    gChosenMove = MOVE_UNAVAILABLE;
    if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_TRANSFORMED) && gLastPrintedMoves[gBattlerTarget] != MOVE_STRUGGLE && gLastPrintedMoves[gBattlerTarget] != MOVE_NONE && gLastPrintedMoves[gBattlerTarget] != MOVE_UNAVAILABLE && gLastPrintedMoves[gBattlerTarget] != MOVE_SKETCH)
    {
        s32 i;
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if (gBattleMons[gBattlerAttacker].moves[i] == MOVE_SKETCH)
                continue;
            if (gBattleMons[gBattlerAttacker].moves[i] == gLastPrintedMoves[gBattlerTarget])
                break;
        }
        if (i != MAX_MON_MOVES) //sketch fail
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        else //sketch worked
        {
            struct MovePPInfo movePPData;
            gBattleMons[gBattlerAttacker].moves[gCurrMovePos] = gLastPrintedMoves[gBattlerTarget];
            gBattleMons[gBattlerAttacker].pp[gCurrMovePos] = gBattleMoves[gLastPrintedMoves[gBattlerTarget]].pp;
            gActiveBattler = gBattlerAttacker;
            for (i = 0; i < MAX_MON_MOVES; i++)
            {
                movePPData.moves[i] = gBattleMons[gBattlerAttacker].moves[i];
                movePPData.pp[i] = gBattleMons[gBattlerAttacker].pp[i];
            }
            movePPData.ppBonuses = gBattleMons[gBattlerAttacker].ppBonuses;
            BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_MOVES_PP_BATTLE, 0, sizeof(struct MovePPInfo), &movePPData);
            MarkBattlerForControllerExec(gActiveBattler);
            PREPARE_MOVE_BUFFER(gBattleTextBuff1, gLastPrintedMoves[gBattlerTarget]);
            gBattlescriptCurrInstr += 5;
        }
    }
    else //sketch fail
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static bool8 IsTwoTurnsMove(u16 move)
{
    u8 effect = gBattleMoves[move].effect;
    if (effect == EFFECT_SKULL_BASH || effect == EFFECT_RAZOR_WIND || effect == EFFECT_SKY_ATTACK || effect == EFFECT_SOLAR_BEAM || effect == EFFECT_SEMI_INVULNERABLE || effect == EFFECT_BIDE)
        return TRUE;
    else
        return FALSE;
}

static bool8 IsInvalidForSleepTalkOrAssist(u16 move)
{
    if (move == 0 || move == MOVE_SLEEP_TALK || move == MOVE_ASSIST || move == MOVE_MIRROR_MOVE || move == MOVE_METRONOME)
        return TRUE;
    else
        return FALSE;
}

static u8 AttacksThisTurn(u8 battler, u16 move) //Note: returns 1 if it's a charging turn, otherwise 2
{
    //first argument is unused
    u8 effect;
    if (gBattleMoves[move].effect == EFFECT_SOLAR_BEAM && (gBattleWeather & B_WEATHER_SUN))
        return 2;
    effect = gBattleMoves[move].effect;
    if (effect == EFFECT_SKULL_BASH || effect == EFFECT_RAZOR_WIND || effect == EFFECT_SKY_ATTACK || effect == EFFECT_SOLAR_BEAM || effect == EFFECT_SEMI_INVULNERABLE || effect == EFFECT_BIDE)
    {
        if ((gHitMarker & HITMARKER_CHARGING))
            return 1;
    }
    return 2;
}

static void Cmd_trychoosesleeptalkmove(void)
{
    u8 unusableMovesBits = 0;
    s32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (IsInvalidForSleepTalkOrAssist(gBattleMons[gBattlerAttacker].moves[i]) || gBattleMons[gBattlerAttacker].moves[i] == MOVE_FOCUS_PUNCH
            || gBattleMons[gBattlerAttacker].moves[i] == MOVE_UPROAR || IsTwoTurnsMove(gBattleMons[gBattlerAttacker].moves[i]))
                unusableMovesBits |= gBitTable[i];
    }
    unusableMovesBits = CheckMoveLimitations(gBattlerAttacker, unusableMovesBits, (MOVE_LIMITATIONS_ALL & ~MOVE_LIMITATION_PP));
    if (unusableMovesBits == ALL_MOVES_MASK) //all 4 moves cannot be chosen
        gBattlescriptCurrInstr += 5;
    else //at least one move can be chosen
    {
        u32 movePosition;
        do
        {
            movePosition = Random() & (MAX_MON_MOVES - 1);
        } while ((gBitTable[movePosition] & unusableMovesBits));

        gCalledMove = gBattleMons[gBattlerAttacker].moves[movePosition];
        gCurrMovePos = movePosition;
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
        gBattlerTarget = GetMoveTarget(gCalledMove, NO_TARGET_OVERRIDE);
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_setdestinybond(void)
{
    gBattleMons[gBattlerAttacker].status2 |= STATUS2_DESTINY_BOND;
    gBattlescriptCurrInstr++;
}

static void TrySetDestinyBondToHappen(void)
{
    u8 sideAttacker = GetBattlerSide(gBattlerAttacker);
    u8 sideTarget = GetBattlerSide(gBattlerTarget);
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_DESTINY_BOND && sideAttacker != sideTarget && !(gHitMarker & HITMARKER_GRUDGE))
        gHitMarker |= HITMARKER_DESTINYBOND;
}

static void Cmd_trysetdestinybondtohappen(void)
{
    TrySetDestinyBondToHappen();
    gBattlescriptCurrInstr++;
}

static void Cmd_remaininghptopower(void)
{
    s32 hpFraction = GetScaledHPFraction(gBattleMons[gBattlerAttacker].hp, gBattleMons[gBattlerAttacker].maxHP, 48);
    s32 i;
    for (i = 0; i < (s32)ARRAY_COUNT(sFlailHpScaleToPowerTable); i += 2)
    {
        if (hpFraction <= sFlailHpScaleToPowerTable[i])
            break;
    }
    gDynamicBasePower = sFlailHpScaleToPowerTable[i + 1];
    gBattlescriptCurrInstr++;
}

static void atkAD_tryspiteppreduce(void)
{
    if (gLastMoves[gBattlerTarget] != MOVE_NONE && gLastMoves[gBattlerTarget] != MOVE_UNAVAILABLE && !(gStatuses3[gBattlerTarget] & STATUS3_SEMI_INVULNERABLE))
    {
        s32 i;
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if (gLastMoves[gBattlerTarget] == gBattleMons[gBattlerTarget].moves[i])
                break;
        }
        if (i != MAX_MON_MOVES && gBattleMons[gBattlerTarget].pp[i] > 1)
        {
            s32 ppToDeduct = (Random() & 3) + 2;
            if (gBattleMons[gBattlerTarget].pp[i] < ppToDeduct)
                ppToDeduct = gBattleMons[gBattlerTarget].pp[i];

            PREPARE_MOVE_BUFFER(gBattleTextBuff1, gLastMoves[gBattlerTarget]);
            ConvertIntToDecimalStringN(gBattleTextBuff2, ppToDeduct, 0, 1);
            PREPARE_BYTE_NUMBER_BUFFER(gBattleTextBuff2, 1, ppToDeduct);

            gBattleMons[gBattlerTarget].pp[i] -= ppToDeduct;
            gActiveBattler = gBattlerTarget;
            if (!(gDisableStructs[gActiveBattler].mimickedMoves & gBitTable[i])
                && !(gBattleMons[gActiveBattler].status2 & STATUS2_TRANSFORMED))
            {
                BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_PPMOVE1_BATTLE + i, 0, sizeof(gBattleMons[gActiveBattler].pp[i]), &gBattleMons[gActiveBattler].pp[i]);
                MarkBattlerForControllerExec(gActiveBattler);
            }
            gBattlescriptCurrInstr += 5;
            if (gBattleMons[gBattlerTarget].pp[i] == 0)
                CancelMultiTurnMoves(gBattlerTarget);
            return;
        }
    }
    gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkAE_healpartystatus(void)
{
    register u32 zero2 asm("r4") = 0;
    u32 zero = zero2;
    u8 toHeal = 0;
    if (gCurrentMove == MOVE_HEAL_BELL)
    {
        struct Pokemon* party;
        s32 i;

        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_BELL;
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
            party = gPlayerParty;
        else
            party = gEnemyParty;

        if (gBattleMons[gBattlerAttacker].ability != ABILITY_SOUNDPROOF)
        {
            gBattleMons[gBattlerAttacker].status1 = STATUS1_NONE;
        }
        else
        {
            RecordAbilityBattle(gBattlerAttacker, gBattleMons[gBattlerAttacker].ability);
            gBattleCommunication[MULTISTRING_CHOOSER] |= B_MSG_BELL_SOUNDPROOF_ATTACKER;
        }

        gActiveBattler = gBattleStruct->battler = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_FLANK);
        if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && !(gAbsentBattlerFlags & gBitTable[gActiveBattler]))
        {
            if (gBattleMons[gActiveBattler].ability != ABILITY_SOUNDPROOF)
            {
                gBattleMons[gActiveBattler].status1 = STATUS1_NONE;
            }
            else
            {
                RecordAbilityBattle(gActiveBattler, gBattleMons[gActiveBattler].ability);
                gBattleCommunication[MULTISTRING_CHOOSER] |= B_MSG_BELL_SOUNDPROOF_PARTNER;
            }
        }

        for (i = 0; i < PARTY_SIZE; i++)
        {
            u16 species = GetMonData(&party[i], MON_DATA_SPECIES_OR_EGG);
            u8 abilityNum = GetMonData(&party[i], MON_DATA_ABILITY_NUM);
            if (species != 0 && species != SPECIES_EGG)
            {
                u8 ability;
                if (gBattlerPartyIndexes[gBattlerAttacker] == i)
                    ability = gBattleMons[gBattlerAttacker].ability;
                else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && gBattlerPartyIndexes[gActiveBattler] == i && !(gAbsentBattlerFlags & gBitTable[gActiveBattler]))
                    ability = gBattleMons[gActiveBattler].ability;
                else
                    ability = GetAbilityBySpecies(species, abilityNum);
                if (ability != ABILITY_SOUNDPROOF)
                    toHeal |= (1 << i);
            }
        }
    }
    else //Aromatherapy
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SOOTHING_AROMA;
        toHeal = ((1 << PARTY_SIZE) - 1);
        gBattleMons[gBattlerAttacker].status1 = zero2;

        gActiveBattler = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_FLANK);
        if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && !(gAbsentBattlerFlags & gBitTable[gActiveBattler]))
            gBattleMons[gActiveBattler].status1 = STATUS1_NONE;

    }
    //missing check?
    gActiveBattler = gBattlerAttacker;
    BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, toHeal, sizeof(zero), &zero);
    MarkBattlerForControllerExec(gActiveBattler);

    gBattlescriptCurrInstr++;
}

static void atkAF_cursetarget(void)
{
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_CURSED || gStatuses3[gBattlerTarget] & STATUS3_SEMI_INVULNERABLE)
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        gBattleMons[gBattlerTarget].status2 |= STATUS2_CURSED;
        gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 2;
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_trysetspikes(void)
{
    u8 targetSide = GetBattlerSide(gBattlerAttacker) ^ BIT_SIDE;
    if (gSideTimers[targetSide].spikesAmount == 3)
    {
        gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        gSideStatuses[targetSide] |= SIDE_STATUS_SPIKES;
        gSideTimers[targetSide].spikesAmount++;
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_setforesight(void)
{
    gBattleMons[gBattlerTarget].status2 |= STATUS2_FORESIGHT;
    gBattlescriptCurrInstr++;
}

static void Cmd_trysetperishsong(void)
{
    s32 notAffectedCount = 0, i;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gStatuses3[i] & STATUS3_PERISH_SONG || gBattleMons[i].ability == ABILITY_SOUNDPROOF)
            notAffectedCount++;
        else
        {
            gStatuses3[i] |= STATUS3_PERISH_SONG;
            gDisableStructs[i].perishSongTimer = 3;
            gDisableStructs[i].perishSongTimerStartValue = 3;
        }
    }

    PressurePPLoseOnUsingPerishSong(gBattlerAttacker);
    if (notAffectedCount == gBattlersCount)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
        gBattlescriptCurrInstr += 5;
}

static void Cmd_rolloutdamagecalculation(void)
{
    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
    {
        CancelMultiTurnMoves(gBattlerAttacker);
        gBattlescriptCurrInstr = BattleScript_MoveMissedPause;
    }
    else
    {
        s32 i;
        if (!(gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS)) //first hit
        {
            gDisableStructs[gBattlerAttacker].rolloutTimer = 5;
            gDisableStructs[gBattlerAttacker].rolloutTimerStartValue = 5;
            gBattleMons[gBattlerAttacker].status2 |= STATUS2_MULTIPLETURNS;
            gLockedMoves[gBattlerAttacker] = gCurrentMove;
        }
        if (--gDisableStructs[gBattlerAttacker].rolloutTimer == 0)
            gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_MULTIPLETURNS);

        gDynamicBasePower = gBattleMoves[gCurrentMove].power;
        for (i = 1; i < (5 - gDisableStructs[gBattlerAttacker].rolloutTimer); i++)
            gDynamicBasePower *= 2;

        if (gBattleMons[gBattlerAttacker].status2 & STATUS2_DEFENSE_CURL)
            gDynamicBasePower *= 2;

        gBattlescriptCurrInstr++;
    }
}

static void Cmd_jumpifconfusedandstatmaxed(void)
{
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_CONFUSION && gBattleMons[gBattlerTarget].statStages[T2_READ_8(gBattlescriptCurrInstr + 1)] == MAX_STAT_STAGE)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 2);
    else
        gBattlescriptCurrInstr += 6;
}

static void Cmd_furycuttercalc(void)
{
    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
    {
        gDisableStructs[gBattlerAttacker].furyCutterCounter = 0;
        gBattlescriptCurrInstr = BattleScript_MoveMissedPause;
    }
    else
    {
        s32 i;

        if (gDisableStructs[gBattlerAttacker].furyCutterCounter != 5)
            gDisableStructs[gBattlerAttacker].furyCutterCounter++;

        gDynamicBasePower = gBattleMoves[gCurrentMove].power;
        for (i = 1; i < gDisableStructs[gBattlerAttacker].furyCutterCounter; i++)
            gDynamicBasePower *= 2;

        gBattlescriptCurrInstr++;
    }
}

static void Cmd_friendshiptodamagecalculation(void)
{
    if (gBattleMoves[gCurrentMove].effect == EFFECT_RETURN)
        gDynamicBasePower = 10 * (gBattleMons[gBattlerAttacker].friendship) / 25;
    else //EFFECT_FRUSTRATION
        gDynamicBasePower = 10 * (MAX_FRIENDSHIP - gBattleMons[gBattlerAttacker].friendship) / 25;
    gBattlescriptCurrInstr++;
}

static void atkB7_presentdamagecalculation(void)
{
    s32 rand = Random() & 0xFF;
    if (rand < 102)
        gDynamicBasePower = 40;
    else if (rand < 178)
        gDynamicBasePower = 80;
    else if (rand < 204)
        gDynamicBasePower = 120;
    else
    {
        gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 4;
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattleMoveDamage *= -1;
    }
    if (rand < 204)
        gBattlescriptCurrInstr = BattleScript_HitFromCritCalc;
    else if (gBattleMons[gBattlerTarget].maxHP == gBattleMons[gBattlerTarget].hp)
        gBattlescriptCurrInstr = BattleScript_AlreadyAtFullHp;
    else
    {
        //gMoveResultFlags &= ~(MOVE_RESULT_DOESNT_AFFECT_FOE); only in Emerald
        gBattlescriptCurrInstr = BattleScript_PresentHealTarget;
    }
}

static void atkB8_setsafeguard(void)
{
    if (gSideStatuses[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE] & SIDE_STATUS_SAFEGUARD)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SIDE_STATUS_FAILED;
    }
    else
    {
        gSideStatuses[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE] |= SIDE_STATUS_SAFEGUARD;
        gSideTimers[GetBattlerPosition(gBattlerAttacker) & BIT_SIDE].safeguardTimer = 5;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SET_SAFEGUARD;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_magnitudedamagecalculation(void)
{
    s32 magnitude = Random() % 100;
    if (magnitude < 5)
    {
        gDynamicBasePower = 10;
        magnitude = 4;
    }
    else if (magnitude < 15)
    {
        gDynamicBasePower = 30;
        magnitude = 5;
    }
    else if (magnitude < 35)
    {
        gDynamicBasePower = 50;
        magnitude = 6;
    }
    else if (magnitude < 65)
    {
        gDynamicBasePower = 70;
        magnitude = 7;
    }
    else if (magnitude < 85)
    {
        gDynamicBasePower = 90;
        magnitude = 8;
    }
    else if (magnitude < 95)
    {
        gDynamicBasePower = 110;
        magnitude = 9;
    }
    else
    {
        gDynamicBasePower = 150;
        magnitude = 10;
    }

    PREPARE_BYTE_NUMBER_BUFFER(gBattleTextBuff1, 2, magnitude);

    for (gBattlerTarget = 0; gBattlerTarget < gBattlersCount; gBattlerTarget++)
    {
        if (gBattlerTarget == gBattlerAttacker)
            continue;
        if (!(gAbsentBattlerFlags & gBitTable[gBattlerTarget])) //a valid target was found
            break;
    }
    gBattlescriptCurrInstr++;
}

static void atkBA_jumpifnopursuitswitchdmg(void)
{
    if (gMultiHitCounter == 1)
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
            gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        else
            gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
    }
    else
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
            gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        else
            gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
    }

    if (gChosenActionByBattler[gBattlerTarget] == B_ACTION_USE_MOVE && gBattlerAttacker == gSharedMem[BSTRUCT_OFF(moveTarget) + gBattlerTarget] && !(gBattleMons[gBattlerTarget].status1 & (STATUS1_SLEEP | STATUS1_FREEZE))
        && gBattleMons[gBattlerAttacker].hp && !gDisableStructs[gBattlerTarget].truantCounter && gChosenMoveByBattler[gBattlerTarget] == MOVE_PURSUIT)
    {
        s32 i;
        for (i = 0; i < gBattlersCount; i++)
        {
            if (gBattlerByTurnOrder[i] == gBattlerTarget)
                gActionsByTurnOrder[i] = B_ACTION_TRY_FINISH;
        }
        gCurrentMove = MOVE_PURSUIT;
        gBattlescriptCurrInstr += 5;
        gBattleStruct->animTurn = 1;
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
    }
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_setsunny(void)
{
    if (gBattleWeather & B_WEATHER_SUN)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEATHER_FAILED;
    }
    else
    {
        gBattleWeather = B_WEATHER_SUN_TEMPORARY;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STARTED_SUNLIGHT;
        gWishFutureKnock.weatherDuration = 5;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_maxattackhalvehp(void) //belly drum
{
    u32 halfHp = gBattleMons[gBattlerAttacker].maxHP / 2;
    if (!(gBattleMons[gBattlerAttacker].maxHP / 2))
        halfHp = 1;

    if (gBattleMons[gBattlerAttacker].statStages[STAT_ATK] < MAX_STAT_STAGE && gBattleMons[gBattlerAttacker].hp > halfHp)
    {
        gBattleMons[gBattlerAttacker].statStages[STAT_ATK] = MAX_STAT_STAGE;
        gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 2;
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattlescriptCurrInstr += 5;
    }
    else
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_copyfoestats(void) //psych up
{
    s32 i;
    for (i = 0; i < NUM_BATTLE_STATS; i++)
    {
        gBattleMons[gBattlerAttacker].statStages[i] = gBattleMons[gBattlerTarget].statStages[i];
    }
    gBattlescriptCurrInstr += 5; //why not 1? possible unused fail possibility?
}

static void atkBE_rapidspinfree(void) //rapid spin
{
    if (gBattleMons[gBattlerAttacker].status2 & STATUS2_WRAPPED)
    {
        gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_WRAPPED);
        gBattlerTarget = gSharedMem[BSTRUCT_OFF(wrappedBy) + gBattlerAttacker];
        gBattleTextBuff1[0] = B_BUFF_PLACEHOLDER_BEGIN;
        gBattleTextBuff1[1] = B_BUFF_MOVE;
        gBattleTextBuff1[2] = gSharedMem[BSTRUCT_OFF(wrappedMove) + 2 * gBattlerAttacker + 0];
        gBattleTextBuff1[3] = gSharedMem[BSTRUCT_OFF(wrappedMove) + 2 * gBattlerAttacker + 1];
        gBattleTextBuff1[4] = B_BUFF_EOS;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_WrapFree;
    }
    else if (gStatuses3[gBattlerAttacker] & STATUS3_LEECHSEED)
    {
        gStatuses3[gBattlerAttacker] &= ~(STATUS3_LEECHSEED);
        gStatuses3[gBattlerAttacker] &= ~(STATUS3_LEECHSEED_BATTLER);
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_LeechSeedFree;
    }
    else if (gSideStatuses[GetBattlerSide(gBattlerAttacker)] & SIDE_STATUS_SPIKES)
    {
        gSideStatuses[GetBattlerSide(gBattlerAttacker)] &= ~(SIDE_STATUS_SPIKES);
        gSideTimers[GetBattlerSide(gBattlerAttacker)].spikesAmount = 0;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_SpikesFree;
    }
    else
        gBattlescriptCurrInstr++;
}

static void Cmd_setdefensecurlbit(void)
{
    gBattleMons[gBattlerAttacker].status2 |= STATUS2_DEFENSE_CURL;
    gBattlescriptCurrInstr++;
}

static void Cmd_recoverbasedonsunlight(void)
{
    gBattlerTarget = gBattlerAttacker;
    if (gBattleMons[gBattlerAttacker].hp != gBattleMons[gBattlerAttacker].maxHP)
    {
        if (!gBattleWeather || !WEATHER_HAS_EFFECT)
            gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 2;
        else if (gBattleWeather & B_WEATHER_SUN)
            gBattleMoveDamage = 20 * gBattleMons[gBattlerAttacker].maxHP / 30;
        else //not sunny weather
            gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 4;
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattleMoveDamage *= -1;
        gBattlescriptCurrInstr += 5;
    }
    else
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_hiddenpowercalc(void)
{
	u8 powerBits = ((gBattleMons[gBattlerAttacker].hpIV & 2) >> 1) |
	           ((gBattleMons[gBattlerAttacker].attackIV & 2)) |
	           ((gBattleMons[gBattlerAttacker].defenseIV & 2) << 1) |
	           ((gBattleMons[gBattlerAttacker].speedIV & 2) << 2) |
	           ((gBattleMons[gBattlerAttacker].spAttackIV & 2) << 3) |
	           ((gBattleMons[gBattlerAttacker].spDefenseIV & 2) << 4);
	u8 typeBits = ((gBattleMons[gBattlerAttacker].hpIV & 1)) |
	          ((gBattleMons[gBattlerAttacker].attackIV & 1) << 1) |
	          ((gBattleMons[gBattlerAttacker].defenseIV & 1) << 2) |
	          ((gBattleMons[gBattlerAttacker].speedIV & 1) << 3) |
	          ((gBattleMons[gBattlerAttacker].spAttackIV & 1) << 4) |
	          ((gBattleMons[gBattlerAttacker].spDefenseIV & 1) << 5);

	gDynamicBasePower = 30 + (powerBits * 40 / 63);

	gBattleStruct->dynamicMoveType = ((typeBits * 15) / 63) + 1;
	if (gBattleStruct->dynamicMoveType >= TYPE_MYSTERY)
		gBattleStruct->dynamicMoveType++;

	gBattleStruct->dynamicMoveType |= (F_DYNAMIC_TYPE_IGNORE_PHYSICALITY | F_DYNAMIC_TYPE_SET);

	gBattlescriptCurrInstr++;
}

static void Cmd_selectfirstvalidtarget(void)
{
    for (gBattlerTarget = 0; gBattlerTarget < gBattlersCount; gBattlerTarget++)
    {
        if (gBattlerTarget == gBattlerAttacker)
            continue;
        if (!(gAbsentBattlerFlags & gBitTable[gBattlerTarget]))
            break;
    }
    gBattlescriptCurrInstr++;
}

static void atkC3_trysetfutureattack(void)
{
    if (gWishFutureKnock.futureSightCounter[gBattlerTarget] != 0)
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        gWishFutureKnock.futureSightMove[gBattlerTarget] = gCurrentMove;
        gWishFutureKnock.futureSightAttacker[gBattlerTarget] = gBattlerAttacker;
        gWishFutureKnock.futureSightCounter[gBattlerTarget] = 3;
        gWishFutureKnock.futureSightDmg[gBattlerTarget] = CalculateBaseDamage(&gBattleMons[gBattlerAttacker], &gBattleMons[gBattlerTarget], gCurrentMove,
                                                    gSideStatuses[GetBattlerPosition(gBattlerTarget) & BIT_SIDE], 0,
                                                    0, gBattlerAttacker, gBattlerTarget);

        if (gProtectStructs[gBattlerAttacker].helpingHand)
            gWishFutureKnock.futureSightDmg[gBattlerTarget] = gWishFutureKnock.futureSightDmg[gBattlerTarget] * 15 / 10;

        if (gCurrentMove == MOVE_DOOM_DESIRE)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DOOM_DESIRE;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_FUTURE_SIGHT;

        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_trydobeatup(void)
{
    struct Pokemon *party;

    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;
    if (gBattleMons[gBattlerTarget].hp == 0)
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        u8 beforeLoop = gBattleCommunication[MULTIUSE_STATE];

        for (;gBattleCommunication[MULTIUSE_STATE] < PARTY_SIZE; ++gBattleCommunication[MULTIUSE_STATE])
        {
            if (GetMonData(&party[gBattleCommunication[MULTIUSE_STATE]], MON_DATA_HP)
             && GetMonData(&party[gBattleCommunication[MULTIUSE_STATE]], MON_DATA_SPECIES_OR_EGG)
             && GetMonData(&party[gBattleCommunication[MULTIUSE_STATE]], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG
             && !GetMonData(&party[gBattleCommunication[MULTIUSE_STATE]], MON_DATA_STATUS))
                break;
        }
        if (gBattleCommunication[MULTIUSE_STATE] < PARTY_SIZE)
        {
            PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattleCommunication[MULTIUSE_STATE]);
            gBattlescriptCurrInstr += 9;

            gBattleMoveDamage = gSpeciesInfo[GetMonData(&party[gBattleCommunication[MULTIUSE_STATE]], MON_DATA_SPECIES)].baseAttack;
            gBattleMoveDamage *= gBattleMoves[gCurrentMove].power;
            gBattleMoveDamage *= (GetMonData(&party[gBattleCommunication[MULTIUSE_STATE]], MON_DATA_LEVEL) * 2 / 5 + 2);
            gBattleMoveDamage /= gSpeciesInfo[gBattleMons[gBattlerTarget].species].baseDefense;
            gBattleMoveDamage = (gBattleMoveDamage / 50) + 2;
            if (gProtectStructs[gBattlerAttacker].helpingHand)
                gBattleMoveDamage = gBattleMoveDamage * 15 / 10;
            ++gBattleCommunication[MULTIUSE_STATE];
        }
        else if (beforeLoop != 0)
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        else
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 5);
    }
}

static void Cmd_setsemiinvulnerablebit(void)
{
    switch (gCurrentMove)
    {
    case MOVE_FLY:
    case MOVE_BOUNCE:
        gStatuses3[gBattlerAttacker] |= STATUS3_ON_AIR;
        break;
    case MOVE_DIG:
        gStatuses3[gBattlerAttacker] |= STATUS3_UNDERGROUND;
        break;
    case MOVE_DIVE:
        gStatuses3[gBattlerAttacker] |= STATUS3_UNDERWATER;
        break;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_clearsemiinvulnerablebit(void)
{
    switch (gCurrentMove)
    {
    case MOVE_FLY:
    case MOVE_BOUNCE:
        gStatuses3[gBattlerAttacker] &= ~STATUS3_ON_AIR;
        break;
    case MOVE_DIG:
        gStatuses3[gBattlerAttacker] &= ~STATUS3_UNDERGROUND;
        break;
    case MOVE_DIVE:
        gStatuses3[gBattlerAttacker] &= ~STATUS3_UNDERWATER;
        break;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_setminimize(void)
{
    if (gHitMarker & HITMARKER_OBEYS)
        gStatuses3[gBattlerAttacker] |= STATUS3_MINIMIZED;
    gBattlescriptCurrInstr++;
}

static void Cmd_sethail(void)
{
    if (gBattleWeather & B_WEATHER_HAIL_TEMPORARY)
    {
        gMoveResultFlags |= MOVE_RESULT_MISSED;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEATHER_FAILED;
    }
    else
    {
        gBattleWeather = B_WEATHER_HAIL_TEMPORARY;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_STARTED_HAIL;
        gWishFutureKnock.weatherDuration = 5;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_trymemento(void) //memento
{
    if (gBattleMons[gBattlerTarget].statStages[STAT_ATK] == MIN_STAT_STAGE
        && gBattleMons[gBattlerTarget].statStages[STAT_SPATK] == MIN_STAT_STAGE
        && gBattleCommunication[MISS_TYPE] != 1)
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gActiveBattler = gBattlerAttacker;
        gBattleMoveDamage = gBattleMons[gActiveBattler].hp;
        BtlController_EmitHealthBarUpdate(B_COMM_TO_CONTROLLER, INSTANT_HP_BAR_DROP);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_setforcedtarget(void) //follow me
{
    gSideTimers[GetBattlerSide(gBattlerAttacker)].followmeTimer = 1;
    gSideTimers[GetBattlerSide(gBattlerAttacker)].followmeTarget = gBattlerAttacker;
    gBattlescriptCurrInstr++;
}

static void Cmd_setcharge(void)
{
    gStatuses3[gBattlerAttacker] |= STATUS3_CHARGED_UP;
    gDisableStructs[gBattlerAttacker].chargeTimer = 2;
    gDisableStructs[gBattlerAttacker].chargeTimerStartValue = 2;
    gBattlescriptCurrInstr++;
}

static void Cmd_callenvironmentattack(void) //nature power
{
    gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
    gCurrentMove = sNaturePowerMoves[gBattleEnvironment];
    gBattlerTarget = GetMoveTarget(gCurrentMove, NO_TARGET_OVERRIDE);
    BattleScriptPush(gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect]);
    gBattlescriptCurrInstr++;
}

static void Cmd_cureifburnedparalyzedorpoisoned(void) //refresh
{
    if (gBattleMons[gBattlerAttacker].status1 & (STATUS1_POISON | STATUS1_BURN | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON))
    {
        gBattleMons[gBattlerAttacker].status1 = STATUS1_NONE;
        gBattlescriptCurrInstr += 5;
        gActiveBattler = gBattlerAttacker;
        BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, 0, sizeof(gBattleMons[gActiveBattler].status1), &gBattleMons[gActiveBattler].status1);
        MarkBattlerForControllerExec(gActiveBattler);
    }
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_settorment(void)
{
    if (gBattleMons[gBattlerTarget].status2 & STATUS2_TORMENT)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gBattleMons[gBattlerTarget].status2 |= STATUS2_TORMENT;
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_jumpifnodamage(void)
{
    if (gProtectStructs[gBattlerAttacker].physicalDmg || gProtectStructs[gBattlerAttacker].specialDmg)
        gBattlescriptCurrInstr += 5;
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_settaunt(void)
{
    if (gDisableStructs[gBattlerTarget].tauntTimer == 0)
    {
        gDisableStructs[gBattlerTarget].tauntTimer = 2;
        gDisableStructs[gBattlerTarget].tauntTimer2 = 2;
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_trysethelpinghand(void)
{
    gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_FLANK);
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && !(gAbsentBattlerFlags & gBitTable[gBattlerTarget])
        && !gProtectStructs[gBattlerAttacker].helpingHand && !gProtectStructs[gBattlerTarget].helpingHand)
    {
        gProtectStructs[gBattlerTarget].helpingHand = TRUE;
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void atkD2_tryswapitems(void)
{
    // opponent can't swap items with player in regular battles
    if ((GetBattlerSide(gBattlerAttacker) == B_SIDE_OPPONENT
         && !(gBattleTypeFlags & (BATTLE_TYPE_LINK
                                  | BATTLE_TYPE_EREADER_TRAINER
                                  | BATTLE_TYPE_BATTLE_TOWER))
         && gTrainerBattleOpponent_A != TRAINER_SECRET_BASE)) {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else {
        u8 sideAttacker = GetBattlerSide(gBattlerAttacker);

        // you can't swap items if they were knocked off in regular battles
        if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK
                                  | BATTLE_TYPE_EREADER_TRAINER
                                  | BATTLE_TYPE_BATTLE_TOWER))
            && gTrainerBattleOpponent_A != TRAINER_SECRET_BASE
            && (gWishFutureKnock.knockedOffMons[sideAttacker] & gBitTable[gBattlerPartyIndexes[gBattlerAttacker]]
            )) {
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        }
            // can't swap if two pokemon don't have an item
            // or if either of them is an enigma berry or a mail
        else if ((gBattleMons[gBattlerAttacker].item == ITEM_NONE && gBattleMons[gBattlerTarget].item == ITEM_NONE)
                 || gBattleMons[gBattlerAttacker].item == ITEM_ENIGMA_BERRY
                 || gBattleMons[gBattlerTarget].item == ITEM_ENIGMA_BERRY
                 || IS_ITEM_MAIL(gBattleMons[gBattlerAttacker].item)
                 || IS_ITEM_MAIL(gBattleMons[gBattlerTarget].item)) {
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        }
            // check if ability prevents swapping
        else if (gBattleMons[gBattlerTarget].ability == ABILITY_STICKY_HOLD) {
            gBattlescriptCurrInstr = BattleScript_StickyHoldActivates;
            gLastUsedAbility = gBattleMons[gBattlerTarget].ability;
            RecordAbilityBattle(gBattlerTarget, gLastUsedAbility);
        }
            // took a while, but all checks passed and items can be safely swapped
        else {
            u16 oldItemAtk, *newItemAtk;

            newItemAtk = (u16 *)(gBattleStruct->changedItems + gBattlerAttacker);
            oldItemAtk = gBattleMons[gBattlerAttacker].item;
            *newItemAtk = gBattleMons[gBattlerTarget].item;

            gBattleMons[gBattlerAttacker].item = ITEM_NONE;
            gBattleMons[gBattlerTarget].item = oldItemAtk;

            gActiveBattler = gBattlerAttacker;
            BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE, 0, sizeof(*newItemAtk), newItemAtk);
            MarkBattlerForControllerExec(gBattlerAttacker);

            gActiveBattler = gBattlerTarget;
            BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE, 0, sizeof(gBattleMons[gBattlerTarget].item), &gBattleMons[gBattlerTarget].item);
            MarkBattlerForControllerExec(gBattlerTarget);

            gSharedMem[BSTRUCT_OFF(choicedMove) + 2 * gBattlerTarget   + 0] = 0;
            gSharedMem[BSTRUCT_OFF(choicedMove) + 2 * gBattlerTarget   + 1] = 0;
            gSharedMem[BSTRUCT_OFF(choicedMove) + 2 * gBattlerAttacker + 0] = 0;
            gSharedMem[BSTRUCT_OFF(choicedMove) + 2 * gBattlerAttacker + 1] = 0;

            gBattlescriptCurrInstr += 5;

            PREPARE_ITEM_BUFFER(gBattleTextBuff1, *newItemAtk)
            PREPARE_ITEM_BUFFER(gBattleTextBuff2, oldItemAtk)

            if (oldItemAtk != ITEM_NONE && *newItemAtk != ITEM_NONE)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ITEM_SWAP_BOTH; // attacker's item -> <- target's item
            else if (oldItemAtk == ITEM_NONE && *newItemAtk != ITEM_NONE)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ITEM_SWAP_TAKEN; // nothing -> <- target's item
            else
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ITEM_SWAP_GIVEN; // attacker's item -> <- nothing
        }
    }
}

static void Cmd_trycopyability(void) //role play
{
    if (gBattleMons[gBattlerTarget].ability != ABILITY_NONE && gBattleMons[gBattlerTarget].ability != ABILITY_WONDER_GUARD)
    {
        gBattleMons[gBattlerAttacker].ability = gBattleMons[gBattlerTarget].ability;
        gLastUsedAbility = gBattleMons[gBattlerTarget].ability;
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_trywish(void)
{
    switch (T2_READ_8(gBattlescriptCurrInstr + 1))
    {
    case 0: //use wish
        if (gWishFutureKnock.wishCounter[gBattlerAttacker] == 0)
        {
            gWishFutureKnock.wishCounter[gBattlerAttacker] = 2;
            gWishFutureKnock.wishMonId[gBattlerAttacker] = gBattlerPartyIndexes[gBattlerAttacker];
            gBattlescriptCurrInstr += 6;
        }
        else
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 2);
        break;
    case 1: //heal effect
        PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, gBattlerTarget, gWishFutureKnock.wishMonId[gBattlerTarget]);
        gBattleMoveDamage = gBattleMons[gBattlerTarget].maxHP / 2;
        if (gBattleMoveDamage == 0)
            gBattleMoveDamage = 1;
        gBattleMoveDamage *= -1;
        if (gBattleMons[gBattlerTarget].hp == gBattleMons[gBattlerTarget].maxHP)
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 2);
        else
            gBattlescriptCurrInstr += 6;
        break;
    }
}

static void Cmd_trysetroots(void) //ingrain
{
    if (gStatuses3[gBattlerAttacker] & STATUS3_ROOTED)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gStatuses3[gBattlerAttacker] |= STATUS3_ROOTED;
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_doubledamagedealtifdamaged(void)
{
    if ((gProtectStructs[gBattlerAttacker].physicalDmg && gProtectStructs[gBattlerAttacker].physicalBattlerId == gBattlerTarget)
        || (gProtectStructs[gBattlerAttacker].specialDmg && gProtectStructs[gBattlerAttacker].specialBattlerId == gBattlerTarget))
            gBattleStruct->dmgMultiplier = 2;
    gBattlescriptCurrInstr++;
}

static void Cmd_setyawn(void)
{
    if (gStatuses3[gBattlerTarget] & STATUS3_YAWN || (u8) gBattleMons[gBattlerTarget].status1)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gStatuses3[gBattlerTarget] |= STATUS3_YAWN_TURN(2);
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_setdamagetohealthdifference(void)
{
    if (gBattleMons[gBattlerTarget].hp <= gBattleMons[gBattlerAttacker].hp)
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        gBattleMoveDamage = gBattleMons[gBattlerTarget].hp - gBattleMons[gBattlerAttacker].hp;
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_scaledamagebyhealthratio(void)
{
    if (gDynamicBasePower == 0)
    {
        u8 power = gBattleMoves[gCurrentMove].power;
        gDynamicBasePower = gBattleMons[gBattlerAttacker].hp * power / gBattleMons[gBattlerAttacker].maxHP;
        if (gDynamicBasePower == 0)
            gDynamicBasePower = 1;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_tryswapabilities(void)
{
    if ((gBattleMons[gBattlerAttacker].ability == ABILITY_NONE && gBattleMons[gBattlerTarget].ability == ABILITY_NONE)
     || gBattleMons[gBattlerAttacker].ability == ABILITY_WONDER_GUARD || gBattleMons[gBattlerTarget].ability == ABILITY_WONDER_GUARD
     || gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        u8 abilityAtk = gBattleMons[gBattlerAttacker].ability;
        gBattleMons[gBattlerAttacker].ability = gBattleMons[gBattlerTarget].ability;
        gBattleMons[gBattlerTarget].ability = abilityAtk;
        gBattlescriptCurrInstr += 5;
    }
}

static void atkDB_tryimprison(void)
{
    u8 r8 = 0;
    if ((gStatuses3[gBattlerAttacker] & STATUS3_IMPRISONED_OTHERS))
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
    else
    {
        u8 battler;
        PressurePPLoseOnUsingImprison(gBattlerAttacker);
        for (battler = 0; battler < gBattlersCount; battler++)
        {
            if (r8 != GetBattlerSide(battler))
            {
                s32 attackerMoveId;
                for (attackerMoveId = 0; attackerMoveId < MAX_MON_MOVES; attackerMoveId++)
                {
                    s32 i;
                    for (i = 0; i < MAX_MON_MOVES; i++)
                    {
                        if (gBattleMons[gBattlerAttacker].moves[attackerMoveId] == gBattleMons[battler].moves[i] && gBattleMons[gBattlerAttacker].moves[attackerMoveId])
                            break;
                    }
                    if (i != MAX_MON_MOVES)
                        break;
                }
                if (attackerMoveId != MAX_MON_MOVES)
                {
                    gStatuses3[gBattlerAttacker] |= STATUS3_IMPRISONED_OTHERS;
                    gBattlescriptCurrInstr += 5;
                    break;
                }
            }
        }
        if (battler == gBattlersCount)
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_trysetgrudge(void)
{
    if (gStatuses3[gBattlerAttacker] & STATUS3_GRUDGE)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gStatuses3[gBattlerAttacker] |= STATUS3_GRUDGE;
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_weightdamagecalculation(void)
{
    s32 i;
    for (i = 0; sWeightToDamageTable[i] != 0xFFFF; i += 2)
    {
        if (sWeightToDamageTable[i] > GetPokedexHeightWeight(SpeciesToNationalPokedexNum(gBattleMons[gBattlerTarget].species), 1))
            break;
    }
    if (sWeightToDamageTable[i] != 0xFFFF)
        gDynamicBasePower = sWeightToDamageTable[i + 1];
    else
        gDynamicBasePower = 120;
    gBattlescriptCurrInstr++;
}

static void Cmd_assistattackselect(void)
{
    s32 chooseableMovesNo = 0;
    struct Pokemon *party;
    s32 monId, moveIndex;
    u16 *validMoves = (u16 *)&gBattleStruct->assistPossibleMoves;

    if (GET_BATTLER_SIDE(gBattlerAttacker) != B_SIDE_PLAYER)
        party = gEnemyParty;
    else
        party = gPlayerParty;
    for (monId = 0; monId < PARTY_SIZE; ++monId)
    {
        if (monId == gBattlerPartyIndexes[gBattlerAttacker]
         || GetMonData(&party[monId], MON_DATA_SPECIES_OR_EGG) == SPECIES_NONE
         || GetMonData(&party[monId], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG)
            continue;
        for (moveIndex = 0; moveIndex < MAX_MON_MOVES; ++moveIndex)
        {
            s32 i = 0;
            u16 move = GetMonData(&party[monId], MON_DATA_MOVE1 + moveIndex);

            if (IsInvalidForSleepTalkOrAssist(move))
                continue;
            for (; sMovesForbiddenToCopy[i] != ASSIST_FORBIDDEN_END && move != sMovesForbiddenToCopy[i]; ++i);
            if (sMovesForbiddenToCopy[i] != ASSIST_FORBIDDEN_END || move == MOVE_NONE)
                continue;
            validMoves[chooseableMovesNo] = move;
            ++chooseableMovesNo;
        }
    }
    if (chooseableMovesNo)
    {
        gHitMarker &= ~(HITMARKER_ATTACKSTRING_PRINTED);
        gCalledMove = validMoves[((Random() & 0xFF) * chooseableMovesNo) >> 8];
        gBattlerTarget = GetMoveTarget(gCalledMove, NO_TARGET_OVERRIDE);
        gBattlescriptCurrInstr += 5;
    }
    else
    {
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    }
}

static void Cmd_trysetmagiccoat(void)
{
    gBattlerTarget = gBattlerAttacker;
    gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
    if (gCurrentTurnActionNumber == gBattlersCount - 1) //last turn
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gProtectStructs[gBattlerAttacker].bounceMove = TRUE;
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_trysetsnatch(void)
{
    gSpecialStatuses[gBattlerAttacker].ppNotAffectedByPressure = TRUE;
    if (gCurrentTurnActionNumber == gBattlersCount - 1) //last turn
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        gProtectStructs[gBattlerAttacker].stealMove = TRUE;
        gBattlescriptCurrInstr += 5;
    }
}

static void Cmd_trygetintimidatetarget(void)
{
    u8 side;

    gBattleStruct->battler = gBattleStruct->intimidateBattler;
    side = GetBattlerSide(gBattleStruct->battler);
    PREPARE_ABILITY_BUFFER(gBattleTextBuff1, gBattleMons[gBattleStruct->battler].ability);

    for (;gBattlerTarget < gBattlersCount; gBattlerTarget++)
    {
        if (GetBattlerSide(gBattlerTarget) == side)
            continue;
        if (!(gAbsentBattlerFlags & gBitTable[gBattlerTarget]))
            break;
    }

    if (gBattlerTarget >= gBattlersCount)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
        gBattlescriptCurrInstr += 5;
}

static void Cmd_switchoutabilities(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    switch (gBattleMons[gActiveBattler].ability)
    {
    case ABILITY_NATURAL_CURE:
        gBattleMons[gActiveBattler].status1 = STATUS1_NONE;
        BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, gBitTable[gSharedMem[BSTRUCT_OFF(battlerPartyIndexes) + gActiveBattler]], sizeof(gBattleMons[gActiveBattler].status1), &gBattleMons[gActiveBattler].status1);
        MarkBattlerForControllerExec(gActiveBattler);
        break;
    }
    gBattlescriptCurrInstr += 2;
}

static void Cmd_jumpifhasnohp(void)
{
    gActiveBattler = GetBattlerForBattleScript(T2_READ_8(gBattlescriptCurrInstr + 1));
    if (gBattleMons[gActiveBattler].hp == 0)
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 2);
    else
        gBattlescriptCurrInstr += 6;
}

static void Cmd_getsecretpowereffect(void)
{
    switch (gBattleEnvironment)
    {
    case BATTLE_ENVIRONMENT_GRASS:
        gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_POISON;
        break;
    case BATTLE_ENVIRONMENT_LONG_GRASS:
        gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_SLEEP;
        break;
    case BATTLE_ENVIRONMENT_SAND:
        gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_ACC_MINUS_1;
        break;
    case BATTLE_ENVIRONMENT_UNDERWATER:
        gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_DEF_MINUS_1;
        break;
    case BATTLE_ENVIRONMENT_WATER:
        gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_ATK_MINUS_1;
        break;
    case BATTLE_ENVIRONMENT_POND:
        gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_SPD_MINUS_1;
        break;
    case BATTLE_ENVIRONMENT_MOUNTAIN:
        gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_CONFUSION;
        break;
    case BATTLE_ENVIRONMENT_CAVE:
        gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_FLINCH;
        break;
    default:
        gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_PARALYSIS;
        break;
    }
    gBattlescriptCurrInstr++;
}

static void atkE5_pickup(void)
{
    s32 i;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);
        u16 heldItem = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
        u8 ability;
        if (GetMonData(&gPlayerParty[i], MON_DATA_ABILITY_NUM))
            ability = gSpeciesInfo[species].ability2;
        else
            ability = gSpeciesInfo[species].ability1;

        if (ability == ABILITY_PICKUP && species != 0 && species != SPECIES_EGG && heldItem == 0 && (Random() % 10) == 0)
        {
            s32 chance = Random() % 100;
            s32 j;
            for (j = 0; j < (s32)ARRAY_COUNT(sPickupItems) - 2; j += 2)
            {
                if (sPickupItems[j + 1] > chance)
                    break;
            }
            SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, (const void*) &sPickupItems[j]);
        }
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_docastformchangeanimation(void)
{
    gActiveBattler = gBattleStruct->battler;
    if (gBattleMons[gActiveBattler].status2 & STATUS2_SUBSTITUTE)
        gBattleStruct->formToChangeInto |= 0x80;
    BtlController_EmitBattleAnimation(B_COMM_TO_CONTROLLER, B_ANIM_CASTFORM_CHANGE, gBattleStruct->formToChangeInto);
    MarkBattlerForControllerExec(gActiveBattler);
    gBattlescriptCurrInstr++;
}

static void Cmd_trycastformdatachange(void)
{
    u8 form;
    gBattlescriptCurrInstr++;
    form = CastformDataTypeChange(gBattleStruct->battler);
    if (form)
    {
        BattleScriptPushCursorAndCallback(BattleScript_CastformChange);
        gBattleStruct->formToChangeInto = form - 1;
    }
}

static void Cmd_settypebasedhalvers(void) //water/mud sport
{
    bool8 worked = FALSE;
    if (gBattleMoves[gCurrentMove].effect == EFFECT_MUD_SPORT)
    {
        if (!(gStatuses3[gBattlerAttacker] & STATUS3_MUDSPORT))
        {
            gStatuses3[gBattlerAttacker] |= STATUS3_MUDSPORT;
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEAKEN_ELECTRIC;
            worked = TRUE;
        }
    }
    else //water sport
    {
        if (!(gStatuses3[gBattlerAttacker] & STATUS3_WATERSPORT))
        {
            gStatuses3[gBattlerAttacker] |= STATUS3_WATERSPORT;
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WEAKEN_FIRE;
            worked = TRUE;
        }
    }
    if (worked)
        gBattlescriptCurrInstr += 5;
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_setweatherballtype(void)
{
    if (WEATHER_HAS_EFFECT)
    {
        if ((u8)(gBattleWeather))
            gBattleStruct->dmgMultiplier = 2;
        if (gBattleWeather & B_WEATHER_RAIN)
            gBattleStruct->dynamicMoveType = TYPE_WATER | F_DYNAMIC_TYPE_SET;
        else if (gBattleWeather & B_WEATHER_SANDSTORM)
            gBattleStruct->dynamicMoveType = TYPE_ROCK | F_DYNAMIC_TYPE_SET;
        else if (gBattleWeather & B_WEATHER_SUN)
            gBattleStruct->dynamicMoveType = TYPE_FIRE | F_DYNAMIC_TYPE_SET;
        else if (gBattleWeather & B_WEATHER_HAIL_TEMPORARY)
            gBattleStruct->dynamicMoveType = TYPE_ICE | F_DYNAMIC_TYPE_SET;
        else
            gBattleStruct->dynamicMoveType = TYPE_NORMAL | F_DYNAMIC_TYPE_SET;
    }
    gBattlescriptCurrInstr++;
}

static void Cmd_tryrecycleitem(void)
{
    u16* usedHeldItem;
    gActiveBattler = gBattlerAttacker;
    usedHeldItem = ((u16 *)&gSharedMem[BSTRUCT_OFF(usedHeldItems) + gActiveBattler * 2]);
    if (*usedHeldItem && gBattleMons[gActiveBattler].item == ITEM_NONE)
    {
        gLastUsedItem = *usedHeldItem;
        *usedHeldItem = ITEM_NONE;
        gBattleMons[gActiveBattler].item = gLastUsedItem;
        BtlController_EmitSetMonData(B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE, 0, sizeof(gBattleMons[gActiveBattler].item), &gBattleMons[gActiveBattler].item);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_settypetoenvironment(void)
{
    if (gBattleMons[gBattlerAttacker].type1 != sEnvironmentToType[gBattleEnvironment] && gBattleMons[gBattlerAttacker].type2 != sEnvironmentToType[gBattleEnvironment])
    {
        gBattleMons[gBattlerAttacker].type1 = sEnvironmentToType[gBattleEnvironment];
        gBattleMons[gBattlerAttacker].type2 = sEnvironmentToType[gBattleEnvironment];
        PREPARE_TYPE_BUFFER(gBattleTextBuff1, sEnvironmentToType[gBattleEnvironment]);
        gBattlescriptCurrInstr += 5;
    }
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_pursuitdoubles(void)
{
    gActiveBattler = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_FLANK);
    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && !(gAbsentBattlerFlags & gBitTable[gActiveBattler]) && gChosenActionByBattler[gActiveBattler] == B_ACTION_USE_MOVE && gChosenMoveByBattler[gActiveBattler] == MOVE_PURSUIT)
    {
        gActionsByTurnOrder[gActiveBattler] = B_ACTION_TRY_FINISH;
        gCurrentMove = MOVE_PURSUIT;
        gBattlescriptCurrInstr += 5;
        gBattleStruct->animTurn = 1;
        gBattleStruct->pursuitDoublesAttacker = gBattlerAttacker;
        gBattlerAttacker = gActiveBattler;
    }
    else
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
}

static void Cmd_snatchsetbattlers(void)
{
    gEffectBattler = gBattlerAttacker;
    if (gBattlerAttacker == gBattlerTarget)
        gBattlerAttacker = gBattlerTarget = gBattleStruct->battler;
    else
        gBattlerTarget = gBattleStruct->battler;
    gBattleStruct->battler = gEffectBattler;
    gBattlescriptCurrInstr++;
}

static void Cmd_removelightscreenreflect(void) //brick break
{
    u8 opposingSide = GetBattlerSide(gBattlerAttacker) ^ BIT_SIDE;
    if (gSideTimers[opposingSide].reflectTimer || gSideTimers[opposingSide].lightscreenTimer)
    {
        gSideStatuses[opposingSide] &= ~(SIDE_STATUS_REFLECT);
        gSideStatuses[opposingSide] &= ~(SIDE_STATUS_LIGHTSCREEN);
        gSideTimers[opposingSide].reflectTimer = 0;
        gSideTimers[opposingSide].lightscreenTimer = 0;
        gBattleStruct->animTurn = 1;
        gBattleStruct->animTargetsHit = 1;
    }
    else
    {
        gBattleStruct->animTurn = 0;
        gBattleStruct->animTargetsHit = 0;
    }
    gBattlescriptCurrInstr++;
}

void atkEF_handleballthrow(void)
{
    u8 ballMultiplier = 0;
    if (gBattleControllerExecFlags)
        return;

    gActiveBattler = gBattlerAttacker;
    gBattlerTarget = gBattlerAttacker ^ BIT_SIDE;
    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        BtlController_EmitBallThrowAnim(B_COMM_TO_CONTROLLER, BALL_TRAINER_BLOCK);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr = BattleScript_TrainerBallBlock;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL)
    {
        BtlController_EmitBallThrowAnim(B_COMM_TO_CONTROLLER, BALL_3_SHAKES_SUCCESS);
        MarkBattlerForControllerExec(gActiveBattler);
        gBattlescriptCurrInstr = BattleScript_WallyBallThrow;
    }
    else
    {
        u32 odds;
        u8 catchRate;
        if (gLastUsedItem == ITEM_SAFARI_BALL)
            catchRate = gBattleStruct->safariCatchFactor * 1275 / 100;
        else
            catchRate = gSpeciesInfo[gBattleMons[gBattlerTarget].species].catchRate;
        if (gLastUsedItem > ITEM_SAFARI_BALL)
        {
            switch (gLastUsedItem)
            {
            case ITEM_NET_BALL:
                if (gBattleMons[gBattlerTarget].type1 == TYPE_WATER || gBattleMons[gBattlerTarget].type2 == TYPE_WATER || gBattleMons[gBattlerTarget].type1 == TYPE_BUG || gBattleMons[gBattlerTarget].type2 == TYPE_BUG)
                    ballMultiplier = 30;
                else
                    ballMultiplier = 10;
                break;
            case ITEM_DIVE_BALL:
                if (GetCurrentMapType() == MAP_TYPE_UNDERWATER)
                    ballMultiplier = 35;
                else
                    ballMultiplier = 10;
                break;
            case ITEM_NEST_BALL:
                if (gBattleMons[gBattlerTarget].level <= 39)
                {
                    ballMultiplier = 40 - gBattleMons[gBattlerTarget].level;
                    if (ballMultiplier <= 9)
                        ballMultiplier = 10;
                }
                else
                    ballMultiplier = 10;
                break;
            case ITEM_REPEAT_BALL:
                if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[gBattlerTarget].species), FLAG_GET_CAUGHT))
                    ballMultiplier = 30;
                else
                    ballMultiplier = 10;
                break;
            case ITEM_TIMER_BALL:
                ballMultiplier = gBattleResults.battleTurnCounter + 10;
                if (ballMultiplier > 40)
                    ballMultiplier = 40;
                break;
            case ITEM_LUXURY_BALL:
            case ITEM_PREMIER_BALL:
                ballMultiplier = 10;
                break;
            }
        }
        else
            ballMultiplier = sBallCatchBonuses[gLastUsedItem - ITEM_ULTRA_BALL];

        odds = (catchRate * ballMultiplier / 10) * (gBattleMons[gBattlerTarget].maxHP * 3 - gBattleMons[gBattlerTarget].hp * 2) / (3 * gBattleMons[gBattlerTarget].maxHP);
        if (gBattleMons[gBattlerTarget].status1 & (STATUS1_SLEEP | STATUS1_FREEZE))
            odds *= 2;
        if (gBattleMons[gBattlerTarget].status1 & (STATUS1_POISON | STATUS1_BURN | STATUS1_PARALYSIS /*| STATUS1_TOXIC_POISON */)) //nice one gf
            odds = (odds * 15) / 10;

        if (gLastUsedItem != ITEM_SAFARI_BALL)
        {
            if (gLastUsedItem == ITEM_MASTER_BALL)
            {
                gBattleResults.usedMasterBall = TRUE;
            }
            else
            {
                if (gBattleResults.catchAttempts[gLastUsedItem - ITEM_ULTRA_BALL] < UCHAR_MAX)
                    gBattleResults.catchAttempts[gLastUsedItem - ITEM_ULTRA_BALL]++;
            }
        }
        if (odds > 254) //poke caught
        {
            BtlController_EmitBallThrowAnim(B_COMM_TO_CONTROLLER, BALL_3_SHAKES_SUCCESS);
            MarkBattlerForControllerExec(gActiveBattler);
            gBattlescriptCurrInstr = BattleScript_SuccessBallThrow;
            SetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerTarget]], MON_DATA_POKEBALL, (const void*) &gLastUsedItem);
            if (CalculatePlayerPartyCount() == PARTY_SIZE)
                gBattleCommunication[MULTISTRING_CHOOSER] = 0;
            else
                gBattleCommunication[MULTISTRING_CHOOSER] = 1;
        }
        else //poke may be caught, calculate shakes
        {
            u8 shakes;
            odds = Sqrt(Sqrt(16711680 / odds));
            odds = 1048560 / odds;
            for (shakes = 0; shakes < BALL_3_SHAKES_SUCCESS && Random() < odds; shakes++) {}
            if (gLastUsedItem == ITEM_MASTER_BALL)
                shakes = BALL_3_SHAKES_SUCCESS; //why calculate the shakes before that check?
            BtlController_EmitBallThrowAnim(B_COMM_TO_CONTROLLER, shakes);
            MarkBattlerForControllerExec(gActiveBattler);
            if (shakes == BALL_3_SHAKES_SUCCESS) //poke caught, copy of the code above
            {
                gBattlescriptCurrInstr = BattleScript_SuccessBallThrow;
                SetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerTarget]], MON_DATA_POKEBALL, (const void*) &gLastUsedItem);
                if (CalculatePlayerPartyCount() == PARTY_SIZE)
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                else
                    gBattleCommunication[MULTISTRING_CHOOSER] = 1;
            }
            else //rip
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = shakes;
                gBattlescriptCurrInstr = BattleScript_ShakeBallThrow;
            }
        }
    }
}

static void atkF0_givecaughtmon(void)
{
    GiveMonToPlayer(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]]);
    gBattleResults.caughtMonSpecies = gBattleMons[gBattlerAttacker ^ BIT_SIDE].species;
    GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_NICKNAME, gBattleResults.caughtMonNick);
    gBattlescriptCurrInstr++;
}

static void atkF1_trysetcaughtmondexflags(void)
{
    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[gBattlerTarget].species), FLAG_GET_CAUGHT))
        gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
    else
    {
        GetSetPokedexFlag(SpeciesToNationalPokedexNum(gBattleMons[gBattlerTarget].species), FLAG_SET_CAUGHT);
        if (gBattleMons[gBattlerTarget].species == SPECIES_UNOWN)
            gSaveBlock2.pokedex.unownPersonality = gBattleMons[gBattlerTarget].personality;
        if (gBattleMons[gBattlerTarget].species == SPECIES_SPINDA) //else if
            gSaveBlock2.pokedex.spindaPersonality = gBattleMons[gBattlerTarget].personality;
        gBattlescriptCurrInstr += 5;
    }
}

extern const u32 gBattleEnvironmentTiles_Building[];
extern const u32 gBattleEnvironmentTilemap_Building[];
extern const u32 gBattleEnvironmentPalette_BattleTower[];

static void atkF2_displaydexinfo(void)
{
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gBattleCommunication[MULTIUSE_STATE]++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
            gBattleCommunication[TASK_ID] = DisplayCaughtMonDexPage(SpeciesToNationalPokedexNum(gBattleMons[gBattlerTarget].species), gBattleMons[gBattlerTarget].otId, gBattleMons[gBattlerTarget].personality);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 2:
        if (!gPaletteFade.active && gMain.callback2 == BattleMainCB2 && !gTasks[gBattleCommunication[TASK_ID]].isActive)
        {
            LZDecompressVram(gBattleEnvironmentTiles_Building, BG_CHAR_ADDR(2));
            LZDecompressVram(gBattleEnvironmentTilemap_Building, BG_SCREEN_ADDR(26));
            LoadCompressedPalette(gBattleEnvironmentPalette_BattleTower, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
            REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(2) | BGCNT_16COLOR | BGCNT_SCREENBASE(26) | BGCNT_TXT512x256;
            gBattle_BG3_X = 0x100;
            BeginNormalPaletteFade(0xFFFC, 0, 16, 0, RGB_BLACK);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
            gBattlescriptCurrInstr++;
        break;
    }
}

void sub_802BBD4(u8 xStart, u8 yStart, u8 xEnd, u8 yEnd, u8 sp0)
{
    int destY;
    int destX;
    int tile;

    for (destY = yStart; destY <= yEnd; destY++)
    {
        for (destX = xStart; destX <= xEnd; destX++)
        {
            u16 * dest = ((u16 *)(BG_VRAM + (24 * BG_SCREEN_SIZE) + (sizeof(u16) * destX) + (0x40 * destY)));
            if (destY == yStart)
            {
                if (destX == xStart)
                    tile = 0x1022;
                else if (destX == xEnd)
                    tile = 0x1024;
                else
                    tile = 0x1023;
            }
            else if (destY == yEnd)
            {
                if (destX == xStart)
                    tile = 0x1028;
                else if (destX == xEnd)
                    tile = 0x102A;
                else
                    tile = 0x1029;
            }
            else
            {
                if (destX == xStart)
                    tile = 0x1025;
                else if (destX == xEnd)
                    tile = 0x1027;
                else
                    tile = 0x1026;
            }
            if (sp0)
                tile = 0;
            *dest = tile;
        }
    }
}

void sub_802BC6C(void)
{
    MenuCursor_SetPos814A880(0xC8, ((gBattleCommunication[CURSOR_POSITION] << 28) + 1207959552) >> 24); //what could that be?
}

void nullsub_6(void)
{
    return;
}

static void atkF3_trygivecaughtmonnick(void)
{
    switch (gBattleCommunication[MULTIUSE_STATE])
    {
    case 0:
        sub_8023A80();
        gBattleCommunication[MULTIUSE_STATE]++;
        gBattleCommunication[CURSOR_POSITION] = 0;
        sub_802BC6C();
        break;
    case 1:
        if (JOY_NEW(DPAD_UP) && gBattleCommunication[CURSOR_POSITION] != 0)
        {
            PlaySE(SE_SELECT);
            nullsub_6();
            gBattleCommunication[CURSOR_POSITION] = 0;
            sub_802BC6C();
        }
        if (JOY_NEW(DPAD_DOWN) && gBattleCommunication[CURSOR_POSITION] == 0)
        {
            PlaySE(SE_SELECT);
            nullsub_6();
            gBattleCommunication[CURSOR_POSITION] = 1;
            sub_802BC6C();
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (gBattleCommunication[CURSOR_POSITION] == 0)
            {
                gBattleCommunication[MULTIUSE_STATE]++;
                BeginFastPaletteFade(FAST_FADE_OUT_TO_BLACK);
            }
            else
                gBattleCommunication[MULTIUSE_STATE] = 4;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gBattleCommunication[MULTIUSE_STATE] = 4;
        }
        break;
    case 2:
        if (!gPaletteFade.active)
        {
            GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_NICKNAME, gBattleStruct->caughtMonNick);
            DoNamingScreen(NAMING_SCREEN_CAUGHT_MON, gBattleStruct->caughtMonNick, GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_SPECIES), GetMonGender(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]]), GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_PERSONALITY, 0), BattleMainCB2);
            gBattleCommunication[MULTIUSE_STATE]++;
        }
        break;
    case 3:
        if (gMain.callback2 == BattleMainCB2 && !gPaletteFade.active )
        {
            SetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker ^ BIT_SIDE]], MON_DATA_NICKNAME, gBattleStruct->caughtMonNick);
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        }
        break;
    case 4:
        if (CalculatePlayerPartyCount() == PARTY_SIZE)
            gBattlescriptCurrInstr += 5;
        else
            gBattlescriptCurrInstr = T1_READ_PTR(gBattlescriptCurrInstr + 1);
        break;
    }
}

static void Cmd_subattackerhpbydmg(void)
{
    gBattleMons[gBattlerAttacker].hp -= gBattleMoveDamage;
    gBattlescriptCurrInstr++;
}

static void Cmd_removeattackerstatus1(void)
{
    gBattleMons[gBattlerAttacker].status1 = STATUS1_NONE;
    gBattlescriptCurrInstr++;
}

static void Cmd_finishaction(void)
{
    gCurrentActionFuncId = B_ACTION_FINISHED;
}

static void Cmd_finishturn(void)
{
    gCurrentActionFuncId = B_ACTION_FINISHED;
    gCurrentTurnActionNumber = gBattlersCount;
}
