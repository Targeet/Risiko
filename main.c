//RUN THE "main.exe" DIRECTLY FROM CMD
#include <stdio.h>
#include <string.h>
#include <stdlib.h> //rand()
#include <time.h> //time() & srand()
/*  CONST  */
#define clear() system("@cls||clear") //Clear screen windows/linux
#define totalCountries 4
#define nTanksForCycle 1
#define roundOptions 2
#define minPlayers 2
#define maxDiceRolls 3
#define totalStartingTanks 20
#define maxPlayers totalCountries

/*  VARIABLES   */
char 
countries[totalCountries][15] = {
    "Italy",
    "France",
    "Spain",
    "Austria"
},
options[roundOptions][15] = {
    "End round",
    "Attack"
};

unsigned short
totPlayers, //Total starting players
playersInGame, //Total players that are still playing
nTanksForPlayer, //Starting tanks for players
countriesState[totalCountries]; //Number that defines who owns a country
// 0 -> Not owned, 1 -> first player, 2 -> second player...

typedef struct {
    char name[30];
    unsigned short
    num, //Player 1, Player 2 etc.
    tanks, 
    dices[maxDiceRolls]; 
    //Booleans
    unsigned short attackedOnce; //Every player needs to attack at least 1 time per round
} Player;

/*  FUNCTIONS   */

    //CHECK INPUT
short CheckPlayersNumber(){
    if(totPlayers > maxPlayers){
        printf("\nWe are going to have spectators, we can't let play more than %hu people.\n", maxPlayers);
        return -1;
    }
    else if(totPlayers == 0){
        printf("\nWell, someone has to play the game, try again please.\n");
        return -1;
    }
    else if(totPlayers < minPlayers){
        printf("\nAre you trying to play alone? Invite some friends!\n");
        return -1;
    }
    return 0;
}
short CheckIfNameAlreadyInUse(char *name, Player *players, unsigned short player){
    if(player == 0) return 0;
    for(unsigned short i = 0; i < player; i++){
        //strcmp() -> Compares 2 strings and returns 0 if they are equal
        if(strcmp(players[i].name, name) == 0) { 
            printf("\nName already in use, try another one.\n");
            return -1;
        }
    }
    return 0;
}
short CheckStartingCountry(unsigned short nCountry, unsigned short player){
    //If the number is not in the range of the available countries
    //Total countries - player (Tells how many countries have been chosen) - 1 (0 based index)
    if(nCountry > (totalCountries - player) - 1){
        printf("\nThe country is not in range!\n");
        return -1;
    }
    return 0;
}
short CheckAttackedCountry(unsigned short country){
    //Remove 1 because it's a 0 based index and another 1 to remove your own country
    if(country < 0 || country > (playersInGame - 2)){
        printf("\nThe country is not in range!\n");
        return -1;
    }
    return 0;
}
short CheckAttackingTanks(unsigned short tanks, unsigned short maxTanks){
    if(tanks <= 0){
        printf("\nAre you scared? You need to use at least 1 tank\n");
        return -1;
    }
    else if(tanks > maxTanks){
        printf("\nToo aggressive! You can only use %hu tanks\n", maxTanks);
        return -1;
    }
    return 0;
}
short CheckChosenOption(unsigned short option){
    if(option < 0 || option > roundOptions - 1){
        printf("\nThe option is not in range.\n");
        return -1;
    }
    return 0;
}
short CheckResponse(unsigned short response){
    if(response < 0 || response > 1){
        printf("\nThe response is not in range.\n");
        return -1;
    }
    return 0;
}

    //GENERAL UTILS
