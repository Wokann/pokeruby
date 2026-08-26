VerdanturfTown_ContestHall_MapScripts:: @ 8153E95
	.byte 0

	.byte 0 @ XXX

VerdanturfTown_ContestHall_EventScript_BoyContestAdvice:: @ 8153E97
	msgbox VerdanturfTown_ContestHall_Text_WhichContestYouEntering, MSGBOX_NPC
	end

VerdanturfTown_ContestHall_EventScript_LittleGirlCutePokemon:: @ 8153EA0
	msgbox VerdanturfTown_ContestHall_Text_RaisedMonToBeCute, MSGBOX_NPC
	end

VerdanturfTown_ContestHall_EventScript_BoyPokemonRules:: @ 8153EA9
	msgbox VerdanturfTown_ContestHall_Text_MyMonRules, MSGBOX_NPC
	end

VerdanturfTown_ContestHall_EventScript_NormalRankSign:: @ 8153EB2
	msgbox VerdanturfTown_ContestHall_Text_NormalRankStage, MSGBOX_SIGN
	end