void fflushInput(){//Buffer input
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    //EOF -> -1 (end of file)
    //Get chars from the input until it's the end of the file or there is a backslash
}
void ResetCountriesStates(){
    for (unsigned short i = 0; i < totalCountries; i++)
        countriesState[i] = 0;
}
void GiveTanksToPlayers(Player *players){
    for(unsigned short i = 0; i < playersInGame; i++)
        players[i].tanks += nTanksForCycle;
}
short GetCountryIndexFromPlayer(unsigned short player){
    unsigned short countryIndex;
    for(unsigned short i = 0; i < totalCountries; i++)
        if(countriesState[i] == player + 1)
            return i;
    return -1;
}
unsigned short GetPlayerIndexFromPlayerNum(unsigned short playerNum, Player *players){
    for(unsigned short i = 0; i < playersInGame; i++)
        if(playerNum == players[i].num) return i;
    return playersInGame;
}
unsigned short RandomBtw(unsigned short startingNumber, unsigned short totalNumbers){
	return  startingNumber + (rand() % totalNumbers);
}

    //PRINT
void PrintLostTanks(unsigned short player, unsigned short lostTanks, Player *players){
    if(lostTanks == 1)
        printf("\n%s lost 1 tank", players[player].name);
    else
        printf("\n%s lost %hu tanks", players[player].name, lostTanks);
}
void PrintPlayerOptions(unsigned short player, Player *players){
    for(unsigned short i = 0, index = 0; i < roundOptions; i++){
        //If the player didn't attack once in his turn then he can't end the round
        if(players[player].attackedOnce == 0 && strcmp(options[i], options[0]) == 0){
            index++; i++;
        }
        printf("%hu %s\n", i - index, options[i]);
    }
}
void PrintAvailableCountries(){
    for(unsigned short i = 0, index = 0; i < totalCountries; i++)
        if(countriesState[i] == 0){
            printf("%hu %s\n", index++, countries[i]);
            fflush(stdout);
        }
}
void PrintAttackableCountries(unsigned short player, Player *players){
    for(unsigned short i = 0, index = 0; i < totalCountries; i++)
        if(countriesState[i] != player + 1 && countriesState[i] != 0){
            //Get the index of the player that owns the country, so it's possible to show the number of tanks 
            unsigned short convertedIndex = GetPlayerIndexFromPlayerNum(countriesState[i] - 1, players);
            printf("%hu %s(%hu)\n", index++, countries[i], players[convertedIndex].tanks);
            fflush(stdout);
        }
}

    //MENU
void PrintMenu(unsigned short option, unsigned short player, char *playerName, Player *players){
    switch (option)
    {
        //Ask number of players
        case 0:
            printf("How many player are in the game? (Min: %hu) (Max: %hu)\n", minPlayers, maxPlayers);
            break;

        //ChooseCountry
        case 1:
            printf("\n%s which country do you want to start in?\n", playerName);
            PrintAvailableCountries();
            break;

        //RoundOptions
        case 2:
            printf("\n%s choose a move?\n", playerName);
            PrintPlayerOptions(player, players);
            break;
        
        //Attackable countries
        case 3:
            printf("\n%s which country do you want to attack?\n", playerName);
            PrintAttackableCountries(player, players);
            break;
    }
}
void AskHowManyPlayers(){
    clear();
    PrintMenu(0, 0, "", NULL);

    do{//CHECK INPUT
        printf("Number of players -> ");
        while(scanf("%1hu", &totPlayers) != 1){
            printf("\nWatch what you type, you can only type a number between %hu and %hu.\n", minPlayers, maxPlayers);
            printf("Number of players -> ");
            fflushInput();//Clear input buffer;
        }
        fflushInput();//Clear input buffer;
    }while(CheckPlayersNumber() == -1);
}
void ChooseStartingCountry(unsigned short player, char *playerName){
    unsigned short nCountry;
    clear();//Clear screen
    PrintMenu(1, player, playerName, NULL);

    do{//CHECK INPUT
        printf("Your country -> ");
        while(scanf("%1hu", &nCountry) != 1){
            printf("\nTo select a country type the number at the left of it.\n");
            printf("Your country -> ");
            fflushInput();//Clear input buffer;
        }
        fflushInput();//Clear input buffer;
    }while(CheckStartingCountry(nCountry, player) == -1);

    //Set the chosen country: owned by the player
    for(unsigned short i = 0, index = 0; i < totalCountries; i++){
        if(countriesState[i] != 0) continue; //If a country is owned 
        if(nCountry != index) index++;
        else if(nCountry == index) {
            countriesState[i] = player + 1; //Assign countryState (1 based index)
            return;
        } 
    }
        
}
void GetName(unsigned short player, Player *players){
    do{//CHECK INPUT
        printf("Player %hu", player + 1);
        printf(" -> ");
        scanf("%29s", players[player].name);//Limit length (Will cut the name if too long)
        fflushInput();//Clear input buffer;
    }while(CheckIfNameAlreadyInUse(players[player].name, players, player) == -1);
}
unsigned short StartRound(unsigned short player, char *playerName, Player *players){
    clear();
    unsigned short chosenOption;
    PrintMenu(2, player, playerName, players);

    //Write a different string if a player has only one tank
    if(players[player].tanks == 1) printf("You have only one tank.\n");
    else printf("You have %hu total tanks.\n", players[player].tanks);

    do{//CHECK INPUT
        printf("Option -> ");
        while(scanf("%1hu", &chosenOption) != 1){
            printf("\nTo select an option type the number on the left of it.\n");
            printf("Option -> ");
            fflushInput();
        }
        if(players[player].attackedOnce == 0) chosenOption++; 
        fflushInput();
    }while(CheckChosenOption(chosenOption) == -1);
    return chosenOption;
}
unsigned short AskToPlayAgain(){
    unsigned short response = 0;
    printf("Nice game!\n");
    printf("\nDo you want to play again?\n0 Yes\n1 No\n");
    do{
        printf("Response -> ");
        while(scanf("%1hu", &response) != 1){
            printf("\nTo select a response type the number at the left of it.\n");
            printf("Response -> ");
            fflushInput();
        }
        fflushInput();
    }while(CheckResponse(response) == -1);
    clear();
    return response;
}

    //PLAYER LOSE/WIN
void WinningPlayer(char *winnerName, unsigned short winnerTanks){
    clear();
    //Write a different string if the player has only one tank remaining
    if(winnerTanks > 1) printf("%s won the game with %hu tanks remaining\n", winnerName, winnerTanks);
    else printf("%s won the game with only %hu tank remaining\n", winnerName, winnerTanks);
}
unsigned short PlayerLost(unsigned short player, Player *players, unsigned short totPlayers){
    printf("\n%s is out of tanks!\n", players[player].name);

    //Set the country of the player to "not owned"
    for(unsigned short i = 0; i < totalCountries; i++) 
        if(countriesState[i] == players[player].num + 1)
            countriesState[i] = 0;

    //Put the player at the end of the players array
    for(unsigned short i = player, k = player + 1; k < totPlayers; i++, k++){
        Player temp = players[i];
        players[i] = players[k];
        players[k] = temp;
    }

    //Reduce total playing players counter
    playersInGame--;
    return 1;
}
unsigned short CheckIfWinner(unsigned short checkTanks, char *winnerName, unsigned short winnerTanks){
    //Check if the player in the second position of the array has 0 tanks, that means that there's only 1 player remaining with tanks
    if(checkTanks == 0){
        WinningPlayer(winnerName, winnerTanks);
        return 1;
    } 
    return 0;
} 

    //ATTACK
void RollDices(unsigned short maxIndex, unsigned short *dices){
    for(unsigned short i = 0; i < maxDiceRolls; i++) dices[i] = 0; //Reset array to 0
    for(unsigned short i = 0; i < maxIndex; i++) dices[i] = RandomBtw(1,6); //Get random values
    
    //Sort array O(n^2)
    for(unsigned short i = 0; i < maxDiceRolls; i++)
        for(unsigned short k = i + 1; k < maxDiceRolls; k++)
            if(dices[i] < dices[k]){
                unsigned short temp = dices[i];
                dices[i] = dices[k];
                dices[k] = temp;
            }
}
unsigned short GetDefendingPlayerNum(unsigned short countryIndex, unsigned short player){
    for(unsigned short i = 0, index = 0; i < totalCountries; i++){
        if(countriesState[i] == player + 1 || countriesState[i] == 0) continue;
        if(index == countryIndex) return countriesState[i];
        else index++;
    }
    return 0;
}
unsigned short GetCountryToAttack(unsigned short player, char *playerName, Player *players){
    unsigned short chosenCountry;
    PrintMenu(3, player, playerName, players);
    do{
        printf("Country -> ");
        while(scanf("%1hu", &chosenCountry) != 1){
            printf("\nTo attack a country type the number at the left of it.\n");
            printf("Country -> ");
            fflushInput();
        }
        fflushInput();
    }while(CheckAttackedCountry(chosenCountry));
    return chosenCountry;
}
unsigned short GetAttackingTanks(char *playerName, unsigned short totalTanks, unsigned short totalDefenderTanks){
    unsigned short maxTanks, nTanks;
    maxTanks = totalTanks > maxDiceRolls ? maxDiceRolls : totalTanks;
    if(totalDefenderTanks < maxTanks) maxTanks = totalDefenderTanks;
    printf("\n%s how many tanks do you want to use (Max: %hu)? (You have %hu total tanks)\n", playerName, maxTanks, totalTanks);
    do{
        printf("Tanks -> ");
        while(scanf("%1hu", &nTanks) != 1){
            printf("Watch what you type, you can only type a number as high as %hu.", maxTanks);
            printf("Tanks -> ");
            fflushInput();
        }
        fflushInput();
    }while(CheckAttackingTanks(nTanks, maxTanks));
    return nTanks;
}    
unsigned short GetDefendingTanks(unsigned short attackingTanks, unsigned short defTotalTanks){
    if(defTotalTanks >= attackingTanks) return attackingTanks; 
    else return defTotalTanks;
}
unsigned short Attack(Player *players, unsigned short playerTurn){
    unsigned short 
    chosenCountry,
    minTanks, //Dices to roll
    attackingTanks, //Chosen number of tanks
    defendingTanks, //Number of tanks that the defender needs to use 
    defendingPlayer; //Index for players array

    //Get country to attack and defending player
    chosenCountry = GetCountryToAttack(players[playerTurn].num, players[playerTurn].name, players);
    defendingPlayer = GetPlayerIndexFromPlayerNum(GetDefendingPlayerNum(chosenCountry, playerTurn) - 1, players);

    unsigned short
    initialAttackerTanks = players[playerTurn].tanks,
    initialDefenderTanks = players[defendingPlayer].tanks;

    //Get tanks
    attackingTanks = GetAttackingTanks(players[playerTurn].name, players[playerTurn].tanks, initialDefenderTanks);
    defendingTanks = GetDefendingTanks(attackingTanks, initialDefenderTanks);
    minTanks = attackingTanks < defendingTanks ? attackingTanks : defendingTanks;

    //Attack message
    printf("\n-----------------------------------------------------------------------\n");
    if (minTanks == 1) printf("%s is attacking %s with 1 tank\n\n", players[playerTurn].name, players[defendingPlayer].name);
    else printf("%s is attacking %s with %hu tanks\n\n", players[playerTurn].name, players[defendingPlayer].name, minTanks);
    fflush(stdout);

    //Get dices values (sorted)
    RollDices(minTanks, players[playerTurn].dices);
    RollDices(minTanks, players[defendingPlayer].dices);

    //Confront values
    for(unsigned short i = 0; i < maxDiceRolls; i++){
        if(players[playerTurn].dices[i] == 0) break; //No more dices

        //Confront dices results and print them
        //Defender won
        if(players[playerTurn].dices[i] <= players[defendingPlayer].dices[i]){
            players[playerTurn].tanks--;
            printf("\033[0;31m"); printf("\tX"); printf("\033[0m");
            printf(" %hu %hu \n", players[playerTurn].dices[i], players[defendingPlayer].dices[i]);
            fflush(stdout);
        }
        //Attacker won
        else{
            players[defendingPlayer].tanks--;
            printf("\t  %hu %hu ", players[playerTurn].dices[i], players[defendingPlayer].dices[i]);
            printf("\033[0;31m"); printf("X\n"); printf("\033[0m");
            fflush(stdout);
        }

        //Check if one of the players lost all tanks (loses)
        if(players[playerTurn].tanks == 0) return PlayerLost(playerTurn, players, totPlayers);
        else if(players[defendingPlayer].tanks == 0) return PlayerLost(defendingPlayer, players, totPlayers);
    }

    //Results (lost tanks)
    //Attacker
    if(players[playerTurn].tanks < initialAttackerTanks) 
        PrintLostTanks(playerTurn, initialAttackerTanks - players[playerTurn].tanks, players);
    //Defender
    if(players[defendingPlayer].tanks < initialDefenderTanks) 
        PrintLostTanks(defendingPlayer, initialDefenderTanks - players[defendingPlayer].tanks, players);

    return 0;
}
void Round(Player *players, unsigned short playerTurn){
    unsigned short loop = 0;
    while(loop != 1){
        switch(StartRound(playerTurn, players[playerTurn].name, players)){
        //End round
        case 0: loop = 1; break;
        //Attack
        case 1: 
            players[playerTurn].attackedOnce = 1;
            loop = Attack(players, playerTurn); 
            //To stop program
            printf("\nPress enter to continue...");
            fgetc(stdin);
            break;
        }
    }
}   
    
    //GREETINGS
void Welcome(){
    printf("Heyyy, welcome in Risiko!\n");
    printf("Press enter to continue...");
    fgetc(stdin);
}
void EndApplication(){
    printf("It was great to have you!\n");
    printf("Press enter to close the application...");
    fgetc(stdin);
}

int main(){
    unsigned short endApplication = 0;

    //Set the random seed
    srand(time(NULL));

    //Welcome message
    Welcome();

    while(endApplication != 1)
    {
        ResetCountriesStates();

        //Get number of players
        AskHowManyPlayers(); 

        //Assign variables
        unsigned short endGame = 0;
        Player players[totPlayers];
        playersInGame = totPlayers;
        nTanksForPlayer = totalStartingTanks/totPlayers < 1? 1 : totalStartingTanks/totPlayers;

        //Get players names
        clear();//Clear screen
        printf("Insert your name\n");
        for(unsigned short i = 0; i < totPlayers; i++){
            players[i].attackedOnce = 0;
            players[i].num = i; //Assign player num (0 based index)
            GetName(i, players);
        }

        //Get player starting countries
        for(unsigned short i = 0; i < totPlayers; i++) {
            players[i].tanks = nTanksForPlayer;
            ChooseStartingCountry(i, players[i].name);
        }

        //Game loop
        unsigned short 
        playerTurn = RandomBtw(0, totPlayers - 1), //Decide a random player to start
        firstMovePlayer = playerTurn;

        while(endGame != 1){
            //Round loop
            players[playerTurn].attackedOnce = 0;
            Round(players, playerTurn);

            //Check if there is a winner
            endGame = CheckIfWinner(players[1].tanks, players[0].name, players[0].tanks);

            //Change player
            playerTurn + 1 >= playersInGame ? playerTurn = 0 : playerTurn++;

            //Cycle completed
            if(firstMovePlayer == playerTurn + (totPlayers - playersInGame)) GiveTanksToPlayers(players);
        }
        endApplication = AskToPlayAgain();
    }
    EndApplication();
    return 0;
}